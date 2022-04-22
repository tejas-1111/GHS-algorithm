#include <mpi.h>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <queue>
#include <vector>

using namespace std;

int main(int argc, char **argv) {
    int rank, numprocs;
    MPI_Init(&argc, &argv);                    // initiate MPI
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);  // get size of the current communicator
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);      // get current process rank
    MPI_Barrier(MPI_COMM_WORLD);               // synchronize all processes
    double start_time = MPI_Wtime();

    // Input and basic constants
    ifstream input(argv[1]);
    int N;
    input >> N;
    vector<int> nodes_handled(numprocs, N / numprocs);
    for (int i = 0; i < N % numprocs; ++i) {
        nodes_handled[i] += 1;
    }
    for (int i = 1; i < numprocs; ++i) {
        nodes_handled[i] += nodes_handled[i - 1];
    }
    for (int i = 0; i < numprocs; ++i) {
        cout << nodes_handled[i] << endl;
    }
    
    return 0;
}
