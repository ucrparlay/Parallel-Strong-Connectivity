#ifndef BFS_H
#define BFS_H


#include <algorithm>
#include <iostream>
#include <queue>  // std::queue

// #include <parlay/parallel.h>
// #include <parlay/random.h>
#include "../parlaylib/include/parlay/parallel.h"
#include "../parlaylib/include/parlay/random.h"

#include "get_time.hpp"
#include "graph.hpp"
#include "utilities.h"

using namespace std;

class BFS {
 private:
  Graph& graph;
  sequence<NodeId> front;
  sequence<EdgeId> degree;
  sequence<bool> edge_flag;
  sequence<NodeId> edge_data;
  sequence<bool> dense_front;
  sequence<bool> new_dense;
  bool sparse_pre;
  size_t n_front;
  size_t threshold;
  size_t n;
  size_t sparse_update(sequence<NodeId>& dst);
  size_t dense_update(sequence<NodeId>& dst);
  void front_sparse2dense();
  void front_dense2sparse();
  bool judge();
  EdgeId dense_sample(NodeId rand_seed);

 public:
  size_t num_round;
  void bfs_seq(NodeId source, sequence<NodeId>& dst);
  size_t bfs(NodeId source, sequence<NodeId>& dst);
  BFS() = delete;
  BFS(Graph& G) : graph(G) {
    n = graph.n;
    front = sequence<NodeId>(n);
    degree = sequence<EdgeId>(n + 1);
    edge_flag = sequence<bool>(graph.m);
    edge_data = sequence<NodeId>(graph.m);
    dense_front = sequence<bool>(n);
    new_dense = sequence<bool>(n);
    threshold = graph.m / 20;
  }
  // void swap_graph() {
  //   swap(graph.in_E, graph.E);
  //   swap(graph.in_offset, graph.offset);
  // }
};

void BFS::bfs_seq(NodeId source, sequence<NodeId>& dst) {
  queue<NodeId> my_queue;
  NodeId current_node;
  parallel_for(0, n, [&](long i) { dst[i] = UINT_N_MAX; });
  my_queue.push(source);
  dst[source] = 0;
  while (!my_queue.empty()) {
    current_node = my_queue.front();
    my_queue.pop();
    for (size_t i = graph.offset[current_node];
         i < graph.offset[current_node + 1]; i++) {
      if (dst[graph.E[i]] == UINT_N_MAX) {
        dst[graph.E[i]] = dst[current_node] + 1;
        my_queue.push(graph.E[i]);
      }
    }
  }
}

void BFS::front_sparse2dense() {
  parallel_for(0, n, [&](size_t i) { dense_front[i] = false; });
  parallel_for(0, n_front, [&](size_t i) { dense_front[front[i]] = true; });
}

void BFS::front_dense2sparse() {
  auto identity = delayed_seq<NodeId>(n, [&](size_t i) { return i; });
  pack_into_uninitialized(identity, dense_front, front);
}

bool BFS::judge() {
  long front_out_edges = 0;
  bool both_sparse_dense = false;
  if (!sparse_pre && n_front < (1 << 14)) {
    front_dense2sparse();
    both_sparse_dense = true;
  }
  if (sparse_pre || both_sparse_dense) {
    parallel_for(0, n_front, [&](size_t i) {
      NodeId node_i = front[i];
      degree[i] = graph.offset[node_i + 1] - graph.offset[node_i];
    });
    front_out_edges = parlay::reduce(degree.cut(0, n_front));
  } else {
    front_out_edges = dense_sample(hash32(num_round));
  }
  bool sparse_now = ((n_front + front_out_edges) < threshold);
  if (!both_sparse_dense && sparse_pre != sparse_now) {
    if (!sparse_now) {
      front_sparse2dense();
    } else {
      front_dense2sparse();
      parallel_for(0, n_front, [&](size_t i) {
        NodeId node_i = front[i];
        degree[i] = graph.offset[node_i + 1] - graph.offset[node_i];
      });
    }
  }
  return sparse_now;
}

size_t BFS::sparse_update(sequence<NodeId>& dst) {
  auto non_zeros = parlay::scan_inplace(degree.cut(0, n_front));
  degree[n_front] = non_zeros;
  parallel_for(
      0, n_front,
      [&](size_t i) {
        NodeId node = front[i];
        parallel_for(
            degree[i], degree[i + 1],
            [&](size_t j) {
              EdgeId offset = j - degree[i];
              NodeId ngb_node = graph.E[graph.offset[node] + offset];
              if (dst[ngb_node] == UINT_N_MAX) {
                edge_flag[j] = atomic_compare_and_swap(
                    &dst[ngb_node], UINT_N_MAX, dst[node] + 1);
                if (edge_flag[j]) edge_data[j] = ngb_node;
              } else {
                edge_flag[j] = false;
              }
            },
            2048);
      },
      1);  // may need to see the block size
#if defined(DEBUG)
  timer pack_timer;
  pack_timer.start();
#endif
  size_t n_front = pack_into_uninitialized(edge_data.cut(0, non_zeros),
                                           edge_flag.cut(0, non_zeros), front);
#if defined(DEBUG)
  cout << "sparse pack time " << pack_timer.stop() << endl;
#endif
  return n_front;
}

size_t BFS::dense_update(sequence<NodeId>& dst) {
  parallel_for(0, graph.n, [&](size_t i) {
    new_dense[i] = false;
    if (dst[i] == UINT_N_MAX) {
      if (graph.symmetric) {
        for (size_t j = graph.offset[i]; j < graph.offset[i + 1]; j++) {
          NodeId ngb_node = graph.E[j];
          if (dense_front[ngb_node]) {
            dst[i] = dst[ngb_node] + 1;
            new_dense[i] = true;
            break;
          }
        }
      } else {
        for (size_t j = graph.in_offset[i]; j < graph.in_offset[i + 1]; j++) {
          NodeId ngb_node = graph.in_E[j];
          if (dense_front[ngb_node]) {
            dst[i] = dst[ngb_node] + 1;
            new_dense[i] = true;
            break;
          }
        }
      }
    }
  });

  swap(dense_front, new_dense);
  return parlay::count(make_slice(dense_front), true);
}

EdgeId BFS::dense_sample(NodeId rand_seed) {
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

size_t BFS::bfs(NodeId source, sequence<NodeId>& dst) {
  // long bfs(Graph& graph, uintT source, uintT* dst){
  parallel_for(0, graph.n, [&](NodeId i) { dst[i] = UINT_N_MAX; });
  // parallel_for(long i = 0; i<graph.n; i++) dst[i] = UINT_E_MAX;
  front[0] = source;
  dst[source] = 0;
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
    // if (num_round == 7) sparse_now = false;
#if defined(DEBUG)
    if (sparse_now)
      cout << "sparse" << endl;
    else
      cout << "dense" << endl;
    cout << "judge time " << judge_timer.stop() << endl;
#endif
    if (sparse_now)
      n_front = sparse_update(dst);
    else
      n_front = dense_update(dst);
    sparse_pre = sparse_now;
#if defined(DEBUG)
    cout << "cost " << t.get_total() << endl;
#endif
  }
  return num_round;
}

#endif
