#pragma once

#include <fstream>

#include "BS_thread_pool.hpp"
#include "InParser.h"

#include <omp.h>

#define MAX_VERTICES 11348
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

using Matrix = std::array<std::array<int, MAX_VERTICES>, MAX_VERTICES>;
using AdjacencyList = std::array<std::vector<Edge>, MAX_VERTICES>;

class ParallelFloydWarshall
{
public:
    ParallelFloydWarshall(Matrix& shortest_dist, Matrix& next_vertex, const AdjacencyList& adjacency)
    : shortest_dist(shortest_dist)
    , next_vertex(next_vertex)
    , adjacency(adjacency)
    {
        omp_set_num_threads(11);
        floyd_warshall();
    }
private:

    void floyd_warshall()
    {
//        std::ifstream fin(floyd_warshall_filename);
        InParser fin(floyd_warshall_filename.c_str());
        if (fin.file_exists())
//        if (fin)
        {
            std::cout << "Shortest distances already computed, reading from file...\n";
            auto start = std::chrono::high_resolution_clock::now();
            read_precomputed_distances_from_file(fin);

            auto stop = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
            std::cout << "File read in: " << duration.count() << " ms" << std::endl;

            return;
        }
        auto start = std::chrono::high_resolution_clock::now();

        // Initializing the distance and next_vertex matrices
        #pragma omp parallel for
        for (int vertex_id = 0; vertex_id < MAX_VERTICES; ++vertex_id)
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

        floyd_warshall_parallel();

        std::cout << "Finished computing shortest distances, writing results to file...\n";
        std::ofstream fout(floyd_warshall_filename);
        write_distances_to_file(fout);

        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
        std::cout << "Processing + writing time: " << duration.count() << " ms" << std::endl;
    }

    void floyd_warshall_parallel()
    {
        for (int k = 0; k < MAX_VERTICES; k++)
        {
            // Parallelize the row-wise updates
            #pragma omp parallel for
            for (int i = 0; i < MAX_VERTICES; i++)
            {
                for (int j = 0; j < MAX_VERTICES; j++)
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
    }

    Matrix& shortest_dist;
    Matrix& next_vertex;
    const AdjacencyList& adjacency;
    static std::string floyd_warshall_filename;

    void read_precomputed_distances_from_file(InParser& fin)
//    void read_precomputed_distances_from_file(std::ifstream& fin)
    {
        for (int i = 0; i < MAX_VERTICES; ++i)
            for (int j = 0; j < MAX_VERTICES; ++j)
                fin >> shortest_dist[i][j];

        for (int i = 0; i < MAX_VERTICES; ++i)
            for (int j = 0; j < MAX_VERTICES; ++j)
                fin >> next_vertex[i][j];
    }

    void write_distances_to_file(std::ofstream& fout)
    {
        for (int i = 0; i < MAX_VERTICES; ++i)
        {
            for (int j = 0; j < MAX_VERTICES; ++j)
                fout << shortest_dist[i][j] << " ";
            fout << '\n';
        }
        fout << '\n';
        for (int i = 0; i < MAX_VERTICES; ++i)
        {
            for (int j = 0; j < MAX_VERTICES; ++j)
                fout << next_vertex[i][j] << " ";
            fout << '\n';
        }
    }
};

std::string ParallelFloydWarshall::floyd_warshall_filename = "../shortest_distances.txt";