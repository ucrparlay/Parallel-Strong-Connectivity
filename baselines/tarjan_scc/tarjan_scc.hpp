#pragma once
#include <parlay/random.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <stack>

#include <get_time.hpp>
#include <graph.hpp>
#include <utilities.h>

using namespace std;

class TARJAN_SCC {
 private:
  Graph& G;
  stack<NodeId> sk;
  sequence<NodeId> low, dfn;
  size_t n, cnt, idx;
  void dfs(NodeId u);

 public:
  sequence<NodeId> scc;
  void tarjan();
  void clear();
  TARJAN_SCC() = delete;
  TARJAN_SCC(Graph& graph) : G(graph) {
    low = sequence<NodeId>(G.n);
    dfn = sequence<NodeId>(G.n);
    scc = sequence<NodeId>(G.n);
    n = G.n;
    cnt = 0;
    idx = 0;
  }
};

void TARJAN_SCC::dfs(NodeId u) {
  low[u] = dfn[u] = ++idx;
  sk.push(u);
  for (size_t i = G.offset[u]; i < G.offset[u + 1]; i++) {
    NodeId v = G.E[i];
    if (!dfn[v]) {
      dfs(v);
      low[u] = min(low[u], low[v]);
    } else if (!scc[v]) {
      low[u] = min(low[u], dfn[v]);
    }
  }
  if (low[u] == dfn[u]) {
    cnt++;
    while (1) {
      NodeId x = sk.top();
      sk.pop();
      scc[x] = cnt;
      if (x == u) break;
    }
  }
}

void TARJAN_SCC::clear() {
  for (size_t i = 0; i < n; i++) {
    dfn[i] = 0;
    scc[i] = 0;
  }
  idx = 0;
  cnt = 0;
  sk = stack<NodeId>();
}

void TARJAN_SCC::tarjan() {
  for (size_t i = 0; i < n; i++) {
    if (!dfn[i]) dfs(i);
  }
}