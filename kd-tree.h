#ifndef __KD_TREE_H__
#define __KD_TREE_H__


#include <algorithm>
#include <array>
#include <functional>
#include <iostream>
#include <list>
#include <vector>
#include <cmath>




template <typename T, size_t N>
class KDTree {
public:
  typedef std::array<T, N> value_type;


private:
  struct Node {
    value_type p;
    Node *less, *more;
  };

  template <typename Titer>
  Titer median_dim_k(Titer first, Titer last, size_t k) {
    // bad algorithm probably
    // but implementing quickselect is annoying
    std::vector<std::pair<T, Titer>> v;
    for (auto it = first; it != last; ++it) {
      v.push_back(std::make_pair(it->p[k], it));
    }
    std::sort(v.begin(), v.end()
              , [](auto a, auto b) {
                return a.first < b.first;
              });
    return v[v.size()/2].second;
  }

  template <typename Titer>
  Node *build(Titer first, Titer last, size_t depth) {

    if (first == last)
      return nullptr;
    // possibly include second base case for first++ == last

    size_t k = depth % N;
    auto median = median_dim_k(first, last, k);
    Node *n = &(*median);
    n->less = 
    return n;
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
    for (auto it = first; it != last; ++it) {
      nodes.emplace_back(Node{*it, nullptr, nullptr});
    }

    for (auto &n: nodes) {
      std::cout << n << std::endl;
    }

    root = build(nodes.begin(), nodes.end(), 0);
  }

  friend std::ostream& operator<<(std::ostream& out, const KDTree &kdt) {
    out << *(kdt.root);
    return out;
  }


private:
  Node *root;

  std::list<Node> nodes;
};


#endif
