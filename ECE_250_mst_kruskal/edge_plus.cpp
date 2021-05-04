/*
    Fall 2020 - ECE 250
    Project3
    Author: Kyle Dyck
*/

#include "edge_plus.h"

edge_plus::edge_plus(const uint64_t& u, const uint64_t& v, double& w) {
    edge_pair.first = u;
    edge_pair.second = v;
    weight = w;

}
edge_plus::edge_plus() {
    edge_pair.first = 0;
    edge_pair.second = 0;
    weight = 0;
}
edge_plus::~edge_plus() {

}
bool edge_plus::operator<(edge_plus other) {
    return this->weight < other.weight;
}