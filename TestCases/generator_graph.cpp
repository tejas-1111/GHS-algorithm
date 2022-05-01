#include <iostream>
#include <bits/stdc++.h>
#include <time.h>
using namespace std;
int par[100], sz[100];

int find_parent(int u)
{
    if (u == par[u])
        return u;
    return par[u] = find_parent(par[u]);
}

int merge_set(int u, int v)
{
    u = find_parent(u);
    v = find_parent(v);
    if (u != v)
    {
        if (sz[u] < sz[v])
            swap(u, v);
        par[v] = u;
        sz[u] += sz[v];
        return 1;
    }
    else
        return 0;
}

int main()
{
    int n;
    cin >> n;
    int prev_val = 1, cnt = 0;
    int values_picked[9000] = {0};
    vector<pair<int, pair<int, int>>> edges;
    for (int i = 0; i < n; i++)
    {
        par[i] = i;
        sz[i] = 1;
    }
    srand(time(0));
    while (1)
    {
        int a = rand() % n;
        int b = rand() % n;
        if (a == b)
            continue;
        int edge_added = merge_set(a, b);
        cnt += edge_added;
        if (edge_added == 0)
            continue;
        int val = prev_val + 1 + rand() % 5;
        values_picked[val] = 1;
        edges.push_back({val, {a, b}});
        prev_val = val;
        if (cnt == n - 1)
            break;
    }
    int adjM[n][n];
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
            adjM[i][j] = 100000;
    }
    for (int i = 0; i < edges.size(); i++)
    {
        adjM[edges[i].second.first][edges[i].second.second] = edges[i].first;
        adjM[edges[i].second.second][edges[i].second.first] = edges[i].first;
    }
    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            if (adjM[i][j] == 100000)
            {
                if (rand() % 2 == 0)
                {
                    int jumpTemp = 1 + rand() % 4;
                    for (int k = 5; k < 9000; k += jumpTemp)
                    {
                        if (values_picked[k] == 0)
                        {
                            adjM[i][j] = k;
                            adjM[j][i] = k;
                            values_picked[k] = 1;
                            break;
                        }
                    }
                }
            }
        }
    }
    cout << n << "\n";
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            cout << adjM[i][j] << " ";
        }
        cout << "\n";
    }
}