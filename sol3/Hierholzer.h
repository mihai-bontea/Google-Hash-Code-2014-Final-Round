#pragma once
#include <vector>
#include <algorithm>

std::vector<std::vector<int>> combine_adjacency_lists()
{

}

std::vector<int> get_eulerian_circuit(std::vector<std::vector<int>> &adjacency, int starting_vertex) {
    int n = adjacency.size();

    if (n == 0)
        return {};

    std::vector<int> current_path;
    current_path.push_back(starting_vertex);

    std::vector<int> result;

    while (!current_path.empty())
    {
        int currNode = current_path[current_path.size() - 1];

        if (!adjacency[currNode].empty())
        {

            int nextNode = adjacency[currNode].back();
            adjacency[currNode].pop_back();

            current_path.push_back(nextNode);
        }
        else
        {
            result.push_back(current_path.back());
            current_path.pop_back();
        }
    }

    std::reverse(result.begin(), result.end());
    return result;
}