#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <algorithm>
#include <functional>

using namespace std;

// Node structure to represent each cell in the map
struct Node {
    int x, y; // coordinates of the cell
    int g, h; // g is the cost from the start, h is the heuristic cost to the destination
    Node* parent; // pointer to the parent node in the path

    Node(int _x, int _y, int _g, int _h, Node* _parent = nullptr) : 
        x(_x), y(_y), g(_g), h(_h), parent(_parent) {}

    // Calculate the total cost (f = g + h) for the A* algorithm
    int f() const {
        return g + h;
    }
};

// Heuristic function to estimate the remaining cost from a cell to the destination
// Using the Manhattan distance heuristic (sum of horizontal and vertical distances)
int heuristic(int x, int y, int destX, int destY) {
    return abs(destX - x) + abs(destY - y);
}

// Function to check if a cell is within the map boundaries
bool isValid(int x, int y, int rows, int cols) {
    return x >= 0 && y >= 0 && x < rows && y < cols;
}

// Function to read the map from a file
// Returns a 2D vector representing the map, and updates startX and startY to the starting point
vector<vector<int>> readMapFromFile(const string& filename, int& startX, int& startY) {
    ifstream file(filename);
    int rows, cols;
    file >> rows >> cols;
    vector<vector<int>> map(rows, vector<int>(cols));

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            file >> map[i][j];
            if (map[i][j] == 2) {
                startX = i;
                startY = j;
            }
        }
    }
    file.close();
    return map;
}

// Function to read destinations and priorities from a file
// Returns a vector of pairs, where each pair contains a destination (x, y) and its priority
vector<pair<pair<int, int>, int>> readDestinationsAndPrioritiesFromFile(const string& filename) {
    ifstream file(filename);
    vector<pair<pair<int, int>, int>> destinationsAndPriorities;
    int destX, destY, priority;
    while (file >> destX >> destY >> priority) {
        destinationsAndPriorities.emplace_back(make_pair(destX, destY), priority);
    }
    file.close();
    return destinationsAndPriorities;
}

// A* algorithm implementation to find the shortest path from start to destination
vector<pair<int, int>> aStar(const vector<vector<int>>& map, int startX, int startY, int destX, int destY) {
    const int rows = map.size();
    const int cols = map[0].size();
    vector<vector<bool>> visited(rows, vector<bool>(cols, false)); // keep track of visited cells
    vector<vector<Node*>> nodes(rows, vector<Node*>(cols, nullptr)); // keep track of nodes in the map

    // Priority queue to store nodes based on their total cost (f = g + h)
    priority_queue<Node*, vector<Node*>, function<bool(Node*, Node*)>> pq(
        [](Node* a, Node* b) { return a->f() > b->f(); });

    Node* start = new Node(startX, startY, 0, heuristic(startX, startY, destX, destY));
    pq.push(start);

    while (!pq.empty()) {
        Node* current = pq.top();
        pq.pop();

        // If the current node is the destination, reconstruct the path and return it
        if (current->x == destX && current->y == destY) {
            vector<pair<int, int>> path;
            while (current != nullptr) {
                path.emplace_back(current->x, current->y);
                current = current->parent;
            }
            reverse(path.begin(), path.end());
            return path;
        }

        visited[current->x][current->y] = true;

        // Possible movements: right, left, down, up
        int dx[] = {1, -1, 0, 0};
        int dy[] = {0, 0, 1, -1};

        for (int i = 0; i < 4; ++i) {
            int newX = current->x + dx[i];
            int newY = current->y + dy[i];

            // Check if the new cell is valid (within bounds, not visited, and not an obstacle)
            if (isValid(newX, newY, rows, cols) && !visited[newX][newY] && map[newX][newY] != 1) {
                int newG = current->g + 1; // Cost from the start to the new cell
                int newH = heuristic(newX, newY, destX, destY); // Heuristic cost from the new cell to the destination
                Node* newNode = new Node(newX, newY, newG, newH, current); // Create a new node with the updated costs
                pq.push(newNode); // Add the new node to the priority queue
            }
        }
    }

    return {}; // No path found
}

int main(int argc, char* argv[]) {
    int startX, startY;
    // Store .txt file names 
    std::string mapGrid, destinationList;
    mapGrid = argv[1];
    destinationList = argv[2];
    vector<vector<int>> map = readMapFromFile(mapGrid, startX, startY); // Read the map from a file
    vector<pair<pair<int, int>, int>> destinationsAndPriorities = readDestinationsAndPrioritiesFromFile(destinationList); // Read destinations and priorities from a file

    // Pigeonhole sorting: Sorting destinations into buckets based on their priorities
    vector<vector<pair<int, int>>> buckets(4); // 0th index unused
    for (const auto& [destination, priority] : destinationsAndPriorities) {
        buckets[priority].push_back(destination);
    }

    cout << "Sorted Addresses:" << endl;
    for (int priority = 1; priority <= 3; ++priority) {
        for (const auto& destination : buckets[priority]) {
            cout << "(" << destination.first << ", " << destination.second << ") with priority " << priority << endl;
        }
    }

    cout << endl;

    for (int priority = 1; priority <= 3; ++priority) {
        for (const auto& destination : buckets[priority]) {
            int destX = destination.first;
            int destY = destination.second;

            vector<pair<int, int>> path = aStar(map, startX, startY, destX, destY); // Find the shortest path to the destination

            if (path.empty()) {
                cout << "Cannot find a path to destination (" << destX << ", " << destY << ") with priority " << priority << "." << endl;
            } else {
                cout << "Optimal Path to destination (" << destX << ", " << destY << ") with priority " << priority << ":" << endl;
                for (const auto& p : path) {
                    cout << "(" << p.first << ", " << p.second << ")" << endl;
                }
                startX = destX; // Update the starting point to the current destination
                startY = destY;
            }
        }
    }

    cout << "All done!" << endl;

    return 0;
}
