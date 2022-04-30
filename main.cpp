#include <bits/stdc++.h>
#include <mpi.h>
using namespace std;
#define INF 100000

// no idea why I did not use enums...

double start_time;

#define CONNECT 1
#define INITIATE 2
#define TEST 3
#define ACCREJ 4
#define REPORT 5
#define CHANGEROOT 6
#define TERMINATE 7

queue<pair<int, vector<int>>> waiting;  // Tag, arguments of the message
vector<tuple<int, int, int>> edges;     // (weight, type, node)
#define W(i) get<0>(edges[i])           // returns weight of edge i
#define T(i) get<1>(edges[i])           // returns type of edge i
#define N(i) get<2>(edges[i])           // returns the other node of edge i
int level = 0;                          // level of fragment
int name = 0;                           // name of the fragment
int parent = 0;                         // node which sent  the initiate message
int state = 0;                          // sleep = 0, find = 1, found = 2
int node;                               // node id = process id
int n;                                  // no of neighbours
int c;                                  // no of children (nodes to whom we have sent initate message)
int bestWt;                             // least weight outgoing edge found till now
int bestNode;                           // child from which the edge was found
int rec;                                // no of received reports
int testNode;                           // Node which has been sent the test msg
bool halt = false;
void input(string s) {
    ifstream input(s);
    int N;
    input >> N;
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
            edges.push_back(make_tuple(temp, 0, i));
        }
    }

    for (int i = node + 1; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            int temp;
            input >> temp;
        }
    }
    n = edges.size();
    sort(edges.begin(), edges.end());
}

void Initialization() {
    printf("[%lf] %d(name=%d, level=%d) initalization\n", MPI_Wtime() - start_time, node, name, level);
    // edges is sorted in ascending order based on weights
    T(0) = 1;
    level = 0;
    state = 2;
    rec = 0;

    // send connect to min neighbour
    int msg = 0;
    MPI_Request sent;
    printf("[%lf] %d(name=%d, level=%d) sending connect to %d with level=%d\n", MPI_Wtime() - start_time, node, name,
           level, N(0), msg);
    MPI_Isend(&msg, 1, MPI_INT, N(0), CONNECT, MPI_COMM_WORLD, &sent);
    MPI_Wait(&sent, MPI_STATUS_IGNORE);
}

void connect(int q, int L) {
    int i = 0;
    while (i < n) {
        if (N(i) == q) {
            break;
        }
        ++i;
    }

    if (L < level) {
        T(i) = 1;
        MPI_Request sent;
        int send[3] = {level, name, state};
        printf("[%lf] %d(name=%d level=%d) sending initiate to %d with level=%d, name=%d, state=%d\n",
               MPI_Wtime() - start_time, node, name, level, q, send[0], send[1], send[2]);
        MPI_Isend(&send, 3, MPI_INT, q, INITIATE, MPI_COMM_WORLD, &sent);
        MPI_Wait(&sent, MPI_STATUS_IGNORE);
    } else if (T(i) == 0) {
        vector<int> args(2);
        args[0] = q;
        args[1] = L;
        printf("[%lf] %d(name=%d level=%d) waiting connect q=%d L=%d\n", MPI_Wtime() - start_time, node, name, level,
               args[0], args[1]);
        waiting.push({CONNECT, args});
    } else {
        MPI_Request sent;
        int send[3] = {level + 1, W(i), 1};
        printf("[%lf] %d(name=%d level=%d) sending initiate to %d with level=%d, name=%d, state=%d\n",
               MPI_Wtime() - start_time, node, name, level, q, send[0], send[1], send[2]);
        MPI_Isend(&send, 3, MPI_INT, q, INITIATE, MPI_COMM_WORLD, &sent);
        MPI_Wait(&sent, MPI_STATUS_IGNORE);
    }
}

void report() {
    if (rec == c && testNode == -1) {
        state = 2;
        MPI_Request sent;
        int send = bestWt;
        printf("[%lf] %d(name=%d level=%d) sending report to %d with bestWt=%d\n", MPI_Wtime() - start_time, node, name,
               level, parent, send);
        MPI_Isend(&send, 1, MPI_INT, parent, REPORT, MPI_COMM_WORLD, &sent);
        MPI_Wait(&sent, MPI_STATUS_IGNORE);
    }
}

void findMin() {
    int found = 0;
    for (int i = 0; i < n; ++i) {
        if (T(i) == 0) {
            found = 1;
            testNode = N(i);
            MPI_Request sent;
            int send[2] = {level, name};
            printf("[%lf] %d(name=%d level=%d) sending test to %d with level=%d name=%d\n", MPI_Wtime() - start_time,
                   node, name, level, testNode, send[0], send[1]);
            MPI_Isend(&send, 2, MPI_INT, testNode, TEST, MPI_COMM_WORLD, &sent);
            MPI_Wait(&sent, MPI_STATUS_IGNORE);
            break;
        }
    }
    if (!found) {
        testNode = -1;
        report();
    }
}

void initiate(int q, int Level, int Name, int State) {
    level = Level;
    name = Name;
    state = State;
    parent = q;
    bestNode = -1;
    bestWt = INF;
    testNode = -1;

    for (int i = 0; i < n; ++i) {
        if (T(i) == 0 && N(i) != parent) {
            ++c;
            MPI_Request sent;
            int send[3] = {level, name, state};
            printf("[%lf] %d(name=%d level=%d) sending init to %d with level=%d name=%d state=%d\n",
                   MPI_Wtime() - start_time, node, name, level, N(i), send[0], send[1], send[2]);
            MPI_Isend(&send, 3, MPI_INT, N(i), INITIATE, MPI_COMM_WORLD, &sent);
            MPI_Wait(&sent, MPI_STATUS_IGNORE);
        }
    }

    if (state == 1) {
        rec = 0;
        findMin();
    }
}

void changeRoot() {
    int i = 0;
    while (i < n) {
        if (N(i) == bestNode) {
            break;
        }
        ++i;
    }

    if (T(i) == 1) {
        MPI_Request sent;
        int msg = 0;
        printf("[%lf] %d(name=%d level=%d) sending changeroot to %d\n", MPI_Wtime() - start_time, node, name, level,
               bestNode);
        MPI_Isend(&msg, 1, MPI_INT, bestNode, CHANGEROOT, MPI_COMM_WORLD, &sent);
        MPI_Wait(&sent, MPI_STATUS_IGNORE);
    } else {
        T(i) = 1;
        int msg = level;
        MPI_Request sent;
        printf("[%lf] %d(name=%d level=%d) sending connect to %d with level=%d\n", MPI_Wtime() - start_time, node, name,
               level, bestNode, msg);
        MPI_Isend(&msg, 1, MPI_INT, bestNode, CONNECT, MPI_COMM_WORLD, &sent);
        MPI_Wait(&sent, MPI_STATUS_IGNORE);
    }
}

void reportRecv(int q, int w) {
    if (q != parent) {
        if (w < bestWt) {
            bestWt = w;
            bestNode = q;
        }
        rec += 1;
        report();
    } else {
        if (state == 1) {
            vector<int> args(2);
            args[0] = q;
            args[1] = w;
            printf("[%lf] %d(name=%d level=%d) waiting with q=%d w=%d\n", MPI_Wtime() - start_time, node, name, level,
                   args[0], args[1]);
            waiting.push({REPORT, args});
        } else if (w > bestWt) {
            changeRoot();
        } else if (w == bestWt && bestWt == INF) {
            int msg = 1;
            halt = true;
            for (int i = 0; i < n; ++i) {
                if (T(i) == 1) {
                    MPI_Request sent;
                    printf("[%lf] %d(name=%d level=%d) sending terminate to q=%d\n", MPI_Wtime() - start_time, node,
                           name, level, N(i));
                    MPI_Isend(&msg, 1, MPI_INT, N(i), TERMINATE, MPI_COMM_WORLD, &sent);
                    MPI_Wait(&sent, MPI_STATUS_IGNORE);
                }
            }
        }
    }
}

void test(int q, int Level, int Name) {
    int i = 0;
    while (i < n) {
        if (N(i) == q) {
            break;
        }
        ++i;
    }

    if (Level > level) {
        vector<int> args(3);
        args[0] = q;
        args[1] = Level;
        args[2] = Name;
        printf("[%lf] %d(name=%d level=%d) waiting test with q=%d level=%d name=%d\n", MPI_Wtime() - start_time, node,
               name, level, args[0], args[1], args[2]);
        waiting.push({TEST, args});
    } else if (Name == name) {
        if (T(i) == 0) {
            T(i) = -1;
        }
        if (N(i) != testNode) {
            int msg = -1;
            MPI_Request sent;
            printf("[%lf] %d(name=%d level=%d) sending reject to q=%d with ds=%d\n", MPI_Wtime() - start_time, node,
                   name, level, q, msg);
            MPI_Isend(&msg, 1, MPI_INT, q, ACCREJ, MPI_COMM_WORLD, &sent);
            MPI_Wait(&sent, MPI_STATUS_IGNORE);
        } else {
            findMin();
        }
    } else {
        int msg = 1;
        MPI_Request sent;
        printf("[%lf] %d(name=%d level=%d) sending accept to q=%d with ds=%d\n", MPI_Wtime() - start_time, node, name,
               level, q, msg);
        MPI_Isend(&msg, 1, MPI_INT, q, ACCREJ, MPI_COMM_WORLD, &sent);
        MPI_Wait(&sent, MPI_STATUS_IGNORE);
    }
}

void accrej(int q, int ds) {
    int i = 0;
    while (i < n) {
        if (N(i) == q) {
            break;
        }
        ++i;
    }
    if (ds == -1) {
        if (T(i) == 0) {
            T(i) == -1;
        }
        findMin();
    } else {
        testNode = -1;
        if (W(i) < bestWt) {
            bestWt = W(i);
            bestNode = q;
        }
        report();
    }
}

int main(int argc, char **argv) {
    int numprocs;
    MPI_Init(&argc, &argv);                    // initiate MPI
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);  // get size of the current communicator
    MPI_Comm_rank(MPI_COMM_WORLD, &node);      // get current process node
    input(argv[1]);
    MPI_Barrier(MPI_COMM_WORLD);  // synchronize all processes
    start_time = MPI_Wtime();

    Initialization();
    while (1) {
        int msg = 0;
        MPI_Status msg_info;
        MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &msg, &msg_info);
        if (msg) {
            if (msg_info.MPI_TAG == CONNECT) {
                int msg;
                MPI_Status msg_info;
                MPI_Recv(&msg, 1, MPI_INT, MPI_ANY_SOURCE, CONNECT, MPI_COMM_WORLD, &msg_info);
                int L = msg;
                int q = msg_info.MPI_SOURCE;
                printf("[%lf] %d(name=%d level=%d) received connect from %d with level=%d\n", MPI_Wtime() - start_time,
                       node, name, level, q, L);
                connect(q, L);
            } else if (msg_info.MPI_TAG == INITIATE) {
                int msg[3];
                MPI_Status msg_info;
                MPI_Recv(&msg, 3, MPI_INT, MPI_ANY_SOURCE, INITIATE, MPI_COMM_WORLD, &msg_info);
                int Level = msg[0];
                int Name = msg[1];
                int State = msg[2];
                int q = msg_info.MPI_SOURCE;
                printf("[%lf] %d(name=%d level=%d) received initate from %d with level=%d name=%d state=%d\n",
                       MPI_Wtime() - start_time, node, name, level, q, Level, Name, State);
                initiate(q, Level, Name, State);
            } else if (msg_info.MPI_TAG == TEST) {
                int msg[2];
                MPI_Status msg_info;
                MPI_Recv(&msg, 2, MPI_INT, MPI_ANY_SOURCE, TEST, MPI_COMM_WORLD, &msg_info);
                int Level = msg[0];
                int Name = msg[1];
                int q = msg_info.MPI_SOURCE;
                printf("[%lf] %d(name=%d level=%d) received test from %d with level=%d name=%d\n",
                       MPI_Wtime() - start_time, node, name, level, q, Level, Name);
                test(q, Level, Name);
            } else if (msg_info.MPI_TAG == ACCREJ) {
                int msg;
                MPI_Status msg_info;
                MPI_Recv(&msg, 1, MPI_INT, MPI_ANY_SOURCE, ACCREJ, MPI_COMM_WORLD, &msg_info);
                int ds = msg;
                int q = msg_info.MPI_SOURCE;
                printf("[%lf] %d(name=%d level=%d) received accrej from %d with ds=%d\n", MPI_Wtime() - start_time,
                       node, name, level, q, ds);
                accrej(q, ds);
            } else if (msg_info.MPI_TAG == REPORT) {
                int msg;
                MPI_Status msg_info;
                MPI_Recv(&msg, 1, MPI_INT, MPI_ANY_SOURCE, REPORT, MPI_COMM_WORLD, &msg_info);
                int w = msg;
                int q = msg_info.MPI_SOURCE;
                printf("[%lf] %d(name=%d level=%d) received report from %d with weight=%d\n", MPI_Wtime() - start_time,
                       node, name, level, q, w);
                reportRecv(q, w);
            } else if (msg_info.MPI_TAG == CHANGEROOT) {
                int msg;
                MPI_Status msg_info;
                MPI_Recv(&msg, 1, MPI_INT, MPI_ANY_SOURCE, REPORT, MPI_COMM_WORLD, &msg_info);
                printf("[%lf] %d(name=%d level=%d) received changeroot from %d\n", MPI_Wtime() - start_time, node, name,
                       level, msg_info.MPI_SOURCE);
                changeRoot();
            } else if (msg_info.MPI_TAG == TERMINATE) {
                int msg;
                MPI_Status msg_info;
                MPI_Recv(&msg, 1, MPI_INT, MPI_ANY_SOURCE, TERMINATE, MPI_COMM_WORLD, &msg_info);
                printf("[%lf] %d(name=%d level=%d) received terminate from %d\n", MPI_Wtime() - start_time, node, name,
                       level, msg_info.MPI_SOURCE);
                for (int i = 0; i < n; ++i) {
                    if (T(i) == 1 && N(i) != msg_info.MPI_SOURCE) {
                        int send = 0;
                        MPI_Request sent;
                        printf("[%lf] %d(name=%d level=%d) sending terminate to q=%d\n", MPI_Wtime() - start_time, node,
                               name, level, N(i));
                        MPI_Isend(&send, 1, MPI_INT, N(i), TERMINATE, MPI_COMM_WORLD, &sent);
                        MPI_Wait(&sent, MPI_STATUS_IGNORE);
                    }
                }
                break;
            }
        }
        if (waiting.size() > 0) {
            pair<int, vector<int>> element = waiting.front();
            waiting.pop();
            if (element.first == CONNECT) {
                printf("[%lf] %d(name=%d level=%d) resolving waiting connect with q=%d level=%d\n",
                       MPI_Wtime() - start_time, node, name, level, element.second[0], element.second[1]);
                connect(element.second[0], element.second[1]);
            } else if (element.first == REPORT) {
                printf("[%lf] %d(name=%d level=%d) resolving waiting report with q=%d bestWt=%d\n",
                       MPI_Wtime() - start_time, node, name, level, element.second[0], element.second[1]);
                reportRecv(element.second[0], element.second[1]);
            } else if (element.first == TEST) {
                printf("[%lf] %d(name=%d level=%d) resolving waiting test with q=%d level=%d name=%d\n",
                       MPI_Wtime() - start_time, node, name, level, element.second[0], element.second[1], element.second[2]);
                test(element.second[0], element.second[1], element.second[2]);
            }
        }
        if (halt) {
            break;
        }
    }
    for (int i = 0; i < n; ++i) {
        if (T(i) == 1 && N(i) > node) {
            cout << node << " " << N(i) << " " << W(i) << endl;
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}
