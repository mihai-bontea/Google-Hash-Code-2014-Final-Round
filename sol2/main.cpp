#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include <queue>
#include <limits>
#include <cassert>
#include <algorithm>

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

    // Distance map, initialized to "infinity"
    std::unordered_map<int, int> distances;
    std::unordered_map<int, int> predecessors;

    for (int junction = 0; junction < data.nr_junctions; ++junction) {
        distances[junction] = std::numeric_limits<int>::max();
    }

    // Set the starting node distance to 0
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
            int newDist = dist + cost;

            if (newDist < distances[neighbor])
            {
                distances[neighbor] = newDist;
                predecessors[neighbor] = node;
                pq.emplace(newDist, neighbor);
            }
        }
    }

    assert(distances[end] != std::numeric_limits<int>::max());

    std::vector<int> path;
    for (int at = end; at != start; at = predecessors[at])
    {
        path.push_back(at);
    }
    std::reverse(path.begin(), path.end());

    return {distances[end], path};
}

std::vector<std::vector<int>> solve(Data &data)
{
    std::array<int, MAX_CARS> starting_junctions = {10210, 2509, 1532, 1331, 1720, 3057, 8762, 6511};
    for (int car_index = 0; car_index < data.nr_cars; ++car_index)
    {
        auto [distance, path] = dijkstra(data, 0, starting_junctions[car_index]);
        std::cout << "For car " << car_index << " the distance is " << distance << std::endl;
    }

    std::vector<std::vector<int>> car_paths;
//    std::set<std::pair<int, int>> visited_overall;
//    unsigned long long total_length = 0;
//
//    std::array<int, MAX_CARS> timeout_minutes = {3, 3, 3, 4, 4, 4, 5, 6};
//    for (int car_index = 0; car_index < data.nr_cars; ++car_index)
//    {
//        auto [path_length, path, visited] = modified_dijsktra(data, visited_overall, timeout_minutes[car_index]);
//
//        std::cout << "Obtained a path of length " << path_length << " for car " << car_index << '\n';
//
//        // Updating the visited nodes(junctions)
//        visited_overall.insert(visited.begin(), visited.end());
//
//        total_length += path_length;
//
//        if (!path.empty())
//            car_paths.push_back(path);
//    }
//    std::cout << "Total score is " << total_length << '\n';
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