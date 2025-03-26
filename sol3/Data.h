#include <map>
#include <fstream>
#include <vector>
#include <array>
#include <algorithm>
#include <cmath>
#include <cassert>
#include <iostream>
#include <limits>

#include "BS_thread_pool.hpp"

#define MAX_VERTICES 11348
//#define MAX_VERTICES 11
#define MAX_CARS 8
#define NO_PATH -1
constexpr int INF = std::numeric_limits<int>::max();

struct Edge
{
    int neighbor, cost, length;

    Edge(int neighbor, int cost, int length)
            : neighbor(neighbor)
            , cost(cost)
            , length(length)
    {}
};

struct Data
{
    int nr_junctions, nr_streets, total_time, nr_cars, starting_junction;
    static std::array<std::vector<Edge>, MAX_VERTICES> adjacency;
    static std::array<std::array<int, MAX_VERTICES>, MAX_VERTICES> shortest_dist;
    static std::array<std::array<int, MAX_VERTICES>, MAX_VERTICES> next_vertex;
    static std::string floyd_warshall_filename;

    explicit Data(const std::string& filename)
    {
        std::ifstream fin(filename);

        if (!fin)
        {
            std::cerr << "Error opening file: " << filename << std::endl;
            return;
        }

        fin >> nr_junctions >> nr_streets >> total_time >> nr_cars >> starting_junction;

        // Read the coords of the junctions
        float x, y;
        for (int index = 0; index < nr_junctions; ++index)
            fin >> x >> y;

        // Read the streets
        int node_a, node_b, type, cost, length;
        for (int index = 0; index < nr_streets; ++index)
        {
            fin >> node_a >> node_b >> type >> cost >> length;

            adjacency[node_a].emplace_back(node_b, cost, length);
            if (type == 2)
                adjacency[node_b].emplace_back(node_a, cost, length);
        }
        floyd_warshall();
    }

    static void write_to_file(const std::string& filename, const std::vector<std::vector<int>>& car_paths)
    {
        std::ofstream fout(filename);

        // Number of cars used
        fout << car_paths.size() << '\n';
        for (const auto& car : car_paths)
        {
            // Number of junctions visited by car
            fout << car.size() << '\n';
            for (int junction : car)
                fout << junction << '\n';
        }
    }
private:
    void floyd_warshall()
    {
        std::ifstream fin(floyd_warshall_filename);
        if (fin)
        {
            std::cout << "Shortest distances already computed, reading from file...\n";
            read_precomputed_distances_from_file(fin);
            return;
        }

        // Initializing the distance and next_vertex matrices
        for (int vertex_id = 0; vertex_id < nr_junctions; ++vertex_id)
        {
            std::fill(shortest_dist[vertex_id].begin(), shortest_dist[vertex_id].end(), INF);
            std::fill(next_vertex[vertex_id].begin(), next_vertex[vertex_id].end(), NO_PATH);

            shortest_dist[vertex_id][vertex_id] = 0;
            next_vertex[vertex_id][vertex_id] = vertex_id;

            for (const auto& edge : adjacency[vertex_id])
            {
                shortest_dist[vertex_id][edge.neighbor] = edge.cost;
                next_vertex[vertex_id][edge.neighbor] = edge.neighbor;
            }
        }
        const int nr_threads = 11;
        int chunk_size = nr_junctions / nr_threads;
        BS::thread_pool pool(nr_threads);
        for (int k = 0; k < nr_junctions; ++k)
        {
            std::vector<std::future<void>> worker_futures;

            for (int t = 0; t < nr_threads; ++t)
            {
                int start = t * chunk_size;
                int end = (t == nr_threads - 1)? nr_junctions : (t + 1) * chunk_size;
                worker_futures.push_back(pool.submit_task([&](){return floyd_warshall_worker(start, end, k);}));
            }
            for (auto& future : worker_futures)
                future.get();
        }

        std::cout << "Finished computing shortest distances, writing results to file...\n";
        std::ofstream fout(floyd_warshall_filename);
        write_distances_to_file(fout);
    }

    void floyd_warshall_worker(int start, int end, int k)
    {
        for (int i = start; i < end; i++)
        {
            for (int j = 0; j < nr_junctions; j++)
            {
                if (shortest_dist[i][k] != INF && shortest_dist[k][j] != INF)
                {
                    if (shortest_dist[i][j] > shortest_dist[i][k] + shortest_dist[k][j])
                    {
                        shortest_dist[i][j] = shortest_dist[i][k] + shortest_dist[k][j];
                        next_vertex[i][j] = next_vertex[i][k];
                    }
                }
            }
        }
    }

    void read_precomputed_distances_from_file(std::ifstream& fin)
    {
        for (int i = 0; i < nr_junctions; ++i)
            for (int j = 0; j < nr_junctions; ++j)
                fin >> shortest_dist[i][j];

        for (int i = 0; i < nr_junctions; ++i)
            for (int j = 0; j < nr_junctions; ++j)
                fin >> next_vertex[i][j];
    }

    void write_distances_to_file(std::ofstream& fout)
    {
        for (int i = 0; i < nr_junctions; ++i)
        {
            for (int j = 0; j < nr_junctions; ++j)
                fout << shortest_dist[i][j] << " ";
            fout << '\n';
        }
        fout << '\n';
        for (int i = 0; i < nr_junctions; ++i)
        {
            for (int j = 0; j < nr_junctions; ++j)
                fout << next_vertex[i][j] << " ";
            fout << '\n';
        }
    }
};

std::array<std::vector<Edge>, MAX_VERTICES> Data::adjacency;
std::array<std::array<int, MAX_VERTICES>, MAX_VERTICES> Data::shortest_dist;
std::array<std::array<int, MAX_VERTICES>, MAX_VERTICES> Data::next_vertex;
std::string Data::floyd_warshall_filename = "../shortest_distances.txt";