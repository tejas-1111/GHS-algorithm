# GHS Algorithm
## Distributed Systems, 2022
Implementation of GHS algorithm to calculate MST using openMPI

### Assumptions
1) Graphs has distinct weights for every edge
2) Graph is undirected
3) Input is given in form of adjacency matrix where no value of infinity is denoted by 100000
4) The number of nodes in graph is equal to the number of process being allocated to mpirun
  
Compile using
```sh
mpic++ main.cpp
```

Run using
```sh
mpirun -np <number of nodes> --mca opal_warn_on_missing_libcuda 0 ./a.out <path to input file> > output.txt
```

### Some additional notes
1) Since we are not using openMPI IO, it might be possible that the output is not in the correct form, but it will always be correct