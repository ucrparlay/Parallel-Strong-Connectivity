#pragma once
#include <assert.h>
#include "parlay/parallel.h"
#include "parlay/random.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <queue>
#include <vector>

#include "bfs.hpp"
#include "get_time.hpp"
#include "graph.hpp"
#include "hash_bag.h"
#include "resizable_set.hpp"
using namespace std;

// only support NodeId = uint32_t;
// ST = (source, target) = tuple<uint32_t, uint32_t>;
using ST = uint64_t;

inline ST combine(NodeId s, NodeId t) { return (((ST)s) << 32) + (ST)t; }
inline NodeId get_first(ST a) { return (NodeId)(a >> 32); }
inline NodeId get_second(ST a) { return (NodeId)(a & (((ST)1 << 32) - 1)); }
struct hash_kv {
  ST operator()(const ST& k) { return _hash(k); }
};

timer pack_timer;
timer table_timer;
timer resize_timer;
timer table_init_timer;
class LE_LIST {
 private:
  Graph& G;
  sequence<NodeId> centers;
  hashbag<ST> bag;
  sequence<ST> front;
  sequence<NodeId> priority;
  sequence<NodeId> dist;
  sequence<NodeId> next_dist;
  NodeId n_front;
  NodeId start_idx;
  size_t m;
  size_t table_m;
  void multi_search();

 public:
  size_t list_offset;
  sequence<tuple<NodeId, NodeId, NodeId>> list;
  void seq_le_list();
  void le_list(double beta);
  size_t num_round;
  LE_LIST() = delete;
  void init() {
    parallel_for(0, G.n, [&](size_t i) {
      dist[i] = UINT_N_MAX;
      next_dist[i] = UINT_N_MAX;
    });
  }
  LE_LIST(Graph& _G) : G(_G), bag(ceil(log(G.n)) * G.n) {
    m = 4 * ceil(log(G.n)) * G.n;  // log_e
    assert(m <= numeric_limits<size_t>::max());
    list = sequence<tuple<NodeId, NodeId, NodeId>>(m);
    centers = parlay::random_permutation((NodeId)G.n);
    front = sequence<ST>(ceil(log(G.n)) *
                         G.n);  // no more than LeList size; choose nlogn
    dist = sequence<NodeId>(G.n);
    next_dist = sequence<NodeId>(G.n);
  }
  void maybe_resize(size_t n_inc);
};

void LE_LIST::seq_le_list() {
  for (size_t i = 0; i < G.n; i++) {
    dist[i] = UINT_N_MAX;
  }
  list_offset = 0;
  for (NodeId i = 0; i < G.n; i++) {
    queue<NodeId> my_queue;
    my_queue.push(centers[i]);
    dist[centers[i]] = 0;
    list[list_offset++] = make_tuple(centers[i], i, 0);
    while (!my_queue.empty()) {
      NodeId current_node;
      current_node = my_queue.front();
      my_queue.pop();
      for (size_t j = G.offset[current_node]; j < G.offset[current_node + 1];
           j++) {
        NodeId ngb_node = G.E[j];
        if (dist[current_node] + 1 < dist[ngb_node]) {
          dist[ngb_node] = dist[current_node] + 1;
          assert(list_offset < m);
          assert(list_offset <= numeric_limits<size_t>::max());
          list[list_offset++] = make_tuple(ngb_node, i, dist[ngb_node]);
          my_queue.push(ngb_node);
        }
      }
    }
  }
  auto cmp = [&](tuple<NodeId, NodeId, NodeId> a,
                 tuple<NodeId, NodeId, NodeId> b) {
    if (get<0>(a) != get<0>(b)) {
      return get<0>(a) < get<0>(b);
    } else {
      return get<1>(a) < get<1>(b);
    }
  };
  std::sort(list.begin(), list.begin() + list_offset, cmp);
  for (size_t i = 0; i < list_offset; i++) {
    get<1>(list[i]) = centers[get<1>(list[i])];
  }
}

void LE_LIST::maybe_resize(size_t n_inc) {
  if (list_offset + n_inc >= m) {
    m = (list_offset + n_inc) * 2;
    cout << "resize list to size " << m << endl;
    assert(m <= numeric_limits<size_t>::max());
    auto old_list = std::move(list);
    list = sequence<tuple<NodeId, NodeId, NodeId>>(m);
    parallel_for(0, list_offset, [&](size_t i) { list[i] = old_list[i]; });
  }
}
void LE_LIST::multi_search() {
  maybe_resize(n_front);
  table_init_timer.start();
  table_m = max(table_m, (size_t)n_front);
  size_t backing_size = 1 << parlay::log2_up(table_m * 2);
  assert(table_m < backing_size);
  auto table = gbbs::resizable_set<ST, hash_kv>(
      backing_size, combine(UINT_N_MAX, UINT_N_MAX), hash_kv());
  table_init_timer.stop();
  double resize_time = 0;
  parallel_for(
      start_idx, start_idx + n_front,
      [&](size_t i) {
        size_t idx = i - start_idx;
        front[idx] = combine(i, centers[i]);  // (source's priority, target)
        table.insert(front[idx]);
        next_dist[centers[i]] = 0;
        list[list_offset + idx] = make_tuple(centers[i], i, 0);
      },
      BLOCK_SIZE);
  list_offset += n_front;
  size_t sub_n_front = n_front;
  size_t sub_round = 0;
  while (sub_n_front > 0) {
    sub_round++;
    // #if defined(DEBUG)
    // cout << "sub-round " << sub_round << " sub_n_front " << sub_n_front
    // << endl;
    // #endif
    auto im_f = [&](size_t i) {
      NodeId v = get_second(front[i]);
      auto pred = [&](const NodeId& ngh) { return dist[ngh] > sub_round; };
      size_t effective_degree =
          parlay::count_if((G.E).cut(G.offset[v], G.offset[v + 1]), pred);
      return effective_degree;
    };
    auto im = parlay::delayed_seq<size_t>(sub_n_front, im_f);
    table_timer.start();
    size_t sum = parlay::reduce(im);
    table_timer.stop();
    // cout << "sum " << sum << endl;
    resize_timer.start();
    table.maybe_resize(sum);
    resize_time += resize_timer.stop();
    maybe_resize(sum);
    parallel_for(
        0, sub_n_front,
        [&](size_t i) {
          NodeId label = get_first(front[i]);
          NodeId current_node = get_second(front[i]);
          // cout << label << " " << current_node << endl;
          parallel_for(
              G.offset[current_node], G.offset[current_node + 1],
              [&](size_t j) {
                NodeId ngb_node = G.E[j];
                if (sub_round < dist[ngb_node]) {
                  ST com = combine(label, ngb_node);
                  if (table.insert(com)) {
                    bag.insert(com);
                    if (next_dist[ngb_node] > sub_round) {
                      next_dist[ngb_node] =
                          sub_round;  // don't need atomic write_min,
                      // because all the sub_round are the same.
                      // write_min(&next_dist[ngb_node],(NodeId)sub_round,
                      // [&](NodeId a, NodeId b) { return a < b; });
                    }
                  }
                }
              },
              BLOCK_SIZE);
        },
        1);
    pack_timer.start();
    sub_n_front = bag.pack(make_slice(front));
    pack_timer.stop();
    parallel_for(
        0, sub_n_front,
        [&](size_t i) {
          list[list_offset + i] =
              make_tuple(get_second(front[i]), get_first(front[i]), sub_round);
        },
        BLOCK_SIZE);
    list_offset += sub_n_front;
    table.update_nelms();
  }
#if defined(DEBUG)
  cout << "table ne " << table.ne << endl;
#endif
  // cout << "resize cost " << resize_time << endl;
  table_m = table.ne;
  parallel_for(0, G.n, [&](size_t i) { dist[i] = next_dist[i]; });
}
void LE_LIST::le_list(double beta) {
  init();
  pack_timer.reset();
  table_timer.reset();
  resize_timer.reset();
  table_init_timer.reset();

  timer multi_timer;
  list_offset = 0;
  // ------------ begin first round --------------
  // #if defined(DEBUG)
  cout << "round 1 n_front 1" << endl;
  // #endif
  sequence<NodeId> first_map(G.n);
  BFS BFS_P(G);
  BFS_P.bfs(centers[0], dist);
  parallel_for(0, G.n, [&](size_t i) {
    NodeId v, d;
    if (dist[i] != UINT_N_MAX) {
      v = i;
      d = dist[i];
      next_dist[i] = d;
      ST com = combine(v, d);
      bag.insert(com);
    }
  });
  n_front = bag.pack(make_slice(front));
  parallel_for(
      0, n_front,
      [&](size_t i) {
        list[list_offset + i] =
            make_tuple(get_first(front[i]), 0, get_second(front[i]));
      },
      BLOCK_SIZE);
  list_offset += n_front;
  // ------------ end of first round--------------
  NodeId step_size = 1;
  NodeId finished = 1;
  NodeId current_round = 1;
  table_m = G.n >> 1;
  while (finished != G.n) {
    step_size = ceil(step_size * beta);
    current_round++;
    // if (current_round<10){
    //   table_m = G.n>>2;
    // }
    NodeId end = std::min(finished + step_size, (NodeId)G.n);
    start_idx = finished;
    n_front = end - start_idx;
    // #if defined(DEBUG)
    cout << "round " << current_round << " n_front " << n_front << endl;
    // #endif
    multi_timer.start();
    multi_search();
    multi_timer.stop();
    finished += n_front;
    table_m = ceil(table_m * beta);
  }
  // triple stores (target vertex, source vertex priority, distance)
  auto cmp = [](tuple<NodeId, NodeId, NodeId> a,
                tuple<NodeId, NodeId, NodeId> b) {
    if (get<0>(a) != get<0>(b)) {
      return get<0>(a) < get<0>(b);
    } else {
      return get<1>(a) < get<1>(b);
    }
  };
  timer sort_timer;
  sort_timer.start();
  sort_inplace(list.cut(0, list_offset), cmp);
  sort_timer.stop();
  // -------begin of filter-----------
  timer filt_timer;
  filt_timer.start();
  sequence<bool> pred(list_offset);
  parallel_for(0, list_offset, [&](size_t i) {
    if ((i == 0) || (get<0>(list[i]) != get<0>(list[i - 1]))) {
      NodeId label = get<0>(list[i]);
      pred[i] = true;
      NodeId min_dist = get<2>(list[i]);
      for (size_t j = i + 1;; j++) {
        if (j == list_offset || get<0>(list[j]) != label) {
          break;
        }
        if (get<2>(list[j]) < min_dist) {
          pred[j] = true;
          min_dist = get<2>(list[j]);
        } else {
          pred[j] = false;
        }
      }
    }
  });
  auto unique = parlay::pack(list.cut(0, list_offset), pred);
  list_offset = unique.size();
  filt_timer.stop();
  // -------end of filter-------------

  cout << "----------------" << endl;
  cout << "pred_time " << filt_timer.get_total() << endl;
  cout << "pack bag time " << pack_timer.get_total() << endl;
  cout << "compute table size time " << table_timer.get_total() << endl;
  cout << "resize table size time " << resize_timer.get_total() << endl;
  cout << "init table size time " << table_init_timer.get_total() << endl;
  cout << "multi_search time " << multi_timer.get_total() << endl;
  parallel_for(0, list_offset, [&](NodeId i) {
    list[i] = make_tuple(get<0>(unique[i]), centers[get<1>(unique[i])],
                         get<2>(unique[i]));
  });
}
