#include "scc.hpp"

#include <algorithm>  // std::max
#include <vector>

#include "get_time.hpp"
#include "parseCommandLine.hpp"

using namespace std;

// void SCC_checker(size_t* label, long n){
//     edge * edge_list = (edge*)malloc(sizeof(edge)*n);
//     std::vector<edge> hash_vec;  // (component_size,hash_sum)
//     cilk_for(auto i = 0; i<n; i++){
//         edge_list[i]=edge(label[i],i);
//     }
//     quickSort(edge_list, n, edgeCmp());
//     uintT hash_sum = 0;
//     uintT count = 0;
//     for (auto i = 0; i<n-1; i++){
//         hash_sum += hashInt(edge_list[i].v);
//         count ++;
//         if (edge_list[i].u != edge_list[i+1].u){
//             hash_vec.push_back(edge(count,hash_sum));
//             hash_sum = 0;
//             count = 0;
//         }
//     }
//     hash_sum += hashInt(edge_list[n-1].v);
//     count ++;
//     hash_vec.push_back(edge(count, hash_sum));
//     sort(hash_vec.begin(), hash_vec.end(), edgeCmp());

//     cout << "n_scc = " << hash_vec.size() << endl;
//     cout << "Largest StronglyConnectedComponents has " <<
//     hash_vec[hash_vec.size()-1].u << " vertices" << endl;

//     hash_sum = 0;
//     for (auto iter = hash_vec.begin(); iter != hash_vec.end(); iter++){
//         // std::cout << std::to_string(iter->u) << " " <<
//         std::to_string(iter->v) << std::endl; hash_sum +=
//         hashInt(hashInt(iter->u)+hashInt(iter->v));
//     }
//     std::cout<< "hash_code " << hash_sum << std::endl;

//     std::cout << "first largest SCC's size " << hash_vec[hash_vec.size()-1].u
//     << " hash_code " << hash_vec[hash_vec.size()-1].v << std::endl; std::cout
//     << "third largest SCC's size " << hash_vec[hash_vec.size()-3].u << "
//     hash_code " << hash_vec[hash_vec.size()-3].v << std::endl; std::cout <<
//     "fifth largest SCC's size " << hash_vec[hash_vec.size()-5].u << "
//     hash_code " << hash_vec[hash_vec.size()-5].v << std::endl; std::cout <<
//     "ninth largest SCC's size " << hash_vec[hash_vec.size()-9].u << "
//     hash_code " << hash_vec[hash_vec.size()-9].v << std::endl;

//     free (edge_list);
// }

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

  Graph graph = read_graph(
      fileName, !P.getOption("-nmmap"));  // symmetric : 1, makeSymmetric; 2,
                                          // makeDirected // binary
  // Graph graph = read_large_graph(fileName);
  bool local_reach = P.getOption("-local_reach");
  bool local_scc = P.getOption("-local_scc");

  t1.stop();
  cout << "read graph finish" << endl;
  cout << "n: " << graph.n << " m: " << graph.m << endl;

  // size_t* label = new size_t[graph.n];
  sequence<size_t> label = sequence<size_t>(graph.n);
  int repeat = P.getOptionInt("-t", (int)5);
  timer t;
  SCC SCC_P(graph);
  SCC_P.front_thresh = P.getOptionInt("-thresh", 1000000000);
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
#if defined(BREAKDOWN)
  SCC_P.breakdown_report(repeat);
#endif
  if (P.getOption("-status")) SCC_status(label, graph.n);

  return 0;
}