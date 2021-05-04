#include "Vertex.h"

Vertex::Vertex() {
	edge = 0;
	weight = 0;
}
Vertex::~Vertex() {

}
void Vertex::update(const bool new_edge, const double& new_weight) {
	edge = new_edge;
	weight = new_weight;
}