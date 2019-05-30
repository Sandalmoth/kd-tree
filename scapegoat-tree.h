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
  static constexpr const double balancing_limit = 0.66;
  static constexpr const double log_balancing_limit = log(1.0/0.66);


  size_t node_size(Node* n) {
    if (n == nullptr) {
      return 0;
    } else {
      return node_size(n->less) + node_size(n->more) + 1;
    }
  }


  Node* flatten(Node* sub_root) {
    if (sub_root == nullptr)
      return nullptr;

    //   std::cout << *sub_root << std::endl;

    Node* ll_less = flatten(sub_root->less);
    Node* ll_more = flatten(sub_root->more);
    sub_root->more = ll_more;

    if (ll_more != nullptr)
      ll_more->less = sub_root;

    if (ll_less == nullptr)
      return sub_root;

    Node* ll_less_end = ll_less;
    while (ll_less_end->more != nullptr)
      ll_less_end = ll_less_end->more;

    sub_root->less = ll_less_end;
    ll_less_end = sub_root;

    return ll_less;
  }

  // Node* flatten(Node* sub_root) {
  //   std::cout << *sub_root << std::endl;
  //   Node* l1 = (sub_root->less) ? flatten(sub_root->less) : nullptr;
  //   Node* l2 = (sub_root->more) ? flatten(sub_root->more) : nullptr;
  //   Node* l3 = sub_root;
  //   l3->more = l2;
  //   if (!l1) return l3;
  //   Node* last = l1;
  //   while (last->more) last = last->more;
  //   last->more = l3;
  //   return l1;
  // }

//   List* flattenToLL(Node* root) {
//     List *list1 = (root->left) ? flattenToLL(root->left) : NULL;
//     List *list2 = (root->right) ? flattenToLL(root->right) : NULL;
//     List *list3 = newNode(root->key);
//     // The "middle" list3 cannot be NULL; append list2 to list3
//     list3->next = list2; // If list2 is NULL, it's OK
//     if (!list1) return list3; // Nothing to prepend
//     List *last = list1;
//     while (last->next) last=last->next; // Go to the end of list1
//     last->next = list3; // Append list3+list2 to the end of list1
//     return list1;
// }

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


  void _insert(Node*& n, T value) {
    Node* insertion_node = n;
    std::stack<Node *> parents;
    int depth = 0;

    // traverse tree to find insertion point
    while (insertion_node != nullptr) {
      parents.push(insertion_node);
      ++depth;
      if (value < insertion_node->value) {
        insertion_node = insertion_node->less;
      } else {
        insertion_node = insertion_node->more;
      }
    }

    // insert node
    insertion_node = parents.top();
    parents.pop();
    Node* new_node = new Node;
    new_node->value = value;
    // TODO restructure to prevent this extra if
    if (value < insertion_node->value) {
      insertion_node->less = new_node;
    } else {
      insertion_node->more = new_node;
    }

    // did we unbalance the tree?
    if (depth > ceil(log(size())/log_balancing_limit)) {
      std::cout << "Rebalancing " << std::endl;

      // check our nodes to find the first scapegoat
      Node* scapegoat = nullptr;
      while (!parents.empty()) {
        scapegoat = parents.top();
        parents.pop();

        int size_less = node_size(scapegoat->less);
        int size_more = node_size(scapegoat->more);
        if (size_less > balancing_limit*(size_less + size_more + 1) ||
            size_more > balancing_limit*(size_less + size_more + 1)) {
          break;
        }
      }
      // identify the pointer in the tree that points to the scapegoat
      Node** scapegoat_pointer;
      if (parents.empty()) {
        scapegoat_pointer = &n;
      } else {
        Node* scapegoat_parent = parents.top();
        if (scapegoat == scapegoat_parent->less) {
          scapegoat_pointer = &(scapegoat_parent->less);
        } else {
          scapegoat_pointer = &(scapegoat_parent->less);
        }
      }

      // rebuild scapegoat subtree
      int scapegoat_size = node_size(scapegoat);
      std::cout << "flattening and rebuilding" << std::endl;
      std::cout << *scapegoat << std::endl;
      Node* flat = flatten(scapegoat);
      std::cout << "is flat?" << std::endl;
      Node* flat_start = flat;
      std::cout << flat_start->value << "  ";
      while (flat_start->more != nullptr) {
        flat_start = flat_start->more;
        std::cout << flat_start->value << "  ";
      } std::cout << std::endl;
      // (*scapegoat_pointer) = build(flatten(scapegoat), scapegoat_size);
      (*scapegoat_pointer) = build(flat, scapegoat_size);
    }

    // if (n == nullptr) {
    //   n = new Node;
    //   n->value = value;

    //   // value inserted, do we need to rebalance the tree?
    //   // consider storing size in tree object to avoid excessive tree traversal
    //   if (depth > ceil(log(size())/log_balancing_limit)) {
    //     std::cout << "Rebalancing " << std::endl;

    //     Node* scapegoat = n;

    //     // // first, find a scapegoat
    //     // Node* this_node = n;
    //     // int this_size = 1;
    //     // int parent_size = 0;
    //     // while (!tmp.empty()) {
    //     //   Node* parent = tmp.top();
    //     //   tmp.pop();
    //     //   int sibling_size = node_size((parent->less == this_node) ?
    //     //                                parent->more : parent->less);
    //     //   parent_size = this_size + sibling_size + 1;
    //     //   if (this_size > floor(balancing_limit*parent_size) ||
    //     //       sibling_size > floor(balancing_limit*parent_size)) {
    //     //     std::cout << "found scapegoat " << parent->value << " in " << (*this) << std::endl;

    //     //     // rebuild by flattening tree below and recursively rebuilding

    //     //     break;
    //     //   }
    //     // }
    //   }

    //   return;
    // }

    // tmp.push(n);

    // if (value < n->value) {
    //   _insert(n->less, value, depth + 1);
    // }
    // else {
    //   _insert(n->more, value, depth + 1);
    // }
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
      while (tmp_node->more != nullptr &&
             new_node->value > tmp_node->more->value) {
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
