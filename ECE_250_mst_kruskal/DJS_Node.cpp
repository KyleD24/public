/*
	Fall 2020 - ECE 250
	Project3
	Author: Kyle Dyck
*/

#include "DJS_Node.h"

// Constructor / Destructor
DJS_Node::DJS_Node(uint64_t& number)
{
	rep = this;
	num = number;
	next = nullptr;

}
DJS_Node::~DJS_Node()
{
	if (next != nullptr) {
		delete next;
		next = nullptr;
		rep = nullptr;
		num = 0;
	}
	else {
		// Last node in list
		rep = nullptr;
		num = 0;
	}
}

// Setters
void DJS_Node::set_next(DJS_Node* next_node)
{
	next = next_node;
}
void DJS_Node::set_num(const uint64_t& number)
{
	num = number;
}
void DJS_Node::set_rep(DJS_Node* rep_node)
{
	rep = rep_node;
	if (next != nullptr) {
		// Sets rep of all other nodes
		next->set_rep(rep_node);
	}
	else {
		// End of list

	}
}

// Getters
DJS_Node* DJS_Node::get_next() const
{
	return next;
}
uint64_t DJS_Node::get_num() const
{
	return num;
}
DJS_Node* DJS_Node::get_rep() const
{
	return rep;
}
