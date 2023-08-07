Parallel-Strong-Connectivity 
====================== 
This repository contains code for our paper "Parallel Strong Connectivity based on Faster Reachability" in [SIGMOD '23](https://dl.acm.org/doi/10.1145/3589259). It includes the implementations of strongly connected components, connectivity, and least-element Lists (LE-Lists). Part of our primitives are from a public repository [GBBS](https://github.com/ParAlg/gbbs), which is derived from [pbbslib](https://github.com/cmuparlay/pbbslib).  




## Deployment

### Requirements 

- Multi-processor machine (tested on 96-core machin with hyper threads running on CentOS 8)

* g++ &gt;= 7 with support for C++17 features (Tested with g++ 7.5.0)

> **_NOTE:_**  It does not compile with g++-8, which is a known bug that we are working on. It works with g++-7, g++-9, g++-11, and g++-12.  

- python3 (used to run scripts, version >= 3.7)
  - pandas (used to collect experiment data)
  - numpy (used to collect experiment data)
  - matplotlib (used to draw figures)
  - seaborn (used to draw figures) 
- We use [parlaylib](https://github.com/cmuparlay/parlaylib) for fork-join parallelism, some primative and as baseline for LE-List algorithm.
- numactl (used to arrange memory on different sockets)
- [Bazel](https://bazel.build/install) 2.1.0 (used to compile the baseline 'gbbs')
- cmake 3.14+ (used to compile the baseline "parlay")
### Get Started

- We include the baselines in the folder `baselines` and `parlaylib` as submodulars.
- To clone the whole reprepository with submodulars, you need to use the following command.

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

Our script of scalability tests (Figure 7,8,11) is written based on the above Machine information. More specifically, when the number of cores used is smaller than the number of cores within one socket, we run the test on the same socket; otherwise, evenly use the cores on 4 sockets. 

If the cores on the testing machine is arranged differently as the above machine, the script can still run, but the result may not consistant with the one reported in the paper.

## Replicability
### Step Zero: Preparation
- Make sure all the requirements listed above are satisfied. 
- Download the graphs by:

  ``` python3 scripts/download.py```

  These command will download graphs  used in this paper to `./data` folder. It may not download successful because of the band width limitation of dropbox. You can try to download them another day.

  <!-- You can also download graphs manually from this link [google drive](https://drive.google.com/drive/folders/1ZuhfaLmdL-EyOiWYqZGD1rOy_oSFRWe4?usp=sharing). -->

  We use the `.bin` binary graph format from [GBBS](https://github.com/ParAlg/gbbs).

  We comment out `clueweb`, `hyperlink2014` and `hyperlink2012`, since their large sizes (166GB, 253GB and 1013GB) are too large to fit in dropbox. If you want to run these graphs, you can contact us by [email](lwang323@ucr.edu) and run on our server. To run on these three graphs, just uncomment these graphs in `scripts/download.py` and change the `GRAPH_DIR` in it.

- Compile all needed algorithms

  ```python3 script/algorithms.py```

  `scripts/algorithms.py` lists and compiles all the algorithms we will test, including our algorithms and baselines.

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
- Experiment 4: Testing the relative running time over $\tau=1$ on six graphs with $\tau$ range from $2^0$ to $2^{17}$. It will generate the data used in `Figure 11` in the paper.

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

All the output files are in folder `./result`.
-  `collect_exp1()` generates:
    - `Table3.csv`: The running time SCC algorithms. For the header of the table: "Ours.par" and "Ours.seq" are our scc algorithm's parallel and sequential running time respectively and "Ours.spd" is "Ours.seq/Ours.par", similar for "GBBS.par", "GBBS.seq", and "GBBS.spd". "SEQ" is the sequential Tarjan's algorithm. "Best/Ours" is the best running time among all baselines over ours parallel running time.
    - `Figure1.csv`:  The heatmap of relative speedup for parallel SCC algorithms over the sequential Tarjan's algorithm using 96 cores (192 hyperthreads). "OverallMean" is the geomean of all graphs' relative running time under certain algorithm.

-  `collect_exp2()` generates:
    - `Figure7.csv`: The running time of SCC algorithms under different number of cores on 6 selected graphs.
    - `Figure8.csv`: The self-speedup of our SCC algorithm under different number of cores on 6 selected graphs. The numbers are the relative running time over the sequential running time.

-  `collect_exp3()` generates:
    - `Figure9.csv`: The breakdown time of baseline GBBS, our SCC without VGC, our SCC only with VGC on single-reach, and our SCC with VGC on both single and multi-reach.

-  `collect_exp4()` generates:
    - `Figure11.csv`: The running time of our scc algorithm with different $\tau$ under different number of cores on six selected graphs. The value in this table is the running time in seconds.
- `collect_exp5()` generates:
    - `Table4.csv`: The parallel running time of connectivity algorithms and LE-List algorithms.
- `collect_exp6()` generates:
    - `Figure10.csv`: The first column is the round id, the second and third columns are the number of sub-rounds inside that round without and with VGC technique.
    - `Table2.csv`: The directed graph informations, including nunber of vertices "n", number of edges "m", Largest SCC size "|SCC1|", the portion largest SCC to n "|SCC1|/n", the number of SCCs "#SCC". Note that in the paper, Table2 also reports the diameter of graphs "D", which are got by the diameter reported in previous papers and the maximum search depth of multiple sampled BFSs. Because they are estimated lower bound of diameters and are not related to the algorithm, so we remote that column here.

### Step Three: Draw Figures

```
python3 scripts/draw_figures.py
```

This script will use the data in `./result` folder to generate figures in `.pdf` format and store them in folder `./figures`.

Note that since Figure 1 is essentially a table, it is not in `./figures`, but `Figure1.cvs` in `./result` folder among with `Table3.csv` and `Table4.csv`

Summary of final results:
- Tables: in folder `./result`
    -  `Figure1.csv`
    -  `Table2.csv`
    -  `Table2.csv`
    -  `Table4.csv`
- Figures: in folder `./figures`
    - `Figure7.pdf`
    - `Figure8.pdf`
    - `Figure9.pdf`
    - `Figure10.pdf`
    - `Figure11.pdf`

## Developing Our Code
### Build
Users can compile the code on their own. A makefile is given under the folder ``./src``, you can compile the code by:  
```shell
make 
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

If you want to know the number of SCC and Largest SCC size, you can set option `-status`.
```shell
numactl -i all ./scc [input_graph] -status
```

If you want to set the number of threads and specify the cores used to run the code, you can set "PARLAY_NUM_THREADS" and "taskset -c". For example, if you want to run on 24 threads and cores 0,4,8,12,...92, you can run:
```shell
PARLAY_NUM_THREADS=24 taskset -c 0-95:4 numactl -i all ./scc [input_graph]
```
