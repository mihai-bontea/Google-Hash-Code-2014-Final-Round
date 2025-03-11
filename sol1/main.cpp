#include <bitset>
#include <chrono>
#include <cassert>
#include <vector>
#include <stack>
#include <random>
#include <iostream>
#include <algorithm>

#include "Data.h"

typedef std::tuple<int, std::vector<int>, std::bitset<MAX_VERTICES>> dijsktra_result;

struct PathDescription
{
    int current_node, cost, length;
    std::vector<int> path;
    std::bitset<MAX_VERTICES> visited;

    PathDescription(int current_node,
                    int cost,
                    int length,
                    const std::vector<int>& path,
                    const std::bitset<MAX_VERTICES>& visited)
        : current_node(current_node)
        , cost(cost)
        , length(length)
        , path(path)
        , visited(visited)
    {}
};

void randomize(std::vector<int>& elems)
{
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(elems.begin(), elems.end(), g);
}

dijsktra_result modified_dijsktra(Data &data,
                                  std::bitset<MAX_VERTICES>& visited_overall,
                                  int timeout_minutes)
{
    std::vector<int> best_path;
    int best_path_length = 0, best_path_cost = 0;
    std::bitset<MAX_VERTICES> best_path_visited;

    std::stack<PathDescription> stack;
    stack.emplace(data.starting_junction, 0, 0, std::vector{data.starting_junction}, visited_overall);

    auto start_time = std::chrono::steady_clock::now();
    while (!stack.empty())
    {
        const auto& [current_node, cost, length, path, visited] = stack.top();
        stack.pop();

        if (length > best_path_length)
        {
            best_path_length = length;
            best_path_cost = cost;
            best_path = path;
            best_path_visited = visited;
        }

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

        auto elapsed = std::chrono::steady_clock::now() - start_time;
        if (std::chrono::duration_cast<std::chrono::minutes>(elapsed).count() >= timeout_minutes)
        {
            std::cout << "Timeout reached! Stopping..." << std::endl;
            break;
        }
    }

    assert(best_path_cost <= data.total_time);
    return {best_path_length, best_path, best_path_visited};
}

std::vector<std::vector<int>> solve(Data &data)
{
    std::vector<std::vector<int>> car_paths;
    std::bitset<MAX_VERTICES> visited_overall;
    unsigned long long total_length = 0;

    std::array<int, MAX_CARS> timeout_times = {3, 3, 3, 4, 4, 4, 5, 6};
    for (int car_index = 0; car_index < data.nr_cars; ++car_index)
    {
        auto [path_length, path, visited] = modified_dijsktra(data, visited_overall, timeout_times[car_index]);

        std::cout << "Obtained a path of length " << path_length << " for car " << car_index << '\n';

        // Updating the visited nodes(junctions)
        visited_overall |= visited;

        total_length += path_length;

        car_paths.push_back(path);
    }
    std::cout << "Total score is " << total_length << '\n';
    return car_paths;
}

int main()
{
    const std::string input_filename = "../../hashcode_2014_final_round.in";
    Data data(input_filename);

    std::cout << "Read data, we got " << data.nr_junctions << " " << data.nr_streets << std::endl;

    auto result = solve(data);
    return 0;
}
