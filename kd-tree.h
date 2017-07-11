#ifndef __KD_TREE_H__
#define __KD_TREE_H__


#include <algorithm>
#include <array>
#include <functional>
#include <iostream>
#include <vector>


template <typename T, size_t N, typename E>
class KDTree {
public:
  typedef std::array<T, N> position_type;
  typedef E value_type;
  typedef std::pair<position_type, value_type> element_type;
  typedef std::pair<position_type, typename std::vector<element_type>::iterator> element_ptr_type;


private:
  struct Node;

  typename std::vector<Node>::iterator build(std::vector<element_ptr_type> elem, size_t depth=0) {
    size_t axis = depth % N;
    ++depth;
    std::sort(elem.begin(), elem.end()
              , [&](const element_ptr_type &a, const element_ptr_type &b) {
                return a.first[axis] < b.first[axis];
              });
    auto median = elem.begin() + (elem.end() - elem.begin()) / 2;
    std::vector<element_ptr_type> lesser {elem.begin(), median};
    std::vector<element_ptr_type> greater {median + 1, elem.end()};
    std::cout << lesser.size() << ' ' << greater.size() << std::endl;
    nodes.push_back(Node());
    auto node = nodes.end();
    --node;
    node->e = median->second;
    if (lesser.size() > 0) {
      node->nless = build(lesser, depth);
      node->nless->nparent = node;
      node->noless = false;
    } else {
      node->nless = nodes.end();
      node->noless = true;
    }
    if (greater.size() > 0) {
      node->nmore = build(greater, depth);
      node->nmore->nparent = node;
      node->nomore = false;
    } else {
      node->nmore = nodes.end();
      node->nomore = true;
    }
    node->leaf = node->noless && node->nomore;
    return node;
  }

  double squared_distance(position_type a, position_type b) {
    double dsq = 0.0;
    for (size_t i = 0; i < N; ++i) {
      double diff = a[i] - b[i];
      dsq += diff*diff;
    }
    return dsq;
  }

  // Recursive printing of tree. Cool eh?
  friend std::ostream& operator<<(std::ostream &out, const Node &node) {
    if (!node.noless)
      out << '(' << *(node.nless) << ") ";
    for (auto &x: node.e->first) {
      out << x << ' ';
    }
    out << '\b';
    if (!node.nomore)
      out << " (" << *(node.nmore) << ')';
    return out;
  }

public:
  KDTree() { }

  template <typename Titer>
  KDTree(Titer first, Titer last) {
    elements = {first, last};
    nodes.reserve(elements.size());
    for (auto x: elements) {
      for (auto y: x.first) std::cout << y << ' ';
      std::cout << x.second << std::endl;
    }
    std::vector<element_ptr_type> eptrs;
    for (auto it = elements.begin(); it < elements.end(); ++it) {
      eptrs.push_back(make_pair(it->first, it));
    }
    root = build(eptrs);
    root->root = true;
    std::cout << elements.size() << std::endl;
    std::cout << nodes.size() << std::endl;
  }

  value_type &nearest_neighbour(position_type pos) {
    // first, traverse to a leaf
    size_t depth = 0;
    typename std::vector<Node>::iterator current_node = root;
    typename std::vector<element_type>::iterator best;
    while (true) {
      size_t dim = depth % N;
      if (current_node->leaf) {
        best = current_node->e;
        break;
      }
      if (pos[dim] < current_node->e->first[dim]) {
        if (current_node->noless) {
          current_node = current_node->nmore;
        } else {
          current_node = current_node->nless;
        }
      } else {
        if (current_node->nomore) {
          current_node = current_node->nless;
        } else {
          current_node = current_node->nmore;
        }
      }
      ++depth;
    }
    // now traverse upwards to make sure there is no better point
    double dsq = squared_distance(best->first, pos);
    std::cout << dsq << std::endl;
    return best->second;
  }

  size_t size() const {
    std::cout << elements.size() << ' ' << nodes.size() << std::endl;
    return elements.size();
  }

  friend std::ostream& operator<<(std::ostream& out, const KDTree &kdt) {
    out << *(kdt.root);
    return out;
  }

private:
  std::vector<element_type> elements;
  std::vector<Node> nodes;
  typename std::vector<Node>::iterator root;

  struct Node {
    bool root = false;
    bool leaf;
    bool noless;
    bool nomore;
    typename decltype(elements)::iterator e;
    typename decltype(nodes)::iterator nless;
    typename decltype(nodes)::iterator nmore;
    typename decltype(nodes)::iterator nparent;
  };

};


#endif
