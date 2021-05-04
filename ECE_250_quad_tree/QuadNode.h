/*
	Fall 2020 - ECE 250
	Project2
	Author: Kyle Dyck
*/

#pragma once

#include <cassert>
#include <string>
#include <iostream>
#include <iomanip>

class QuadNode
{
	// To access different members, attributes and direction have been mapped accordingly:
	// 0, 1, 2, 3			:			ne, nw, sw, se		for direction
	// 0, 1, 2				:			cost, avg, pop		for attributes
	private:
		QuadNode* ne;				// North East
		QuadNode* nw;				// North West
		QuadNode* sw;				// South West
		QuadNode* se;				// South East
		
		double x;					// longitude
		double y;					// latitude
		double cost;				// Cost of Living
		double avg;					// Average Net Salary
		double pop;					// Population
		std::string value;			// Value

	public:

		// Constructors
		QuadNode(const double& longitude,
			const double& latitude,
			const double& cost_to_live,
			const double& average,
			const double& population,
			const std::string name);

		QuadNode();

		// Destructor
		~QuadNode();


		// Setters

		void set_ne(QuadNode*);
		void set_nw(QuadNode*);
		void set_sw(QuadNode*);
		void set_se(QuadNode*);

		void set_x(const double&);
		void set_y(const double&);
		void set_cost(const double&);
		void set_avg(const double&);
		void set_pop(const double&);

		// Recursively attempts to insert a new node
		// Deletes argument if current x, y coordinates
		// already exist in the tree and returns false
		// Returns true if successful
		bool insert_node(QuadNode*);

		// Getters

		QuadNode* get_ne() const;
		QuadNode* get_nw() const;
		QuadNode* get_sw() const;
		QuadNode* get_se() const;

		double get_x() const;
		double get_y() const;
		double get_cost() const;
		double get_avg() const;
		double get_pop() const;

		// Uses rules to determine which direction to place
		// given x, y coordinates
		// Returns number corresponding to calculated direction
		int get_direction(const double&, const double&) const;
		// Recursively goes through tree and returns node at x, y coordinates
		// Note: assumes node exists in tree, should be used after search 
		QuadNode* get_node(const double&, const double&);

		// Recursively goes through tree searching for x, y coordinates
		// If found, prints value and returns true
		// else returns false
		bool search(const double&, const double&, bool);
		// Recursively traverse tree to determine size
		int get_size(int&);

		// Prints current node's value then recursively goes through tree doing the same
		// Boolean for first value
		bool print_values(bool) const;
		// Recursively goes through tree, returns maximum value of given attribute
		double find_max(const int&, double&);
		// Recursively goes through tree, returns minimum value of given attribute
		double find_min(const int&, double&);
		// Recursively goes through tree, returns total value of given attribute
		double find_total(const int&, double&);

		// Searches tree for x, y then finds max of given attribute
		// Prints to screen depending on output
		void q_max(const double&, const double&, const int&, const int&);
		// Searches tree for x, y then finds min of given attribute
		// Prints to screen depending on output
		void q_min(const double&, const double&, const int&, const int&);
		// Searches tree for x, y then finds total of given attribute
		// Prints to screen depending on output
		void q_total (const double&, const double&, const int&, const int&);
};

