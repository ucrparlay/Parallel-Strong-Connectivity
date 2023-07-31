Parallel-Strong-Connectivity 
====================== 
This repository contains code for our paper "Parallel Strong Connectivity based on Faster Reachability" in [SIGMOD '23](https://dl.acm.org/doi/10.1145/3589259). It includes the implementations of strongly connected components, connectivity, and least-element Lists (LE-Lists). Part of our primitives are from a public repository [GBBS](https://github.com/ParAlg/gbbs), which is derived from [pbbslib](https://github.com/cmuparlay/pbbslib).  




## Deployment

### Requirements 

- Multi-processor machine (tested on CentOS 8 and MacOS 10 & 13)

* g++ &gt;= 7 with support for C++17 features (Tested with g++ 7.5.0)

> **_NOTE:_**  It does not compile with g++-8, which is a known bug that we are working on. It works with g++-7, g++-9, g++-11, and g++-12.  

- python3 (used to run scripts)
  - pandas (used to collect experiment data)
- We use [parlaylib](https://github.com/cmuparlay/parlaylib) for fork-join parallelism and some primative.
- numactl (used for arrange memory on different sockets)
- We include the baselines in the folder `baselines`
- bazel (used to compile the baseline 'gbbs')
### Get Started

Code download: git clone with submodules

```
git clone --recurse-submodules git@github.com:ucrparlay/Parallel-Strong-Connectivity.git
```
<!-- 
We provide 3 small directed graphs as examples. They are located in `./data/directed`. Run the following commands to check if the deployment was successful:

```
make -j
python3 scripts/test.py
``` -->

### Download Graphs

- Download the all graphs to `./data`

  ``` python3 scripts/download.py```

These command will download the directed graphs to `./data/dir` and undirected graphs to `./data/sym` that are used in this paper. It not download successful because of the band width limitation of dropbox. You can try to download them another day.

You can also download graphs manually from this link [google drive](https://drive.google.com/drive/folders/1ZuhfaLmdL-EyOiWYqZGD1rOy_oSFRWe4?usp=sharing).

We use the `.bin` binary graph format from [GBBS](https://github.com/ParAlg/gbbs).

We comment out `clueweb`, `hyperlink2014` and `hyperlink2012`, since their large sizes (166GB, 253GB and 1013GB) are too large to fit in dropbox. If you want to run these graphs, you can contact us and run on our server. To run them on our server, just uncomment them in `./scripts/graphs.py`. 

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

## Replicability





## Developing

### Prerequisites 
* g++ &gt;= 7 with support for C++17 features (Tested with g++ 7.5.0)

> **_NOTE:_**  It does not compile with g++-8, which is a known bug that we are working on. It works with g++-7, g++-9, g++-11, and g++-12.  

### Setting up 
Download the library
```shell
wget -c -O Parallel-Strong-Connectivity.zip "https://www.dropbox.com/s/q3ri2xj31o2awuo/Parallel-Strong-Connectivity.zip?dl=1"  
unzip Parallel-Strong-Connectivity.zip   
cd Parallel-Strong-Connectivity/  
```

## Example
To try the toy examples in this repository, first download the dataset with:  
```shell
./scripts/download_dataset.sh  
```
And run all three applications (SCC, Connectivity, LE-lists) with:  
```shell
./scripts/run_all.sh  
```
Then the result will be save in the ``result/`` folder.  

## Building
Alternatively, users can compile the code on their own. A makefile is given under the folder ``src/``, you can compile the code by:  
```shell
make 
```

## Usage
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

The application can auto-detect the format of the input graph based on the suffix of the filename. It supports the adjacency graph format from [Problem Based Benchmark suite](http://www.cs.cmu.edu/~pbbs/benchmarks/graphIO.html), whose filename should end with ".adj". It also supports binary representation. For storage limit, we only provide two sample binary graphs in an anonymous [Google Drive](https://drive.google.com/drive/folders/1ztlrVgfLlmbR-McyhiRCtDYoMcR9Tyq3?usp=sharing) folder. We will provide all tested graphs in the camera-ready version.  

If you are running our code on a machine with more than one socket, **numactl** can potentially improve the performance.  
```shell
numactl -i all ./scc [input_graph]  
```

