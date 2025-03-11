#include <iostream>
#include <vector>
#include <stack>

#include <algorithm>
#include <random>
#include <bitset>



#include "Data.h"

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

std::vector<int> backtrack(Data &data)
{
    std::stack<PathDescription> stack;
    stack.emplace(data.starting_junction, 0, 0, std::vector{data.starting_junction}, std::bitset<MAX_VERTICES>());

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
//        auto adjacency_list = graph.get(current_node);
//        randomize(adjacency_list);

        for (auto [neighbor, edge_cost, edge_length] : data.adjacency[current_node])
        {
            int new_cost = cost + edge_cost;
            int new_length = length + ((visited[neighbor])? 0 : edge_length);

            if (new_cost <= data.total_time)
            {
                auto path_copy = path;
                path_copy.push_back(neighbor);

                auto visited_copy = visited;
                visited_copy[neighbor] = true;

                stack.emplace(neighbor, new_cost, new_length, path_copy, visited_copy);
            }
        }
    }
    return best_path;
}

int main()
{
    const std::string input_filename = "../../hashcode_2014_final_round.in";
    Data data(input_filename);

    std::cout << "Read data, we got " << data.nr_junctions << " " << data.nr_streets << std::endl;

    auto result = backtrack(data);
    return 0;
}
