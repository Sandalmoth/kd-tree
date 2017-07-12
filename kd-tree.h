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

  void visitreset() {
    for (auto &n: nodes) {
      n.visited = false;
    }
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
        current_node->visited = true;
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
    while (true) {
      std::cout << '\n' << *current_node << std::endl;
      // are we at a better node?
      double ndsq = squared_distance(current_node->e->first, pos);
      // std::cout << dsq << std::endl;
      if (ndsq < dsq) {
        best = current_node->e;
        dsq = ndsq;
      }
      // move up tree
      if (current_node->root)
        break;
      std::cout << *current_node << "  --  " << *(current_node->nparent) << std::endl;
      current_node = current_node->nparent;
      --depth;
      std::cout << *current_node << std::endl;
      size_t dim = depth % N;
      // could there be a better node in the other branch?
      double planediff = pos[dim] - current_node->e->first[dim];
      double planedist = planediff * planediff;
      std::cout << pos[dim] << ' ' << current_node->e->first[dim] << std::endl;
      std::cout << planedist << ' ' << dsq << std::endl;
      if (planedist < dsq) {
        // std::cout << planedist << std::endl;
        while (true) {
          std::cout << "we're goin' down!" << std::endl;
          if (current_node->leaf) {
            current_node->visited = true;
            std::cout << "at leaf" << std::endl;
            break;
          }
          if (pos[dim] < current_node->e->first[dim]) {
            if (current_node->noless) {
              if (current_node->nmore->visited) {
                break;
              }
              current_node = current_node->nmore;
            } else {
              if (current_node->nless->visited) {
                if (current_node->nomore) {
                  break;
                } else {
                  if (current_node->nmore->visited) {
                    break;
                  } else {
                    current_node = current_node->nmore;
                  }
                }
                if (current_node->nmore->visited) {
                  break;
                } else {
                  current_node = current_node->nmore;
                }
              }
              current_node = current_node->nless;
            }
          } else {
            if (current_node->nomore) {
              if (current_node->nless->visited) {
                break;
              }
              current_node = current_node->nless;
            } else {
              if (current_node->nmore->visited) {
                if (current_node->noless) {
                  break;
                } else {
                  if (current_node->nless->visited) {
                    break;
                  } else {
                    current_node = current_node->nless;
                  }
                }
              }
              current_node = current_node->nmore;
            }
          }
          ++depth;
        }
      } else {
        current_node->visited = true;
      }
    }
    std::cout << dsq << std::endl;
    visitreset();
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
    bool visited = false;
    typename decltype(elements)::iterator e;
    typename decltype(nodes)::iterator nless;
    typename decltype(nodes)::iterator nmore;
    typename decltype(nodes)::iterator nparent;
  };

};


#endif
