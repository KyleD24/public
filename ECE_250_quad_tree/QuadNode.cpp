/*
    Fall 2020 - ECE 250
    Project2
    Author: Kyle Dyck
*/

#include "QuadNode.h"

// Constructors / Destructor
QuadNode::QuadNode(const double& longitude, const double& latitude, const double& cost_to_live, const double& average, const double& population, const std::string name)
{
    x = longitude;
    y = latitude;
    cost = cost_to_live;
    avg = average;
    pop = population;
    value = name;

    ne = nullptr;
    nw = nullptr;
    sw = nullptr;
    se = nullptr;

}
QuadNode::QuadNode() {
    x = 0;
    y = 0;
    cost = 0;
    avg = 0;
    pop = 0;
    value = "\0";

    ne = nullptr;
    nw = nullptr;
    sw = nullptr;
    se = nullptr;
}
QuadNode::~QuadNode()
{
    // Delete chilren if they exist
    if (ne != nullptr) {
        delete ne;
        ne = nullptr;
    }
    if (nw != nullptr) {
        delete nw;
        nw = nullptr;
    }
    if (sw != nullptr) {
        delete sw;
        sw = nullptr;
    }
    if (se != nullptr) {
        delete se;
        se = nullptr;
    }

    x = 0;
    y = 0;
    cost = 0;
    avg = 0;
    pop = 0;
    value = "\0";
}

// Setters 

void QuadNode::set_ne(QuadNode* ne_node)
{
    ne = ne_node;
}
void QuadNode::set_nw(QuadNode* nw_node)
{
    nw = nw_node;
}
void QuadNode::set_sw(QuadNode* sw_node)
{
    sw = sw_node;
}
void QuadNode::set_se(QuadNode* se_node)
{
    se = se_node;
}
void QuadNode::set_x(const double& lo) {
    x = lo;
}
void QuadNode::set_y(const double& la) {
    x = la;
}
void QuadNode::set_cost(const double& cost_to_live) {
    cost = cost_to_live;
}
void QuadNode::set_avg(const double& average) {
    avg = average;
}
void QuadNode::set_pop(const double& population) {
    pop = population;
}
bool QuadNode::insert_node(QuadNode* new_node) {
    if (x == new_node->get_x() && y == new_node->get_y()) {
        // A city already exists at these coordinates
        return false;
    }

    // Determine where to insert new node
    int direction = get_direction(new_node->get_x(), new_node->get_y());

    // Tries to place node in proper direction, goes to next node if occupied
    if (direction == 0) {
        // Place in NE direction
        if (ne == nullptr) {
            // Node empty, insert here
            this->set_ne(new_node);
            return true;
        }
        else {
            return ne->insert_node(new_node);
        }
    }
    else if (direction == 1) {
        // Place in NW direction
        if (nw == nullptr) {
            // Node empty, insert here
            this->set_nw(new_node);
            return true;
        }
        else {
            return nw->insert_node(new_node);
        }
    }
    else if (direction == 2) {
        // Place in SW direction
        if (sw == nullptr) {
            // Node empty, insert here
            this->set_sw(new_node);
            return true;
        }
        else {
            return sw->insert_node(new_node);
        }
    }
    else {
        // Place in SE direction
        if (se == nullptr) {
            // Node empty, insert here
            this->set_se(new_node);
            return true;
        }
        else {
            return se->insert_node(new_node);
        }
    }
}

// Getters

QuadNode* QuadNode::get_ne() const
{
    return ne;
}
QuadNode* QuadNode::get_nw() const
{
    return nw;
}
QuadNode* QuadNode::get_sw() const
{
    return sw;
}
QuadNode* QuadNode::get_se() const
{
    return se;
}
double QuadNode::get_x() const
{
    return x;
}
double QuadNode::get_y() const
{
    return y;
}
double QuadNode::get_cost() const
{
    return cost;
}
double QuadNode::get_avg() const
{
    return avg;
}
double QuadNode::get_pop() const
{
    return pop;
}

int QuadNode::get_direction(const double& lo, const double& la) const
{
    // Based on conditions described in project description
    if (lo >= x && la > y) {
        return 0;
    }
    else if (lo < x && la >= y) {
        return 1;
    }
    else if (lo <= x && la < y) {
        return 2;
    }
    else if (lo > x && la <= y){
        return 3;
    }
    else {
        assert(false && "This shouldn't happen");
    }
}
QuadNode* QuadNode::get_node(const double& lo, const double& la)
{
    // Note: Assumes node exists in tree
    if (x == lo && y == la) {
        return this;
    }
    else {
        // Continue searching for node
        int direction = this->get_direction(lo, la);
        if (direction == 0) {
            if (ne != nullptr) {
                return ne->get_node(lo, la);
            }
            else {
                // Node not found, programmer error
                // Node should exist in tree before use
                assert(false && ("Node not found in tree"));
            }
        }
        else if (direction == 1) {
            if (nw != nullptr) {
                return nw->get_node(lo, la);
            }
            else {
                // Node not found, programmer error
                // Node should exist in tree before use
                assert(false && ("Node not found in tree"));
            }
        }
        else if (direction == 2) {
            if (sw != nullptr) {
                return sw->get_node(lo, la);
            }
            else {
                // Node not found, programmer error
                // Node should exist in tree before use
                assert(false && ("Node not found in tree"));
            }
        }
        else {
            if (se != nullptr) {
                return se->get_node(lo, la);
            }
            else {
                // Node not found, programmer error
                // Node should exist in tree before use
                assert(false && ("Node not found in tree"));
            }
        }
    }
}
bool QuadNode::search(const double& lo, const double& la, bool print)
{
    if (x == lo && y == la) {
        // Node found, print and return true
        if (print) {
            // Print enabled
            std::cout << "found " << value << std::endl;
        }
        return true;
    }
    else {
        int direction = get_direction(lo, la);
        if (direction == 0) {
            if (ne == nullptr) {
                // NE child empty, node not in tree
                return false;
            }
            else {
                // Recursively search for node
                return ne->search(lo, la, print);
            }
        }
        else if (direction == 1) {
            if (nw == nullptr) {
                // NW child empty, node not in tree
                return false;
            }
            else {
                // Recursively search for node
                return nw->search(lo, la, print);
            }
        }
        else if (direction == 2) {
            if (sw == nullptr) {
                // SW child empty, node not in tree
                return false;
            }
            else {
                // Revursively search for node
                return sw->search(lo, la, print);
            }
        }
        else {
            if (se == nullptr) {
                // SE child empty, node not in tree
                return false;
            }
            else {
                // Recursively search for node
                return se->search(lo, la, print);
            }
        }
    }
}
int QuadNode::get_size(int& counter)
{
    counter++;
    if (ne != nullptr) {
        // Recursively count NE direction
        counter = ne->get_size(counter);
    }
    if (nw != nullptr) {
        // Recursively count NW direction
        counter = nw->get_size(counter);
    }
    if (sw != nullptr) {
        // Recursively count SW direction
        counter = sw->get_size(counter);
    }
    if (se != nullptr) {
        // Recursively count SE direction
        counter = se->get_size(counter);
    }

    return counter;
}
bool QuadNode::print_values(bool first) const
{
    // Print in order
    // NE, NW, ROOT, SW, SE

    // NE
    if (ne != nullptr) {
        // Recursively print values
        first = ne->print_values(first);     
    }
    // NW
    if (nw != nullptr) {
        // Recursively print values
        first = nw->print_values(first);
    }

    // Reached first node to print
    if (first) {
        // No space for first node
        std::cout << value;
        first = false;
    }
    else {
        // Space for every node after the first
        std::cout << " " << value;
    }

    // SW
    if (sw != nullptr) {
        // Recursively print values
        first = sw->print_values(first);
    }
    // SE
    if (se != nullptr) {
        // Recursively print values
        first = se->print_values(first);
    }

    return first;
}

double QuadNode::find_max(const int& attribute, double& maximum)
{
    if (attribute == 0) {
        // Cost
        // current node
        if (cost >= maximum) {
            // New max
            maximum = cost;
        }
        // NE node
        if (ne != nullptr) {
            maximum = ne->find_max(attribute, maximum);
        }
        // NW node
        if (nw != nullptr) {
            maximum = nw->find_max(attribute, maximum);
        }
        // SW node
        if (sw != nullptr) {
            maximum = sw->find_max(attribute, maximum);
        }
        // SE node
        if (se != nullptr) {
            maximum = se->find_max(attribute, maximum);
        }

        return maximum;
    }
    else if (attribute == 1) {
        // Average
        // current node
        if (avg >= maximum) {
            // New max
            maximum = avg;
        }
        // NE node
        if (ne != nullptr) {
            maximum = ne->find_max(attribute, maximum);
        }
        // NW node
        if (nw != nullptr) {
            maximum = nw->find_max(attribute, maximum);
        }
        // SW node
        if (sw != nullptr) {
            maximum = sw->find_max(attribute, maximum);
        }
        // SE node
        if (se != nullptr) {
            maximum = se->find_max(attribute, maximum);
        }

        return maximum;
    }
    else if (attribute == 2) {
        // Population
        // current node
        if (pop >= maximum) {
            // New max
            maximum = pop;
        }
        // NE node
        if (ne != nullptr) {
            maximum = ne->find_max(attribute, maximum);
        }
        // NW node
        if (nw != nullptr) {
            maximum = nw->find_max(attribute, maximum);
        }
        // SW node
        if (sw != nullptr) {
            maximum = sw->find_max(attribute, maximum);
        }
        // SE node
        if (se != nullptr) {
            maximum = se->find_max(attribute, maximum);
        }

        return maximum;
    }
    else {
        // Invalid attribute value
        assert(false && ("Invalid attribute value."));
    }
}
double QuadNode::find_min(const int& attribute, double& min)
{
    if (attribute == 0) {
        // Cost
        // current node
        if (cost <= min) {
            // New min
            min = cost;
        }
        // NE node
        if (ne != nullptr) {
            min = ne->find_min(attribute, min);
        }
        // NW node
        if (nw != nullptr) {
            min = nw->find_min(attribute, min);
        }
        // SW node
        if (sw != nullptr) {
            min = sw->find_min(attribute, min);
        }
        // SE node
        if (se != nullptr) {
            min = se->find_min(attribute, min);
        }

        return min;
    }
    else if (attribute == 1) {
        // Average
        // current node
        if (avg <= min) {
            // New min
            min = avg;
        }
        // NE node
        if (ne != nullptr) {
            min = ne->find_min(attribute, min);
        }
        // NW node
        if (nw != nullptr) {
            min = nw->find_min(attribute, min);
        }
        // SW node
        if (sw != nullptr) {
            min = sw->find_min(attribute, min);
        }
        // SE node
        if (se != nullptr) {
            min = se->find_min(attribute, min);
        }

        return min;
    }
    else if (attribute == 2) {
        // Population
        // current node
        if (pop <= min) {
            // New min
            min = pop;
        }
        // NE node
        if (ne != nullptr) {
            min = ne->find_min(attribute, min);
        }
        // NW node
        if (nw != nullptr) {
            min = nw->find_min(attribute, min);
        }
        // SW node
        if (sw != nullptr) {
            min = sw->find_min(attribute, min);
        }
        // SE node
        if (se != nullptr) {
            min = se->find_min(attribute, min);
        }

        return min;
    }
    else {
        // Invalid attribute value
        assert(false && ("Invalid attribute value."));
    }
}
double QuadNode::find_total(const int& attribute, double& total)
{
    if (attribute == 0) {
        // Cost
        // current node
        total = total + cost;
        // NE node
        if (ne != nullptr) {
            total = ne->find_total(attribute, total);
        }
        // NW node
        if (nw != nullptr) {
            total = nw->find_total(attribute, total);
        }
        // SW node
        if (sw != nullptr) {
            total = sw->find_total(attribute, total);
        }
        // SE node
        if (se != nullptr) {
            total = se->find_total(attribute, total);
        }

        return total;
    }
    else if (attribute == 1) {
        // Average
        // current node
        total = total + avg;
        // NE node
        if (ne != nullptr) {
            total = ne->find_total(attribute, total);
        }
        // NW node
        if (nw != nullptr) {
            total = nw->find_total(attribute, total);
        }
        // SW node
        if (sw != nullptr) {
            total = sw->find_total(attribute, total);
        }
        // SE node
        if (se != nullptr) {
            total = se->find_total(attribute, total);
        }

        return total;
    }
    else if (attribute == 2) {
        // Population
        // current node
        total = total + pop;
        // NE node
        if (ne != nullptr) {
            total = ne->find_total(attribute, total);
        }
        // NW node
        if (nw != nullptr) {
            total = nw->find_total(attribute, total);
        }
        // SW node
        if (sw != nullptr) {
            total = sw->find_total(attribute, total);
        }
        // SE node
        if (se != nullptr) {
            total = se->find_total(attribute, total);
        }

        return total;
    }
    else {
        // Invalid attribute value
        assert(false && ("Invalid attribute value."));
    }
}

void QuadNode::q_max(const double& lo, const double& la, const int& attr, const int& dir)
{
    // Boolean determining whether or not seach function should print
    bool print = false;
    if (search(lo, la, print)) {
        // Node exists in tree, continue
        
        // Get to desired node
        QuadNode* current_node = get_node(lo, la);
        double max = 0.0;

        // Look for values in given direction
        if (dir == 0) {
            // Look in NE direction
            if (current_node->get_ne() != nullptr) {
                max = current_node->get_ne()->find_max(attr, max);
            }
            else {
                // Direction empty, failure
                std::cout << "failure" << std::endl;
                return;
            }
        }
        else if (dir == 1) {
            // Look in NW direction
            if (current_node->get_nw() != nullptr) {
                max = current_node->get_nw()->find_max(attr, max);
            }
            else {
                // Direction empty, failure
                std::cout << "failure" << std::endl;
                return;
            }
        }
        else if (dir == 2) {
            // Look in SW direction
            if (current_node->get_sw() != nullptr) {
                max = current_node->get_sw()->find_max(attr, max);
            }
            else {
                // Direction empty, failure
                std::cout << "failure" << std::endl;
                return;
            }
        }
        else if (dir == 3) {
            // Look in SE direction
            if (current_node->get_se() != nullptr) {
                max = current_node->get_se()->find_max(attr, max);
            }
            else {
                // Direction empty, failure
                std::cout << "failure" << std::endl;
                return;
            }
        }

        // Don't print decimals for population
        if (attr == 2) {
            std::int32_t population = static_cast<std::int32_t>(max);
            std::cout << std::fixed << std::setprecision(2) << "max " << population << std::endl;
            return;
        }
        // Print max value to 2 decimal places
        std::cout << std::fixed << std::setprecision(2) << "max " << max << std::endl;
        return;
    }
    else {
        // Node not in tree, fail
        std::cout << "failure" << std::endl;
        return;
    }
}
void QuadNode::q_min(const double& lo, const double& la, const int& attr, const int& dir)
{
    // Boolean determining whether or not seach function should print
    bool print = false;
    if (search(lo, la, print)) {
        // Node exists in tree, continue

        // Get to desired node
        QuadNode* current_node = get_node(lo, la);
        double min =800000000000;

        // Look for values in given direction
        if (dir == 0) {
            // Look in NE direction
            if (current_node->get_ne() != nullptr) {
                min = current_node->get_ne()->find_min(attr, min);
            }
            else {
                // Direction empty, failure
                std::cout << "failure" << std::endl;
                return;
            }
        }
        else if (dir == 1) {
            // Look in NW direction
            if (current_node->get_nw() != nullptr) {
                min = current_node->get_nw()->find_min(attr, min);
            }
            else {
                // Direction empty, failure
                std::cout << "failure" << std::endl;
                return;
            }
        }
        else if (dir == 2) {
            // Look in SW direction
            if (current_node->get_sw() != nullptr) {
                min = current_node->get_sw()->find_min(attr, min);
            }
            else {
                // Direction empty, failure
                std::cout << "failure" << std::endl;
                return;
            }
        }
        else if (dir == 3) {
            // Look in SE direction
            if (current_node->get_se() != nullptr) {
                min = current_node->get_se()->find_min(attr, min);
            }
            else {
                // Direction empty, failure
                std::cout << "failure" << std::endl;
                return;
            }
        }

        // Don't print decimals for population
        if (attr == 2) {
            std::int32_t population = static_cast<std::int32_t>(min);
            std::cout << std::fixed << std::setprecision(2) << "min " << population << std::endl;
            return;
        }
        // Print max value to 2 decimal places
        std::cout << std::fixed << std::setprecision(2) << "min " << min << std::endl;
        return;
    }
    else {
        // Node not in tree, fail
        std::cout << "failure" << std::endl;
        return;
    }
}
void QuadNode::q_total(const double& lo, const double& la, const int& attr, const int& dir)
{
    // Boolean determining whether or not seach function should print
    bool print = false;
    if (search(lo, la, print)) {
        // Node exists in tree, continue

        // Get to desired node
        QuadNode* current_node = get_node(lo, la);
        double total = 0.0;

        // Look for values in given direction
        if (dir == 0) {
            // Look in NE direction
            if (current_node->get_ne() != nullptr) {
                total = current_node->get_ne()->find_total(attr, total);
            }
            else {
                // Direction empty, failure
                std::cout << "failure" << std::endl;
                return;
            }
        }
        else if (dir == 1) {
            // Look in NW direction
            if (current_node->get_nw() != nullptr) {
                total = current_node->get_nw()->find_total(attr, total);
            }
            else {
                // Direction empty, failure
                std::cout << "failure" << std::endl;
                return;
            }
        }
        else if (dir == 2) {
            // Look in SW direction
            if (current_node->get_sw() != nullptr) {
                total = current_node->get_sw()->find_total(attr, total);
            }
            else {
                // Direction empty, failure
                std::cout << "failure" << std::endl;
                return;
            }
        }
        else if (dir == 3) {
            // Look in SE direction
            if (current_node->get_se() != nullptr) {
                total = current_node->get_se()->find_total(attr, total);
            }
            else {
                // Direction empty, failure
                std::cout << "failure" << std::endl;
                return;
            }
        }

        // Don't print decimals for population
        if (attr == 2) {
            std::int32_t population = static_cast<std::int32_t>(total);
            std::cout << std::fixed << std::setprecision(2) << "total " << population << std::endl;
            return;
        }
        // Print max value to 2 decimal places
        std::cout << std::fixed << std::setprecision(2) << "total " << total << std::endl;
        return;
    }
    else {
        // Node not in tree, fail
        std::cout << "failure" << std::endl;
        return;
    }
}
