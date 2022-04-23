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

int name;                        // name of fragment
int level;                       // level of fragment
int node;                        // Node = process rank
int parent;                      // Node which sent initiate message
int n;                           // Number of neighbors
vector<pair<int, int>> adjlist;  // Stores (weight, neighbor)
vector<int> edgetype;            // 0 -> basic edge, 1 -> Branch edge, -1 -> Reject edge

/**
 * @brief Returns original indexes of elements in an sorted array
 */
template <typename T>
vector<size_t> sort_indexes(const vector<T> &v) {
    vector<size_t> idx(v.size());
    iota(idx.begin(), idx.end(), 0);
    stable_sort(idx.begin(), idx.end(), [&v](size_t i1, size_t i2) { return v[i1] < v[i2]; });

    return idx;
}

/**
 * @brief Takes input, creates adjlist and sets basic variables up
 *
 * @param s -> Path for input
 */
void input(string s) {
    ifstream input(s);
    int N;
    input >> N;
    vector<pair<int, int>> adjlist;
    for (int i = 0; i < node; ++i) {
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
        }
    }

    for (int i = node + 1; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            int temp;
            input >> temp;
        }
    }

    edgetype.resize(adjlist.size(), 0);
    name = node;
    level = 0;
    n = edgetype.size();
}

/**
 * @brief Carries out Initiate stage
 */
void Initiate() {
    // TODO change to MPI_Isend / MPI_Irecv
    if (name == node) {
        // Fragment root
        parent = -1;

        // Send out initiate message along branch edges
        for (int i = 0; i < n; ++i) {
            if (edgetype[i] == 1) {
                int msg[2];
                msg[0] = name;
                msg[1] = level;
                MPI_Send(&msg, 2, MPI_INT, adjlist[i].second, 1, MPI_COMM_WORLD);
            }
        }
    } else {
        // Not a fragment root

        // Receive initiate message and set name, level, parent
        int msg[2];
        MPI_Status status;
        MPI_Recv(&msg, 2, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
        parent = status.MPI_SOURCE;
        name = msg[0];
        level = msg[1];

        // Forward initiate message along branch edges except to the parent
        for (int i = 0; i < n; ++i) {
            if (edgetype[i] == 1 && adjlist[i].second != parent) {
                MPI_Send(&msg, 2, MPI_INT, adjlist[i].second, 1, MPI_COMM_WORLD);
            }
        }
    }
}

/**
 * @brief Carries out convergecast operation
 *
 */
void ConvergeResults() {
    if (name == node) {
        // Fragment root

        // Send test message along basic edges and reply to any test messages received
        for (auto i : sort_indexes(adjlist)) {
            if (edgetype[i] == 0) {
                // Send test message
                MPI_Request request;
                int msg[2];
                msg[0] = name;
                msg[1] = level;
                MPI_Isend(&msg, 2, MPI_INT, adjlist[i].second, 3, MPI_COMM_WORLD, &request);
                MPI_Wait(&request, MPI_STATUS_IGNORE);

                // TODO Make a function to answer any test probes received and keep on calling it (after sending join request and the other node not having the answer for its test probe)
                // TODO Deadlock possible below, change it to use the new above function
                // Reply to test messages until we receive test probe answer
                int result;
                MPI_Request reply;
                MPI_Irecv(&result, 1, MPI_INT, adjlist[i].second, 4, MPI_COMM_WORLD, &reply);
                int rep;  // Received reply yet? 1 if yes, 0 if no
                int ans;  // Unanswered test probes?  1 if yes, 0 if no
                MPI_Test(&reply, &rep, MPI_STATUS_IGNORE);
                MPI_Iprobe(MPI_ANY_SOURCE, 3, MPI_COMM_WORLD, &ans, MPI_STATUS_IGNORE);
                while (ans || !rep) {
                    if (ans) {
                        MPI_Status status;
                        MPI_Recv(&msg, 2, MPI_INT, MPI_ANY_SOURCE, 3, MPI_COMM_WORLD, &status);
                        int res = -1;
                        if (name == msg[0]) {
                            res = 1;
                            for (int j = 0; j < n; ++j) {
                                if (adjlist[j].second == status.MPI_SOURCE) {
                                    edgetype[j] = -1;
                                    break;
                                }
                            }
                        } else {
                            if (msg[1] <= level) {
                                res = 0;
                            } else {
                                res = 2;
                            }
                        }
                        MPI_Request sent;
                        MPI_Isend(&res, 1, MPI_INT, status.MPI_SOURCE, 4, MPI_COMM_WORLD, &sent);
                        MPI_Wait(&sent, MPI_STATUS_IGNORE);
                    }
                    if (!rep) {
                        MPI_Test(&reply, &rep, MPI_STATUS_IGNORE);
                    }
                    MPI_Iprobe(MPI_ANY_SOURCE, 3, MPI_COMM_WORLD, &ans, MPI_STATUS_IGNORE);
                }
            }
        }
    }
}

int main(int argc, char **argv) {
    int node, numprocs;
    MPI_Init(&argc, &argv);                    // initiate MPI
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);  // get size of the current communicator
    MPI_Comm_rank(MPI_COMM_WORLD, &node);      // get current process node
    MPI_Barrier(MPI_COMM_WORLD);               // synchronize all processes
    double start_time = MPI_Wtime();

    input(argv[1]);

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}
