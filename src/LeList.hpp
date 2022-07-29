#pragma once
#include <assert.h>
#include <parlay/parallel.h>
#include <parlay/random.h>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <queue>
#include <vector>

#include "get_time.hpp"
#include "graph.hpp"
#include "hash_bag.h"
#include "resizable_table.h"
using namespace std;

using K = NodeId;
using V = NodeId;
using T = std::tuple<K, V>;
struct hash_kv {
  NodeId operator()(const K& k) { return _hash(k); }
};

class LE_LIST {
 private:
  const Graph& G;
  sequence<NodeId> centers;
  hashbag<NodeId> bag;
  sequence<NodeId> front;
  sequence<NodeId> priority;
  NodeId n_front;
  NodeId start_idx;
  void multi_search(sequence<tuple<NodeId, NodeId>>& dist);

 public:
  size_t list_offset;
  sequence<tuple<NodeId, NodeId, NodeId>> list;
  void seq_le_list();
  void le_list(double beta);
  size_t num_round;
  LE_LIST() = delete;
  LE_LIST(Graph& _G) : G(_G), bag(G.n) {
    n_front = 0;
    size_t m = 2 * ceil(log(G.n)) * G.n;  // log_e
    cout << "list size initialize to be " << m << endl;
    list = sequence<tuple<NodeId, NodeId, NodeId>>(m);
    centers = parlay::random_permutation((NodeId)G.n);
    priority = sequence<NodeId>(G.n);
    front = sequence<NodeId>(G.n);
    parallel_for(0, G.n, [&](size_t i) { priority[centers[i]] = i; });
  }
};

void LE_LIST::seq_le_list() {
  sequence<NodeId> dist;
  dist = sequence<NodeId>(G.n, UINT_N_MAX);
  for (size_t i = 0; i < G.n; i++) {
    dist[i] = UINT_N_MAX;
  }
  size_t m = 2 * ceil(log(G.n)) * G.n;  // log_e
  list_offset = 0;
  for (NodeId i = 0; i < G.n; i++) {
    queue<NodeId> my_queue;
    // cout << "push " << centers[i] << endl;
    my_queue.push(centers[i]);
    dist[centers[i]] = 0;
    while (!my_queue.empty()) {
      NodeId current_node;
      current_node = my_queue.front();
      my_queue.pop();
      for (size_t j = G.offset[current_node]; j < G.offset[current_node + 1];
           j++) {
        NodeId ngb_node = G.E[j];
        if (dist[current_node] + 1 < dist[ngb_node]) {
          dist[ngb_node] = dist[current_node] + 1;
          if (centers[i] == 81301358 && ngb_node < 8) {
            cout << "add " << ngb_node << " " << centers[i] << " "
                 << dist[ngb_node] << " " << list_offset << endl;
          }
          assert(list_offset < m);
          assert(list_offset <= numeric_limits<size_t>::max());
          list[list_offset] = make_tuple(ngb_node, centers[i], dist[ngb_node]);
          list_offset++;
          if (centers[i] == 81301358 && ngb_node < 8) {
            cout << "offset " << list_offset << endl;
          }
          my_queue.push(ngb_node);
        }
      }
    }
  }
  cout << "list_offset " << list_offset << endl;
  auto cmp = [&](tuple<NodeId, NodeId, NodeId> a,
                 tuple<NodeId, NodeId, NodeId> b) {
    if (get<0>(a) != get<0>(b)) {
      return get<0>(a) < get<0>(b);
    } else {
      return priority[get<1>(a)] < priority[get<1>(b)];
    }
  };
  std::sort(list.begin(), list.begin() + list_offset, cmp);
}

void LE_LIST::multi_search(sequence<tuple<NodeId, NodeId>>& dist) {
  parallel_for(
      0, n_front,
      [&](size_t i) {
        front[i] = centers[i + start_idx];
        dist[front[i]] = make_tuple(0, i + start_idx);
      },
      BLOCK_SIZE);
  size_t sub_n_front = n_front;
#if defined(DEBUG)
  size_t sub_round = 0;
#endif
  auto cmp = [&](tuple<NodeId, NodeId> a, tuple<NodeId, NodeId> b) {
    if (get<0>(a) != get<0>(b)) {
      return get<0>(a) < get<0>(b);
    } else {
      return get<1>(a) < get<1>(b);
    }
  };
  while (sub_n_front > 0) {
#if defined(DEBUG)
    cout << "sub-round " << sub_round++ << " sub_n_front " << sub_n_front
         << endl;
#endif
    parallel_for(
        0, sub_n_front,
        [&](size_t i) {
          NodeId current_node = front[i];
          EdgeId degree = G.offset[current_node + 1] - G.offset[current_node];
          parallel_for(
              0, degree,
              [&](size_t j) {
                NodeId ngb_node = G.E[G.offset[current_node] + j];
                if (get<0>(dist[current_node]) + 1 <= get<0>(dist[ngb_node])) {
                  if (write_min(&dist[ngb_node],
                                make_tuple(get<0>(dist[current_node]) + 1,
                                           get<1>(dist[current_node])),
                                cmp)) {
                    bag.insert(ngb_node);
                  }
                }
              },
              BLOCK_SIZE);
        },
        1);
    sub_n_front = bag.pack(make_slice(front));
    parallel_for(
        0, sub_n_front,
        [&](size_t i) {
          list[list_offset + i] = make_tuple(front[i], get<1>(dist[front[i]]),
                                             get<0>(dist[front[i]]));
        },
        BLOCK_SIZE);
    list_offset += sub_n_front;
  }
}
void LE_LIST::le_list(double beta) {
  sequence<tuple<NodeId, NodeId>> dist = sequence<tuple<NodeId, NodeId>>(
      G.n);  // tuple stores (dist,source)  source stores the priority of that
             // source node
  parallel_for(0, G.n,
               [&](size_t i) { dist[i] = make_tuple(UINT_N_MAX, UINT_E_MAX); });
  list_offset = 0;
  NodeId step_size = 1, finished = 0, current_round = 0;
  n_front = 0;
  while (finished != G.n) {
    current_round++;
    NodeId end = std::min(finished + step_size, (NodeId)G.n);
    start_idx = finished;
    n_front = end - start_idx;
#if defined(DEBUG)
    cout << "round " << current_round << " n_front " << n_front << endl;
#endif
    finished += n_front;
    step_size = ceil(step_size * beta);
    multi_search(dist);
  }
  auto cmp = [](tuple<NodeId, NodeId, NodeId> a,
                tuple<NodeId, NodeId, NodeId> b) {
    if (get<0>(a) != get<0>(b)) {
      return get<0>(a) < get<0>(b);
    } else {
      return get<1>(a) < get<1>(b);
    }
  };
  sort_inplace(list.cut(0, list_offset), cmp);
  auto pred = delayed_seq<bool>(list_offset, [&](size_t i) {
    return (i == 0) || (list[i - 1] != list[i]);
  });
  auto unique = parlay::pack(list.cut(0, list_offset), pred);
  list_offset = unique.size();
  parallel_for(0, list_offset, [&](NodeId i) {
    list[i] = make_tuple(get<0>(unique[i]), centers[get<1>(unique[i])],
                         get<2>(unique[i]));
  });
}
