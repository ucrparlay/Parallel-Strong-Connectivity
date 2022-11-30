Parallel-Strong-Connectivity 
====================== 
This repository includes the implementations of strongly connected components, connectivity, and least-element Lists (LE-Lists). Part of our primitives are from a public repository [GBBS](https://github.com/ParAlg/gbbs).  

## Developing 

### Prerequisites 
* g++ &gt;= 7 with support for C++17 features (Tested with g++ 7.5.0) 
* [Parlaylib](https://github.com/cmuparlay/parlaylib/tree/281cc092be61629d3c944e0facb3b1869160564c) (Tested with version 2.0.3) 

### Setting up 
Download the library
```shell
mkdir Parallel-Strong-Connectivity  
cd Parallel-Strong-Connectivity  
wget -c -O Parallel-Strong-Connectivity.zip https://anonymous.4open.science/api/repo/Parallel-Strong-Connectivity-876E/zip  
unzip Parallel-Strong-Connectivity.zip  
```

### Building
A makefile is given under src/, you can compile the code by: 
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