#pragma once
/*
	Fall 2020 - ECE 250
	Project3
	Author: Kyle Dyck
*/

#include <stdint.h>

class DJS_Node
{
	// Represents node in a disjoint set
	// Labelled with an integer
	private:
		uint64_t num;
		DJS_Node* rep;
		DJS_Node* next;

	public:

		// Constructor
		// Sets rep to self, next to null, num to given num
		DJS_Node(uint64_t&);


		// Destructor
		// If next is not null, delete
		~DJS_Node();


		// Setters
		void set_next(DJS_Node*);
		void set_num(const uint64_t&);
		// Sets rep and calls set_rep for next node until NULL
		void set_rep(DJS_Node*);


		// Getters
		DJS_Node* get_next() const;
		uint64_t get_num() const;
		DJS_Node* get_rep() const;

};

