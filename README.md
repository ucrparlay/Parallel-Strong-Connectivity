Parallel-Strong-Connectivity 
====================== 
This repository contains code for our paper "Parallel Strong Connectivity based on Faster Reachability" in [SIGMOD '23](https://dl.acm.org/doi/10.1145/3589259). It includes the implementations of strongly connected components, connectivity, and least-element Lists (LE-Lists). Part of our primitives are from a public repository [GBBS](https://github.com/ParAlg/gbbs), which is derived from [pbbslib](https://github.com/cmuparlay/pbbslib).  

[TOC]



## Deployment

### Requirements 

- Multi-processor machine (tested on CentOS 8 and XXX)

* g++ &gt;= 7 with support for C++17 features (Tested with g++ 7.5.0)

> **_NOTE:_**  It does not compile with g++-8, which is a known bug that we are working on. It works with g++-7, g++-9, g++-11, and g++-12.  

- python3 (used to run scripts)
- We use [parlaylib](https://github.com/cmuparlay/parlaylib) for fork-join parallelism and some primative. It is provided as submodules in our repository.

### Get Started

Code download:

```
git clone --recurse-submodules git@github.com:ucrparlay/Parallel-Strong-Connectivity.git
```

We provide 5 small graphs as examples. They are located in `./data`. Run the following commands to check if the deployment was successful:

```
make -j
python3 scripts/test.py
```

### Download Graphs

- Download the all graphs to `./data`

  ``` python3 scripts/download.py```

These command will download the graphs used in this paper to `./data`.

We comment out `hyperlink2014` and `hyperlink2012`, since their large sizes (253G and 1013G to store them separately). If you want to download these graphs, you can uncomment them in `./scripts/graphs.py`, but may not download successful because of the band width limitation of dropbox. You can try to download them another day.

For ClueWeb, it is too large to fit in dropbox. You can find it at [Web Data Commons](http://webdatacommons.org/hyperlinkgraph/).

You can also download graphs manually from this link [google drive](https://drive.google.com/drive/folders/1C86nDTo76aalBcmtgWWBLW6sOIhe1Btq?usp=share_link).

We use the `.bin` binary graph format from [GBBS](https://github.com/ParAlg/gbbs).

## Reproducibility

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

### Building
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

