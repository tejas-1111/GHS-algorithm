# include <iostream>
# include "mpi.h"

// state_pi: includes state for each branch
// adjlist: adjacency for a given node i. 1-D of pair of integers

vector<pair<long, long> > adjlist;  //1: weight 2: vertex
vector<int> status; //status for each edge for this node. 0: basic, -1: reject, 1: branch 
int root;   // root for the given fragment this node is present in
int level;  // current level of the fragment this node is present in
int state;  // state for given node. 0: sleeping, 1: find, 2: found
int father;  // parent for this given node in the fragment tree. 
int bestch; // index of the best outgoing edge for this given node in adjlist. 
int bestwt; // weight of the best outgoing edge for this given node. 
//------------------------------------------------------------------------
/*
tag: message type
1. initiate
2. report 
*/
//------------------------------------------------------------------------
void initiate(int rroot, int rlevel, int rstate, int rsent_from)
{
    // do initalise
    level = rlevel;
    name = rroot;
    state = rstate;
    father = rsent_from;
    bestch = -1;
    bestwt = 1e9 + 7;
    int rec = 0;
    int branch_edges = 0;
    for (auto i : adjlist)
    {
        if (status[i] == 1)
        {
            branch_edges++;
            // mpi send the initiate message to the child
        }
    }
    // find your own minimum outgoing edge
    if (state == 1)
    {
        while (branch_edges != rec_p)
        {
            // wait till all the children give reply and increment rec_p 
        }
    }
    // send to parent
}

void connect(int rlevel, int rsent_from)
{
    if (level > rlevel)
    {
        status[rsent_from]
    }
}
