#pragma once

#include <vector>
#include "Vertex.h"
#include <cassert>
#include <iostream>
#include "illegal_argument.h"

class Adj_Mat
{
	// Represents a weighted adjacency matrix
	// Operations: insert edge, delete edge, get dgree, get edge count, clear edges

	private:
		uint64_t num_nodes;
		uint64_t num_edges;
		// This is the weighted matrix
		std::vector <std::vector <Vertex>>* matrix;
		// Holds the degrees of every node
		// where each node is an index
		uint64_t* degrees;

	public:
		// Constructors
		// Sets num_nodes to given value, num edges to 0, makes matrix of 0s with given size
		Adj_Mat(long long&);


		// Destructors
		// Deletes matrix
		~Adj_Mat();


		// Setters

		// Updates matrix and increments num_edges, asserts valid input
		void insert_edge(const long long&, const long long&, const double&);
		// Tries to delete edge
		// Returns false if no edge exists, asserts valid values
		bool delete_edge(const long long&, const long long&);
		// Clears all edges from matrix
		void clear();

		// Getters

		// Returns degree of given node, asserts valid argument
		uint64_t get_degree(const uint64_t&);
		uint64_t get_num_edges() const;
		uint64_t get_num_nodes() const;
		void print_matrix() const;
		Vertex read_matrix(const uint64_t&, const uint64_t&) const;

};

