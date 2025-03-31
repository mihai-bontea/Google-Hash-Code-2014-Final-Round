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
#include "HungarianAlgorithm.h"

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

void call_hungarian_alg(Data& data)
{
    // Calculate in-degree for each vertex
    std::map<int, int> vertex_to_in_degree;

    for (int vertex_id = 0; vertex_id < data.nr_junctions; ++vertex_id)
        vertex_to_in_degree[vertex_id] = 0;

    for (int vertex_id = 0; vertex_id < data.nr_junctions; ++vertex_id)
    {
        for (const auto& edge : data.adjacency[vertex_id])
        {
            vertex_to_in_degree[edge.neighbor]++;
        }
    }

    std::map<int, int> excess_in, excess_out;
    for (int vertex_id = 0; vertex_id < data.nr_junctions; ++vertex_id)
    {
        const int out_degree = data.adjacency[vertex_id].size();
        const int in_degree = vertex_to_in_degree[vertex_id];

        int diff = out_degree - in_degree;

        if (diff > 0)
        {
            excess_out[vertex_id] = diff;
        }
        else if (diff < 0)
        {
            excess_in[vertex_id] = -diff;
        }
    }

    std::cout << excess_in.size() << " with excess in, " << excess_out.size() << " with excess out\n";

    auto value_sum = [](const auto& container){
        int sum = 0;
        for (const auto& elem : container)
            sum += elem.second;
        return sum;
    };
    std::cout << value_sum(excess_in) << " " << value_sum(excess_out) << std::endl;

    // Build a cost matrix
    // On the left(workers) we got in_excess
    // On the right(jobs) we got out_excess

    // or give as input 2 vectors, one with workers(duplicated) and jobs(also duplicated)
    // Then have it return the ideal matchmaking
    // also have vertex ID : index in the matrix?
}

int main()
{
    const std::string input_filename = "../../hashcode_2014_final_round.in";
    const std::string output_filename = "../../hashcode_2014_final_round.out";

    Data data(input_filename);
    call_hungarian_alg(data);

//    data.make_eulerian();

    std::cout << "Read data, we got " << data.nr_junctions << " " << data.nr_streets << std::endl;

//    const auto result = solve(data);
//    Data::write_to_file(output_filename, result);
    std::cout << sizeof(int) << " " << sizeof(short) << std::endl;
    return 0;
}