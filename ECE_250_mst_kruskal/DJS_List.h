#pragma once
/*
	Fall 2020 - ECE 250
	Project3
	Author: Kyle Dyck
*/

#include "DJS_Node.h"

class DJS_List
{
	// Represents a set of vertices whose head is the representative vertex
	private:
		DJS_Node* rep;
		DJS_Node* tail;
		uint64_t size;

	public:

		// Constructors
		// Creates list of size 1 with node labelled with given number
		DJS_List(uint64_t&);
		// Creates a list of size 0 having tail / rep set to null
		DJS_List();


		// Destructors
		// Deletes rep if it is not null
		~DJS_List();


		// Setters
		// Sets rep and tail to null, size to 0, DOES NOT DELETE REP
		void clear_list();
		// Adds a node to end of list, calls set_rep of the given node to the current rep
		void append(uint64_t&);
		void set_tail(DJS_Node*);


		// Getters
		DJS_Node* get_rep() const;
		DJS_Node* get_tail() const;
		uint64_t get_size() const;
		// Searches list for given number
		// Returns true if found, false otherwise
		bool search_vertex(uint64_t&) const;
};

