/*
	Fall 2020 - ECE 250
	Project3
	Author: Kyle Dyck
*/

#include "DJS.h"
#include <cassert>

// Constructor / Destructor
DJS::DJS(uint64_t size)
{
	num_nodes = size;
	num_sets = num_nodes;
	// Creates an array of DJS_List pointers
	set_list = new DJS_List*[num_nodes]();
	// Initializes the array with empty lists
	for (uint64_t i = 0; i < num_nodes; i++) {
		set_list[i] = new DJS_List();
	}

}
DJS::~DJS()
{
	for (uint64_t i = 0; i < num_nodes; i++) {
		// Deletes every list in array
		// Note: Must make sure there aren't the same nodes in different lists
		delete set_list[i];
		set_list[i] = nullptr;
	}
	// Deletes array of pointers
	delete [] set_list;


}

// Setters
bool DJS::set_union(uint64_t& u, uint64_t& v) {
	auto x = findset(u);
	auto y = findset(v);

	assert(x != y);

	// Merge the smaller set into the larger set
	if (x->get_size() < y->get_size()) {
		// Merge x into y

		// Attaches x to end of y
		y->get_tail()->set_next(x->get_rep());
		// Sets rep of all nodes in x to rep of y
		x->get_rep()->set_rep(y->get_rep());
		// Sets new tail to end of x
		y->set_tail(x->get_tail());
		// Clears x (doesn't delete)
		x->clear_list();
		num_sets--;

	}
	else {
		// Either same size or x is larger

		// Attaches y to end of x
		x->get_tail()->set_next(y->get_rep());
		// Sets rep of all nodes in y to rep of x
		y->get_rep()->set_rep(x->get_rep());
		// Sets new tail to end of y
		x->set_tail(y->get_tail());
		// Clears y (doesn't delete)
		y->clear_list();
		num_sets--;
	}

	if (num_sets == 1) {
		// One set containing all the nodes, done
		return false;
	}
	else {
		// Merge successful
		return true;
	}
}
void DJS::makeset(uint64_t& number) {
	// Gets index to next available slot
	uint64_t index = 0;

	while (set_list[index]->get_rep() != nullptr){
		auto temp = set_list[index]->get_rep()->get_num();
		// Current slot not available, keep looking
		index++;
		// Must be in range
		assert(index < num_nodes);
		// Can't make set of the same node
		assert(temp != number);
	}

	// Available slot found
	set_list[index]->append(number);
}

// Getters
DJS_List* DJS::findset(uint64_t& number) {
	
	// Go through array of lists
	for (int i = 0; i < num_nodes; i++) {
		auto current_list = set_list[i];
		auto current_node = current_list->get_rep();

		while (current_node != nullptr) {
			if (current_node->get_num() == number) {
				// Number found, return list that this node is in
				return current_list;
			}

			current_node = current_node->get_next();
		}
	}

	// Shouldn't make it here since it is assumed that the number
	// is in the list
	assert(false);
	return 0;
}