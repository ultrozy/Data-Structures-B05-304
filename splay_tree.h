#ifndef SPLAY_TREE_H_
#define SPLAY_TREE_H_

#include <cstddef>
#include <utility>

template <class T>
struct NodeSplay {
  NodeSplay<T>* parent;
  NodeSplay<T>* left;
  NodeSplay<T>* right;
  T value;
};

template <class T>
class SplayTree {
 private:
  NodeSplay<T>* p_root_ = nullptr;

 public:
  SplayTree() : p_root_(nullptr) {}
  SplayTree(const T* sorted_array, size_t size)
      : p_root_{BuildSplay(sorted_array, sorted_array + size)} {}
  SplayTree(const std::initializer_list<T>& list)
      : p_root_{BuildSplay(list.begin(), list.end())} {}
  ~SplayTree() { Clear(); }

  void Insert(const T& value) {
    if (!p_root_) {
      p_root_ = new NodeSplay<T>{nullptr, nullptr, nullptr, value};
      return;
    }
    auto p_closest = FindClosest(value);
    p_root_ = Splay(p_closest);

    // Unique elements in tree
    if (p_closest->value == value) {
      return;
    }

    // Determine subtrees
    auto p_left = p_closest->left;
    auto p_right = p_closest->right;
    if (p_closest->value < value) {
      p_left = p_closest;
      p_left->right = nullptr;
    } else {
      p_right = p_closest;
      p_right->left = nullptr;
    }

    // New root
    p_root_ = new NodeSplay<T>{nullptr, p_left, p_right, value};
    if (p_left) {
      p_left->parent = p_root_;
    }
    if (p_right) {
      p_right->parent = p_root_;
    }
  }
  void Delete(const T& value) {
    // No such element
    if (!p_root_) {
      return;
    }
    p_root_ = Splay(FindClosest(value));
    if (p_root_->value != value) {
      return;
    }

    // If one of root subtrees is empty
    if (!p_root_->left) {
      delete std::exchange(p_root_, p_root_->right);
      if (p_root_) {
        p_root_->parent = nullptr;
      }
      return;
    }
    if (!p_root_->right) {
      delete std::exchange(p_root_, p_root_->left);
      p_root_->parent = nullptr;
      return;
    }

    // If both root subtrees are not empty
    auto p_new_root = p_root_->left;
    while (p_new_root->right) {
      p_new_root = p_new_root->right;
    }
    p_root_->left->parent = nullptr;
    Splay(p_new_root);
    p_new_root->right = p_root_->right;
    p_new_root->right->parent = p_new_root;
    delete std::exchange(p_root_, p_new_root);
  }
  void Clear() {
    auto p_current = p_root_;
    while (p_current) {
      if (p_current->left) {
        p_current = std::exchange(p_current->left, nullptr);
      } else if (p_current->right) {
        p_current = std::exchange(p_current->right, nullptr);
      } else {
        delete std::exchange(p_current, p_current->parent);
      }
    }
  }

 private:
  static NodeSplay<T>* Splay(NodeSplay<T>* p_node) {
    while (p_node->parent) {
      auto p_parent = p_node->parent;
      auto p_grand_parent = p_parent->parent;

      // Zig
      if (!p_grand_parent) {
        RotateUp(p_node);
        return p_node;
      }

      if ((p_grand_parent->left == p_parent && p_parent->left == p_node) ||
          (p_grand_parent->right == p_parent && p_parent->right == p_node)) {
        // Zig-Zig
        RotateUp(p_parent);
        RotateUp(p_node);
      } else {
        // Zig-Zag
        RotateUp(p_node);
        RotateUp(p_node);
      }
    }
    return p_node;
  }
  NodeSplay<T>* FindClosest(const T& value) {
    NodeSplay<T>* p_prev = nullptr;
    auto p_current = p_root_;
    while (p_current) {
      if (p_current->value == value) {
        return p_current;
      }
      p_prev = p_current;
      p_current = p_current->value > value ? p_current->left : p_current->right;
    }
    return p_prev;
  }
  static NodeSplay<T>* BuildSplay(const T* begin, const T* end) {
    if (begin == end) {
      return nullptr;
    }
    auto mid = begin + ((end - begin) / 2);
    auto p_node = new NodeSplay<T>{nullptr, BuildSplay(begin, mid),
                                   BuildSplay(mid + 1, end), *mid};
    if (p_node->left) {
      p_node->left->parent = p_node;
    }
    if (p_node->right) {
      p_node->right->parent = p_node;
    }
    return p_node;
  }
  static void RotateUp(NodeSplay<T>* p_node) {
    auto p_parent = p_node->parent;

    // Change pointers of children
    if (p_parent->left == p_node) {
      p_parent->left = p_node->right;
      p_node->right = p_parent;
      if (p_parent->left) {
        p_parent->left->parent = p_parent;
      }
    } else {
      p_parent->right = p_node->left;
      p_node->left = p_parent;
      if (p_parent->right) {
        p_parent->right->parent = p_parent;
      }
    }

    // Change parents of rotating nodes
    p_node->parent = p_parent->parent;
    p_parent->parent = p_node;

    // Change pointers of grand_parent
    if (p_node->parent) {
      if (p_node->parent->left == p_parent) {
        p_node->parent->left = p_node;
      } else {
        p_node->parent->right = p_node;
      }
    }
  }
};

#endif