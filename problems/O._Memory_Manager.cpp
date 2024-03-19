#include <iostream>
#include <utility>

template <class T>
struct NodeSplay {
  NodeSplay<T>* parent;
  NodeSplay<T>* left;
  NodeSplay<T>* right;
  T value;
};

template <class T, class Compare>
class SplayTree {
 private:
  NodeSplay<T>* p_root_ = nullptr;
  NodeSplay<T>*& pool_;

 public:
  explicit SplayTree(NodeSplay<T>*& pool) : p_root_(nullptr), pool_(pool) {
  }

  void Insert(const T& value) {
    if (!p_root_) {
      p_root_ = new (pool_++) NodeSplay<T>{nullptr, nullptr, nullptr, value};
      return;
    }
    auto p_closest = FindClosest(value);
    p_root_ = Splay(p_closest);

    // Unique elements in tree
    if (Compare::Equal(p_closest->value, value)) {
      return;
    }

    // Determine subtrees
    auto p_left = p_closest->left;
    auto p_right = p_closest->right;
    if (Compare::Less(p_closest->value, value)) {
      p_left = p_closest;
      p_left->right = nullptr;
    } else {
      p_right = p_closest;
      p_right->left = nullptr;
    }

    // New root
    p_root_ = new (pool_++) NodeSplay<T>{nullptr, p_left, p_right, value};
    if (p_left) {
      p_left->parent = p_root_;
    }
    if (p_right) {
      p_right->parent = p_root_;
    }
  }
  T Delete(const T& value) {
    // No such element
    if (!p_root_) {
      return {};
    }
    p_root_ = Splay(FindClosest(value));
    if (!Compare::Equal(p_root_->value, value)) {
      return {};
    }

    // If one of root subtrees is empty
    if (!p_root_->left) {
      T deleted = p_root_->value;
      p_root_ = p_root_->right;
      if (p_root_) {
        p_root_->parent = nullptr;
      }
      return deleted;
    }
    if (!p_root_->right) {
      T deleted = p_root_->value;
      p_root_ = p_root_->left;
      p_root_->parent = nullptr;
      return deleted;
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
    T deleted = p_root_->value;
    p_root_ = p_new_root;
    return deleted;
  }
  bool IsEmpty() {
    return !p_root_;
  }
  const T& GetMax() {
    auto p_current = p_root_;
    while (p_current->right) {
      p_current = p_current->right;
    }
    p_root_ = Splay(p_current);
    return p_root_->value;
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
      if (Compare::Equal(p_current->value, value)) {
        return p_current;
      }
      p_prev = p_current;
      p_current = Compare::Less(value, p_current->value) ? p_current->left : p_current->right;
    }
    return p_prev;
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

struct MemoryUnit {
  size_t start = 0;
  size_t size = 0;
  size_t is_allocated = 0;
  MemoryUnit* prev = nullptr;
  MemoryUnit* next = nullptr;
};

struct CompareMemUnitPtr {
  static bool Less(MemoryUnit* left, MemoryUnit* right) {
    if (left->is_allocated) {
      return left->is_allocated < right->is_allocated;
    }
    if (left->size != right->size) {
      return left->size < right->size;
    }
    return left->start > right->start;
  }
  static bool Equal(MemoryUnit* left, MemoryUnit* right) {
    if (left->is_allocated) {
      return left->is_allocated == right->is_allocated;
    }
    return left->start == right->start && left->size == right->size;
  }
};

void Allocate(size_t req_index, size_t request, size_t& units_size, MemoryUnit* mem_units,
              SplayTree<MemoryUnit*, CompareMemUnitPtr>& free_mem,
              SplayTree<MemoryUnit*, CompareMemUnitPtr>& allocated) {
  if (free_mem.IsEmpty() || free_mem.GetMax()->size < request) {
    std::cout << "-1\n";
    return;
  }
  auto p_unit = free_mem.GetMax();
  free_mem.Delete(p_unit);
  std::cout << p_unit->start + 1 << std::endl;
  if (p_unit->size == request) {
    p_unit->is_allocated = req_index;
    allocated.Insert(p_unit);
  } else {
    units_size += 2;
    mem_units[units_size - 2] = {p_unit->start, static_cast<size_t>(request), req_index, p_unit->prev,
                                 mem_units + units_size - 1};
    mem_units[units_size - 1] = {p_unit->start + request, p_unit->size - request, 0, mem_units + units_size - 2,
                                 p_unit->next};
    if (p_unit->prev) {
      p_unit->prev->next = mem_units + units_size - 2;
    }
    if (p_unit->next) {
      p_unit->next->prev = mem_units + units_size - 1;
    }
    allocated.Insert(mem_units + units_size - 2);
    free_mem.Insert(mem_units + units_size - 1);
  }
}

void Deallocate(size_t request, size_t& units_size, MemoryUnit* mem_units,
                SplayTree<MemoryUnit*, CompareMemUnitPtr>& free_mem,
                SplayTree<MemoryUnit*, CompareMemUnitPtr>& allocated) {
  MemoryUnit target{};
  target.is_allocated = request;
  auto p_deleted = allocated.Delete(&target);
  if (!p_deleted) {
    return;
  }
  if ((!p_deleted->prev || p_deleted->prev->is_allocated) && (!p_deleted->next || p_deleted->next->is_allocated)) {
    p_deleted->is_allocated = 0;
    free_mem.Insert(p_deleted);
    return;
  }
  MemoryUnit* p_new = mem_units + (units_size++);
  *p_new = *p_deleted;
  p_new->is_allocated = 0;
  if (p_new->prev) {
    if (p_new->prev->is_allocated) {
      p_new->prev->next = p_new;
    } else {
      free_mem.Delete(p_new->prev);
      p_new->size += p_new->prev->size;
      p_new->start = p_new->prev->start;
      p_new->prev = p_new->prev->prev;
      if (p_new->prev) {
        p_new->prev->next = p_new;
      }
    }
  }
  if (p_new->next) {
    if (p_new->next->is_allocated) {
      p_new->next->prev = p_new;
    } else {
      free_mem.Delete(p_new->next);
      p_new->size += p_new->next->size;
      p_new->next = p_new->next->next;
      if (p_new->next) {
        p_new->next->prev = p_new;
      }
    }
  }
  free_mem.Insert(p_new);
}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  size_t ram = 0;
  size_t num_requests = 0;
  std::cin >> ram >> num_requests;

  auto mem_units = new MemoryUnit[(num_requests << 1) + 1]{};
  mem_units[0].size = ram;
  size_t units_size = 1;
  auto original_pool = new NodeSplay<MemoryUnit*>[(num_requests << 1) + 1];
  auto pool = original_pool;
  SplayTree<MemoryUnit*, CompareMemUnitPtr> free_mem(pool);
  SplayTree<MemoryUnit*, CompareMemUnitPtr> allocated(pool);
  free_mem.Insert(mem_units);

  for (size_t req_index = 1; req_index <= num_requests; ++req_index) {
    int request = 0;
    std::cin >> request;

    if (request > 0) {
      Allocate(req_index, request, units_size, mem_units, free_mem, allocated);
    } else {
      Deallocate(-request, units_size, mem_units, free_mem, allocated);
    }
  }

  delete[] mem_units;
  delete[] original_pool;
  return 0;
}