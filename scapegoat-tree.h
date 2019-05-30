#ifndef __SCAPEGOAT_TREE_H__
#define __SCAPEGOAT_TREE_H__


#include <cmath>
#include <cstddef>
#include <iostream>
#include <stack>


template <typename T>
class ScapegoatTree {
public:
private:
  struct Node {
    Node* less = nullptr;
    Node* more = nullptr;
    T value;
  };


  Node* root = nullptr;
  size_t max_node_count = 0;
  std::stack<Node *> tmp;
  static constexpr const double balancing_limit = 0.66;
  static constexpr const double log_balancing_limit = log(1.0/0.66);


  size_t node_size(Node* n) {
    if (n == nullptr) {
      return 0;
    } else {
      return node_size(n->less) + node_size(n->more) + 1;
    }
  }


  Node* build(Node* first, size_t n) {
    // Takes a linked list made from nodes, and it's length and
    // builds a balanced binary tree

    if (n == 0) {
      return nullptr;
    }

    int median = (n - 1)/2;

    if (median <= 0) {
      // median 0 means we have only one or two items.
      // In that case the we have already have a balanced tree
      first->less = nullptr;
      if (first->more != nullptr) {
        first->more->less = nullptr;
        first->more->less = nullptr;
      }
      return first;
    }

    // find median element of list
    Node* sub_root = first;
    for (int i = 0; i < median; ++i) {
      sub_root = sub_root->more;
    }
    sub_root->less->more = nullptr;
    sub_root->less = build(first, median);
    sub_root->more->less = nullptr;
    sub_root->more = build(sub_root->more, n - median - 1);

    return sub_root;
  }


  void _insert(Node* n, T value, int depth = 0) {
    if (n == nullptr) {
      n = new Node;
      n->value = value;

      // value inserted, do we need to rebalance the tree?
      // consider storing size in tree object to avoid excessive tree traversal
      std::cout << depth << " : " << ceil(log(size())/log_balancing_limit) << std::endl;
      if (depth > ceil(log(size())/log_balancing_limit)) {
        std::cout << "Created unbalance, rebalancing " << (*this) << std::endl;

        // first, find a scapegoat
        Node* this_node = n;
        int this_size = 1;
        int parent_size = 0;
        while (!tmp.empty()) {
          Node* parent = tmp.top();
          tmp.pop();
          int sibling_size = node_size((parent->less == this_node) ?
                                       parent->more : parent->less);
          parent_size = this_size + sibling_size + 1;
          if (this_size > floor(balancing_limit*parent_size) ||
              sibling_size > floor(balancing_limit*parent_size)) {
            std::cout << "found scapegoat " << parent->value << " in " << (*this) << std::endl;

            // rebuild by flattening tree below and recursively rebuilding

            break;
          }
        }
      }

      return;
    }

    tmp.push(n);

    if (value < n->value) {
      _insert(n->less, value, depth + 1);
    }
    else {
      _insert(n->more, value, depth + 1);
    }
  }


  friend std::ostream& operator<<(std::ostream &out, const Node &n) {
    if (n.less != nullptr) {
      out << '(' << *(n.less) << ") ";
    }
    out << n.value;
    if (n.more != nullptr) {
      out << " (" << *(n.more) << ')';
    }
    return out;
  }


public:
  void insert(T value) {
    _insert(root, value);
  }

  void erase(T value) {
  }


  ScapegoatTree() {}


  template <typename Iter>
  ScapegoatTree(Iter first, Iter last) {
    if (first == last) return;

    // First, create a sorted "linked list" of nodes
    size_t n = 1;
    Node* first_node = new Node;
    first_node->value = (*first);
    ++first;
    while (first != last) {
      Node* new_node = new Node;
      new_node->value = (*first);
      ++n;
      ++first;

      // does it go first in the list?
      // special case since we need to update first_node in that case
      if (new_node->value <= first_node->value) {
        first_node->less = new_node;
        new_node->more = first_node;
        first_node = new_node;
        continue;
      }

      // find position and insert
      Node *tmp_node = first_node;
      // note reliance on short-circuit to prevent dereferencing nullptr
      while (tmp_node->more != nullptr && new_node->value > tmp_node->more->value) {
        tmp_node = tmp_node->more;
      }

      new_node->less = tmp_node;
      new_node->more = tmp_node->more;
      if (tmp_node->more != nullptr) {
        tmp_node->more->less = new_node;
      }
      tmp_node->more = new_node;

    }

    root = build(first_node, n);
  }


  size_t size() {
    return node_size(root);
  }


  friend std::ostream& operator<<(std::ostream& out, const ScapegoatTree &st) {
    if (st.root != nullptr) {
      out << *(st.root);
    }
    return out;
  }

};


#endif
