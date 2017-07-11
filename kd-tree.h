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
    nodes.push_back(Node());
    auto node = nodes.end();
    --node;
    node->e = median->second;
    if (lesser.size() > 0)
      node->nless = build(lesser, depth);
    else
      node->nless = nodes.end();
    if (greater.size() > 0)
      node->nmore = build(greater, depth);
    else
      node->nmore = nodes.end();
    if (node->nmore == nodes.end() and node->nless == nodes.end())
      node->leaf = true;
    else
      node->leaf = false;
    return node;
  }

public:
  KDTree() { }

  template <typename Titer>
  KDTree(Titer first, Titer last) {
    elements = {first, last};
    for (auto x: elements) {
      for (auto y: x.first) std::cout << y << ' ';
      std::cout << x.second << std::endl;
    }
    std::vector<element_ptr_type> eptrs;
    for (auto it = elements.begin(); it < elements.end(); ++it) {
      eptrs.push_back(make_pair(it->first, it));
    }
    root = build(eptrs);
    std::cout << elements.size() << std::endl;
    std::cout << nodes.size() << std::endl;
  }

  friend std::ostream& operator<<(std::ostream &out, const Node &node) {
    if (node.leaf)
      return out;
    out << '(' << *(node.nless) << ") ";
    for (auto &x: node.e->first) {
      out << x << ' ';
    }
    out << '(' << *(node.nmore) << ')';
    return out;
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
    bool leaf;
    typename decltype(elements)::iterator e;
    typename decltype(nodes)::iterator nless;
    typename decltype(nodes)::iterator nmore;
  };

};


#endif
