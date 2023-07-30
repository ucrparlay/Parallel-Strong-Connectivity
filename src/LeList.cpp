#include "LeList.hpp"

#include "get_time.hpp"
#include "graph.hpp"
#include "parseCommandLine.hpp"
using namespace std;

int main(int argc, char** argv) {
  CommandLine P(argc, argv);
  if (argc < 2) {
    cout << "usage: ./LeList fileName" << endl;
    return 0;
  }
  char* filename = argv[1];
  timer t1;
  t1.start();
  Graph G = read_graph(filename, !P.getOption("-nmmap"), P.getOption("-s"));
  t1.stop();
  cout << "Graph: " << filename << endl;
  cout << "#vertices: " << G.n << " #edges: " << G.m << '\n';
  double beta = P.getOptionDouble("-beta", 1.5);

  LE_LIST LE_LIST_P(G);
  // cout << "initialize LE_LIST finish " << endl;
  int repeat = P.getOptionInt("-t", (int)3);

  if (P.getOption("-seq")) {
    LE_LIST_P.seq_le_list();
    timer t;
    for (int i = 0; i < repeat; i++) {
      t.start();
      LE_LIST_P.seq_le_list();
      double cost = t.stop();
      cout << "sequential LE-List cost: " << cost << endl;
      cout << "LE-List size " << LE_LIST_P.list_offset << endl;
    }
    cout << "average sequential LE-List cost: " << t.get_total() / repeat
         << endl;
    // for (size_t i = 0; i<100; i++){
    //   cout << get<0>(LE_LIST_P.list[i]) << " " << get<1>(LE_LIST_P.list[i])
    //   << " " << get<2>(LE_LIST_P.list[i]) << endl;
    // }
  } else {
    LE_LIST_P.le_list(beta);
    timer t;
    for (int i = 0; i < repeat; i++) {
      t.start();
      LE_LIST_P.le_list(beta);
      double cost = t.stop();
      cout << "LE-List cost: " << cost << endl;
    }
    cout << "# LE-List size: " << LE_LIST_P.list_offset << endl;
    cout << "average cost: " << t.get_total() / repeat << endl;
    // for (size_t i = 0; i<LE_LIST_P.list_offset; i++){
    //   cout << get<0>(LE_LIST_P.list[i]) << " " << get<1>(LE_LIST_P.list[i])
    //   << " " << get<2>(LE_LIST_P.list[i]) << endl;
    // }
  }
  if (P.getOption("-check")) {
    LE_LIST_P.seq_le_list();
    size_t LeList_seq_size = LE_LIST_P.list_offset;
    sequence<tuple<NodeId, NodeId, NodeId>> LeList_seq =
        sequence<tuple<NodeId, NodeId, NodeId>>(LeList_seq_size);
    parallel_for(0, LeList_seq_size,
                 [&](size_t i) { LeList_seq[i] = LE_LIST_P.list[i]; });

    LE_LIST_P.le_list(beta);
    size_t LeList_size = LE_LIST_P.list_offset;
    size_t j = 0;
    size_t i = 0;
    for (i = 0; i < LeList_seq_size; i++) {
      if (get<0>(LeList_seq[i]) == get<0>(LE_LIST_P.list[j]) &&
          get<1>(LeList_seq[i]) == get<1>(LE_LIST_P.list[j]) &&
          get<2>(LeList_seq[i]) == get<2>(LE_LIST_P.list[j])) {
        j++;
      }
      if (j == LeList_size) break;
    }
    if (LeList_seq_size == LeList_size && i == j - 1) {
      cout << "pass check" << endl;
    } else {
      cout << "LeList_seq size " << LeList_seq_size << endl;
      cout << "LeList size " << LeList_size << endl;
      cout << "i " << i << endl;
      cout << "j " << j << endl;
      cout << "------ seq -----" << endl;
      for (int i = 0; i < 100; i++) {
        cout << get<0>(LeList_seq[i]) << " " << get<1>(LeList_seq[i]) << " "
             << get<2>(LeList_seq[i]) << endl;
      }
      cout << "----- parallel -----" << endl;
      for (int i = 0; i < 100; i++) {
        cout << get<0>(LE_LIST_P.list[i]) << " " << get<1>(LE_LIST_P.list[i])
             << " " << get<2>(LE_LIST_P.list[i]) << endl;
      }
    }
  }
  return 0;
}