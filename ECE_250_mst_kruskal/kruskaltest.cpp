/*
    Fall 2020 - ECE 250
    Project3
    Author: Kyle Dyck
*/

#include <iostream>
#include "DJS.h" // Disjoint Set 
#include "Adj_Mat.h" // Graph as adjacency matrix
#include "edge_plus.h" // Weighted edge
#include <algorithm> // For sort 
#include <iomanip> // Input / Output
#include <string>

// Declarations
// Function for performing Kruskal's MST algorithm
double kruskal_mst(Adj_Mat*);

// Definitions
double kruskal_mst(Adj_Mat* graph) {
    // List of edges for minimum spanning tree
    auto tree = new std::vector<edge_plus>();
    // List of all edges
    auto size = graph->get_num_nodes();
    auto edges = new std::vector<edge_plus>();

    // Populate initial set with all edges
    for (uint64_t i = 0; i < size; i++) {
        for (uint64_t j = 0; j <= i; j++) {
            auto temp_read = graph->read_matrix(i, j);
            if (temp_read.edge) {
                // Found edge, add to initial list
                edge_plus temp(i, j, temp_read.weight);
                edges->push_back(temp);
                
            }
        }
    }
    // Sort initial set according to ascending weight
    std::sort(edges->begin(), edges->end());

    // Make the disjoint set of all the nodes
    DJS djs(graph->get_num_nodes());
    for (uint64_t i = 0; i < graph->get_num_nodes(); i++) {
        djs.makeset(i);
    }

    // Create MST
    for (uint64_t i = 0; i < edges->size(); i++) {
        edge_plus temp_edge = edges[0][i];
        auto temp_x = djs.findset(temp_edge.edge_pair.first);
        auto temp_y = djs.findset(temp_edge.edge_pair.second);

        bool result = true;
        if (temp_x != temp_y) {
            tree->push_back(temp_edge);
            result = djs.set_union(temp_edge.edge_pair.first, temp_edge.edge_pair.second);
        }
        if (!result) {
            // One set remaining, done unions
            // Get out of loop
            i = edges->size();
        }
    }

    // Checking if graph is connected or not
    if (tree->size() < size-1) {
        // After going through disjoint sets, there are not enough edges
        // in tree to span entire graph, therefore the graph is disconnected
        delete tree;
        tree = nullptr;
        delete edges;
        edges = nullptr;
        throw illegal_argument();
    }

    // Sum the weights of the MST
    long double sum = 0;
    for (uint64_t i = 0; i < tree->size(); i++) {
        sum = sum + tree[0][i].weight;
    }

    delete tree;
    tree = nullptr;
    delete edges;
    edges = nullptr;

    return sum;
}

int main() {

    // Variable to hold the graph
    Adj_Mat* GRAPH = nullptr;

    // Variable for storing command component of input string
    std::string command_s{ "\0" };

    // Get first input from user
    std::string cmdline{ "\0" };
    std::getline(std::cin, cmdline);

    bool in_loop = true;
    while (!std::cin.eof() && in_loop) {
        if (cmdline == "exit") {
            // Exit Program
            in_loop = false;
        }
        else {
            // Execute program
            // Extracts command from cmdline
            command_s = cmdline.substr(0, cmdline.find(" "));
            // Ensures only stripping cmdline if there are arguments following it
            if (command_s != "edge_count" && command_s != "clear" && command_s != "mst") {
                // Removes command from cmdline and leaves arguments
                cmdline = cmdline.substr(command_s.size() + 1);
            }

            // Execute command
            if (command_s == "n") {
                // New graph
                // Save pointer in case exception thrown
                auto temp = GRAPH;
                try {
                    auto m = std::stoll(cmdline);
                    // Deletes graph if it already exists
                    GRAPH = new Adj_Mat(m);
                    // Free up old graph
                    delete temp;
                    temp = nullptr;
                    std::cout << "success" << std::endl;
                }
                catch(illegal_argument){
                    std::cout << "invalid argument" << std::endl;
                }
            }
            else if (command_s == "i"){
                // Insert edge
                // Extract first vertex
                std::string temp_s = cmdline.substr(0, cmdline.find(";"));
                cmdline = cmdline.substr(temp_s.size() + 1);
                auto u = std::stoll(temp_s);
                // Extract second vertex                   
                temp_s = cmdline.substr(0, cmdline.find(";"));
                cmdline = cmdline.substr(temp_s.size() + 1);
                auto v = std::stoll(temp_s);
                // Extract weight
                auto w = std::stold(cmdline);

                try {
                    GRAPH->insert_edge(u, v, w);
                    
                    std::cout << "success" << std::endl;
                }
                catch (illegal_argument) {
                    std::cout << "invalid argument" << std::endl;
                }
            }
            else if (command_s == "d") {
                // Delete edge
                // Extract first vertex
                std::string temp_s = cmdline.substr(0, cmdline.find(";"));
                cmdline = cmdline.substr(temp_s.size() + 1);
                auto u = std::stoll(temp_s);
                // Extract second vertex                   
                temp_s = cmdline.substr(0, cmdline.find(";"));
                auto v = std::stoll(temp_s);

                try {
                    bool result = GRAPH->delete_edge(u, v);
                    if (result) {
                        // Deletion succcessful
                        std::cout << "success" << std::endl;
                    }
                    else {
                        // No edge conecting u and v
                        std::cout << "failure" << std::endl;
                    }
                }
                catch (illegal_argument) {
                    std::cout << "invalid argument" << std::endl;
                }

            }
            else if (command_s == "degree") {
                // Print Degree
                auto u = std::stoll(cmdline);

                try {
                    auto temp_degree = GRAPH->get_degree(u);
                    std::cout << "degree of " << u <<  " is " << temp_degree << std::endl;
                }
                catch (illegal_argument) {
                    std::cout << "invalid argument" << std::endl;
                }
            }
            else if (command_s == "edge_count"){
                assert(GRAPH != nullptr);
                auto edges = GRAPH->get_num_edges();
                std::cout << "edge count is " << edges << std::endl;
            }
            else if (command_s == "clear") {
                GRAPH->clear();
                std::cout << "success" << std::endl;
            }
            else if (command_s == "mst") {
                try {
                    auto mst_weight = kruskal_mst(GRAPH);

                    std::cout << std::fixed << std::setprecision(2) << "mst " << mst_weight << std::endl;
                }
                catch (illegal_argument) {
                    std::cout << "not connected" << std::endl;
                }
                
            }
            else {
                // Shouldn't make it here
                std::cout << "bad command" << std::endl;
            }

            // Gets next line
            std::getline(std::cin, cmdline);
        }


    }

    delete GRAPH;
    GRAPH = nullptr;
    return 0;
}