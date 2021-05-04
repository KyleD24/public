#include "Adj_Mat.h"

// Constructor / Destructor
Adj_Mat::Adj_Mat(long long& number_in)
{
	if (number_in <= 0) {
		// Invalid
		throw illegal_argument();
	}

	auto number = static_cast<uint64_t>(number_in);

	num_edges = 0;
	num_nodes = number;

	matrix = new std::vector <std::vector <Vertex>>(num_nodes, std::vector<Vertex>(num_nodes));

	// Note: when indexing matrix, make first index 0, then the next two will be coords

	degrees = new uint64_t[num_nodes]{ 0 };

}
Adj_Mat::~Adj_Mat() {
	delete matrix;
	matrix = nullptr;
	delete [] degrees;
	degrees = nullptr;
}

// Setters
void Adj_Mat::insert_edge(const long long& u_in, const long long& v_in, const double& w_in) {
	auto signed_num_nodes = static_cast<long long>(num_nodes);
	if (u_in >= signed_num_nodes || v_in >= signed_num_nodes || w_in <= 0 || u_in < 0 || v_in < 0) {
		// u, v outside range or negative weight
		// Invalid
		throw illegal_argument();
	}

	auto u = static_cast<uint64_t>(u_in);
	auto v = static_cast<uint64_t>(v_in);
	auto w = static_cast<double>(w_in);
	
	bool temp_assert = u < num_nodes && v < num_nodes;
	assert(temp_assert);

	if (matrix[0][u][v].edge) {
		assert(matrix[0][v][u].edge);
		// Edge already exists, prepare for overwrite
		degrees[u]--;
		degrees[v]--;
		num_edges--;
	}

	// Update matrix in u-v spot
	matrix[0][u][v].update(1, w);
	// Graph is undirected, update slot in v-u
	matrix[0][v][u].update(1, w);

	num_edges++;
	degrees[u]++;
	degrees[v]++;
}
bool Adj_Mat::delete_edge(const long long& u_in, const long long& v_in) {
	auto signed_num_nodes = static_cast<int64_t>(num_nodes);
	if (u_in >= signed_num_nodes || v_in >= signed_num_nodes || u_in < 0 || v_in < 0) {
		// Outisde range, invalid
		throw illegal_argument();
	}

	auto u = static_cast<uint64_t>(u_in);
	auto v = static_cast<uint64_t>(v_in);
	
	bool temp_assert = u < num_nodes && v < num_nodes;
	assert(temp_assert);

	if (matrix[0][u][v].edge) {
		assert(matrix[0][v][u].edge);
		// Edge exists, delete it
		matrix[0][u][v].update(0, 0);
		matrix[0][v][u].update(0, 0);

		num_edges--;
		degrees[u]--;
		degrees[v]--;
	}
	else {
		// Edge does not exist
		return false;
	}

	return true;
}
void Adj_Mat::clear() {
	for (int i = 0; i < num_nodes; i++) {
		for (int j = 0; j < num_nodes; j++) {
			delete_edge(i, j);
		}
	}
}

// Getters
uint64_t Adj_Mat::get_degree(const uint64_t& u_in) {
	auto signed_num_nodes = static_cast<long long>(num_nodes);
	if (u_in >= signed_num_nodes || u_in < 0) {
		// Outside range, invalid
		throw illegal_argument();
	}

	auto u = static_cast<uint64_t>(u_in);
	
	return degrees[u];
}
uint64_t Adj_Mat::get_num_edges() const {
	return num_edges;
}
uint64_t Adj_Mat::get_num_nodes() const {
	return num_nodes;
}
void Adj_Mat::print_matrix() const {
	std::cout << "--------------------------------------------------------------" << std::endl;
	for (int i = 0; i < num_nodes; i++) {
		std::cout << "deg: " << degrees[i] << "    ";
		for (int j = 0; j < num_nodes; j++) {
			Vertex temp = matrix[0][i][j];
			auto edge = temp.edge;
			auto weight = temp.weight;

			std::cout << "(" << edge << ", " << weight << ") ";
		}
		std::cout << std::endl;
	}
	std::cout << "--------------------------------------------------------------" << std::endl << std::endl;
}
Vertex Adj_Mat::read_matrix(const uint64_t& i, const uint64_t& j) const
{
	return matrix[0][i][j];
}
