#pragma once

#include <stdint.h>
#include <utility>

struct edge_plus
{
    std::pair<uint64_t, uint64_t> edge_pair;
    double weight;

    edge_plus(const uint64_t& u, const uint64_t& v, double& w);
    edge_plus();
    ~edge_plus();

    bool operator<(edge_plus other);
};





