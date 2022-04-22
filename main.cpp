#include <bits/stdc++.h>
#include <mpi.h>

using namespace std;

/*
1	Blue	9	Light Blue
2	Green	0	Black
3	Aqua	A	Light Green
4	Red	    B	Light Aqua
5	Purple	C	Light Red
6	Yellow	D	Light Purple
7	White	E	Light Yellow
8	Gray	F	Bright White

Color 0A -> bg Black, fg Light green
*/

#define INF 100000

/*
Coding stuff

(INF, INF, INF) being returned as a result of init indicates that no outgoing edge is present
(INF, 0, 0) being returned indicates waiting (to take care of false termination)
While merging or absorbing, new fragment root will be the smaller of the two roots

Tag:
1 -> Init msg -> (name, level)
2 -> Passing results of probing -> (weight, n1, n2)
3 -> Test msg -> (name, level)
4 -> Accept/Reject/Wait -> (0/1/2)

*/

// returns original indexes of elements in a sorted array
template <typename T>
vector<size_t> sort_indexes(const vector<T> &v) {
    vector<size_t> idx(v.size());
    iota(idx.begin(), idx.end(), 0);
    stable_sort(idx.begin(), idx.end(), [&v](size_t i1, size_t i2) { return v[i1] < v[i2]; });

    return idx;
}

int main(int argc, char **argv) {
    int rank, numprocs;
    MPI_Init(&argc, &argv);                    // initiate MPI
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);  // get size of the current communicator
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);      // get current process rank
    MPI_Barrier(MPI_COMM_WORLD);               // synchronize all processes
    double start_time = MPI_Wtime();

    //_____________________________________________________________________________________________________________
    // Input and basic constants
    ifstream input(argv[1]);
    int N;
    input >> N;

    vector<pair<int, int>> adjlist;  // (weight, endpoint);
    vector<int> edgetype;            // edgetype = 1 -> branch edge, 0 -> Basic edge, -1 -> Rejected edge
    int level = 0;
    int state = 0;  // 0 -> Sleep, 1 -> Find, 2 -> Found
    int name = rank;

    for (int i = 0; i < rank; ++i) {
        for (int j = 0; j < N; ++j) {
            int temp;
            input >> temp;
        }
    }

    for (int i = 0; i < N; ++i) {
        int temp;
        input >> temp;
        if (temp != INF) {
            adjlist.push_back({temp, i});
            edgetype.push_back(0);
        }
    }

    for (int i = rank + 1; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            int temp;
            input >> temp;
        }
    }
    //_____________________________________________________________________________________________________________
    while (true) {
        // Initiate stage
        if (rank == name) {
            // Node is a root

            // Send initiate along branch edges
            for (int i = 0; i < edgetype.size(); ++i) {
                if (edgetype[i] == 1) {
                    system("Color 01");
                    cout << rank << " sending initiate message to " << adjlist[i].second << endl;
                    int msg[2];
                    msg[0] = name;
                    msg[1] = level;
                    MPI_Send(&msg, 2, MPI_INT, adjlist[i].second, 1, MPI_COMM_WORLD);
                }
            }

            // Probe along basic edges to find minimum outgoing edge
            vector<pair<int, pair<int, int>>> results;
            for (auto i : sort_indexes(adjlist)) {
                if (edgetype[i] == 0) {
                    int msg[2];
                    msg[0] = name;
                    msg[1] = level;
                    MPI_Request request;
                    system("Color 02");
                    cout << rank << " sending test to " << adjlist[i].second << endl;
                    MPI_Isend(&msg, 2, MPI_INT, adjlist[i].second, 3, MPI_COMM_WORLD, &request);
                    MPI_Wait(&request, MPI_STATUS_IGNORE);
                   
                    MPI_Status status;
                    MPI_Recv(&msg, 2, MPI_INT, adjlist[i].second, 3, MPI_COMM_WORLD, &status);
                    system("Color 02");
                    cout<< rank << " received test from "<<status.MPI_SOURCE<<endl;
                    if (name == msg[0]) {
                           // reject
                    } else {
                        if (msg[1] <= level) {
                           // accept
                        } else {
                            // wait
                        }
                    }

                    MPI_Irecv(&msg, msg.length(), MPI_CHAR, adjlist[i].second, 4, MPI_COMM_WORLD, &request);
                    // Wait basically means continue and wait for next iteration
                    MPI_Wait()
                }
            }

            // Receive results

            // Decide the minimum outgoing edge

        } else {
            // Not a root
            // Wait for initiate
            int msg[2];
            MPI_Status status;
            MPI_Recv(&msg, 2, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
            system("Color 01");
            cout << rank << " received initiate message from " << status.MPI_SOURCE << endl;
            name = msg[0];
            level = msg[1];

            // Forward init along branch edges
            for (int i = 0; i < edgetype.size(); ++i) {
                if (edgetype[i] == 1 && adjlist[i].second != status.MPI_SOURCE) {
                    system("Color 01");
                    cout << rank << " forwarding initiate message to " << adjlist[i].second << endl;
                    MPI_Send(&msg, 2, MPI_INT, adjlist[i].second, 1, MPI_COMM_WORLD);
                }
            }

            // Probe along basic edges to determine minimum outgoing edge

            // Fetch results from childeren

            // Return results to parent
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}
