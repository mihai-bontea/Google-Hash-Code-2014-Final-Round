
input_file = "hashcode_2014_final_round.in"
output_file = "hashcode_2014_final_round.out"

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

    # Get all the unique streets visited
    streets_visited = set()
    with open(output_file, "r") as out_file:
        nr_cars_used = int(out_file.readline())

        for car_index in range(nr_cars_used):
            nr_junctions_visited = int(out_file.readline())
            junctions_visited = [int(out_file.readline()) for _ in range(nr_junctions_visited)]

            # Each two consecutive junctions means a street
            for junction_index in range(nr_junctions_visited - 1):
                node_a = junctions_visited[junction_index]
                node_b = junctions_visited[junction_index + 1]
                streets_visited.add((node_a, node_b))
    
    # The total score is the sum of unique streets' length
    score = sum(length for _, length in streets.values())
    print(f"Total score: {score}")
    


            
