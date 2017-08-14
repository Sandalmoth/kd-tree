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


// Balancing limits sets a cap on how unbalance the tree can become during insertion/deletion
// lower -> faster indel, slower lookup
// higher -> faster lookup, slower indel
#ifndef __BALANCING_LIMIT__
#define __BALANCING_LIMIT__ 0.66
#endif


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
      return node_size(n->less) + node_size(n->more) + 1;
  }


  // Get a list of all nodes below src (including src)
  void _get_subtree(std::vector<Node *> &v, Node *src) {
    if (src == nullptr)
      return;
    v.push_back(src);
    _get_subtree(v, src->less);
    _get_subtree(v, src->more);
  }
  std::vector<Node *> get_subtree(Node *src) {
    std::vector<Node *> v;
    if (src == nullptr)
      return v;
    v.push_back(src);
    _get_subtree(v, src->less);
    _get_subtree(v, src->more);
    return v;
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
    if (np.size() == 1) {
      // it's a leaf, so make sure it doesn't point anywhere
      np.front()->less = nullptr;
      np.front()->more = nullptr;
      return np.front();
    }

    size_t k = depth % N;
    // TODO implement better median finding algorithm?
    std::sort(np.begin(), np.end()
              , [&](Node *a, Node *b) {
                return a->p[k] < b->p[k];
              });
    auto median = np.begin() + np.size()/2;
    // NOTE as we use strictly less ordering for traversing the tree,
    // if we have duplicates, we have to pick the first occurence.
    // Because x >= x but not x < x, thus equal nodes will correctly be in the more subtree.
    // So, step back so long as there are equal nodes
    // Also, note that the order of the and statement plays a role
    // as (*(median - 1)) is invalid if median = np.begin()
    // but lazy evaluation prevents that.
    while (median != np.begin() && (*median)->p[k] == (*(median - 1))->p[k]) {
      --median;
    }
    // We can end up with the first element, so cover that edge case
    if (median == np.begin())
      (*median)->less = nullptr;
    else
      (*median)->less = build(std::vector<Node *>{np.begin(), median}, depth + 1);
    (*median)->more = build(std::vector<Node *>{median + 1, np.end()}, depth + 1);
    return *median;
  }


  std::pair<bool, size_t> _insert_node(Node *&src, Node *n, size_t depth = 0) {
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
      return std::make_pair(depth > log(nodes.size()) / log(1.0/balancing_limit), 2 + node_size(other));
    } else {
      // Recursively insert node in a subtree
      auto rebuild_info = _insert_node(next, n, depth+1);
      // Do we need to rebuild?
      if (rebuild_info.first) {
        // Is this the scapegoat node?
        size_t this_size = rebuild_info.second + 1 + node_size(other);
        if (rebuild_info.second > balancing_limit * this_size) {
          std::cout << "Scapegoat found" << std::endl;
          // Child of this node is the scapegoat, rebuild;
          auto subtree = get_subtree(next);
          next = build(subtree, depth);
          // We're done, no need to keep signalling rebuilding.
          return std::make_pair(false, 0);
        } else {
          // keep moving up the tree looking for scapegoats
          std::cout << "No scapegoat" << std::endl;
          rebuild_info.second = this_size;
          return rebuild_info;
        }
      } else {
        // we don't care about the subtree size if there is no need to rebuild
        return std::make_pair(false, 0);
      }
    }
  }
  void insert_node(Node *n) {
    auto rebuild_info = _insert_node(root, n, 0);
    if (rebuild_info.first) {
      std::cout << "rebuilding whole tree: " << root << std::endl;
      auto tree = get_subtree(root);
      root = build(tree, 0);
    }
  }


  Node *_erase_node(Node *&src, Node *&parent, value_type p, size_t depth=0) {
    std::cout << "dpth " << depth << ' ' << *src << std::endl;

    if (src == nullptr)
      return nullptr;

    size_t k = depth % N;

    if (src->p == p) {
      std::cout << "found " << *src << std::endl;
      if (src->less == nullptr && src->more == nullptr) {
        std::cout << "leaf" << std::endl;
        auto tmp = src;
        if (parent->less == src)
          parent->less = nullptr;
        else
          parent->more = nullptr;
        return tmp;
      } else {
        Node *&sub = (src->more == nullptr) ? src->less : src->more;
        // std::cout << *sub << std::endl;
        // find node with minimum p[k] in subtree;
        // This function is less space efficient than a tree-traversing method. Maybe replace
        auto subtree = get_subtree(sub);
        Node *min = nullptr;
        if (src->more == nullptr) {
          min = *std::max_element(subtree.begin(), subtree.end()
                                        , [&](auto a, auto b) {
                                          return a->p[k] < b->p[k];
                                        });
        } else {
          min = *std::min_element(subtree.begin(), subtree.end()
                                        , [&](auto a, auto b) {
                                          return a->p[k] < b->p[k];
                                        });
        }
        // std::cout << "  min  " << *min << std::endl;
        // replace with min, and then recursively remove min from subtree
        std::cout << "recursively erasing " << min << '\t' << *min << std::endl;
        _erase_node(sub, src, min->p, depth + 1);
        auto erased = src;
        src = min;
        min->less = erased->less;
        min->more = erased->more;
        erased->less = nullptr;
        erased->more = nullptr;
        return erased;
      }
    }

    // Oneliners ftw?
    Node *&next = (src->less != nullptr && src->more != nullptr) ?
      (p[k] < src->p[k]) ?src->less : src->more :
      (src->less == nullptr) ? src->more : src->less;
    return _erase_node(next, src, p, depth + 1);
  }
  Node * erase_node(value_type p) {
    if (nodes.size() == 1 && root->p == p) {
      auto tmp = root;
      root = nullptr;
      return tmp;
    }
    Node *erased = _erase_node(root, root, p); // root as parent here will never matter, as single node case is covered already
      // Has the tree become too unbalanced? Do we need to rebuild it all?
    size_t current_size = node_size(root);
    std::cout << "SIZES: " << current_size << ' ' << max_node_count << std::endl;
    if (current_size < balancing_limit * static_cast<double>(max_node_count)) {
      std::cout << "rebuilding after deletion" << std::endl;
      std::vector<Node *> np;
      for (auto &n: nodes) {
        if (&n == erased) continue;
        np.push_back(&n);
      }
      root = build(std::move(np));
      max_node_count = current_size;
    }
    return erased;
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


  double kdist2(value_type a, value_type b, size_t k) {
    double d = a[k] - b[k];
    return d * d;
  }


  std::pair<Node *, double> _nearest_node(Node *src, value_type p, size_t depth=0) {
    if (src == nullptr) return std::make_pair(nullptr, 1e100);

    size_t k = depth % N;

    double d = dist2(src->p, p);
    double dk = kdist2(src->p, p, k);
    std::cout << *src << ' ' << d << std::endl;

    Node *next = (src->p[k] < p[k]) ? src->less : src->more;
    Node *other = (src->p[k] < p[k]) ? src->more : src->less;

    if (next == nullptr && other == nullptr) {
      return std::make_pair(src, d);
    } else {
      auto tmp = next;
      next = other;
      other = tmp;
    }

    auto nn = _nearest_node(next, p, depth + 1);

    if (nn.second > d || nn.first == nullptr) {
      nn.first = src;
      nn.second = d;
    }

    if (nn.second > dk) {
      std::cout << "exploring other tree from " << *src << " as " << dk << " < " << nn.second << std::endl;
      std::cout << other << ' ' << next << std::endl;
      auto nn2 = _nearest_node(other, p, depth + 1);
      if (nn2.second < nn.second && nn2.first != nullptr) {
        nn = nn2;
      }
    }

    return nn;

  }
  Node *nearest_node(value_type p) {
    if (nodes.size() == 1)
      return root;

    return _nearest_node(root, p).first;
  }


  void _nodes_within_distance(std::vector<Node *> &nwd, Node *src, value_type p, double d, size_t depth=0) {
    // Does this node exist?
    if (src == nullptr) return;

    // Is this node within distance?
    if (dist2(p, src->p) < d) {
      nwd.push_back(src);
    }

    size_t k = depth % N;

    // Do we need to explore both subtrees?
    if (kdist2(p, src->p, k) < d) {
      std::cout << "exploring both" << std::endl;
      _nodes_within_distance(nwd, src->less, p, d, depth+1);
      _nodes_within_distance(nwd, src->more, p, d, depth+1);
      return;
    } else {
      // explore the relevant subtree
      _nodes_within_distance(nwd, (p[k] < src->p[k]) ? src->less : src->more, p, d, depth+1);
      return;
    }
  }
  std::vector<Node *> nodes_within_distance(value_type p, double d) {
    std::vector<Node *> nwd;
    _nodes_within_distance(nwd, root, p, d*d);
    return nwd;
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
    // Include a compile time check that this container contains objects that can construct value_type?
    // Although, i guess it wont compile anyway if it doesn't?

    for (auto it = first; it != last; ++it) {
      nodes.emplace_back(Node{*it, nullptr, nullptr});
    }

    std::vector<Node *> np;
    np.reserve(nodes.size());
    for (auto &n: nodes) np.push_back(&n);
    root = build(std::move(np));
    max_node_count = nodes.size();
  }


  size_t size() {
    return node_size(root);
  }


  void insert(value_type p) {
    nodes.emplace_back(Node{p, nullptr, nullptr});
    if (root == nullptr) {
      root = &nodes.back();
    } else {
      insert_node(&nodes.back());
    }
    ++max_node_count;
  }


  void erase(value_type p) {
    auto erased = erase_node(p);
    auto it = std::find_if(nodes.begin(), nodes.end()
                           , [&] (auto &a) {
                             return &a == erased;
                           });
    std::cout << erased << ' ' << &(*it) << std::endl;
    nodes.erase(it);
  }


  value_type nearest(value_type p) {
    return nearest_node(p)->p;
  }


  std::vector<value_type> within_distance(value_type p, double d) {
    auto ndw = nodes_within_distance(p, d);
    std::vector<value_type> v;
    for (auto n: ndw) {
      v.push_back(n->p);
    }
    return v;
  }


  friend std::ostream& operator<<(std::ostream& out, const KDTree &kdt) {
    out << *(kdt.root);
    return out;
  }


#ifdef __ENABLE_KDTREE_DEBUG__ // Debug functions

  value_type print_distances(value_type p) {
    std::vector< std::pair<value_type, double> > v;
    for (auto &n: nodes) {
      std::cout << dist(n.p, p) << '\t' << n << std::endl;
      v.push_back(make_pair(n.p, dist2(n.p, p)));
    }
    return std::min_element(v.begin(), v.end()
                            , [](auto a, auto b){
                              return a.second < b.second;
                            })->first;
  }

#endif


private:
  Node *root = nullptr;
  size_t max_depth = 0;
  size_t max_node_count = 0;
  std::list<Node> nodes;

  // Balancing limits sets a cap on how unbalance the tree can become during insertion/deletion
  // lower -> faster indel, slower lookup
  // higher -> faster lookup, slower indel
  double balancing_limit = __BALANCING_LIMIT__;
};


#endif
