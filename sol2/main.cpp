#include <set>
#include <map>
#include <queue>
#include <vector>
#include <limits>
#include <chrono>
#include <cassert>
#include <iostream>
#include <algorithm>
#include <unordered_map>

#include "Data.h"

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

typedef std::pair<int, std::vector<int>> simple_path;
typedef std::tuple<int, std::vector<int>, std::set<std::pair<int, int>>> dijsktra_result;

simple_path dijkstra(Data& data, int start, int end) {
    auto min_heap_comp = [](const auto& lhs, const auto& rhs){
        return lhs > rhs;
    };

    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, decltype(min_heap_comp)> pq;

    std::unordered_map<int, int> distances;
    std::unordered_map<int, int> predecessors;

    for (int junction = 0; junction < data.nr_junctions; ++junction)
    {
        distances[junction] = std::numeric_limits<int>::max();
    }

    distances[start] = 0;
    pq.emplace(0, start);

    while (!pq.empty())
    {
        auto [dist, node] = pq.top();
        pq.pop();

        if (node == end)
            break;

        for (auto &[neighbor, cost, _]: data.adjacency[node])
        {
            int new_dist = dist + cost;

            if (new_dist < distances[neighbor])
            {
                distances[neighbor] = new_dist;
                predecessors[neighbor] = node;
                pq.emplace(new_dist, neighbor);
            }
        }
    }

    assert(distances[end] != std::numeric_limits<int>::max());
    assert(distances[end] <= data.total_time);

    std::vector<int> path;
    for (int at = end; at != start; at = predecessors[at])
    {
        path.push_back(at);
    }
    std::reverse(path.begin(), path.end());

    return {distances[end], path};
}

int determine_length_and_update_visited(Data& data,
                                        const std::vector<int>& path,
                                        std::set<std::pair<int, int>>& visited_overall)
{
    int init_length = 0;
    for (int path_index = 0; path_index < path.size() - 1; ++path_index)
    {
        const int node_a = path[path_index];
        const int node_b = path[path_index + 1];

        // Find the edge {node_a, node_b} in the adjacency list
        const auto edge_it = std::find_if(data.adjacency[node_a].begin(),
                                    data.adjacency[node_a].end(),
                                    [&](const auto& edge) {
                                            return edge.neighbor == node_b;
                                    });

        // This should always be true if the path produced is valid
        assert(edge_it != data.adjacency[node_a].end());

        const auto it1 = visited_overall.find({node_a, node_b});
        const auto it2 = visited_overall.find({node_b, node_a});
        if (it1 == visited_overall.end() && it2 == visited_overall.end())
        {
            init_length += edge_it->length;
            visited_overall.emplace(node_a, node_b);
        }
    }
    return init_length;
}

dijsktra_result modified_dijsktra(Data &data,
                                  std::set<std::pair<int, int>>& visited_overall,
                                  int init_cost,
                                  int init_length,
                                  int starting_junction,
                                  const std::vector<int>& init_path,
                                  int timeout_minutes)
{
    std::vector<int> best_path;
    int best_path_length = 0, best_path_cost = 0;
    std::set<std::pair<int, int>> best_path_visited;

    std::priority_queue<PathDescription> pqueue;

    pqueue.emplace(starting_junction, init_cost, init_length, init_path, visited_overall);

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
    std::array<int, MAX_CARS> starting_junctions = {10210, 2509, 1532, 1331, 1720, 3057, 8762, 6511};
    for (int car_index = 0; car_index < data.nr_cars; ++car_index)
    {
        // Determine the lowest cost path from junction 0 to starting_junctions[car]
        auto [init_cost, init_path] = dijkstra(data, 0, starting_junctions[car_index]);
        int init_length = determine_length_and_update_visited(data, init_path, visited_overall);

        // Determine the best path starting from starting_junctions[car]
        auto [path_length, path, visited] = modified_dijsktra(data,
                                                              visited_overall,
                                                              init_cost,
                                                              init_length,
                                                              init_path[init_path.size() - 1],
                                                              init_path,
                                                              timeout_minutes[car_index]);

        std::cout << "Obtained a path of length " << path_length << " for car " << car_index << '\n';

        // Updating the visited nodes(junctions) unless it's the last car
        if (car_index != data.nr_cars - 1)
        {
            visited_overall.insert(visited.begin(), visited.end());
        }
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