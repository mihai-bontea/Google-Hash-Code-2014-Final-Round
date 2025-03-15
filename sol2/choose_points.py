import random
import math
import numpy as np

input_file = "../hashcode_2014_final_round.in"

def euclidean_distance(node_a, node_b):
    return math.sqrt((node_a[0] - node_b[0])**2 + (node_a[1] - node_b[1])**2)
                   
def greedy_max_min_selection(nodes, k):
    selected = [random.choice(nodes)]
    
    while len(selected) < k:
        best_node = None
        max_min_dist = -1
        
        for node in nodes:
            if node in selected:
                continue
            
            min_dist = min(euclidean_distance(node, s) for s in selected)
            
            if min_dist > max_min_dist:
                max_min_dist = min_dist
                best_node = node
        
        selected.append(best_node)
    
    return selected

with open(input_file, "r") as in_file:
    nr_junctions, nr_streets, total_time, nr_cars, starting_junction = list(map(int, (in_file.readline()).split()))
    
    # Read the junction coords
    coords = [tuple(map(float, in_file.readline().split())) for _ in range(nr_junctions)]
    
    # Read the streets
    streets = {}
    for i in range(nr_streets):
        node_a, node_b, direction, cost, length = list(map(int, (in_file.readline()).split()))
        streets[(node_a, node_b)] = (cost, length)
        if (direction == 2):
            streets[(node_b, node_a)] = (cost, length)

    selected_coords = greedy_max_min_selection(coords, 8)
    print(f"The selected junction coords are: {selected_coords}")
    selected_nodes = [coords.index(coord) for coord in selected_coords]
    print(f"Their indices are: {selected_nodes}")