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
#include "Hierholzer.h"

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

bool is_eulerian(Data& data, std::vector<std::vector<int>>& adjacency)
{
    // Calculate in-degree for each vertex
    std::map<int, int> vertex_to_in_degree;

    for (int vertex_id = 0; vertex_id < data.nr_junctions; ++vertex_id)
        vertex_to_in_degree[vertex_id] = 0;

    for (int vertex_id = 0; vertex_id < data.nr_junctions; ++vertex_id)
    {
        for (int edge : adjacency[vertex_id])
        {
            vertex_to_in_degree[edge]++;
        }
    }

    std::map<int, int> excess_in, excess_out;
    for (int vertex_id = 0; vertex_id < data.nr_junctions; ++vertex_id)
    {
        const int out_degree = adjacency[vertex_id].size();
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

    for (auto& elem : excess_in)
        std::cout << elem.first << " " << elem.second;
    std::cout << std::endl;
    for (auto& elem : excess_out)
        std::cout << elem.first << " " << elem.second;

    return excess_in.empty() && excess_out.empty();
}

std::vector<std::pair<int, int>> call_hungarian_alg(Data& data)
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

//    int worker_index = 0;
//    for (const auto& [in_excess_vertex_id, in_excess] : excess_in)
//    {
//        for (int i = 0; i < in_excess; ++i)
//        {
//            // Memo
//            worker_to_vertex_id[worker_index] = in_excess_vertex_id;
//
//        }
//    }
    std::vector<int> worker_to_vertex_id, job_to_vertex_id;

    for (const auto& [vertex_id, excess] : excess_in)
        for (int times = 0; times < excess; ++times)
            worker_to_vertex_id.push_back(vertex_id);

    for (const auto& [vertex_id, excess] : excess_out)
        for (int times = 0; times < excess; ++times)
            job_to_vertex_id.push_back(vertex_id);

    std::vector<std::vector<int>> costs;
    // TODO
    for (int worker_id = 0; worker_id < worker_to_vertex_id.size(); ++worker_id)
    {
        std::vector<int> tmp;
        const int vertex_id = worker_to_vertex_id[worker_id];
        for (int job_id = 0; job_id < job_to_vertex_id.size(); ++job_id)
        {
            const int neighbor_id = job_to_vertex_id[job_id];
            tmp.push_back(data.shortest_dist[vertex_id][neighbor_id]);
        }
        costs.push_back(tmp);
    }
    std::cout << "Calling hungarian algorithm...\n";
    auto min_assignment = hungarianAlgorithm(costs); // The matching is with respect to the indices in the costs matrix
    cout << min_assignment.size() << std::endl;

    // So we gotta convert it into the indices in the actual graph
    std::vector<std::pair<int, int>> result;
    for (const auto& [worker_id, job_id] : min_assignment)
    {
        result.emplace_back(worker_to_vertex_id[worker_id], job_to_vertex_id[job_id]);
    }

    assert(result.size() == worker_to_vertex_id.size());
    assert(worker_to_vertex_id.size() == job_to_vertex_id.size());
    // Now to obtain the worker->job path for each matching
    // imaginary_edge = {worker_id, job_id} : path
    return result;
}

std::vector<std::vector<int>> combine_adjacency_lists(Data& data, const std::vector<std::pair<int, int>>& matchings)
{
    std::vector<std::vector<int>> adjacency(data.nr_junctions);

    // Insert the real edges
    for (int vertex_id = 0; vertex_id < data.nr_junctions; ++vertex_id)
    {
        for (auto& edge : data.adjacency[vertex_id])
            adjacency[vertex_id].push_back(edge.neighbor);
    }
    // Insert the 'imaginary' edges
    std::set<std::pair<int, int>> imaginary_edges;

    for (auto& [node_a, node_b] : matchings)
    {
        adjacency[node_a].push_back(node_b);
    }
    return adjacency;
}

std::vector<std::vector<int>> split_itinerary_on_cars(Data& data, const std::vector<int>& eulerian_circuit)
{
    std::vector<std::vector<int>> itinerary(MAX_CARS);

    int index_in_circuit = 0;
    for (int car_id = 0; car_id < MAX_CARS; ++car_id)
    {
        int current_cost = 0;

        if (car_id != 0)
            // Bagi drumu de la starting_junction la eulerian_circuit[index_in_circuit]
        {
            itinerary[car_id].push_back(data.starting_junction);
            itinerary[car_id].push_back(eulerian_circuit[index_in_circuit]);
            current_cost += data.shortest_dist[data.starting_junction][eulerian_circuit[index_in_circuit]];
        }
        else
            itinerary[car_id].push_back(data.starting_junction);

        while (current_cost < data.total_time)
        {
            if (current_cost + data.shortest_dist[index_in_circuit][index_in_circuit + 1] < data.total_time)
            {
                current_cost += data.shortest_dist[index_in_circuit][index_in_circuit + 1];

                index_in_circuit++;
                itinerary[car_id].push_back(eulerian_circuit[index_in_circuit]);
            }
            else
                break;
        }
    }

    std::cout << "Eulerian circuit starts with " << eulerian_circuit[0] << " and ends with " << eulerian_circuit[eulerian_circuit.size() - 1] << std::endl;
    std::cout << "We managed to cover " << index_in_circuit << " edges out of " << eulerian_circuit.size() << std::endl;
    return itinerary;
}

int main()
{
    const std::string input_filename = "../../hashcode_2014_final_round.in";
    const std::string output_filename = "../../hashcode_2014_final_round.out";

    Data data(input_filename);
    auto perfect_match = call_hungarian_alg(data);
    auto adj_list = combine_adjacency_lists(data, perfect_match);
    auto adj_list_copy = adj_list;
//    auto eulerian_circuit = get_eulerian_circuit(adj_list_copy, data.starting_junction);

    std::cout << is_eulerian(data, adj_list);

//    for (int i = 0; i < eulerian_circuit.size() - 1; ++i)
//    {
//        int node_a = eulerian_circuit[i];
//        int node_b = eulerian_circuit[i + 1];
//        auto it = std::find(adj_list[node_a].begin(), adj_list[node_a].end(), node_b);
//        if (it == adj_list[node_a].end())
//        {
//            std::cout << "WTF!!!" << node_a << " " << node_b << std::endl;
//        }
//    }
//    std::cout << "starting vertex: " << data.starting_junction;

//    auto itinerary = split_itinerary_on_cars(data, eulerian_circuit);

//    vector<vector<int>> adj = {{2, 3}, {0}, {1}, {4}, {0}};
//    auto circuit = get_eulerian_circuit(adj, 0);
//    for (int i : circuit)
//        std::cout << i << " ";
//    std::cout << std::endl;

//    std::cout << "Read data, we got " << data.nr_junctions << " " << data.nr_streets << std::endl;

//    const auto result = solve(data);
//    Data::write_to_file(output_filename, result);
    return 0;
}