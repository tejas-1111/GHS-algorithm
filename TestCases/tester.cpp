#include <iostream>
#include <bits/stdc++.h>
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
    int n, arr[100][100];
    vector<pair<int, pair<int, int>>> edges;
    vector<pair<int, pair<int, int>>> selected_edges;
    cin >> n;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            cin >> arr[i][j];
            if (j > i && arr[i][j] < 100000)
            {
                edges.push_back({arr[i][j], {i, j}});
            }
        }
    }
    for (int i = 0; i < n; i++)
    {
        par[i] = i;
        sz[i] = 1;
    }
    int cnt = 0, edge_selected = 0;
    sort(edges.begin(), edges.end());
    for (int i = 0; i < edges.size(); i++)
    {
        edge_selected = merge_set(edges[i].second.first, edges[i].second.second);
        cnt += edge_selected;
        if (edge_selected == 1)
            selected_edges.push_back(edges[i]);
        if (cnt == n - 1)
            break;
    }
    for (int i = 0; i < selected_edges.size(); i++)
    {
        cout << selected_edges[i].second.first << " " << selected_edges[i].second.second << " " << selected_edges[i].first << "\n";
    }
}