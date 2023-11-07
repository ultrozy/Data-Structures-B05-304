#ifndef BINARY_SEARCH_TREE_H_
#define BINARY_SEARCH_TREE_H_

#include <cstddef>

template <class T>
struct NodeBST {
  NodeBST<T>* parent;
  NodeBST<T>* left;
  NodeBST<T>* right;
  T value;
};

template <class T>
class BinarySearchTree {
 private:
  NodeBST<T>* root_ = nullptr;

  T* HelperInOrderTraversal(NodeBST<T>* current, T* ptr) {
    if (current) {
      ptr = HelperInOrderTraversal(current->left, ptr);
      *(ptr++) = current->value;
      ptr = HelperInOrderTraversal(current->right, ptr);
    }
    return ptr;
  }
  NodeBST<T>* BuildBST(const T* begin, const T* end) {
    if (begin == end) {
      return nullptr;
    }
    auto mid = begin + ((end - begin) / 2);
    auto pnode = new NodeBST<T>{nullptr, BuildBST(begin, mid),
                                BuildBST(mid + 1, end), *mid};
    if (pnode->left) {
      pnode->left->parent = pnode;
    }
    if (pnode->right) {
      pnode->right->parent = pnode;
    }
    return pnode;
  }
  NodeBST<T>* Predecessor(NodeBST<T>* pnode) {
    if (pnode->left) {
      pnode = pnode->left;
      while (pnode->right) {
        pnode = pnode->right;
      }
      return pnode;
    }
    while (pnode->parent && pnode->parent->left == pnode) {
      pnode = pnode->parent;
    }
    return pnode->parent;
  }
  NodeBST<T>* Successor(NodeBST<T>* pnode) {
    if (pnode->right) {
      pnode = pnode->right;
      while (pnode->left) {
        pnode = pnode->left;
      }
      return pnode;
    }
    while (pnode->parent && pnode->parent->right == pnode) {
      pnode = pnode->parent;
    }
    return pnode->parent;
  }
  void RemoveNode(NodeBST<T>* pnode) {
    // Leaf
    if (!pnode->left && !pnode->right) {
      if (pnode->parent) {
        if (pnode->parent->left == pnode) {
          pnode->parent->left = nullptr;
        } else {
          pnode->parent->right = nullptr;
        }
      }
      delete pnode;
      return;
    }
    // Not Leaf
    if (pnode->left) {
      // Left non-null
      auto p_prev = Predecessor(pnode);
      pnode->value = p_prev->value;
      RemoveNode(p_prev);
    } else {
      // Right non-null
      auto p_next = Successor(pnode);
      pnode->value = p_next->value;
      RemoveNode(p_next);
    }
  }
  void HelperClear(NodeBST<T>* pnode) {
    if (pnode) {
      HelperClear(pnode->left);
      HelperClear(pnode->right);
      delete pnode;
    }
  }

 public:
  BinarySearchTree(const T* sorted_array, size_t size) {
    root_ = BuildBST(sorted_array, sorted_array + size);
  }
  void InsertElement(const T& value) {
    if (!root_) {
      root_ = new NodeBST<T>{nullptr, nullptr, nullptr, value};
      return;
    }
    auto p_curr = root_;
    NodeBST<T>* p_prev;
    while (p_curr) {
      p_prev = p_curr;
      p_curr = (p_curr->value < value ? p_curr->right : p_curr->left);
    }
    if (p_prev->value < value) {
      p_prev->right = new NodeBST<T>{p_prev, nullptr, nullptr, value};
    } else {
      p_prev->left = new NodeBST<T>{p_prev, nullptr, nullptr, value};
    }
  }
  void EraseElement(const T& value) {
    auto p_curr = root_;
    while (p_curr) {
      if (p_curr->value == value) {
        break;
      }
      p_curr = (p_curr->value < value ? p_curr->right : p_curr->left);
    }
    // No element
    if (!p_curr) {
      return;
    }
    RemoveNode(p_curr);
  }
  void InorderTraversal(T* sequence) {
    HelperInOrderTraversal(root_, sequence);
  }
  void Clear() { HelperClear(root_); }
};

#endif