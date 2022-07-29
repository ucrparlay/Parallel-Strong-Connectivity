#pragma once
#include "get_time.hpp"
#include "graph.hpp"
#include "ldd.hpp"
#include "resizable_table.h"
#include "union_find_rules.h"
using namespace std;

struct hash_kv {
  uint32_t operator()(const NodeId &k) { return parlay::hash32(k); }
};

NodeId get_max_label(sequence<NodeId> &label) {
  constexpr int NUM_SAMPLES = 1e4;
  size_t n = label.size();
  size_t seed = _hash(n + 1);
  sequence<NodeId> samples(NUM_SAMPLES);
  for (size_t i = 0; i < NUM_SAMPLES; i++) {
    samples[i] = label[_hash(i + seed) % n];
  }
  sort(samples.begin(), samples.end());
  NodeId max_label = 0, max_cnt = 0;
  for (size_t i = 0; i < NUM_SAMPLES;) {
    size_t j = 1;
    while (i + j < NUM_SAMPLES && samples[i + j] == samples[i]) {
      j++;
    }
    if (j > max_cnt) {
      max_cnt = j;
      max_label = samples[i];
    }
    i += j;
  }
  return max_label;
}

tuple<sequence<NodeId>, sequence<pair<NodeId, NodeId>>> Connectivity(
    const Graph &G, double beta,
    function<bool(NodeId, NodeId)> pred = [](NodeId, NodeId) { return true; },
    bool spanning_forest = false) {
  timer t;
  timer ti;
  ti.start();
  LDD ldd_solver(G, pred);
  ti.stop();
#if defined(DEBUG)
  cout << "initialize ldd_solver time " << ti.get_total() << endl;
#endif
  t.start();
  // auto [label, parent] = ldd_solver.ldd(beta, spanning_forest);
  sequence<NodeId> label;
  sequence<NodeId> parent;
  tie(label, parent) = ldd_solver.ldd(beta, spanning_forest);
  // cout << "ldd time " << t.get_total() << " " << '\n';

  timer t0;
  t0.start();
  NodeId max_label = get_max_label(label);
  t0.stop();
  auto find = gbbs::find_variants::find_compress;
  auto splice = gbbs::splice_variants::split_atomic_one;
  auto unite =
      gbbs::unite_variants::UniteRemCAS<decltype(splice), decltype(find),
                                        find_atomic_halve>(find, splice);
  timer t2;
  t2.start();
  // TODO: try different hash tables

  auto table = gbbs::resizable_table<NodeId, NodeId, hash_kv>();
  if (spanning_forest) {
    table = gbbs::resizable_table<NodeId, NodeId, hash_kv>(
        G.n, {UINT_N_MAX, UINT_N_MAX}, hash_kv());
  }
  parallel_for(
      0, G.n,
      [&](NodeId i) {
        if (find(label[i], label) != find(max_label, label)) {
          parallel_for(
              G.offset[i], G.offset[i + 1],
              [&](size_t j) {
                NodeId v = G.E[j];
                if (pred(i, v)) {
                  if (unite(i, v, label) != UINT_N_MAX) {
                    if (spanning_forest) {
                      table.insert({i, v});
                    }
                  }
                }
              },
              BLOCK_SIZE);
        }
      },
      512);

  parallel_for(0, G.n, [&](size_t i) { label[i] = find(label[i], label); });
  t2.stop();
  // auto ret = remove_duplicates_ordered(label, less<NodeId>());
  // printf("# connected components: %zu\n", ret.size());
  sequence<pair<NodeId, NodeId>> tree_edges;
  if (spanning_forest) {
    auto ldd_edges = tabulate(G.n, [&](size_t i) {
      return static_cast<NodeId>(parent[i] == i ? 0 : 1);
    });
    auto union_find_edges = table.entries();
    size_t v1 = scan_inplace(ldd_edges);
    size_t v2 = union_find_edges.size();
    tree_edges = sequence<pair<NodeId, NodeId>>(v1 + v2);
    parallel_for(0, ldd_edges.size(), [&](NodeId i) {
      if (parent[i] != i) {
        tree_edges[ldd_edges[i]] = {i, parent[i]};
      }
    });
    parallel_for(0, v2, [&](size_t i) {
      tree_edges[i + v1] = {get<0>(union_find_edges[i]),
                            get<1>(union_find_edges[i])};
    });
  }
  return {label, tree_edges};
}
