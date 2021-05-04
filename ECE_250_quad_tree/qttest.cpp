/*
    Fall 2020 - ECE 250
    Project2
    Author: Kyle Dyck
*/

#include "QuadNode.h"
#include <vector>

// Declarations

// Project Functions
// Inserts given city (node) into tree
// Prints to screen depending on result
QuadNode* insert_city(QuadNode*, QuadNode*);
// Searches tree for city (node)
// Prints to screen depending on result
void search_city(QuadNode*, std::vector<double>*);
// Executes one of the q commands depending on input
// Prints to screen depending on result. Mapped as follows:
// 0, 1, 2          :           max, min, total
void q_comm(QuadNode*, std::vector<double>*, const int&);
// Prints all cities 
void print_cities(QuadNode*);
// Clears tree
QuadNode* clear(QuadNode*);
// Prints size to screen
void size(QuadNode*);

// Helper Functions
// Parses input for any string representing a list of doubles
// Seperated by ';', returns a vector of those doubles
std::vector<double>* std_parsing(std::string, std::vector<double>*);
// Parses input specifically for insert project function
// Creates QuadNode and returns a pointer to it, must be saved into tree
// to prevent memory leaks
QuadNode* insert_parsing(std::string);

// Definitions

// Project Functions
QuadNode* insert_city(QuadNode* root, QuadNode* new_node) {
    if (root == nullptr) {
        // First node in tree, add it
        root = new_node;
        std::cout << "success" << std::endl;
        return root;
    }
    else {
        if (root->insert_node(new_node)) {
            // Successful
            std::cout << "success" << std::endl;
            // Don't need to delete new_node since it was inserted
            return root;
        }
        else {
            // Failure
            std::cout << "failure" << std::endl;
            
            delete new_node;
            new_node = nullptr;
            return root;
        }
    }
}
void search_city(QuadNode* root, std::vector<double>* input_v) {
    if (root == nullptr) {
        // Nothing in tree, failure
        std::cout << "not found" << std::endl;
        return;
    }
    else {
        // Get vector values
        std::vector<double> temp = *input_v;
        double x = temp[0];
        double y = temp[1];
        if (root->search(x, y, true)) {
            // Success
            // Search prints, do nothing
        }
        else {
            // Failure
            std::cout << "not found" << std::endl;
        }
        return;
    }


}
void q_comm(QuadNode* root, std::vector<double>* input_v, const int& q_call) {
    if (root == nullptr) {
        // Tree empty, failure
        std::cout << "failure" << std::endl;
        return;
    }
    else {
        // Get parameter values
        std::vector<double> temp = *input_v;
        double x = temp[0];
        double y = temp[1];
        int d = static_cast<int>(temp[2]);
        int attr = static_cast<int>(temp[3]);

        if (q_call == 0) {
            // q_max
            root->q_max(x, y, attr, d);
        }
        else if (q_call == 1) {
            // q_min
            root->q_min(x, y, attr, d);
        }
        else if (q_call == 2) {
            // q_total
            root->q_total(x, y, attr, d);
        }
        else {
            assert(false && "Invalid input for q_call");
        }
    }
}
void print_cities(QuadNode* root) {
    if (root == nullptr) {
        // Tree empty, do nothing
        return;
    }
    else {
        root->print_values(true);
        std::cout << std::endl;
    }
}
QuadNode* clear(QuadNode* root) {
    // Deconstructor takes care of clearing tree
    // If tree is empty (root == nullptr) deleting doesn't hurt
    delete root;
    root = nullptr;
    std::cout << "success" << std::endl;
    return root;
}
void size(QuadNode* root) {
    if (root == nullptr) {
        // Tree is empty, print 0
        std::cout << "tree size 0" << std::endl;
    }
    else {
        // Print size
        int size = 0;
        size = root->get_size(size);
        std::cout << "tree size " << size << std::endl;
    }
}

// Helper Functions
std::vector<double>* std_parsing(std::string input_s, std::vector<double>* input_vector) {
    assert(input_s != "\0");
    std::string current{ "\0" };

    // Frees up memory
    delete input_vector;
    // Creates fresh vector
    std::vector<double>* input_v = new std::vector<double>();
    
    while (input_s != "\0") {
        current = input_s.substr(0, input_s.find(";"));
        // Maps direction values to integers
        if (current == "NE") {
            current = "00";
        }
        else if (current == "NW") {
            current = "01";
        }
        else if (current == "SW") {
            current = "02";
        }
        else if (current == "SE") {
            current = "03";
        }
        // Maps attributes to integers
        if (current == "r") {
            // Cost
            current = "00";
        }
        else if (current == "s") {
            // Average Salary
            current = "01";
        }
        else if (current == "p") {
            // Population
            current = "02";
        }
        input_v->push_back(std::stod(current));
        if (current.size() < (input_s.size() - 1)) {
            input_s = input_s.substr(current.size() + 1);
        }
        else {
            input_s = "\0";
        }
    }
    return input_v;
}
QuadNode* insert_parsing(std::string input_s) {
    // Variables for initiating QuadNode
    std::string name { "\0" };
    double x{ 0 };
    double y{ 0 };
    double pop{ 0 };
    double cost{ 0 };
    double avg{ 0 };

    assert(input_s != "\0");
    std::string temp{ "\0" };

    // Sets variables in order as explained in project description
    for (int i = 0; input_s != "\0"; i++) {
        temp = input_s.substr(0, input_s.find(";"));
        if (i == 0) {
            // Name first
            name = temp;
        }
        else if (i == 1) {
            // x next
            x = std::stod(temp);
        }
        else if (i == 2) {
            // y next
            y = std::stod(temp);
        }
        else if (i == 3) {
            // Population next
            pop = std::stod(temp);
        }
        else if (i == 4) {
            // Cost of Living next
            cost = std::stod(temp);
        }
        else if (i == 5) {
            // Average Net Salary next
            avg = std::stod(temp);
        }
        
        // Changes input for next loop
        if (temp.size() < (input_s.size() - 1)) {
            input_s = input_s.substr(temp.size() + 1);
        }
        else {
            input_s = "\0";
        }
    }
    // After retrieving inputs, return new QuadNode
    return new QuadNode(x, y, cost, avg, pop, name);


}

int main() {

    // Variable to hold the root of the Quad Tree
    QuadNode* ROOT = nullptr;
    // Variable to temporarily hold QuadNode values
    QuadNode* temp_qn = nullptr;
    // Variable to temporarily hold input values (doubles)
    std::vector<double>* input_v = nullptr;
    
    // Variable for storing command component of input string
    std::string command_s{ "\0" };

    // Get first input from user
    std::string cmdline{ "\0" };
    std::getline(std::cin, cmdline);

    bool in_loop = true;
    while (!std::cin.eof() && in_loop) {
        if (cmdline == "exit") {
            // Exit program
            in_loop = false;
        }
        else {
            // Execute program
            // Extracts command from cmdline
            command_s = cmdline.substr(0, cmdline.find(" "));
            // Ensures only stripping cmdline if there are arguments following it
            if (command_s != "print" && command_s != "clear" && command_s != "size") {
                // Removes command from cmdline and leaves arguments
                cmdline = cmdline.substr(command_s.size() + 1);
            }
            

            // Input parsing
            if (command_s != "print" && command_s != "clear" && command_s != "size" && command_s != "i") {
                // Parse arguments (not insert function, nor other functions)
                input_v = std_parsing(cmdline, input_v);
            }
            else if (command_s == "i") {
                // This must be stored in tree or deleted manually
                temp_qn = insert_parsing(cmdline);
            }
            else {
                // Execute print, clear, or size commands that
                // don't have arguments
            }

            if (command_s == "i") {
                // Insert command
                ROOT = insert_city(ROOT, temp_qn);
            }
            else if (command_s == "s") {
                // Search command
                search_city(ROOT, input_v);
            }
            else if (command_s == "q_max") {
                // q_max command
                q_comm(ROOT, input_v, 0);
            }
            else if (command_s == "q_min") {
                // q_min command
                q_comm(ROOT, input_v, 1);
            }
            else if (command_s == "q_total") {
                // q_total command
                q_comm(ROOT, input_v, 2);
            }
            else if (command_s == "print") {
                // Print command
                print_cities(ROOT);
            }
            else if (command_s == "clear") {
                // Clear command
                ROOT = clear(ROOT);
            }
            else if (command_s == "size") {
                // Size command
                size(ROOT);
            }

            // Gets next line
            std::getline(std::cin, cmdline);
        }

    }


    delete ROOT;
    ROOT = nullptr;
    delete input_v;
    input_v = nullptr;

    return 0;
}