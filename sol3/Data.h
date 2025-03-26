#include <map>
#include <fstream>
#include <vector>
#include <array>
#include <algorithm>
#include <cmath>
#include <cassert>
#include <iostream>
#include <limits>

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
        // main floyd-warshall
        for (int k = 0; k < nr_junctions; ++k)
        {
            for (int i = 0; i < nr_junctions; ++i)
            {
                for (int j = 0; j < nr_junctions; ++j)
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
};

std::array<std::vector<Edge>, MAX_VERTICES> Data::adjacency;
std::array<std::array<int, MAX_VERTICES>, MAX_VERTICES> Data::shortest_dist;
std::array<std::array<int, MAX_VERTICES>, MAX_VERTICES> Data::next_vertex;