#include "Connectivity.h"

#include <parlay/monoid.h>
#include <parlay/primitives.h>

#include "parseCommandLine.hpp"

using namespace std;
using namespace parlay;

int main(int argc, char** argv) {
  CommandLine P(argc, argv);
  if (argc < 2) {
    cout << "usage: ./Connectivity filename" << endl;
    return 0;
  }
  char* filename = argv[1];
  timer t1;
  t1.start();
  Graph G = read_graph(filename, !P.getOption("-nmmap"));
  t1.stop();

  cout << "#vertices: " << G.n << " #edges: " << G.m << '\n';

  int repeat = P.getOptionInt("-t", (int)10);
  double beta = P.getOptionDouble("-beta", (double)0.2);
  timer t;
  Connectivity(G, beta);
  for (int i = 0; i < repeat; i++) {
    t.start();
    Connectivity(G, beta);
    double time = t.stop();
    cout << "Connectivity time: " << time << " " << endl;
  }
  cout << "average_time: " << t.get_total() / repeat << " " << endl;
  return 0;
}
