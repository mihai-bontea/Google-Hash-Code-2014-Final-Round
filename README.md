# Google Hash Code 2014 Final Round (Street View Routing)

## Task

Given a description of city streets(junctions, time to traverse, and length), and a number of street view cars, schedule the movement of cars to maximize
the total length of city streets that are traversed at least once, in a certain amount of time.

## Solution 1

A modified version of Dijsktra's algorithm is used for each car. A priority queue holds objects of the type **{node, cost, length, path, visited}**, and they are ordered
based on length. Junctions and streets can be traversed more than once, but no extra points are awarded for visiting the same street twice. Despite that, the algorithm allows
it, as a previously visited street(by a different car) could block the new car from accessing a section of the map with unvisited streets. The process is time-consuming,
so a timer has been added to the main function. Generally, the longer the timer, the better the score obtained.

![Image](https://github.com/user-attachments/assets/8d38fa5c-228d-47ef-b6b3-4aaecd3e891d)

**Total score: 1426334**