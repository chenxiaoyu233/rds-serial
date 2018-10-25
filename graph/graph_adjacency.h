#ifndef _GRAPH_ADJACENCY_H
#define _GRAPH_ADJACENCY_H

#include <vector>
#include <unordered_set>
#include "graph.h"
#include "orders.h"

class graph_adjacency: public graph {
private:
  std::vector<std::unordered_set<uint>>  adj;
  void reorder_custom(const std::vector<uint>& order) final override;

public:
  graph_adjacency(uint n): graph(n), adj(n) {}
  ~graph_adjacency() {}

  inline void add_edge(uint i, uint j) final override {adj[i].emplace(j); adj[j].emplace(i);}
  inline bool is_edge(uint i, uint j) const final override {return (adj[i].find(j) != adj[i].end());}
  void separate_vertex(uint i) final override;
  
  using neighbourhood_iterator = std::unordered_set<uint>::const_iterator;

  inline neighbourhood_iterator nbh_begin(const uint v) {
    return adj[v].begin();
  }

  inline neighbourhood_iterator nbh_end(const uint v) {
    return adj[v].end();
  }
};

#endif
