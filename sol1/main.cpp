#include <chrono>
#include <cassert>
#include <vector>
#include <stack>
#include <queue>
#include <random>
#include <iostream>
#include <algorithm>
#include <set>

#include "Data.h"

typedef std::tuple<int, std::vector<int>, std::set<std::pair<int, int>>> dijsktra_result;

struct PathDescription
{
    int current_node, cost, length;
    std::vector<int> path;
    std::set<std::pair<int, int>> visited;

    PathDescription(int current_node,
                    int cost,
                    int length,
                    const std::vector<int>& path,
                    const std::set<std::pair<int, int>>& visited)
        : current_node(current_node)
        , cost(cost)
        , length(length)
        , path(path)
        , visited(visited)
    {}

    bool operator<(const PathDescription& other) const
    {
        return length < other.length;
    }
};

void randomize(std::vector<int>& elems)
{
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(elems.begin(), elems.end(), g);
}

dijsktra_result modified_dijsktra(Data &data,
                                  std::set<std::pair<int, int>>& visited_overall,
                                  int timeout_minutes)
{
    std::vector<int> best_path;
    int best_path_length = 0, best_path_cost = 0;
    std::set<std::pair<int, int>> best_path_visited;

    std::priority_queue<PathDescription> pqueue;

    pqueue.emplace(data.starting_junction, 0, 0, std::vector{data.starting_junction}, visited_overall);

    auto start_time = std::chrono::steady_clock::now();
    while (!pqueue.empty())
    {
        const auto [current_node, cost, length, path, visited] = pqueue.top();
        pqueue.pop();

        if (length > best_path_length)
        {
            best_path_length = length;
            best_path_cost = cost;
            best_path = path;
            best_path_visited = visited;
        }

        for (auto [neighbor, edge_cost, edge_length] : data.adjacency[current_node])
        {
            const int new_cost = cost + edge_cost;
            const bool is_street_visited = ((visited.count({current_node, neighbor}) +
                    visited.count({neighbor, current_node})) != 0);
            const int new_length = length + ((is_street_visited)? 0 : edge_length);

            if (new_cost <= data.total_time)
            {
                auto path_copy = path;
                path_copy.push_back(neighbor);

                auto visited_copy = visited;
                visited_copy.insert({current_node, neighbor});

                pqueue.emplace(neighbor, new_cost, new_length, path_copy, visited_copy);

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
    std::set<std::pair<int, int>> visited_overall;
    unsigned long long total_length = 0;

    std::array<int, MAX_CARS> timeout_minutes = {3, 3, 3, 4, 4, 4, 5, 6};
    for (int car_index = 0; car_index < data.nr_cars; ++car_index)
    {
        auto [path_length, path, visited] = modified_dijsktra(data, visited_overall, timeout_minutes[car_index]);

        std::cout << "Obtained a path of length " << path_length << " for car " << car_index << '\n';

        // Updating the visited nodes(junctions)
        visited_overall.insert(visited.begin(), visited.end());

        total_length += path_length;

        if (!path.empty())
            car_paths.push_back(path);
    }
    std::cout << "Total score is " << total_length << '\n';
    return car_paths;
}

int main()
{
    const std::string input_filename = "../../hashcode_2014_final_round.in";
    const std::string output_filename = "../../hashcode_2014_final_round.out";

    Data data(input_filename);

    std::cout << "Read data, we got " << data.nr_junctions << " " << data.nr_streets << std::endl;

    const auto result = solve(data);
    Data::write_to_file(output_filename, result);
    return 0;
}
