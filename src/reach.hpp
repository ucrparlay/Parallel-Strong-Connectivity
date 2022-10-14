#pragma once
#ifndef REACH_H
#define REACH_H

#include <parlay/parallel.h>
#include <parlay/random.h>

#include <algorithm>
#include <iostream>
#include <queue>  // std::queue

#include "get_time.hpp"
#include "graph.hpp"
#include "hash_bag.h"
#include "utilities.h"

using namespace std;

size_t tau = 1024;
size_t per_core = 10000;

class REACH {
 private:
  Graph& graph;
  sequence<NodeId> front;
  hashbag<NodeId> bag;
  sequence<bool> dense_front;
  sequence<bool> new_dense;
  bool sparse_pre;
  size_t n_front;
  size_t threshold;
  size_t n;
  size_t sparse_update(sequence<bool>& dst, bool local);
  size_t dense_update(sequence<bool>& dst);
  void front_sparse2dense();
  void front_dense2sparse();
  bool judge();
  EdgeId dense_sample(NodeId rand_seed);

 public:
  size_t num_round;
  void reach_seq(NodeId source, sequence<bool>& dst);
  void reach(NodeId source, sequence<bool>& dst, bool local);
  REACH() = delete;
  REACH(Graph& G) : graph(G), bag(G.n) {
    n = graph.n;
    front = sequence<NodeId>(n);
    dense_front = sequence<bool>(n);
    new_dense = sequence<bool>(n);
    threshold = graph.m / 20;
  }
  void swap_graph() {
    swap(graph.in_E, graph.E);
    swap(graph.in_offset, graph.offset);
  }
};

void REACH::reach_seq(NodeId source, sequence<bool>& dst) {
  queue<NodeId> my_queue;
  NodeId current_node;
  parallel_for(0, n, [&](long i) { dst[i] = false; });
  my_queue.push(source);
  dst[source] = true;
  while (!my_queue.empty()) {
    current_node = my_queue.front();
    my_queue.pop();
    for (size_t i = graph.offset[current_node];
         i < graph.offset[current_node + 1]; i++) {
      if (dst[graph.E[i]] == false) {
        dst[graph.E[i]] = true;
        my_queue.push(graph.E[i]);
      }
    }
  }
}

void REACH::front_sparse2dense() {
  parallel_for(0, n, [&](size_t i) { dense_front[i] = false; });
  parallel_for(0, n_front, [&](size_t i) { dense_front[front[i]] = true; });
}

void REACH::front_dense2sparse() {
  auto identity = delayed_seq<NodeId>(n, [&](size_t i) { return i; });
  pack_into_uninitialized(identity, dense_front, front);
}

bool REACH::judge() {
  size_t front_out_edges = 0;
  bool both_sparse_dense = false;
  // gbbs: if dense && n_front > numVertices/10  -> dense
  if (!sparse_pre && n_front < (1 << 14)) {
    front_dense2sparse();
    both_sparse_dense = true;
  }
  if (sparse_pre || both_sparse_dense) {
    auto degree_seq = delayed_seq<size_t>(n_front, [&](size_t i) {
      NodeId u = front[i];
      return graph.offset[u + 1] - graph.offset[u];
    });
    front_out_edges = reduce(degree_seq);
  } else {
    front_out_edges = dense_sample(hash32(num_round));
  }
  bool sparse_now = ((n_front + front_out_edges) < threshold);
  if (!both_sparse_dense && sparse_pre != sparse_now) {
    if (!sparse_now) {
      front_sparse2dense();
    } else {
      front_dense2sparse();
    }
  }
  return sparse_now;
}

size_t REACH::sparse_update(sequence<bool>& dst, bool local) {
  size_t queue_front = ceil(per_core*num_workers()/ n_front);
  size_t queue_size=min(queue_front, tau);
  parallel_for(
      0, n_front,
      [&](size_t i) {
        NodeId node = front[i];
        size_t edge_explored_cnt = 0;
        size_t deg_f = graph.offset[node + 1] - graph.offset[node];
        // size_t block_size = 2048;
        if (local && (deg_f < queue_size) && (deg_f > 0) && (edge_explored_cnt < queue_size)) {
          NodeId local_queue[queue_size];
          size_t head = 0, tail = 0;
          local_queue[tail++] = node;
          while (head < tail && tail < queue_size) {
            NodeId u = local_queue[head++];
            size_t deg_u = graph.offset[u + 1] - graph.offset[u];
            if (deg_u > queue_size ) {
              bag.insert(u);
              break;
            } else {
              for (size_t j = graph.offset[u]; j < graph.offset[u + 1]; j++) {
                NodeId v = graph.E[j];
                if (dst[v] == false) {
                  if (compare_and_swap(&dst[v], false, true)) {
                    edge_explored_cnt ++;
                    if (tail < queue_size && edge_explored_cnt < queue_size) {
                      local_queue[tail++] = v;
                    } else {
                      bag.insert(v);
                    }
                  }
                }
              }
            }
          }
          for (size_t j = head; j < tail; j++) {
            bag.insert(local_queue[j]);
          }
        } else if (deg_f > 0) {
          parallel_for(
              graph.offset[node], graph.offset[node + 1],
              [&](size_t j) {
                NodeId v = graph.E[j];
                if (dst[v] == false) {
                  if (compare_and_swap(&dst[v], false, true)) {
                    bag.insert(v);
                  }
                }
              },
              2048);
        }
      },
      1);
  return bag.pack(make_slice(front));
}

size_t REACH::dense_update(sequence<bool>& dst) {
  parallel_for(0, graph.n, [&](size_t i) {
    new_dense[i] = false;
    if (dst[i] == false) {
      for (size_t j = graph.in_offset[i]; j < graph.in_offset[i + 1]; j++) {
        NodeId ngb_node = graph.in_E[j];
        if (dense_front[ngb_node]) {
          dst[i] = true;
          new_dense[i] = true;
          break;
        }
      }
    }
  });

  swap(dense_front, new_dense);
  return parlay::count(make_slice(dense_front), true);
}

EdgeId REACH::dense_sample(NodeId rand_seed) {
  int count = 0;
  EdgeId out_edges = 0;
  NodeId index;
  int i = 0;
  while (count < 50) {
    i++;
    index = _hash(rand_seed + i) % graph.n;
    if (dense_front[index]) {
      count++;
      out_edges += graph.offset[index + 1] - graph.offset[index];
    }
  }
  return n_front * (out_edges / count);
}

void REACH::reach(NodeId source, sequence<bool>& dst, bool local) {
  cout << "reach_tau " << tau << endl;
  parallel_for(0, graph.n, [&](NodeId i) { dst[i] = false; });
  front[0] = source;
  dst[source] = true;
  n_front = 1;
  sparse_pre = true;       // whether previous round is sparse
  bool sparse_now = true;  // whether should use sparse in this round
  num_round = 0;
  while (n_front > 0) {
    num_round++;
#if defined(DEBUG)
    timer t;
    t.start();
    cout << "round " << num_round << " front " << n_front << endl;
    timer judge_timer;
    judge_timer.start();
#endif
    sparse_now = judge();
#if defined(DEBUG)
    if (sparse_now)
      cout << "sparse" << endl;
    else
      cout << "dense" << endl;
    cout << "judge time " << judge_timer.stop() << endl;
#endif
    if (sparse_now)
      n_front = sparse_update(dst, local);
    else
      n_front = dense_update(dst);
    sparse_pre = sparse_now;
#if defined(DEBUG)
    cout << "cost " << t.get_total() << endl;
#endif
  }
}

#endif
