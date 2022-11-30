#!/bin/bash

cd src/
make

./scc ../data/soc-LiveJournal1.bin -local_reach -local_scc >> ../result/scc.out
./Connectivity ../data/soc-LiveJournal1_sym.bin >> ../result/connectivity.out
./LeList ../data/soc-LiveJournal1_sym.bin >> ../result/lelists.out
