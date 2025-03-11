#include <fstream>
#include <vector>
#include <array>

#define MAX_VERTICES 11348

struct Edge
{
    int neighbor, cost, length;

    Edge(int neighbor, int cost, int length)
        : neighbor(neighbor)
        , cost(cost)
        , length(length)
    {
    }
};

struct Data
{
    int nr_junctions, nr_streets, total_time, nr_cars, starting_junction;
    std::array<std::vector<Edge>, MAX_VERTICES> adjacency;

    explicit Data(const std::string& filename)
    {
        std::ifstream fin(filename);
        fin >> nr_junctions >> nr_streets >> total_time >> nr_cars >> starting_junction;

        // Read the coords of the junctions
        float x, y;
        for (int index = 0; index < nr_junctions; ++index)
            fin >> x >> y;

        // Read the streets
        int node_a, node_b, type, cost, length;
        for (int index = 0; index < nr_streets; ++index)
        {
            fin >>node_a >> node_b >> type >> cost >> length;

            adjacency[node_a].emplace_back(node_b, cost, length);
            if (type == 2)
                adjacency[node_b].emplace_back(node_a, cost, length);
        }
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
};