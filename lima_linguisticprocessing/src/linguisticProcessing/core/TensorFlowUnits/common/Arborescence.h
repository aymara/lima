#include <vector>
#include <set>
#include <map>
#include <iostream>
#include <sstream>
#include <boost/pending/disjoint_sets.hpp>

#ifndef ARBORESCENCE_H
#define ARBORESCENCE_H

namespace impl
{

template <typename vertex_idx_t,
          typename weight_t>
class Arborescence
{
  struct edge_t
  {
    vertex_idx_t source;
    vertex_idx_t target;
    weight_t weight;
    edge_t* parent = nullptr;
    std::vector<edge_t*> children;
    bool removed = false;

    edge_t(const vertex_idx_t s, const vertex_idx_t t, const weight_t w)
        : source(s), target(t), weight(w)
    {}
  };

  static void remove_from_f(edge_t* e, std::vector<edge_t*>& f_roots)
  {
    while (e != nullptr)
    {
      e->removed = true;
      for (edge_t* child : e->children)
      {
        f_roots.push_back(child);
        child->parent = nullptr;
      }

      std::vector<edge_t*>().swap(e->children);

      e = e->parent;
    }
  }

public:

  // adj_matrix[i][j]: i <- j (j is head)
  static void arborescence_impl(const std::function <float(size_t, size_t)>& adj_matrix,
                                size_t len,
                                std::vector<size_t>& heads)
  {
    std::vector<edge_t> all_edges;
    all_edges.reserve(len * len); // TODO: move memory allocation out of this function
    std::vector<std::vector<edge_t*>> in_edges;
    in_edges.resize(len);

    for (vertex_idx_t i = 1; i < len; i++)
      for (vertex_idx_t j = 1; j < len; j++)
      {
        if (i == j)
          continue;
        all_edges.push_back(edge_t(j, i, adj_matrix(i, j)));
        in_edges[i].push_back(&all_edges.back());
      }

    for (edge_t& e : all_edges)
      in_edges[e.target].push_back(&e);

    std::vector<std::vector<edge_t*>> cycle(len);
    std::vector<edge_t*> lambda(len);
    std::vector<vertex_idx_t> roots;
    std::vector<vertex_idx_t> final_roots; // = { 0 };
    boost::disjoint_sets_with_storage<> S(2 * len);
    boost::disjoint_sets_with_storage<> W(2 * len);
    std::vector<vertex_idx_t> min(len);
    std::vector<edge_t*> enter(len);
    std::vector<edge_t*> F;
    std::vector<weight_t> edge_weight_change(len);

    for (vertex_idx_t v = 0; v < len; ++v)
    {
      S.make_set(v);
      W.make_set(v);
      min[v] = v;
      if (v != 0)
        roots.push_back(v);
    }

    while (!roots.empty())
    {
      vertex_idx_t curr = roots.back();
      roots.pop_back();

      if (in_edges[curr].empty())
      {
        final_roots.push_back(min[curr]);
        continue;
      }

      edge_t *optimal_in_edge = in_edges[curr].front();
      for (edge_t* e : in_edges[curr])
        if (e->weight > optimal_in_edge->weight)
          optimal_in_edge = e;

      F.push_back(optimal_in_edge);
      for (edge_t* e : cycle[curr])
      {
        e->parent = optimal_in_edge;
        optimal_in_edge->children.push_back(e);
      }

      if (cycle[curr].empty())
        lambda[curr] = optimal_in_edge;

      // adding optimal_in_edge don't create a cycle
      if (W.find_set(optimal_in_edge->source) != W.find_set(optimal_in_edge->target))
      {
        enter[curr] = optimal_in_edge;
        W.union_set(optimal_in_edge->source, optimal_in_edge->target);
      }
      else // adding optimal_in_edge creates a cycle
      {
        std::vector<edge_t*> cycle_edges = { optimal_in_edge };
        std::vector<vertex_idx_t> cycle_repr = { S.find_set(optimal_in_edge->target) };
        edge_t* least_costly_edge = optimal_in_edge;
        enter[curr] = nullptr;

        for (vertex_idx_t v = S.find_set(optimal_in_edge->source);
             enter[v] != nullptr;
             v = S.find_set(enter[v]->source))
        {
          cycle_edges.push_back(enter[v]);
          cycle_repr.push_back(v);

          if (enter[v]->weight < least_costly_edge->weight)
            least_costly_edge = enter[v];
        }

        for (edge_t* e : cycle_edges)
          edge_weight_change[S.find_set(e->target)] = least_costly_edge->weight - e->weight;

        vertex_idx_t cycle_root = min[S.find_set(least_costly_edge->target)];

        // Union all components of the cycle into one component
        vertex_idx_t new_repr = cycle_repr.front();
        for (vertex_idx_t v : cycle_repr)
        {
          S.link(v, new_repr);
          new_repr = S.find_set(new_repr);
        }
        min[new_repr] = cycle_root;
        roots.push_back(new_repr);
        cycle[new_repr].swap(cycle_edges);

        for (vertex_idx_t v : cycle_repr)
        {
          for (edge_t* e : in_edges[v])
          {
            e->weight += edge_weight_change[v];
          }
        }

        std::vector<edge_t*> new_in_edges;
        for (size_t i = 1; i < cycle_repr.size(); ++i)
        {
          typename std::vector<edge_t*>::iterator i1 = in_edges[cycle_repr[i]].begin();
          typename std::vector<edge_t*>::iterator e1 = in_edges[cycle_repr[i]].end();
          typename std::vector<edge_t*>::iterator i2 = in_edges[cycle_repr[i-1]].begin();
          typename std::vector<edge_t*>::iterator e2 = in_edges[cycle_repr[i-1]].end();

          while (i1 != e1 || i2 != e2)
          {
            while (i1 != e1 && S.find_set((*i1)->source) == new_repr)
              ++i1;

            while (i2 != e2 && S.find_set((*i2)->source) == new_repr)
              ++i2;

            if (i1 == e1 && i2 == e2)
              break;

            if (i1 == e1)
            {
              new_in_edges.push_back(*i2);
              ++i2;
            }
            else if (i2 == e2)
            {
              new_in_edges.push_back(*i1);
              ++i1;
            }
            else if ( (*i1)->source < (*i2)->source )
            {
              new_in_edges.push_back(*i1);
              ++i1;
            }
            else if ( (*i1)->source > (*i2)->source )
            {
              new_in_edges.push_back(*i2);
              ++i2;
            }
            else
            {
              if ( (*i1)->weight > (*i2)->weight )
                new_in_edges.push_back(*i1);
              else
                new_in_edges.push_back(*i2);

              ++i1;
              ++i2;
            }
          }

          in_edges[cycle_repr[i]].swap(new_in_edges);
          new_in_edges.clear();
        }

        in_edges[new_repr].swap(in_edges[cycle_repr.back()]);
        edge_weight_change[new_repr] = weight_t(0);
      }
    }

    std::vector<edge_t*> F_roots;
    for (edge_t* e : F)
    {
      if (e->parent == nullptr)
        F_roots.push_back(e);
    }

    for (vertex_idx_t v : final_roots)
    {
      if (lambda[v] != nullptr)
        remove_from_f(lambda[v], F_roots);
    }

    while (!F_roots.empty())
    {
      edge_t* e = F_roots.back();
      F_roots.pop_back();

      if (e->removed)
        continue;

      heads[e->target] = e->source;
      remove_from_f(lambda[e->target], F_roots);
    }

    return;
  }
};

}

template <typename vertex_idx_t,
          typename weight_t>
void arborescence(const std::function <weight_t(vertex_idx_t, vertex_idx_t)>& adj_matrix,
                  size_t len,
                  std::vector<vertex_idx_t>& heads)
{
  impl::Arborescence<vertex_idx_t, weight_t>::arborescence_impl(adj_matrix, len, heads);
}

#endif
