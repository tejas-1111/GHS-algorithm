# GHS-algorithm
## Distributed Systems, Spring 2022
Implementation of GHS algorithm to calculate MST using open MPI

### Assumptions
1) Graph has distinct weights for every edge
2) Graph is undirected
3) Input is given in form of adjacency matrix where no edge between two nodes is denoted by 100000
4) The number of nodes in graph is equal to the number of processes being allocated for the program

mpirun -np 50 --oversubscribe --mca opal_warn_on_missing_libcuda 0 ./a.out