# Temporal-Dijkstra
Advanced Algorithms &amp; Data Structures Assignment

You are given two files:
• Airports: There are 307 unique airport codes.
• Flights: There about 13950 flight which operate daily between US airports.
You will model this as a graph (there will be multiple edges though. Each airport will be a vertex and each flight will be an edge. Moreover, each edge has departure time and arrival time. This will make the graph what is known as a temporal graph. We will modify Dijkstra’s algorithm so that it minimizes time (instead of distance)). Here dvalue will stand for earliest time (we know so far) to reach the airport v. We will implement a priority queue using two data structures:
• Heap array: Make it array of pointers to vertex object. Also maintain int heap-size.
• Hash array: This is to be able to search the vertex object starting from its three-letter
code.
