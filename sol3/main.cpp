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

int main()
{
    const std::string input_filename = "../../hashcode_2014_final_round.in";
    const std::string output_filename = "../../hashcode_2014_final_round.out";

    Data data(input_filename);
//    data.make_eulerian();

    std::cout << "Read data, we got " << data.nr_junctions << " " << data.nr_streets << std::endl;

//    const auto result = solve(data);
//    Data::write_to_file(output_filename, result);
    return 0;
}