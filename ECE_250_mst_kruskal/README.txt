This is my implementation of project3 for ECE 250 taken in Fall 2020 at the University of Waterloo. 
Unless otherwise stated in comments, all program design and implementation was done by me.

This project received a mark of 100% for passing all the test cases. 
This project passed 100% of all the memory leak test cases using Valgrind.

Students had to create a Makefile that was used for executing the program and all testing was done in an Ubuntu environment

Students were required to implement a minimum spanning tree (MST) of a weighted undirected graph using Kruskal's algorithm.
In my implementation, the graph was represented as an adjacency matrix. 
A disjoint set class was also implemented to ensure the procedure did not loop in the MST.

The following functions were required:
* create a graph with any given number of nodes
* insert an edge between two nodes
* delete an edge between two nodes
* return the degree of a given vertex
* return total number of edges in the graph
* remove all the edges from the graph
* calculate the minimum spanning tree and return its total weight

The main file is:
* kruskaltest.cpp
