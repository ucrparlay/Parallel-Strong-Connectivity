#ifndef SCC_H
#define SCC_H

#include <math.h>
#include <parlay/random.h>
#include <stdio.h>

#include <algorithm>
#include <iostream>
#include <queue>  // std::queue

#include "get_time.hpp"
#include "graph.hpp"
#include "hash_bag.h"
#include "reach.hpp"
#include "resizable_table.h"
#include "utilities.h"

using namespace std;

constexpr size_t TOP_BIT = size_t(UINT_N_MAX) + 1;
constexpr size_t VAL_MASK = UINT_N_MAX;
size_t _min_bag_size_;
// size_t multi_tau;
// size_t per_core;
using label_type = size_t;

using K = NodeId;
using V = NodeId;
using T = std::tuple<K, V>;
struct hash_kv {
  NodeId operator()(const K& k) { return _hash(k); }
};

// hash32 is sufficient

class SCC {
 private:
  Graph& graph;
  gbbs::resizable_table<K, V, hash_kv> table_forw;
  gbbs::resizable_table<K, V, hash_kv> table_back;
  hashbag<NodeId> bag;
  sequence<NodeId> front;
  sequence<NodeId> substep_front;
  sequence<bool> bits;
  sequence<NodeId> centers;
  sequence<uint32_t> bag_stamp;
  uint32_t current_stamp;
  size_t n_front;
  size_t n;
  size_t label_offset;
  timer bits_clear_timer;
  bool edge_map(NodeId cur_node, NodeId ngb_node,
                gbbs::resizable_table<K, V, hash_kv>& table);
  bool edge_map_first(NodeId cur_node, NodeId ngb_node,
                      gbbs::resizable_table<K, V, hash_kv>& table);
  int multi_search(sequence<size_t>& labels,
                   gbbs::resizable_table<K, V, hash_kv>& table, bool forward,
                   bool local);
  int multi_search_safe(sequence<size_t>& labels,
                        gbbs::resizable_table<K, V, hash_kv>& table,
                        bool forward, bool local);
  void time_stamp_update() {
    current_stamp++;
    if (current_stamp == numeric_limits<uint32_t>::max() - 1) {
      parallel_for(0, n, [&](size_t i) {
        bag_stamp[i] =
            (bag_stamp[i] ==
             current_stamp);  // if current stamp will overflow, reset to 1
      });
      current_stamp = 1;
    }
  }

 public:
  size_t front_thresh;
  size_t num_round;
  void scc(sequence<size_t>& labels, double beta, bool local1, bool local2);
  SCC() = delete;
  SCC(Graph& G) : graph(G), bag(G.n, _min_bag_size_) {
    n = graph.n;
    num_round = 0;
    front = sequence<NodeId>(n);
    substep_front = sequence<NodeId>(n);
    bits = sequence<bool>(n);
    bag_stamp = sequence<uint32_t>(bag.get_bag_capacity());
    table_forw = gbbs::resizable_table<K, V, hash_kv>(
        n, {UINT_N_MAX, UINT_N_MAX}, hash_kv());
    table_back = gbbs::resizable_table<K, V, hash_kv>(
        n, {UINT_N_MAX, UINT_N_MAX}, hash_kv());
  }

#if defined(BREAKDOWN)
  timer multi_search_timer;
  timer pack_bag_timer;
  timer init_timer;
  timer first_round_timer;
  timer set_interset_timer;
  timer edge_map_timer;
#endif
  timer scc_timer;
  void timer_reset() {
#if defined(BREAKDOWN)
    multi_search_timer.reset();
    pack_bag_timer.reset();
    init_timer.reset();
    first_round_timer.reset();
    set_interset_timer.reset();
    edge_map_timer.reset();
#endif
    scc_timer.reset();
  }
#if defined(BREAKDOWN)
  void breakdown_report(int repeat) {
    cout << "average multi_search time "
         << multi_search_timer.get_total() / repeat << endl;
    cout << "average edge_map time " << edge_map_timer.get_total() / repeat
         << endl;
    cout << "average pack_bag_time " << pack_bag_timer.get_total() / repeat
         << endl;
    cout << "average init_time " << init_timer.get_total() / repeat << endl;
    cout << "average first_round_time "
         << first_round_timer.get_total() / repeat << endl;
    cout << "average set_interset_time "
         << set_interset_timer.get_total() / repeat << endl;
    cout << "number of threads " << num_workers() << endl;
  }
#endif
};

// size_t table_operate;
bool SCC::edge_map(NodeId cur_node, NodeId ngb_node,
                   gbbs::resizable_table<K, V, hash_kv>& table) {
  bool labels_changed = false;
  auto s_iter = table.get_iter(cur_node);
  // size_t num_probes = 0;
  // int local_table_operate = 0;
  if (s_iter.init()) {
    auto table_entry = s_iter.next();
    NodeId s_label = std::get<1>(table_entry);
    // auto results = table.insert(std::make_tuple(ngb_node, s_label));
    labels_changed |= table.insert(std::make_tuple(ngb_node, s_label));
    // labels_changed |= results.first;
    // num_probes+= results.second;
    if (table.is_full()) return false;
    // local_table_operate ++;

    while (s_iter.has_next()) {
      auto table_entry = s_iter.next();
      s_label = std::get<1>(table_entry);
      // auto results = table.insert(std::make_tuple(ngb_node, s_label));
      labels_changed |= table.insert(std::make_tuple(ngb_node, s_label));
      // labels_changed |= results.first;
      // num_probes += results.second;
      if (table.is_full()) return false;
      // local_table_operate ++;
    }
    // num_probes += s_iter.num_probes;
  }

  return labels_changed;
}

bool SCC::edge_map_first(NodeId s_label, NodeId ngb_node,
                         gbbs::resizable_table<K, V, hash_kv>& table) {
  return table.insert(std::make_tuple(ngb_node, s_label));
}

// template <class SI>
// inline ?
int SCC::multi_search(sequence<size_t>& labels,
                      gbbs::resizable_table<K, V, hash_kv>& table, bool forward,
                      bool local) {
  parallel_for(0, n_front, [&](size_t i) {
    table.insert(make_tuple(front[i], label_offset + i));
    substep_front[i] = front[i];
  });
  table.update_nelms();
  size_t sub_n_front = n_front;

  sequence<EdgeId>& offset = forward ? graph.offset : graph.in_offset;
  sequence<NodeId>& E = forward ? graph.E : graph.in_E;

  size_t round = 0;
  size_t queue_front = ceil(per_core * num_workers() / n_front);
  size_t queue_size = min(queue_front, tau);
  // cout << "QUEUE_SIZE " << queue_size << endl;
  int sub_round = 0;
  // sequence<size_t> probes;
  while (sub_n_front > 0) {
    sub_round++;
    time_stamp_update();
    round++;
#if defined(DEBUG)
    cout << "multi-search round " << round << " frontier " << sub_n_front
         << endl;
    cout << "resizable table m " << table.m << endl;
#endif
    // timer sub_search_timer;
    parallel_for(
        0, sub_n_front, [&](size_t i) { bits[substep_front[i]] = 0; }, 2048);
    // sub_search_timer.start();

#if defined(BREAKDOWN)
    edge_map_timer.start();
#endif
    // if (sub_round == 1){
    //   probes = sequence<size_t>(n_front);
    // }
    parallel_for(
        0, sub_n_front,
        [&](size_t i) {
          NodeId node = substep_front[i];
          EdgeId degree = offset[node + 1] - offset[node];
          // size_t num_probes = 0;
          EdgeId edge_map_cnt = 0;
          if (local && (degree < queue_size) && (edge_map_cnt < queue_size)) {
            NodeId Q[queue_size];
            int head = 0, tail = 0;
            Q[tail] = node;
            tail++;
            while (head < tail && (edge_map_cnt < queue_size)) {
              NodeId u = Q[head];
              EdgeId deg = offset[u + 1] - offset[u];
              if (deg >= queue_size) {
                break;
              }
              head++;
              for (EdgeId j = offset[u]; j < offset[u + 1]; j++) {
                NodeId v = E[j];
                if (!(labels[v] & TOP_BIT) && (labels[v] == labels[u])) {
                  edge_map_cnt++;
                  // auto results = edge_map(node, v, table);
                  // num_probes += results.second;
                  bool edge_map_success =
                      (sub_round == 1 && local)
                          ? edge_map_first(label_offset + i, v, table)
                          : edge_map(node, v, table);
                  // if (sub_round == 1){
                  //   edge_map_first(label_offset+i, v, table);
                  // }
                  // if (edge_map(node,v,table)) {
                  if (edge_map_success) {
                    // if (tail < (int)queue_size  && edge_map_cnt < 300) {
                    if (edge_map_cnt < queue_size) {
                      Q[tail] = v;
                      tail++;
                    } else {
                      if (compare_and_swap(&bits[v], false, true)) {
                        bag.insert(v, make_slice(bag_stamp), current_stamp);
                      }
                    }
                  }
                }
              }
            }

            if (head < tail) {
              for (int j = head; j < tail; j++) {
                NodeId u = Q[j];
                if (compare_and_swap(&bits[u], false, true)) {
                  bag.insert(u, make_slice(bag_stamp), current_stamp);
                }
              }
            }

          } else if (degree > 0) {
            parallel_for(
                0, degree,
                [&](size_t j) {
                  NodeId ngb_node = E[offset[node] + j];
                  if (!(labels[ngb_node] & TOP_BIT) &&
                      (labels[ngb_node] == labels[node])) {
                    bool edge_map_success =
                        (sub_round == 1 && local)
                            ? edge_map_first(label_offset + i, ngb_node, table)
                            : edge_map(node, ngb_node, table);
                    // if (edge_map(node, ngb_node, table)) {
                    if (edge_map_success) {
                      if (compare_and_swap(&bits[ngb_node], false, true)) {
                        bag.insert(ngb_node, make_slice(bag_stamp),
                                   current_stamp);
                      }
                    }
                  }
                },
                1024);
          }
        },
        1);
    if (table.is_full()) {
      return -1;
    }

#if defined(BREAKDOWN)
    pack_bag_timer.start();
#endif
    sub_n_front = bag.pack(make_slice(substep_front), make_slice(bag_stamp),
                           current_stamp);
#if defined(BREAKDOWN)
    pack_bag_timer.stop();
    edge_map_timer.stop();
    // cout << "multi_round time " << sub_search_timer.stop() << endl;
#endif

    table.update_nelms();
#if defined(DEBUG)
    cout << "table.ne = " << table.ne << endl;
#endif
  }
  return round;
}

int SCC::multi_search_safe(sequence<size_t>& labels,
                           gbbs::resizable_table<K, V, hash_kv>& table,
                           bool forward, bool local) {
  int round;
  while ((round = multi_search(labels, table, forward, local)) == -1) {
    cout << "trigger table resize" << endl;
    parallel_for(0, graph.n, [&](size_t i) { bits[i] = 0; });
    table.double_size();
  }
  return round;
}

void SCC::scc(sequence<size_t>& labels, double beta, bool local_reach,
              bool local_scc) {
#if defined(BREAKDOWN)
  init_timer.start();
#endif
  scc_timer.reset();
  scc_timer.start();
  current_stamp = 1;

  sequence<bool> dist_1(n);
  sequence<bool> dist_2(n);

  sequence<NodeId> vertices = sequence<NodeId>(n);
  parallel_for(0, n, [&](size_t i) {
    labels[i] = 0;
    bits[i] = false;
    vertices[i] = i;
  });
  parallel_for(0, bag.get_bag_capacity(), [&](size_t i) { bag_stamp[i] = 0; });
  // parallel_for(0, n, [&](size_t i){vertices[i]=i;});

  auto NON_ZEROS = parlay::filter(vertices, [&](NodeId v) {
    return ((graph.offset[v + 1] - graph.offset[v]) != 0) &&
           ((graph.in_offset[v + 1] - graph.in_offset[v]) != 0);
  });
  auto ZEROS = parlay::filter(vertices, [&](NodeId v) {
    return ((graph.offset[v + 1] - graph.offset[v]) == 0) ||
           ((graph.in_offset[v + 1] - graph.in_offset[v]) == 0);
  });
  auto P = parlay::random_shuffle(NON_ZEROS);
  parallel_for(0, ZEROS.size(),
               [&](NodeId i) { labels[ZEROS[i]] = 1 + (i | TOP_BIT); });
#if defined(BREAKDOWN)
  init_timer.stop();
#endif

  cout << "------------------------------------" << endl;
  cout << "initial time " << scc_timer.stop() << endl;
  cout << "Filtered: " << ZEROS.size()
       << " vertices. Num remaining = " << NON_ZEROS.size() << endl;
  scc_timer.start();
  NodeId step_size = 1, cur_offset = 0, finished = 0, cur_round = 0;
  label_offset = ZEROS.size() + 1;

  // ----------------first round, for the BIG SCC -------------------------
  long bfs_forward_depth, bfs_backward_depth;
  NodeId source = P[0];
  // BFS BFS_P(graph);
  cout << "source " << source << endl;
#if defined(BREAKDOWN)
  first_round_timer.start();
#endif
  REACH REACH_P(graph);
  // bool no_dense = false;
  REACH_P.reach(source, dist_1, local_reach);
  bfs_forward_depth = REACH_P.num_round;

  REACH_P.swap_graph();
  REACH_P.reach(source, dist_2, local_reach);

  // bfs_backward_time = first_round_timer.stop();
  // first_round_timer.start();
  bfs_backward_depth = REACH_P.num_round;
  cout << "first round, "
       << "source " << source << " bfs_forward_depth " << bfs_forward_depth
       << " bfs_backward_depth " << bfs_backward_depth << endl;
  REACH_P.swap_graph();
  NodeId label = label_offset;
  parallel_for(0, P.size(), [&](size_t i) {
    if ((dist_1[P[i]] != false) && (dist_2[P[i]] != false)) {
      labels[P[i]] = label | TOP_BIT;
    } else if ((dist_1[P[i]] != false) || (dist_2[P[i]] != false)) {
      labels[P[i]] = label;
    }
  });

  centers = parlay::filter(P, [&](NodeId v) { return !(labels[v] & TOP_BIT); });
#if defined(BREAKDOWN)
  first_round_timer.stop();
#endif
#if defined(DEBUG)
  cout << "First Round Time " << scc_timer.stop() << endl;
  scc_timer.start();
#endif
  label_offset++;
  cout << "After first round, |FRONT| = " << centers.size()
       << " vertices remain. Total done = " << P.size() - centers.size()
       << endl;
  // cout << "centers: ";
  // for (int i = 0; i<10; i++){
  //     cout << centers[i] << " ";
  // }
  // cout << endl;

  timer t_search;
  timer t_interset;
  timer t_round;
  double forward_time = 0;
  double backward_time = 0;
  // double intersect_time=0;
  double round_time = 0;
  NodeId n_remaining = centers.size();

  NodeId in_table_ne = 1;
  NodeId out_table_ne = 1;
  while (finished != n_remaining) {
    t_round.start();
    NodeId end = std::min(cur_offset + step_size, n_remaining);
    NodeId vs_size = end - cur_offset;
    finished += vs_size;

    step_size = ceil(step_size * beta);
    cur_round++;
    cout << "round = " << cur_round << " ";
    // front = parlay::filter(centers.cut(cur_offset, end), [&](NodeId v){return
    // !(labels[v] & TOP_BIT);});
    auto pred = delayed_seq<bool>(vs_size, [&](NodeId i) {
      return !(labels[centers[cur_offset + i]] & TOP_BIT);
    });
    n_front = pack_into_uninitialized(centers.cut(cur_offset, end), pred,
                                      make_slice(front));
    cur_offset = end;
    cout << "n_front = " << n_front << ", origin was " << vs_size
         << " Total vertices remaining = " << n_remaining - finished << endl;
    if (n_front == 0) continue;
    cout << "QUEUE_SIZE "
         << min((size_t)ceil(per_core * num_workers() / n_front), (size_t)tau)
         << endl;
    t_search.start();
#if defined(BREAKDOWN)
    multi_search_timer.start();
#endif
    T empty = std::make_tuple(UINT_N_MAX, UINT_N_MAX);
    // size_t out_table_m = max((size_t)min((NodeId)ceil(0.35*n_remaining) ,
    // (NodeId)6000000), (size_t)(beta)*out_table_ne);
    size_t out_table_m =
        2 * max((size_t)min((NodeId)ceil(0.3 * n_remaining), (NodeId)6000000),
                (size_t)(beta)*out_table_ne);
    table_forw =
        gbbs::resizable_table<K, V, hash_kv>(out_table_m, empty, hash_kv());
    int out_depth = multi_search_safe(labels, table_forw, true, local_scc);
    forward_time = t_search.stop();
    // #if defined(BREAKDOWN)
    //     multi_search_timer.stop();
    // #endif

    // #if defined(BREAKDOWN)
    //     multi_search_timer.start();
    // #endif
    t_search.start();
    // size_t in_table_m = max((size_t)min((NodeId)ceil(0.35*n_remaining),
    // (NodeId)6000000), (size_t)(beta)*in_table_ne);
    size_t in_table_m =
        2 * max((size_t)min((NodeId)ceil(0.3 * n_remaining), (NodeId)6000000),
                (size_t)(beta)*in_table_ne);
    table_back =
        gbbs::resizable_table<K, V, hash_kv>(in_table_m, empty, hash_kv());
    int in_depth = multi_search_safe(labels, table_back, false, local_scc);
    backward_time = t_search.stop();
#if defined(BREAKDOWN)
    multi_search_timer.stop();
#endif

    label_offset += n_front;
    std::cout << "in_table, m = " << table_back.m << " ne = " << table_back.ne
              << "\n";
    std::cout << "out_table, m = " << table_forw.m << " ne = " << table_forw.ne
              << "\n";
    cout << "In search time " << backward_time << endl;
    cout << "Out search time " << forward_time << endl;
    cout << "In search depth " << in_depth << endl;
    cout << "Out search depth " << out_depth << endl;

    auto& smaller_t = (table_forw.m <= table_back.m) ? table_forw : table_back;
    auto& larger_t = (table_forw.m > table_back.m) ? table_forw : table_back;

#if defined(BREAKDOWN)
    set_interset_timer.start();
#endif
    // intersect the tables
    auto map_f = [&](const std::tuple<K, V>& kev) {
      NodeId v = std::get<0>(kev);
      size_t label = std::get<1>(kev);
      if (larger_t.contains(v, label)) {
        // in 'label' scc
        // Max visitor from this StronglyConnectedComponents acquires it.
        write_max(&labels[v], label | TOP_BIT,
                  [&](size_t a, size_t b) { return a < b; });
      } else {
        write_max(&labels[v], label, [&](size_t a, size_t b) { return a < b; });
      }
    };
    smaller_t.map(map_f);

    // set the subproblems
    auto sp_map = [&](const std::tuple<K, V>& kev) {
      NodeId v = std::get<0>(kev);
      size_t label = std::get<1>(kev);
      // note that if v is already in an StronglyConnectedComponents (from (1)),
      // the pbbslib::write_max will read, compare and fail, as the top bit is
      // already set.
      write_max(&labels[v], label, [&](size_t a, size_t b) { return a < b; });
    };
    larger_t.map(sp_map);

#if defined(BREAKDOWN)
    set_interset_timer.stop();
#endif

    out_table_ne = table_forw.ne;
    in_table_ne = table_back.ne;

    // in_table.del();
    // out_table.del();
    round_time = t_round.stop();
    cout << "Round time " << round_time << endl;
  }

  parallel_for(0, graph.n,
               [&](NodeId i) { labels[i] = (labels[i] & VAL_MASK) - 1; });
  scc_timer.stop();
  cout << "scc_time " << scc_timer.get_total() << endl;
  cout << endl;
}

#endif