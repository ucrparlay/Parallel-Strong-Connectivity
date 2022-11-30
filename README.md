Parallel-Strong-Connectivity 
====================== 

This repository includes the implementations of strongly connected components, connectivity, and least-element Lists (LE-Lists). 

## Developing 

### Prerequisites 
* g++ &gt;= 7 with support for C++17 features (Tested with g++ 7.5.0) 
* [Parlaylib](https://github.com/cmuparlay/parlaylib/tree/281cc092be61629d3c944e0facb3b1869160564c) (Tested with version 2.0.3) 

### Setting up 
Clone the library 
```shell
git clone https://github.com/ucrparlay/Parallel-Strong-Connectivity.git 
cd Parallel-Strong-Connectivity/ 
```

### Building
A makefile is given under src/, you can compile the code by: 
```shell
make 
```

## Usage
```shell
./application filename 
```
For example, if you want to run strongly connected components: 
```shell
./scc filename 
```
To enable local search in single-reachability and multi-reachability, add the "-local\_reach" and "-local\_scc" options.

```shell
./scc filename -local_reach -local_scc 
```
Try a toy example in this repository: 
```shell
./scc ../data/soc-LiveJournal1.bin -local_reach -local_scc 
```


The application can auto-detect the format of the input graph based on the suffix of the filename. It supports the adjacency graph format from [Problem Based Benchmark suite](http://www.cs.cmu.edu/~pbbs/benchmarks/graphIO.html), whose filename should end with ".adj". It also supports binary representation. For storage limit, we only provide two sample binary graphs in an anonymous [Google Drive](https://drive.google.com/drive/folders/1ztlrVgfLlmbR-McyhiRCtDYoMcR9Tyq3?usp=sharing) folder. We will provide all tested graphs in the camera-ready version. 

For storage limit, we don't provide some of the large graphs used in our paper. They can be found in [Stanford Network Analysis Project](http://snap.stanford.edu/) and [Web Data Commons](http://webdatacommons.org/hyperlinkgraph/). 
