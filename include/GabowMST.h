#ifndef GABOW_MST_H
#define GABOW_MST_H

#include "WeightedGraph.h"
#include <vector>
#include <limits>
#include <stdexcept>

/*
 * GabowMST.h
 * Implementação prática inspirada nas técnicas de Gabow et al.
 */

struct GabowEdge
{
  int from;
  int to;
  double weight;
  GabowEdge(int f = -1, int t = -1, double w = 0.0) : from(f), to(t), weight(w) {}
};

class GabowMST
{
private:
  int n;
  std::vector<GabowEdge> edges;

  std::vector<GabowEdge> arborescence;
  double totalWeight;

  static constexpr double INF = std::numeric_limits<double>::infinity();

public:
  // extrai arestas do WeightedGraph
  GabowMST(const WeightedGraph &g);

  // Computa a arborescência mínima enraizada em root
  std::vector<GabowEdge> compute(int root);

  double getTotalWeight() const { return totalWeight; }

  void printArborescence(const std::vector<GabowEdge> &arb) const;
};

#endif // GABOW_MST_H
