Parallel-Strong-Connectivity 
====================== 

This repository includes the implmentations of strongly connected components, connectivity, and least-element Lists (LE-Lists). 

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

The application can auto-detect the format of the input graph based on the suffix of the filename. We support the adjacency graph format from [Problem Based Benchmark suite](http://www.cs.cmu.edu/~pbbs/benchmarks/graphIO.html), whose filename should end with ".adj". 

## Reference 
L. Wang, X. Dong, Y. Gu, Y. Sun. Parallel Strong Connectivity Based on Faster Reachability. In submission. 
