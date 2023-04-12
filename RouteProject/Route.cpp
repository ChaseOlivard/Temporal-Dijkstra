#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <sstream>

using namespace std;
// Forward declaration of Edge structure
struct Edge;

// Vertex structure representing an airport
struct Vertex {
    char airport_code[4];    // Airport code (3-letter code)
    int heap_pos;            // Position in the heap array
    int hash_pos;            // Position in the hash array
    Edge* adj_list;          // Adjacency list containing edges (flights) from this airport
    int dvalue;              // Earliest arrival time at this airport
    char parent[4];          // Airport code of the parent airport in the shortest path
    Edge* parentedge;        // Edge corresponding to the parent airport in the shortest path
};

// Edge structure representing a flight
struct Edge {
    char origin[4];          // Origin airport code
    char dest[4];            // Destination airport code
    string airlines;         // Airline code (not used in this implementation)
    int fltno;               // Flight number (not used in this implementation)
    int deptime;             // Departure time
    int arrtime;             // Arrival time
    Edge* next;              // Next edge in the adjacency list

    // Constructor for Edge structure
    Edge() : fltno(0), deptime(0), arrtime(0), next(nullptr) {
        origin[0] = '\0';
        dest[0] = '\0';
    }
};

// Hash array for quick look-up of vertices using airport codes
Vertex* hasharray[1000];
// Heap array for maintaining the priority queue of vertices
Vertex* heaparray[1000];

// Hash function for converting airport codes to hash array indices
int myhash(char c[]) {
    int p0 = (int)c[0] - 'A' + 1;
    int p1 = (int)c[1] - 'A' + 1;
    int p2 = (int)c[2] - 'A' + 1;
    int p3 = p0 * 467 * 467 + p1 * 467 + p2;
    int p4 = p3 % 7193;
    // Ensures the returned value is positive (Keep getting inerger overflow)
    int result = (p4 % 1000 + 1000) % 1000;
    cout << "Hash value for " << c << ": " << result << endl;

    return result;
}

// Function to retrieve a vertex object given an airport code
Vertex* getvertex(char code[4]) {
    int p = myhash(code);
    if (hasharray[p] == nullptr) return nullptr;
    if (strcmp(hasharray[p]->airport_code,code) == 0) return hasharray[p];
    
    int i;
    for (int i = p + 1; (i != p); i++) {
        if (i >= 1000) i = i % 1000;
        if (hasharray[p]->airport_code == code) break;
        if (hasharray[p] == nullptr) return nullptr;
    }

    if (i == p) return nullptr;
    return hasharray[p];
}

void floatUp(Vertex* heaparray[], int index) {
    int parent = (index - 1) / 3;
    while (index > 0 && heaparray[index]->dvalue < heaparray[parent]->dvalue) {
        // Swap the vertices in the heap
        swap(heaparray[index], heaparray[parent]);

        // Update the heap_pos for both vertices
        heaparray[index]->heap_pos = index;
        heaparray[parent]->heap_pos = parent;

        // Move up the heap
        index = parent;
        parent = (index - 1) / 3;
    }
}


// Function to decrease the key (dvalue) of a vertex in the heap
void decrease_key(Vertex* heaparray[], Vertex* v) {
    int index = v->heap_pos; // Find the position of the vertex in the heap array
    floatUp(heaparray, index); // Float up the vertex to maintain the heap structure
}

// Function to relax an edge, i.e., update the dvalue of the destination airport if a shorter path is found
void relax(Edge* e) {
    Vertex* v_origin = getvertex(e->origin);
    if (v_origin->dvalue <= e->deptime) return;
    
    Vertex* v_dest = getvertex(e->dest);
    if (v_dest->dvalue > e->arrtime) {
        v_dest->dvalue = e->arrtime;
        decrease_key(heaparray, v_dest);
        strcpy(v_dest->parent, e->origin);
        v_dest->parentedge = e;
    }
}

// Maintains the min-heap property by floating down a vertex to its correct position
void float_down(Vertex* heaparray[], int index, int size) {
    int left_child = 2 * index + 1;
    int right_child = 2 * index + 2;
    int smallest = index;

    // Find the smallest among the current vertex, its left child, and right child
    if (left_child < size && heaparray[left_child]->dvalue < heaparray[smallest]->dvalue) {
        smallest = left_child;
    }

    if (right_child < size && heaparray[right_child]->dvalue < heaparray[smallest]->dvalue) {
        smallest = right_child;
    }

    // If the smallest is not the current vertex, swap them and continue floating down
    if (smallest != index) {
        swap(heaparray[index], heaparray[smallest]);
        heaparray[index]->heap_pos = index;
        heaparray[smallest]->heap_pos = smallest;
        float_down(heaparray, smallest, size);
    }
}

// Extract the vertex with the smallest dvalue from the min-heap
Vertex* extract_min(Vertex* heaparray[]) {
    // If the heap is empty, return nullptr
    if (heaparray[0] == nullptr) {
        return nullptr;
    }

    Vertex* min_vertex = heaparray[0];
    int size = 0;
    while (heaparray[size] != nullptr) {
        size++;
    }

    // Move the last vertex to the root and remove it from the last position
    heaparray[0] = heaparray[size - 1];
    heaparray[size - 1] = nullptr;
    if (heaparray[0] != nullptr) {
        heaparray[0]->heap_pos = 0;
        // Maintain the heap property after the change
        float_down(heaparray, 0, size - 1);
    }

    return min_vertex;
}


// Function to process a vertex (i.e., explore its adjacent vertices)
void process_vertex(Vertex* vertex) {
    Vertex* v = extract_min(heaparray);
    for (Edge* e = v->adj_list; e != nullptr; e = e->next) {
        relax(e);
    }
}


void read_airports_file(const string& filename) {
    ifstream in(filename);

    if (!in.is_open()) {
        cerr << "Error: Unable to open airports file." << endl;
        exit(1);
    }

    string line;
    for (int i = 0; i < 1000; ++i) {
    hasharray[i] = nullptr;
    }

   while (getline(in, line)) {
    Vertex* v = new Vertex();
    strncpy(v->airport_code, line.c_str(), 3);
    v->airport_code[3] = '\0';
    v->dvalue = INT_MAX;
    int hash_pos = myhash(v->airport_code);
    cout << "Calculated hash_pos: " << hash_pos << endl; // Add this line
    while (hasharray[hash_pos] != nullptr) {
         cout << "Current hash_pos: " << hash_pos << endl;
       hash_pos = ((hash_pos + 1) % 1000 + 1000) % 1000; // While loop added to prevent interger overflow
    }
    v->hash_pos = hash_pos;
    hasharray[hash_pos] = v;
}


    in.close();
}
void read_flights_file(const string& filename) {
    ifstream in(filename);

    if (!in.is_open()) {
        cerr << "Error: Unable to open flights file." << endl;
        exit(1);
    }

    string line;
    getline(in, line); // Skip the first line (header)

    while (getline(in, line)) {
        stringstream ss(line);
        string token;
        getline(ss, token, '\t');
        string airline = token;
        getline(ss, token, '\t');
        int flight_number = stoi(token);
        getline(ss, token, '\t');
        string origin = token;
        getline(ss, token, '\t');
        string dest = token;
        getline(ss, token, '\t');
        int scheduled_departure = stoi(token);
        getline(ss, token, '\t');
        int scheduled_arrival = stoi(token);
        getline(ss, token, '\t');
        int distance = stoi(token);

        Edge* edge = new Edge;
        strncpy(edge->origin, origin.c_str(), 3);
        strncpy(edge->dest, dest.c_str(), 3);
        edge->origin[3] = '\0';
        edge->dest[3] = '\0';
        edge->airlines = airline;
        edge->fltno = flight_number;
        edge->deptime = scheduled_departure;
        edge->arrtime = scheduled_arrival;

        Vertex* src_vertex = getvertex(edge->origin);
        edge->next = src_vertex->adj_list;
        src_vertex->adj_list = edge;
    }

    in.close();
}
// Build a min-heap from the hash array
void build_heap(Vertex* heaparray[]) {
    int size = 0;
    // Add all non-null vertices from the hash array to the heap array
    for (int i = 0; i < 1000; i++) {
        if (hasharray[i] != nullptr) {
            heaparray[size] = hasharray[i];
            hasharray[i]->heap_pos = size;
            size++;
        }
    }

    // Adjust the heap property from the bottom up
    for (int i = size / 2 - 1; i >= 0; i--) {
        float_down(heaparray, i, size);
    }
}


// Check if the heap is empty
bool is_heap_empty(Vertex* heaparray[]) {
    return heaparray[0] == nullptr;
}





int main(int argc, char* argv[]){
     // Check if the correct number of command-line arguments is provided
    if (argc !=4){
        cerr << "Usage: ./Route [source] [destination] [beginning-time]"<< endl;
        return 1;
    }
    // Store command-line arguments in variables    
    string source = argv[1];
    string destination = argv[2];
    int beginning_time = stoi(argv[3]);

    read_airports_file("airports.txt");
    read_flights_file("flights.txt");
     
    Vertex* src_vertex = getvertex(const_cast<char*>(source.c_str()));
    if(src_vertex == nullptr){
        cerr << "Source Airport not found" << endl;
        return 1;
    }
    src_vertex->dvalue = beginning_time;
    build_heap(heaparray);

    //Run modified Dijkstra algorithm 
     while (!is_heap_empty(heaparray)) {
        Vertex* v = extract_min(heaparray);
        process_vertex(v);

        // Break if we reach the destination vertex
        if (strcmp(v->airport_code, destination.c_str()) == 0) {
            break;
        }
    }

    //TODO Print earliest possible arrival time at the destination airport
    Vertex* dest_vertex = getvertex(const_cast<char*>(destination.c_str()));
    if (dest_vertex == nullptr) {
        cerr << "Destination airport not found." << endl;
        return 1;
    }

    if (dest_vertex->dvalue == INT_MAX) {
        cout << "No available route found." << endl;
    } else {
        cout << "Earliest arrival time at " << destination << ": " << dest_vertex->dvalue << endl;
    }

    return 0;
};