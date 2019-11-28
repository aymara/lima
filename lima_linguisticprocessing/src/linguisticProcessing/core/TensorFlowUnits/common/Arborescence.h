#include <vector>
#include <set>
#include <map>
#include <list>
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

  static void fill_heads_with_max(const std::function <float(size_t, size_t)>& adj_matrix,
                                  size_t len,
                                  std::vector<size_t>& heads)
  {
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

  static size_t count_roots(const std::vector<size_t>& heads, size_t len)
  {
    size_t c = 0;
    for (size_t i = 1; i < len; i++)
    {
      if (heads[i] == 0)
        c++;
    }
    return c;
  }

  static bool is_connected(const std::vector<size_t>& heads, size_t len)
  {
    std::vector< std::vector<size_t> > head2child;
    head2child.resize(len);
    for (size_t i = 1; i < len; i++)
    {
      head2child[heads[i]].push_back(i);
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

  static void find_loops(const std::vector<size_t>& heads,
                         std::vector< std::vector<size_t> >& loops,
                         const std::vector<size_t>& connected,
                         size_t len)
  {
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
        j = heads[j];
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

  static void make_connected(const std::function <float(size_t, size_t)>& adj_matrix,
                             size_t len,
                             std::vector<size_t>& heads)
  {
    std::vector< std::vector<size_t> > head2child;
    head2child.resize(len);
    for (size_t i = 1; i < len; i++)
    {
      head2child[heads[i]].push_back(i);
    }

    std::vector<size_t> connected(len, 0);
    find_disconnected_groups(head2child, len, connected);

    while (std::find(std::next(connected.begin()), connected.end(), 0) != connected.end())
    {
      std::vector< std::vector<size_t> > loops;
      find_loops(heads, loops, connected, len);

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
            if (j == heads[from])
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
        throw;

      heads[best_new_arc.first] = best_new_arc.second;

      head2child.clear();
      head2child.resize(len);
      for (size_t i = 1; i < len; i++)
      {
        head2child[heads[i]].push_back(i);
      }

      std::fill(connected.begin(), connected.end(), 0);
      find_disconnected_groups(head2child, len, connected);
    }
  }

  static void choose_one_root(const std::function <float(size_t, size_t)>& adj_matrix,
                              size_t len,
                              std::vector<size_t>& heads)
  {
    std::vector<std::pair<size_t, float>> roots;
    roots.reserve(16);
    for (size_t i = 1; i < len; i++)
    {
      if (heads[i] == 0)
      {
        roots.push_back(std::make_pair(i, adj_matrix(i, 0)));
      }
    }

    if (roots.size() == 0)
      throw; // This function is intended to choose from a several (> 0) of roots

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
        heads[r.first] = best_root;
    }
  }

  static void choose_root(const std::function <float(size_t, size_t)>& adj_matrix,
                            size_t len,
                            std::vector<size_t>& heads)
  {
    if (len < 3)
    {
      heads[1] = 0;
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

    heads[best_root] = 0;
  }

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
  impl::Arborescence<vertex_idx_t, weight_t>::fill_heads_with_max(adj_matrix, len, heads);

  size_t n_roots = impl::Arborescence<vertex_idx_t, weight_t>::count_roots(heads, len);

  if (n_roots == 0)
    impl::Arborescence<vertex_idx_t, weight_t>::choose_root(adj_matrix, len, heads);

  if (n_roots > 1)
    impl::Arborescence<vertex_idx_t, weight_t>::choose_one_root(adj_matrix, len, heads);

  n_roots = impl::Arborescence<vertex_idx_t, weight_t>::count_roots(heads, len);

  if (n_roots != 1)
    throw; // there is a bug in choose_one_root

  // now n_roots == 1
  if (impl::Arborescence<vertex_idx_t, weight_t>::is_connected(heads, len))
   return;

  impl::Arborescence<vertex_idx_t, weight_t>::make_connected(adj_matrix, len, heads);

  n_roots = impl::Arborescence<vertex_idx_t, weight_t>::count_roots(heads, len);

  if (n_roots == 1)
    if (impl::Arborescence<vertex_idx_t, weight_t>::is_connected(heads, len))
      return;

  // we still have problems. Retrying with full tree reconstruction.
  std::fill(heads.begin(), heads.end(), 0);

  impl::Arborescence<vertex_idx_t, weight_t>::arborescence_impl(adj_matrix, len, heads);
}

#endif
