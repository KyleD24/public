#pragma once
/*
	Fall 2020 - ECE 250
	Project3
	Author: Kyle Dyck
*/

#include "DJS_List.h"

class DJS
{
	// Represents a disjoint set data structure
	// Contains an array of linked lists that each represent a disjoint set
	// Operations: union, makeset, findset

	private:
		DJS_List** set_list;
		uint64_t num_nodes;
		uint64_t num_sets;

	public:

		// Constructor
		// Creates aray of given size (number of vertices)
		// Initializes lists as empty
		DJS(uint64_t);


		// Destructor
		// Goes through set_list and deletes every list
		~DJS();

		
		// Setters

		// Attempts to join two sets together, assuming they aren't in the same set
		// Returns bool depending on the result:
		// False: one set remaining, done	True: merge successful
		bool set_union(uint64_t&, uint64_t&);
		// Makes a set in the next available slot in set_list
		// Note: will cause error if set_list is full
		void makeset(uint64_t&);

		
		// Getters

		// Searches for number in list
		// Assumes number exists somewhere in list
		DJS_List* findset(uint64_t&);
};

