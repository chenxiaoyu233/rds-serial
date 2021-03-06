#ifndef __RDS_UTILS_HPP__
#define __RDS_UTILS_HPP__
#include <chrono>
#include <iostream>
#include "../graph/graph_matrix.h"
#include "../graph/graph_utils.hpp"

struct algorithm_run {
  std::string graphname;
  bool valid = false;
  bool complete = false;
  bool reverse = false;
  bool complement = false;
  bool correct = false;

  unsigned int time_limit;

  std::chrono::duration<double> time;
  unsigned int value;
  unsigned int last_i;

  std::vector<uint> certificate;

  bool allout = false; // output for all mu[i]. default: only for the last of when m[i] != m[i+1]
};

struct vertex_set {
  std::vector<uint> vertices;
  uint weight = 0;

  inline void add_vertex(const uint v, const uint w) {
    vertices.push_back(v);
    weight += w;
  }

  inline void pop_vertex(const uint w) {
    vertices.pop_back();
    weight -= w;
  }

  inline size_t size() const {
    return vertices.size();
  }

  inline bool empty() const {
    return vertices.empty();
  }

  inline void clear() {
    vertices.resize(0);
    weight = 0;
  }

  inline void reserve(const size_t size) {
    vertices.reserve(size);
  }

  inline void assign(const vertex_set& vs) {
    vertices.assign(vs.vertices.begin(), vs.vertices.end());
    weight = vs.weight;
  }

  operator std::vector<uint>&() { return vertices; }

  inline const uint& operator[](const size_t& idx) const { return vertices[idx]; }
  inline uint& operator[](const size_t& idx) { return vertices[idx]; }
  const decltype(vertices)::value_type& back() const { return vertices.back(); }

  inline std::vector<uint>::iterator begin() { return vertices.begin(); }
  inline std::vector<uint>::iterator end() { return vertices.end(); }
};

using RDSMethod = std::function<algorithm_run(ordering, bool, bool, bool, unsigned int, const std::string&)>;

template <typename Verifier> algorithm_run run_rds(std::vector<int> verifier_parameters, ordering order, bool reverse, bool do_complement, bool allout, unsigned int time_limit, const std::string& graph_file) {
  Verifier *v = new Verifier;
  for (auto& p: verifier_parameters) v->provide_parameter(p);
  algorithm_run result;
  result.graphname = graph_file;
  result.reverse = reverse;
  result.time_limit = time_limit;
  result.complement = do_complement;
  result.allout = allout;

  Graph* g = from_dimacs<Graph>(graph_file.c_str());

  if (!g) {
    result.valid = false;
    return result;
  }

  std::cerr<<"Done"<<std::endl;
  if (do_complement) {
    auto ng = complement<Graph>(g);
    delete g;
    g = ng;
  }

  g->apply_order(order, reverse);
  std::cerr<<"Done"<<std::endl;
  v->bind_graph(g);
  std::cerr<<"Calling rds..."<<std::endl;
  rds(v, g, result);
  std::cerr<<"Done"<<std::endl;

  result.correct = v->check_solution(result.certificate);
  g->restore_order(result.certificate);
  std::sort(result.certificate.begin(), result.certificate.end());
  delete v;
  delete g;
  return result;
}
#endif
