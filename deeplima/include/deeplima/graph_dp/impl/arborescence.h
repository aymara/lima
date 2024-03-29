// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef ARBORESCENCE_H
#define ARBORESCENCE_H

#include <vector>
#include <set>
#include <map>
#include <list>
#include <iostream>
#include <iterator> // needed for std::ostram_iterator
#include <sstream>
#include <boost/pending/disjoint_sets.hpp>

namespace deeplima
{

// template <typename T>
// std::ostream& operator<< (std::ostream& out, const std::vector<T>& v) {
//   out << '[';
//   if ( !v.empty() ) {
//     std::copy (v.begin(), v.end(), std::ostream_iterator<T>(out, ", "));
//   }
//   out << "]";
//   return out;
// }

namespace impl
{

template <class M,
          typename vertex_idx_t,
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
    // std::cerr << "remove_from_f" << std::endl;
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

  static void fill_heads_with_max(const std::function <float(size_t, size_t)>& adj_matrix,
                                  size_t len,
                                  std::vector<size_t>& heads)
  {
    // std::cerr << "fill_heads_with_max" << std::endl;
    for (vertex_idx_t i = 1; i < len; i++)
    {
      vertex_idx_t max_id = 0;
      float max_value = adj_matrix(i, max_id);
      for (vertex_idx_t j = 1; j < len; j++)
      {
        if (i == j)
          continue;

        if (adj_matrix(i, j) > max_value)
        {
          max_id = j;
          max_value = adj_matrix(i, j);
        }
      }
      heads[i] = max_id;
    }
  }

  /**
   * A node is a root if its head id is 0.
   */
  static size_t count_roots(typename std::vector<vertex_idx_t>::const_iterator pos,
                            typename std::vector<vertex_idx_t>::const_iterator end)
  {
    // std::cerr << "count_roots pos=" << *pos << "; end=" << *end << std::endl;
    size_t c = 0;
    pos++;
    for (; pos != end; pos++)
    {
      if (*pos == 0)
        c++;
    }
    // std::cerr << "count_roots: " << c << std::endl;
    return c;
  }

  static bool is_connected(typename std::vector<vertex_idx_t>::const_iterator pos,
                           typename std::vector<vertex_idx_t>::const_iterator end)
  {
    // std::cerr << "is_connected" << std::endl;
    std::vector< std::vector<size_t> > head2child;
    size_t len = end - pos;
    head2child.resize(len);
    pos++;
    for (size_t i = 1; i < len; i++)
    {
      head2child[*pos].push_back(i);
      pos++;
    }

    std::vector<size_t> visited(len, 0);
    std::vector<size_t> stack;
    stack.reserve(len);
    stack.push_back(0);
    while (stack.size() > 0)
    {
      size_t from = stack.back();
      stack.pop_back();

      for (size_t to : head2child[from])
      {
        if (visited[to] == 0)
        {
          stack.push_back(to);
          visited[to] += 1;
        }
      }
    }

    for (size_t i = 1; i < visited.size(); i++)
      if (visited[i] == 0)
        return false;

    return true;
  }

  static void find_disconnected_groups(const std::vector< std::vector<size_t> >& head2child,
                                       size_t len,
                                       std::vector<size_t>& accessibility_map)
  {
    // std::cerr << "find_disconnected_groups" << std::endl;
    std::fill(accessibility_map.begin(), accessibility_map.end(), 0);

    std::vector<size_t> stack;
    stack.reserve(len);
    stack.push_back(0);
    while (stack.size() > 0)
    {
      size_t from = stack.back();
      stack.pop_back();

      for (size_t to : head2child[from])
      {
        if (accessibility_map[to] == 0)
        {
          stack.push_back(to);
          accessibility_map[to] += 1;
        }
      }
    }
  }

  static void find_loops(const std::vector<vertex_idx_t>& heads,
                         std::vector< std::vector<size_t> >& loops,
                         const std::vector<size_t>& connected,
                         size_t len,
                         size_t offset)
  {
    // std::cerr << "find_loops" << std::endl;
    std::vector<size_t> visited = connected;
    loops.clear();

    for (size_t i = 1; i < len; i++)
    {
      if (visited[i] > 0)
      {
        visited[i] = 1;
        continue;
      }

      std::vector<size_t> loop_counter(len, 0);
      std::list<size_t> loop_items;
      size_t j = i;
      while (loop_counter[j] == 0 && visited[j] == 0)
      {
        loop_counter[j] += 1;
        loop_items.push_back(j);
        j = heads[offset+j];
      }

      if (visited[j] > 0)
        continue;

      while (loop_items.front() != j)
        loop_items.pop_front();

      std::vector<size_t> new_loop;
      for (size_t x : loop_items)
      {
        visited[x] = 1;
        new_loop.push_back(x);
      }

      loops.push_back(new_loop);
    }
  }

  static void make_connected(const M& adj_matrix,
                             std::vector<vertex_idx_t>& heads,
                             size_t offset)
  {
    // std::cerr << "make_connected" << std::endl;
    std::vector< std::vector<size_t> > head2child;
    size_t len = adj_matrix.rows();
    assert(adj_matrix.rows() == adj_matrix.cols());
    head2child.resize(len);
    for (size_t i = 1; i < len; i++)
    {
      head2child[heads[offset+i]].push_back(i);
    }

    std::vector<size_t> connected(len, 0);
    find_disconnected_groups(head2child, len, connected);

    while (std::find(std::next(connected.begin()), connected.end(), 0) != connected.end())
    {
      std::vector< std::vector<size_t> > loops;
      find_loops(heads, loops, connected, len, offset);

      std::pair<size_t, size_t> best_new_arc = std::make_pair(0, 0); // child -> parent
      float best_score = 0;
      for (size_t l = 0; l < loops.size(); l++)
      {
        for (size_t i = 0; i < loops[l].size(); i++)
        {
          size_t from = loops[l][i];
          for (size_t j = 1; j < len; j++)
          {
            if (connected[j] == 0)
              continue; // we have no intentions to create new loops
            if (j == heads[offset+from])
              continue; // this value of j changes nothing
            if (adj_matrix(from, j) > best_score || best_new_arc.first == 0)
            {
              best_score = adj_matrix(from, j);
              best_new_arc = std::make_pair(from, j);
            }
          }
        }
      }

      if (best_new_arc.first == 0 || best_new_arc.second == 0)
        throw std::runtime_error("make_connected best_new_arc first or second should be 0");

      heads[offset+best_new_arc.first] = best_new_arc.second;

      head2child.clear();
      head2child.resize(len);
      for (size_t i = 1; i < len; i++)
      {
        head2child[heads[offset+i]].push_back(i);
      }

      std::fill(connected.begin(), connected.end(), 0);
      find_disconnected_groups(head2child, len, connected);
    }
  }

  static void choose_one_root(const M& adj_matrix,
                              std::vector<vertex_idx_t>& heads,
                              size_t offset)
  {
    // std::cerr << "choose_one_root" << std::endl;
    assert(adj_matrix.rows() == adj_matrix.cols());
    size_t len = adj_matrix.rows();

    std::vector<std::pair<size_t, float>> roots;
    roots.reserve(16);
    for (size_t i = 1; i < len; i++)
    {
      if (heads[offset + i] == 0)
      {
        roots.push_back(std::make_pair(i, adj_matrix(i, 0)));
      }
    }

    if (roots.size() == 0)
      // This function is intended to choose from a several (> 0) of roots
      throw std::runtime_error("choose_one_root should have at least one root");

    if (roots.size() == 1)
      return;

    size_t best_root = 0;
    float best_root_score = 0;
    for (size_t i = 0; i < roots.size(); i++)
    {
      if (best_root == 0 || roots[i].second > best_root_score)
      {
        best_root = roots[i].first;
        best_root_score = roots[i].second;
      }
    }

    for (auto& r : roots)
    {
      if (r.first != best_root)
        heads[offset + r.first] = best_root;
    }
  }

  static void choose_root(const M& adj_matrix,
                          std::vector<vertex_idx_t>& heads,
                          size_t offset)
  {
    // std::cerr << "choose_root" << std::endl;
    assert(adj_matrix.rows() == adj_matrix.cols());
    size_t len = adj_matrix.rows();

    if (len < 3)
    {
      heads[offset + 1] = 0;
      return;
    }

    size_t best_root = 1;
    float best_root_score = adj_matrix(best_root, 0);
    for (size_t i = 2; i < len; i++)
    {
      if (adj_matrix(i, 0) > best_root_score)
      {
        best_root = i;
        best_root_score = adj_matrix(i, 0);
      }
    }

    heads[offset + best_root] = 0;
  }

  // adj_matrix[i][j]: i <- j (j is head)
  static void arborescence_impl(const M& adj_matrix,
                                std::vector<vertex_idx_t>& heads,
                                size_t offset)
  {
    // std::cerr << "arborescence_impl" << std::endl;
    assert(adj_matrix.rows() == adj_matrix.cols());
    size_t len = adj_matrix.rows();

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

      heads[offset + e->target] = e->source;
      remove_from_f(lambda[e->target], F_roots);
    }

    return;
  }
};

template <typename vertex_idx_t,
          typename weight_t>
class ArborescenceHeads
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
    // std::cerr << "remove_from_f" << std::endl;
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

  /**
   * A node is a root if its head id is 0.
   */
  static size_t count_roots(typename std::vector<vertex_idx_t>::const_iterator pos,
                            typename std::vector<vertex_idx_t>::const_iterator end)
  {
    // std::cerr << "count_roots pos=" << *pos << "; end=" << *end << std::endl;
    size_t c = 0;
    pos++;
    for (; pos != end; pos++)
    {
      if (*pos == 0)
        c++;
    }
    // std::cerr << "count_roots: " << c << std::endl;
    return c;
  }

  static bool is_connected(typename std::vector<vertex_idx_t>::const_iterator pos,
                           typename std::vector<vertex_idx_t>::const_iterator end)
  {
    // std::cerr << "is_connected" << std::endl;
    std::vector< std::vector<size_t> > head2child;
    size_t len = end - pos;
    head2child.resize(len);
    pos++;
    for (size_t i = 1; i < len; i++)
    {
      head2child[*pos].push_back(i);
      pos++;
    }

    std::vector<size_t> visited(len, 0);
    std::vector<size_t> stack;
    stack.reserve(len);
    stack.push_back(0);
    while (stack.size() > 0)
    {
      size_t from = stack.back();
      stack.pop_back();

      for (size_t to : head2child[from])
      {
        if (visited[to] == 0)
        {
          stack.push_back(to);
          visited[to] += 1;
        }
      }
    }

    for (size_t i = 1; i < visited.size(); i++)
      if (visited[i] == 0)
        return false;

    return true;
  }

  static void find_disconnected_groups(const std::vector< std::vector<size_t> >& head2child,
                                       size_t len,
                                       std::vector<size_t>& accessibility_map)
  {
    // std::cerr << "find_disconnected_groups" << std::endl;
    std::fill(accessibility_map.begin(), accessibility_map.end(), 0);

    std::vector<size_t> stack;
    stack.reserve(len);
    stack.push_back(0);
    while (stack.size() > 0)
    {
      size_t from = stack.back();
      stack.pop_back();

      for (size_t to : head2child[from])
      {
        if (accessibility_map[to] == 0)
        {
          stack.push_back(to);
          accessibility_map[to] += 1;
        }
      }
    }
  }

  static void find_loops(const std::vector<vertex_idx_t>& heads,
                         std::vector< std::vector<size_t> >& loops,
                         const std::vector<size_t>& connected,
                         size_t len,
                         size_t offset)
  {
    // std::cerr << "find_loops" << std::endl;
    std::vector<size_t> visited = connected;
    loops.clear();

    for (size_t i = 1; i < len; i++)
    {
      if (visited[i] > 0)
      {
        visited[i] = 1;
        continue;
      }

      std::vector<size_t> loop_counter(len, 0);
      std::list<size_t> loop_items;
      size_t j = i;
      while (loop_counter[j] == 0 && visited[j] == 0)
      {
        loop_counter[j] += 1;
        loop_items.push_back(j);
        j = heads[offset+j];
      }

      if (visited[j] > 0)
        continue;

      while (loop_items.front() != j)
        loop_items.pop_front();

      std::vector<size_t> new_loop;
      for (size_t x : loop_items)
      {
        visited[x] = 1;
        new_loop.push_back(x);
      }

      loops.push_back(new_loop);
    }
  }

  // static void make_connected(std::vector<vertex_idx_t>& heads,
  //                            size_t offset)
  // {
  //   // std::cerr << "make_connected" << std::endl;
  //   std::vector< std::vector<size_t> > head2child;
  //   size_t len = adj_matrix.rows();
  //   assert(adj_matrix.rows() == adj_matrix.cols());
  //   head2child.resize(len);
  //   for (size_t i = 1; i < len; i++)
  //   {
  //     head2child[heads[offset+i]].push_back(i);
  //   }
  //
  //   std::vector<size_t> connected(len, 0);
  //   find_disconnected_groups(head2child, len, connected);
  //
  //   while (std::find(std::next(connected.begin()), connected.end(), 0) != connected.end())
  //   {
  //     std::vector< std::vector<size_t> > loops;
  //     find_loops(heads, loops, connected, len, offset);
  //
  //     std::pair<size_t, size_t> best_new_arc = std::make_pair(0, 0); // child -> parent
  //     float best_score = 0;
  //     for (size_t l = 0; l < loops.size(); l++)
  //     {
  //       for (size_t i = 0; i < loops[l].size(); i++)
  //       {
  //         size_t from = loops[l][i];
  //         for (size_t j = 1; j < len; j++)
  //         {
  //           if (connected[j] == 0)
  //             continue; // we have no intentions to create new loops
  //           if (j == heads[offset+from])
  //             continue; // this value of j changes nothing
  //           if (adj_matrix(from, j) > best_score || best_new_arc.first == 0)
  //           {
  //             best_score = adj_matrix(from, j);
  //             best_new_arc = std::make_pair(from, j);
  //           }
  //         }
  //       }
  //     }
  //
  //     if (best_new_arc.first == 0 || best_new_arc.second == 0)
  //       throw std::runtime_error("make_connected best_new_arc first or second should be 0");
  //
  //     heads[offset+best_new_arc.first] = best_new_arc.second;
  //
  //     head2child.clear();
  //     head2child.resize(len);
  //     for (size_t i = 1; i < len; i++)
  //     {
  //       head2child[heads[offset+i]].push_back(i);
  //     }
  //
  //     std::fill(connected.begin(), connected.end(), 0);
  //     find_disconnected_groups(head2child, len, connected);
  //   }
  // }

  static void choose_one_root(std::vector<vertex_idx_t>& heads,
                              size_t offset)
  {
    // std::cerr << "choose_one_root" << std::endl;
  //   assert(adj_matrix.rows() == adj_matrix.cols());
  //   size_t len = adj_matrix.rows();
  //
  //   std::vector<std::pair<size_t, float>> roots;
  //   roots.reserve(16);
  //   for (size_t i = 1; i < len; i++)
  //   {
  //     if (heads[offset + i] == 0)
  //     {
  //       roots.push_back(std::make_pair(i, adj_matrix(i, 0)));
  //     }
  //   }
  //
  //   if (roots.size() == 0)
  //     // This function is intended to choose from a several (> 0) of roots
  //     throw std::runtime_error("choose_one_root should have at least one root");
  //
  //   if (roots.size() == 1)
  //     return;
  //
  //   size_t best_root = 0;
  //   float best_root_score = 0;
  //   for (size_t i = 0; i < roots.size(); i++)
  //   {
  //     if (best_root == 0 || roots[i].second > best_root_score)
  //     {
  //       best_root = roots[i].first;
  //       best_root_score = roots[i].second;
  //     }
  //   }
  //
  //   for (auto& r : roots)
  //   {
  //     if (r.first != best_root)
  //       heads[offset + r.first] = best_root;
  //   }
  }

  static void choose_root(std::vector<vertex_idx_t>& heads,
                          size_t offset)
  {
  //   // std::cerr << "choose_root" << std::endl;
  //   assert(adj_matrix.rows() == adj_matrix.cols());
  //   size_t len = adj_matrix.rows();
  //
  //   if (len < 3)
  //   {
  //     heads[offset + 1] = 0;
  //     return;
  //   }
  //
  //   size_t best_root = 1;
  //   float best_root_score = adj_matrix(best_root, 0);
  //   for (size_t i = 2; i < len; i++)
  //   {
  //     if (adj_matrix(i, 0) > best_root_score)
  //     {
  //       best_root = i;
  //       best_root_score = adj_matrix(i, 0);
  //     }
  //   }
  //
  //   heads[offset + best_root] = 0;
  }

  // // adj_matrix[i][j]: i <- j (j is head)
  // static void arborescence_impl(std::vector<vertex_idx_t>& heads,
  //                               size_t offset)
  // {
  //   // std::cerr << "arborescence_impl" << std::endl;
  //   assert(adj_matrix.rows() == adj_matrix.cols());
  //   size_t len = adj_matrix.rows();
  //
  //   std::vector<edge_t> all_edges;
  //   all_edges.reserve(len * len); // TODO: move memory allocation out of this function
  //   std::vector<std::vector<edge_t*>> in_edges;
  //   in_edges.resize(len);
  //
  //   for (vertex_idx_t i = 1; i < len; i++)
  //     for (vertex_idx_t j = 1; j < len; j++)
  //     {
  //       if (i == j)
  //         continue;
  //       all_edges.push_back(edge_t(j, i, adj_matrix(i, j)));
  //       in_edges[i].push_back(&all_edges.back());
  //     }
  //
  //   for (edge_t& e : all_edges)
  //     in_edges[e.target].push_back(&e);
  //
  //   std::vector<std::vector<edge_t*>> cycle(len);
  //   std::vector<edge_t*> lambda(len);
  //   std::vector<vertex_idx_t> roots;
  //   std::vector<vertex_idx_t> final_roots; // = { 0 };
  //   boost::disjoint_sets_with_storage<> S(2 * len);
  //   boost::disjoint_sets_with_storage<> W(2 * len);
  //   std::vector<vertex_idx_t> min(len);
  //   std::vector<edge_t*> enter(len);
  //   std::vector<edge_t*> F;
  //   std::vector<weight_t> edge_weight_change(len);
  //
  //   for (vertex_idx_t v = 0; v < len; ++v)
  //   {
  //     S.make_set(v);
  //     W.make_set(v);
  //     min[v] = v;
  //     if (v != 0)
  //       roots.push_back(v);
  //   }
  //
  //   while (!roots.empty())
  //   {
  //     vertex_idx_t curr = roots.back();
  //     roots.pop_back();
  //
  //     if (in_edges[curr].empty())
  //     {
  //       final_roots.push_back(min[curr]);
  //       continue;
  //     }
  //
  //     edge_t *optimal_in_edge = in_edges[curr].front();
  //     for (edge_t* e : in_edges[curr])
  //       if (e->weight > optimal_in_edge->weight)
  //         optimal_in_edge = e;
  //
  //     F.push_back(optimal_in_edge);
  //     for (edge_t* e : cycle[curr])
  //     {
  //       e->parent = optimal_in_edge;
  //       optimal_in_edge->children.push_back(e);
  //     }
  //
  //     if (cycle[curr].empty())
  //       lambda[curr] = optimal_in_edge;
  //
  //     // adding optimal_in_edge don't create a cycle
  //     if (W.find_set(optimal_in_edge->source) != W.find_set(optimal_in_edge->target))
  //     {
  //       enter[curr] = optimal_in_edge;
  //       W.union_set(optimal_in_edge->source, optimal_in_edge->target);
  //     }
  //     else // adding optimal_in_edge creates a cycle
  //     {
  //       std::vector<edge_t*> cycle_edges = { optimal_in_edge };
  //       std::vector<vertex_idx_t> cycle_repr = { S.find_set(optimal_in_edge->target) };
  //       edge_t* least_costly_edge = optimal_in_edge;
  //       enter[curr] = nullptr;
  //
  //       for (vertex_idx_t v = S.find_set(optimal_in_edge->source);
  //            enter[v] != nullptr;
  //            v = S.find_set(enter[v]->source))
  //       {
  //         cycle_edges.push_back(enter[v]);
  //         cycle_repr.push_back(v);
  //
  //         if (enter[v]->weight < least_costly_edge->weight)
  //           least_costly_edge = enter[v];
  //       }
  //
  //       for (edge_t* e : cycle_edges)
  //         edge_weight_change[S.find_set(e->target)] = least_costly_edge->weight - e->weight;
  //
  //       vertex_idx_t cycle_root = min[S.find_set(least_costly_edge->target)];
  //
  //       // Union all components of the cycle into one component
  //       vertex_idx_t new_repr = cycle_repr.front();
  //       for (vertex_idx_t v : cycle_repr)
  //       {
  //         S.link(v, new_repr);
  //         new_repr = S.find_set(new_repr);
  //       }
  //       min[new_repr] = cycle_root;
  //       roots.push_back(new_repr);
  //       cycle[new_repr].swap(cycle_edges);
  //
  //       for (vertex_idx_t v : cycle_repr)
  //       {
  //         for (edge_t* e : in_edges[v])
  //         {
  //           e->weight += edge_weight_change[v];
  //         }
  //       }
  //
  //       std::vector<edge_t*> new_in_edges;
  //       for (size_t i = 1; i < cycle_repr.size(); ++i)
  //       {
  //         typename std::vector<edge_t*>::iterator i1 = in_edges[cycle_repr[i]].begin();
  //         typename std::vector<edge_t*>::iterator e1 = in_edges[cycle_repr[i]].end();
  //         typename std::vector<edge_t*>::iterator i2 = in_edges[cycle_repr[i-1]].begin();
  //         typename std::vector<edge_t*>::iterator e2 = in_edges[cycle_repr[i-1]].end();
  //
  //         while (i1 != e1 || i2 != e2)
  //         {
  //           while (i1 != e1 && S.find_set((*i1)->source) == new_repr)
  //             ++i1;
  //
  //           while (i2 != e2 && S.find_set((*i2)->source) == new_repr)
  //             ++i2;
  //
  //           if (i1 == e1 && i2 == e2)
  //             break;
  //
  //           if (i1 == e1)
  //           {
  //             new_in_edges.push_back(*i2);
  //             ++i2;
  //           }
  //           else if (i2 == e2)
  //           {
  //             new_in_edges.push_back(*i1);
  //             ++i1;
  //           }
  //           else if ( (*i1)->source < (*i2)->source )
  //           {
  //             new_in_edges.push_back(*i1);
  //             ++i1;
  //           }
  //           else if ( (*i1)->source > (*i2)->source )
  //           {
  //             new_in_edges.push_back(*i2);
  //             ++i2;
  //           }
  //           else
  //           {
  //             if ( (*i1)->weight > (*i2)->weight )
  //               new_in_edges.push_back(*i1);
  //             else
  //               new_in_edges.push_back(*i2);
  //
  //             ++i1;
  //             ++i2;
  //           }
  //         }
  //
  //         in_edges[cycle_repr[i]].swap(new_in_edges);
  //         new_in_edges.clear();
  //       }
  //
  //       in_edges[new_repr].swap(in_edges[cycle_repr.back()]);
  //       edge_weight_change[new_repr] = weight_t(0);
  //     }
  //   }
  //
  //   std::vector<edge_t*> F_roots;
  //   for (edge_t* e : F)
  //   {
  //     if (e->parent == nullptr)
  //       F_roots.push_back(e);
  //   }
  //
  //   for (vertex_idx_t v : final_roots)
  //   {
  //     if (lambda[v] != nullptr)
  //       remove_from_f(lambda[v], F_roots);
  //   }
  //
  //   while (!F_roots.empty())
  //   {
  //     edge_t* e = F_roots.back();
  //     F_roots.pop_back();
  //
  //     if (e->removed)
  //       continue;
  //
  //     heads[offset + e->target] = e->source;
  //     remove_from_f(lambda[e->target], F_roots);
  //   }
  //
  //   return;
  // }
};

} // namespace impl

template <class M,
          typename vertex_idx_t,
          typename weight_t>
void arborescence(const M& adj_matrix,
                  std::vector<vertex_idx_t>& heads,
                  size_t offset)
{
  typedef impl::Arborescence<M, vertex_idx_t, weight_t> Tree;
  //Tree::fill_heads_with_max(adj_matrix, len, heads.begin);
  assert(adj_matrix.rows() == adj_matrix.cols());
  size_t len = adj_matrix.rows();
  // std::cerr << "arborescence heads=" << heads << ", offset=" << offset << ", len=" << len << std::endl;

  const typename std::vector<vertex_idx_t>::const_iterator begin = heads.cbegin() + offset;
  const typename std::vector<vertex_idx_t>::const_iterator end = begin + len;

  size_t n_roots = Tree::count_roots(begin, end);

  if (n_roots == 0)
    Tree::choose_root(adj_matrix, heads, offset);

  if (n_roots > 1)
    Tree::choose_one_root(adj_matrix, heads, offset);

  n_roots = Tree::count_roots(begin, end);

  if (n_roots != 1)
    throw std::runtime_error("choose_one_root should have selected one root.");
    // there is a bug in choose_one_root

  // now n_roots == 1
  // if (Tree::is_connected(begin, end))
  //   // TODO detect and clear cycles
  //   return;

  Tree::make_connected(adj_matrix, heads, offset);

  n_roots = Tree::count_roots(begin, end);

  if (n_roots == 1)
    if (Tree::is_connected(begin, end))
      return;

  // we still have problems. Retrying with full tree reconstruction.
  std::fill(heads.begin() + offset, heads.begin() + offset + len, 0);

  Tree::arborescence_impl(adj_matrix, heads, offset);

  // final check for the number of roots
  n_roots = Tree::count_roots(begin, end);

  if (n_roots != 1)
    throw std::runtime_error("choose_one_root should have selected one root.");

}

template <typename vertex_idx_t,
          typename weight_t>
void arborescence(std::vector<uint32_t>& heads, size_t len)
{
  // typedef impl::ArborescenceHeads<vertex_idx_t, weight_t> Tree;
  // std::cerr << "arborescence len=" << len << ", heads=" << heads << std::endl;

  // hyp: there can be several root (head == 0), then select the first one and point the others to it
  auto h = std::numeric_limits<vertex_idx_t>::max();
  for (size_t i = 1; i < len; i++)
  {
    if (heads[i] == 0)
    {
      h = i;
      // std::cerr << "arborescence setting head to " << h << std::endl;
      break;
    }
  }
  // if there is no root, select the first node
  if (h == std::numeric_limits<vertex_idx_t>::max())
  {
    h = 1;
    heads[h] = 0;
    // std::cerr << "arborescence no head. selecting first node" << h << "->" << heads[h] << std::endl;
  }
  for (size_t i = h+1; i < len; i++)
  {
    if (heads[i] == 0)
    {
      heads[i] = h;
      // std::cerr << "arborescence duplicate head on "<<i<<". pointing it to the first one: " << i << "->" << heads[i] << std::endl;
    }
    else if (heads[i] >= len)
    {
      // std::cerr << "arborescence head on " << i << " (" << heads[i] << ") is out of sentence of size " << len
      //           << ". pointing it to the first one: " << i << "->" << h << std::endl;
      heads[i] = h;
    }
  }
  // std::cerr << "AFTER scence len=" << len << ", heads=" << heads << std::endl;
}

} // namespace deeplima

#endif
