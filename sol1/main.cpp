#include <iostream>
#include <vector>
#include <stack>

#include <algorithm>
#include <random>
#include <bitset>

#define MAX_VERTICES 11348

struct PathDescription
{
    int current_node, cost, length;
    std::vector<int> path;
    std::bitset<MAX_VERTICES> visited;

    PathDescription(int current_node, int cost, int length, const std::vector<int>& path, const std::bitset<MAX_VERTICES>& visited)
        : current_node(current_node)
        , cost(cost)
        , length(length)
        , path(path)
        , visited(visited)
    {
    }
};

std::vector<int> best_path;
int best_length;

void randomize(std::vector<int>& elems)
{
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(elems.begin(), elems.end(), g);
}

std::vector<int> backtrack(int start_node, int max_cost)
{
    std::stack<PathDescription> stack;
    stack.emplace(start_node, 0, 0, std::vector{start_node});

    bool timer = true;
    while (!stack.empty() && timer)
    {
        const auto& [current_node, cost, length, path, visited] = stack.top();
        stack.pop();

        if (length > best_length)
        {
            best_length = length;
            best_path = path;
        }

        // Maybe randomize, if threaded will fit into RAM
        auto adjacency_list = graph.get(current_node);
        randomize(adjacency_list);

        for (auto& [neighbor, edge_length, edge_cost] : adjacency_list)   // TODO
        {
            int new_cost = cost + edge_cost;
            int new_length = length + (visited[neighbor])? 0 : edge_length;

            if (new_cost <= max_cost)
            {
                auto path_copy = path;
                path.push_back(neighbor);

                auto visited_copy = visited;
                visited[neighbor] = true;

                stack.emplace(neighbor, new_cost, new_length, path_copy, visited_copy);
            }
        }
    }
}

int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
