Parallel-Strong-Connectivity 
====================== 
This repository contains code for our paper "Parallel Strong Connectivity based on Faster Reachability" in [SIGMOD '23](https://dl.acm.org/doi/10.1145/3589259). It includes the implementations of strongly connected components, connectivity, and least-element Lists (LE-Lists). Part of our primitives are from a public repository [GBBS](https://github.com/ParAlg/gbbs), which is derived from [pbbslib](https://github.com/cmuparlay/pbbslib).  




## Deployment

### Requirements 

- Multi-processor machine (tested on CentOS 8)

* g++ &gt;= 7 with support for C++17 features (Tested with g++ 7.5.0)

> **_NOTE:_**  It does not compile with g++-8, which is a known bug that we are working on. It works with g++-7, g++-9, g++-11, and g++-12.  

- python3 (used to run scripts, version >= 3.7)
  - pandas (used to collect experiment data)
  - numpy (used to collect experiment data)
  - matplotlib (used to draw figures)
  - seaborn (used to draw figures) 
- We use [parlaylib](https://github.com/cmuparlay/parlaylib) for fork-join parallelism and some primative.
- numactl (used for arrange memory on different sockets)
- We include the baselines in the folder `baselines`
- [Bazel](https://bazel.build/install) 2.1.0 (used to compile the baseline 'gbbs')
### Get Started

Code download: git clone with submodules

```
git clone --recurse-submodules https://github.com/ucrparlay/Parallel-Strong-Connectivity.git
```
<!-- 
We provide 3 small directed graphs as examples. They are located in `./data/directed`. Run the following commands to check if the deployment was successful:

```
make -j
python3 scripts/test.py
``` -->
<!-- For ClueWeb, it is too large to fit in dropbox. You can find it at [Web Data Commons](http://webdatacommons.org/hyperlinkgraph/). -->


## Reproducibility

### Our Machine Information
- CPU: 4x Intel(R) Xeon(R) Gold 6252 CPU @ 2.10GHz
- Physical CPU cores: 96
- Threads per core: 2
- NUMA nodes: 4
  - node0 CPUs: 0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,64,68,72,76,80,84,88,92,96,100,104,108,112,116,120,124,128,132,136,140,144,148,152,156,160,164,168,172,176,180,184,188
  - node1 CPUs: 1,5,9,13,17,21,25,29,33,37,41,45,49,53,57,61,65,69,73,77,81,85,89,93,97,101,105,109,113,117,121,125,129,133,137,141,145,149,153,157,161,165,169,173,177,181,185,189
  - node2 CPUs: 2,6,10,14,18,22,26,30,34,38,42,46,50,54,58,62,66,70,74,78,82,86,90,94,98,102,106,110,114,118,122,126,130,134,138,142,146,150,154,158,162,166,170,174,178,182,186,190
  - node3 CPUs: 3,7,11,15,19,23,27,31,35,39,43,47,51,55,59,63,67,71,75,79,83,87,91,95,99,103,107,111,115,119,123,127,131,135,139,143,147,151,155,159,163,167,171,175,179,183,187,191
- Memory: 1510 GB
- Operation system: CentOS Stream 8

Our scalability test script is written based on the above Machine information. When we run with fewer cores, try to run the code on the same socket.

## Replicability
### Step Zero: Download Graphs
- Download the all graphs to `./data`

  ``` python3 scripts/download.py```

These command will download the directed and undirected graphs to `./data` that are used in this paper. The file name of undirected graphs is ended by "_sym.bin". It may not download successful because of the band width limitation of dropbox. You can try to download them another day.

<!-- You can also download graphs manually from this link [google drive](https://drive.google.com/drive/folders/1ZuhfaLmdL-EyOiWYqZGD1rOy_oSFRWe4?usp=sharing). -->

We use the `.bin` binary graph format from [GBBS](https://github.com/ParAlg/gbbs).

We comment out `clueweb`, `hyperlink2014` and `hyperlink2012`, since their large sizes (166GB, 253GB and 1013GB) are too large to fit in dropbox. If you want to run these graphs, you can contact us by [email](lwang323@ucr.edu) and run on our server. 


### Step One: Run Experiments
The scripts of running experiment are under `./scripts` folder.
- Experiment 1: Testing the running times (in seconds) of all tested algorithms on SCC. It will generate the data listed in `Table 3` and `Figure 1` in the paper.

  ``` python3 scripts/Experiment1.py ```
  
  The output files are stored in `./log/exp1`.
- Experiment 2: Testing the speedup over Tarjan’s sequential algorithm for different
algorithms on different numbers of processors (Scalability). It will generate the data used in `Figure 7` and `Figure 8` in the paper. 

  ```python3 scripts/Experiment2.py```

  The output files are stored in `./log/exp2`.
- Experiment 3: Testing the SCC breakdown time. It will generate the data used in `Figure 9` in the paper.

  ```python3 scripts/Experiment3.py```

  The output files are stored in `./log/exp3`.
- Experiment 4: Testing the relative running time of $\tau=1$ on six graphs with $\tau$ range from $2^0$ to $2^{17}$. It will generate the data used in `Figure 11` in the paper.

  ```python3 scripts/Experiment4.py```
  
  The output files are stored in `./log/exp4`.
- Experiment 5:  Testing the running time of connectivity and LE-Lists implementations. It will generate the data listed in `Table 4` in the paper.

  ```python3 scripts/Experiment5.py```

  The output files are stored in `./log/exp5`.
- Experiment 6: Testing the number of rounds with and without VGC for each reachability search. It will generate the data used in `Figure 10` in the paper.

  ```python3 scripts/Experiment6.py```

  The output files are stored in `./log/exp6`.

### Step Two: Collect Data
It will collect the data in `./log` folder, and generate the `.csv` format files in `./result`

```python3 scripts/data_collection.py```


### Step Three: Draw Figures
It will use the data in `./result` folder to generate figures of `.pdf` format in `./figures`.
Note that since Figure 1 is essentially a table, it is not in `./figures`. But `Figure1.cvs` is in `./result` folder among with `Table3.csv` and `Table4.csv`



## Developing
### Build
Users can compile the code on their own. A makefile is given under the folder ``src/``, you can compile the code by:  
```shell
make 
```
To let the SCC code output the breakdown time/number of rounds/run scc serial, compile the code with flag "BREAKDOWN=1"/"ROUND=1"/"SERIAL=1", such as:
```shell 
make scc BREAKDOWN=1
``` 
### Usage
```shell
./application [input_graph]  
```
For example, if you want to run strongly connected components: 
```shell
./scc [input_graph]  
```
To enable local search in single-reachability and multi-reachability, add the "-local\_reach" and "-local\_scc" options.

```shell
./scc [input_graph] -local_reach -local_scc  
```

The application can auto-detect the format of the input graph based on the suffix of the filename. It supports the adjacency graph format from [Problem Based Benchmark suite](http://www.cs.cmu.edu/~pbbs/benchmarks/graphIO.html), whose filename should end with ".adj". It also supports binary representation. 

If you are running our code on a machine with more than one socket, **numactl** can potentially improve the performance.  
```shell
numactl -i all ./scc [input_graph]  
```

If you want to know the number of SCC and Largest SCC size, you can set option `-stats`.
```shell
numactl -i all ./scc [input_graph] -status
```

If you want to set the number of threads and specify the cores used to run the code, you can set "PARLAY_NUM_THREADS" and "taskset -c". For example, if you want to run on 24 threads and cores 0,4,8,12,...92, you can run:
```shell
PARLAY_NUM_THREADS=24 taskset -c 0-95:4 numactl -i all ./scc [input_graph]
```
