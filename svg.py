from xml.dom.minidom import Document

colors = ["black", "crimson", "navy", "forestgreen", "darkviolet", "orange", "magenta", "turquoise", "brown", "charcoal"]

input_file = "hashcode_2014_final_round.in"
output_file = "hashcode_2014_final_round.out"

def generate_svg(filename, junction_coords, streets, car_to_streets):
    minx = min(j[0] for j in junction_coords)
    maxx = max(j[0] for j in junction_coords)
    miny = min(j[1] for j in junction_coords)
    maxy = max(j[1] for j in junction_coords)

    scale = 10000
    doc = Document()
    svg_elem = doc.createElementNS("http://www.w3.org/2000/svg", "svg")
    svg_elem.setAttribute("width", str((maxy - miny) * scale))
    svg_elem.setAttribute("height", str((maxx - minx) * scale))
    svg_elem.setAttribute("xmlns", "http://www.w3.org/2000/svg")

    doc.appendChild(svg_elem)

    g_elem = doc.createElement("g")
    svg_elem.appendChild(g_elem)
    
    for street in streets.keys():
        line_elem = doc.createElement("line")
        node_a, node_b = street
        
        start_coords = junction_coords[node_a]
        end_coords = junction_coords[node_b]
        
        line_elem.setAttribute("x1", str((start_coords[1] - miny) * scale))
        line_elem.setAttribute("y1", str((maxx - start_coords[0]) * scale))
        line_elem.setAttribute("x2", str((end_coords[1] - miny) * scale))
        line_elem.setAttribute("y2", str((maxx - end_coords[0]) * scale))
        
        car_index = get_car_index_of_street_(street, car_to_streets)
        if car_index:
            line_elem.setAttribute("stroke", colors[car_index])
            line_elem.setAttribute("stroke-width", "1")
        else:
            line_elem.setAttribute("stroke", "gray")
            line_elem.setAttribute("stroke-width", "1")
        
        g_elem.appendChild(line_elem)
    
    with open(f"{filename}.svg", "w", encoding="utf-8") as f:
        f.write(doc.toprettyxml(indent="  "))


def get_car_index_of_street_(street, car_to_streets):
    for car_index in car_to_streets.keys():
        if street in car_to_streets[car_index]:
            return car_index
    return None

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

    # Get all the streets visited
    with open(output_file, "r") as out_file:
        nr_cars_used = int(out_file.readline())
        car_to_streets = {key: set() for key in range(nr_cars_used)}

        for car_index in range(nr_cars_used):
            nr_junctions_visited = int(out_file.readline())
            junctions_visited = [int(out_file.readline()) for _ in range(nr_junctions_visited)]

            # Each two consecutive junctions means a street
            for junction_index in range(nr_junctions_visited - 1):
                node_a = junctions_visited[junction_index]
                node_b = junctions_visited[junction_index + 1]
                car_to_streets[car_index].add((node_a, node_b))

        generate_svg("paths", coords, streets, car_to_streets)
    


            
