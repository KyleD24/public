/*
	Fall 2020 - ECE 250
	Project3
	Author: Kyle Dyck
*/

#include "DJS_List.h"

// Constructors / Destructor
DJS_List::DJS_List(uint64_t& number)
{
	rep = nullptr;
	tail = rep;
	size = 0;

	append(number);
}
DJS_List::DJS_List()
{
	rep = nullptr;
	tail = rep;
	size = 0;
}
DJS_List::~DJS_List() {
	delete rep;
	rep = nullptr;
	tail = nullptr;
	size = 0;
}

// Setters
void DJS_List::clear_list()
{
	rep = nullptr;
	tail = nullptr;
	size = 0;
}
void DJS_List::append(uint64_t& number) {
	if (rep != nullptr && tail != nullptr) {
		// List isn't empty
		auto temp = new DJS_Node(number);
		tail->set_next(temp);
		tail = temp;
		tail->set_rep(rep);
		size++;
	}
	else {
		// Assumes both are null and adds to list
		auto temp = new DJS_Node(number);
		rep = temp;
		tail = temp;
		size++;
	}
}
void DJS_List::set_tail(DJS_Node* new_tail)
{
	tail = new_tail;
}

// Getters
DJS_Node* DJS_List::get_rep() const
{
	return rep;
}
DJS_Node* DJS_List::get_tail() const {
	return tail;
}
uint64_t DJS_List::get_size() const {
	return size;
}
bool DJS_List::search_vertex(uint64_t& number) const {
	auto current = rep;

	while (current != nullptr) {
		if (current->get_num() == number) {
			// Node exists in this list
			return true;
		}
		// Traverse through the list
		current = current->get_next();
	}
	// End of list, not found
	return false;
}
