#ifndef __KD_TREE_H__
#define __KD_TREE_H__


#include <algorithm>
#include <array>
#include <functional>
#include <iostream>
#include <list>
#include <vector>
#include <cmath>
#include <tuple>


// k-d tree with scapegoat rebalancing
// typename T needs to have operator<


template <typename T, size_t N>
class KDTree {
public:
  typedef std::array<T, N> value_type;


private:
  struct Node {
    value_type p;
    Node *less = nullptr;
    Node *more = nullptr;
  };

  size_t node_size(Node *n) {
    if (n == nullptr)
      return 0;
    else
      return node_size(n->less) + node_size(n->less) + 1;
  }

  std::vector<Node *> get_subtree(Node *src) {
    
  }

  // TODO memory usage of this function is a little bad, with the node *list copying
  // still, pointers are fairly small, so unless the tree is massive, this solution should be fine.
  Node *build(std::vector<Node *> np, size_t depth = 0) {
    // no more nodes base case
    if (np.size() == 0)
      return nullptr;

    if (depth > max_depth)
      max_depth = depth;

    // trivial base case, median in vector of size 1 can be only one element
    if (np.size() == 1)
      return np.front();

    size_t k = depth % N;
    // TODO implement better median finding algorithm?
    std::sort(np.begin(), np.end()
              , [&](Node *a, Node *b) {
                return a->p[k] < b->p[k];
              });
    auto median = np.begin() + np.size()/2;
    (*median)->less = build(std::vector<Node *>{np.begin(), median}, depth + 1);
    (*median)->more = build(std::vector<Node *>{median + 1, np.end()}, depth + 1);
    return *median;
  }

  std::pair<bool, size_t> insert_node(Node *&src, Node *n, size_t depth = 0) {
    if (src == nullptr) {
      src = n;
      return std::make_pair(false, 0);
    }

    size_t k = depth % N;
    Node *&next = (n->p[k] < src->p[k]) ? src->less : src->more;
    Node *&other = (n->p[k] < src->p[k]) ? src->more : src->less;
    if (next == nullptr) {
      // We hit a leaf, insert node.
      if (depth > max_depth)
        max_depth = depth;
      next = n;
      // first argument: did we unbalance the tree beyond the limit?
      // second argument: we know that this node is one node, and that we just added one node.
      // So we need to find the size of the other child subtree
      return std::make_pair(depth > log(nodes.size()) / log(balancing_limit), 2 + node_size(other));
    } else {
      // Recursively insert node in a subtree
      auto rebuild_info = insert_node(next, n, depth+1);
      // Do we need to rebuild?
      if (rebuild_info->first) {
        // Is this the scapegoat node?
        size_t this_size = rebuild_info.second + 1 + node_size(other);
        if (rebuild_info.second > balancing_limit * this_size) {
          // Child of this node is the scapegoat, rebuild;
        } else {
          // keep moving up the tree looking for scapegoats
          rebuild_info.second = this_size;
          return rebuild_info;
        }
      } else {
        // we don't care about the subtree size if there is no need to rebuild
        return std::make_pair(false, 0);
      }
    }
  }

  double dist(value_type a, value_type b) {
    double dsq = 0.0;
    for (size_t i = 0; i < N; ++i) {
      double diff = a[i] - b[i];
      dsq += diff*diff;
    }
    return sqrt(dsq);
  }

  double dist2(value_type a, value_type b) {
    double dsq = 0.0;
    for (size_t i = 0; i < N; ++i) {
      double diff = a[i] - b[i];
      dsq += diff*diff;
    }
    return dsq;
  }

  friend std::ostream& operator<<(std::ostream &out, const Node &n) {
    if (n.less != nullptr)
      out << '(' << *(n.less) << ") ";
    for (auto &x: n.p) {
      out << x << ' ';
    }
    out << '\b';
    if (n.more != nullptr)
      out << " (" << *(n.more) << ')';
    return out;
  }


public:
  KDTree() { }

  template <typename Titer>
  KDTree(Titer first, Titer last) {
    // Include a compile time check that this container contains value_type objects
    // Although, i guess it wont compile anyway if it doesn't?
    for (auto it = first; it != last; ++it) {
      nodes.emplace_back(Node{*it, nullptr, nullptr});
    }

    std::vector<Node *> np;
    np.reserve(nodes.size());
    for (auto &n: nodes) np.push_back(&n);
    root = build(std::move(np));
  }

  void insert(value_type p) {
    nodes.emplace_back(Node{p, nullptr, nullptr});
    if (root == nullptr) {
      root = &nodes.back();
    } else {
      insert_node(root, &nodes.back());
    }
  }

  friend std::ostream& operator<<(std::ostream& out, const KDTree &kdt) {
    out << *(kdt.root);
    return out;
  }


private:
  Node *root = nullptr;
  size_t max_depth = 0;
  std::list<Node> nodes;

  // Balancing limits sets a cap on how unbalance the tree can become during insertion/deletion
  // lower -> faster indel, slower lookup
  // higher -> faster lookup, slower indel
  double balancing_limit = 0.66;
};


#endif
