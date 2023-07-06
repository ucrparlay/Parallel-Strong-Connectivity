#include "tarjan_scc.hpp"

#include <algorithm>  // std::max

#include "get_time.hpp"
#include "parseCommandLine.hpp"

using namespace std;

void SCC_status(sequence<NodeId> label, size_t n) {
  sequence<NodeId> flag(n + 1);
  parallel_for(0, n + 1, [&](NodeId i) { flag[i] = 0; });
  for (size_t i = 0; i < n; i++) {
    flag[label[i]]++;
  }
  cout << parlay::count_if(flag, [&](NodeId label_size) {
    return label_size != 0;
  }) << '\n';
  cout << parlay::reduce(flag, maxm<NodeId>()) << '\n';
}

int main(int argc, char** argv) {
  CommandLine P(argc, argv);
  if (argc < 2) {
    cout << "usage: ./tarjan_scc fileName" << endl;
    return 0;
  }
  char* fileName = argv[1];
  cout << "input graph: "<< fileName << endl;
  Graph graph = read_graph(fileName);
  cout << "number of vertices: " << graph.n << ", number of edges: " << graph.m << endl;


  int repeat = P.getOptionInt("-t", (int)10);
  timer scc_timer;
  TARJAN_SCC SCC_P(graph);
  SCC_P.clear();
  SCC_P.tarjan();
  double scc_cost;
  for (int i = 0; i < repeat; i++) {
    SCC_P.clear(); 
    scc_timer.start();
    SCC_P.tarjan();
    scc_cost = scc_timer.stop();
    cout<< "scc cost: " << scc_cost << endl;
  }
  cout << "average cost " << scc_timer.get_total()/repeat << endl;
  // cout << "average cost " << t.get_total()/repeat << endl;
  if (P.getOption("-status")) SCC_status(SCC_P.scc, graph.n);

  return 0;
}