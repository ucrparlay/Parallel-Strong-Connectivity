#include "scc.hpp"

#include <algorithm>  // std::max
#include <vector>

#include "get_time.hpp"
#include "parseCommandLine.hpp"

using namespace std;



void SCC_status(sequence<size_t> label, size_t n) {
  // uintT* flag = new uintT[n+1];
  sequence<NodeId> flag(n + 1);
  // uintT MASK = UINT_T_MAX >>1;
  NodeId MASK = UINT_N_MAX >> 1;
  parallel_for(0, n + 1, [&](NodeId i) { flag[i] = 0; });
  for (size_t i = 0; i < n; i++) {
    flag[label[i] & MASK]++;
  }
  // cout << "n_scc = " << sequence::sum(flag, n, [&](NodeId label_size){return
  // label_size!=0;}) << endl;
  cout << "n_scc = " << parlay::count_if(flag, [&](NodeId label_size) {
    return label_size != 0;
  }) << endl;
  cout << "Largest StronglyConnectedComponents has "
       << parlay::reduce(flag, maxm<NodeId>()) << endl;
}

int main(int argc, char** argv) {
  CommandLine P(argc, argv);
  if (argc < 2) {
    cout << "usage: ./scc fileName" << endl;
    return 0;
  }
  char* fileName = argv[1];
  timer t1;
  t1.start();
  double beta = P.getOptionDouble("-beta", 1.5);
  cout << "--beta " << beta << "--" << endl;
  Graph graph = P.getOption("-large")? read_large_graph(fileName):read_graph(fileName);
  bool local_reach = P.getOption("-local_reach");
  bool local_scc = P.getOption("-local_scc");
  _min_bag_size_ = P.getOptionInt("-lambda", 1 << 10);
  tau = P.getOptionInt("-tau", 512);
  cout << "tau " << tau  << endl;
  t1.stop();
  cout << "input graph: " << fileName << endl;
  cout << "number of vertices: " << graph.n << ", number of edges: " << graph.m << endl;
  cout << "enable VGC1: " << local_reach << ", enable VGC_multi: " << local_scc << endl;

  // size_t* label = new size_t[graph.n];
  sequence<size_t> label = sequence<size_t>(graph.n);
  int repeat = P.getOptionInt("-t", (int)10);
  timer t;
  SCC SCC_P(graph);
  SCC_P.scc(label, beta, local_reach, local_scc);

  SCC_P.timer_reset();
  double scc_cost;
  for (int i = 0; i < repeat; i++) {
    t.start();
    SCC_P.scc(label, beta, local_reach, local_scc);
    scc_cost = t.stop();
    cout << "scc cost: " << scc_cost << endl;
  }
  cout << "average cost " << t.get_total() / repeat << endl;
  SCC_P.breakdown_report(repeat);
  if (P.getOption("-status")) SCC_status(label, graph.n);

  return 0;
}