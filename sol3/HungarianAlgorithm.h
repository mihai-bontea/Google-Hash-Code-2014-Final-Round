#pragma once
#include <iostream>
#include <vector>
#include <limits>

using namespace std;

//const int INF = numeric_limits<int>::max();

// build the cost matrix
// from the in-excess -> out-excess
// duplicate rows/columns where needed(excess greater than 1)



void hungarianAlgorithm(vector<vector<int>>& cost)
{
    int n = cost.size();
    vector<int> u(n, 0), v(n, 0), p(n, 0), way(n, 0);

    for (int i = 0; i < n; ++i)
    {
        p[0] = i;
        int j0 = 0;
        vector<int> minv(n, INF);
        vector<bool> used(n, false);

        do
        {
            used[j0] = true;
            int i0 = p[j0], delta = INF, j1;
            for (int j = 1; j < n; ++j)
            {
                if (!used[j])
                {
                    int cur = cost[i0][j] - u[i0] - v[j];
                    if (cur < minv[j])
                    {
                        minv[j] = cur;
                        way[j] = j0;
                    }
                    if (minv[j] < delta)
                    {
                        delta = minv[j];
                        j1 = j;
                    }
                }
            }
            for (int j = 0; j < n; ++j)
            {
                if (used[j])
                    u[p[j]] += delta;
                else
                    minv[j] -= delta;
            }
            j0 = j1;
        } while (p[j0] != 0);

        do
        {
            int j1 = way[j0];
            p[j0] = p[j1];
            j0 = j1;
        } while (j0);
    }

    for (int j = 1; j < n; ++j)
    {
        cout << "Agent " << p[j] << " assigned to task " << j << endl;
    }
}

//int main() {
//    vector<vector<int>> cost = {
//            {4, 2, 8},
//            {2, 4, 6},
//            {8, 6, 4}
//    };
//
//    hungarianAlgorithm(cost);
//    return 0;
//}