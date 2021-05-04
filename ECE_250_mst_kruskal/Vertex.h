#pragma once

#include <stdint.h>
struct Vertex
{
		bool edge;
		double weight;

		Vertex();
		~Vertex();
		
		// Updates attributes, last parameters is for incrementing / decrementing degree
		// False: decrement		True: Increment
		void update(const bool, const double&);
};

