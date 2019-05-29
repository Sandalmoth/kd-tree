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


  void insert(Node*& n, T value, int depth = 0) {
    if (n == nullptr) {
      n = new Node;
      n->value = value;

      // value inserted, do we need to rebalance the tree?
      // consider storing size in tree object to avoid excessive tree traversal
      std::cout << depth << " : " << ceil(log(size())/log_balancing_limit) << std::endl;
      if (depth > ceil(log(size())/log_balancing_limit)) {
        std::cout << "Created unbalance, rebalancing " << (*this) << std::endl;
        // find first unbalanced parent
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
      insert(n->less, value, depth + 1);
    }
    else {
      insert(n->more, value, depth + 1);
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
  void push(T value) {
    insert(root, value);
  }


  ScapegoatTree() {}


  template <typename Iter>
  ScapegoatTree(Iter first, Iter last) {
    // Replace this awful method with proper construction
    // Something like sort and recursively insert median(s)
    while (first != last) {
      std::cout << "inserting" << *first << std::endl;
      insert(root, *first);
      ++first;
    }
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
