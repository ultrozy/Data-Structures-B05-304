#ifndef BINARY_SEARCH_TREE_H_
#define BINARY_SEARCH_TREE_H_

#include <algorithm>
#include <cstddef>

// #define NODE_HAS_PARENT

template <class T>
struct NodeAVL {
#ifdef NODE_HAS_PARENT
  NodeAVL<T>* parent;
#endif
  NodeAVL<T>* left;
  NodeAVL<T>* right;
  size_t height;
  T value;
};

template <class T>
class AVLtree {
 private:
  NodeAVL<T>* p_root_ = nullptr;

 public:
  AVLtree() : p_root_(nullptr) {}
  AVLtree(const T* sorted_array, size_t size)
      : p_root_{BuildAVL(sorted_array, sorted_array + size)} {}
  AVLtree(const std::initializer_list<T>& list)
      : p_root_{BuildAVL(list.begin(), list.end())} {}
  ~AVLtree() { HelperClear(p_root_); }

  void Insert(const T& value) { p_root_ = HelperInsert(p_root_, value); }
  void Delete(const T& value) { p_root_ = HelperDelete(p_root_, value); }
  T* InorderTraversal(T* sequence) const {
    return HelperInOrderTraversal(p_root_, sequence);
  }
  void Clear() { HelperClear(p_root_); }

 private:
  static NodeAVL<T>* HelperInsert(NodeAVL<T>* p_node, const T& value) {
    // Break recursion
    if (!p_node) {
      return new NodeAVL<T>{nullptr, nullptr, 1, value};
    }

    // Assume there are unique values in tree
    if (p_node->value == value) {
      return p_node;
    }

    // Recursive call
    if (p_node->value < value) {
      p_node->right = HelperInsert(p_node->right, value);
    } else {
      p_node->left = HelperInsert(p_node->left, value);
    }
    return LocalBalance(p_node);
  }

  /*
  Отказавшись от поля parent в NodeAVL операция Delete становится чуть сложнее.
  Весь путь вершин нужно хранить либо в искусственном стеке, либо в стеке
  функций (рекурсивных) Для простоты реализации будем писать рекурсивные
  функции.

  Идея примерно та же, что в BinarySearchTree:
    1) ищем NodeAVL (назовём его target) с удаляемым значением
    2) если это лист, удаляем target, балансируем от target до root_, выходим
    3) иначе ищем Predecessor/Successor (назовём его offspring)
    4) копируем значение от offspring в target, удаляем offspring
    5) балансируем от offspring до root_

  Здесь, как и в BST, может возникнуть проблема, что offspring - не лист.
  Например допустим, что offspring - Successor у target.
  Тогда offspring не имеет левого поддерева, но правое может быть непустым.
  Но, благодаря свойствам AVL дерева, оказывается, что правое поддерево
    либо пусто, либо содержит только одну вершину.

  Учитывая всё это, можно сократить количество вспомогательных функций до трёх:
    1) HelperDelete - ищет NodeAVL с удаляемым значением. Балансирует от target
  до root_. 2) RemoveOnLeft - ищет Predecessor у target. Балансирует от
  offspring до target. 2) RemoveOnRight - ищет Successor у target. Балансирует
  от offspring до target.
  */
  static NodeAVL<T>* HelperDelete(NodeAVL<T>* p_node, const T& value) {
    // Break recursion
    if (!p_node) {
      return nullptr;
    }

    // Found target. Remove target or its descendant
    if (p_node->value == value) {
      if (!p_node->left && !p_node->right) {
        delete p_node;
        return nullptr;
      }
      if (p_node->left) {
        p_node->left = RemoveOnLeft(p_node->left, p_node->value);
      } else {
        p_node->right = RemoveOnRight(p_node->right, p_node->value);
      }
      return LocalBalance(p_node);
    }

    // Recursion. Still searching for target
    if (p_node->value < value) {
      p_node->right = HelperDelete(p_node->right, value);
    } else {
      p_node->left = HelperDelete(p_node->left, value);
    }
    return LocalBalance(p_node);
  }
  static NodeAVL<T>* RemoveOnLeft(NodeAVL<T>* p_node, T& new_value) {
    if (p_node->right) {
      p_node->right = RemoveOnLeft(p_node->right, new_value);
      return LocalBalance(p_node);
    }
    new_value = p_node->value;
    auto replacement = p_node->left;
    delete p_node;
    return replacement;
  }
  static NodeAVL<T>* RemoveOnRight(NodeAVL<T>* p_node, T& new_value) {
    if (p_node->left) {
      p_node->left = RemoveOnRight(p_node->left, new_value);
      return LocalBalance(p_node);
    }
    new_value = p_node->value;
    auto replacement = p_node->right;
    delete p_node;
    return replacement;
  }

  static T* HelperInOrderTraversal(NodeAVL<T>* current, T* ptr) {
    if (current) {
      ptr = HelperInOrderTraversal(current->left, ptr);
      *(ptr++) = current->value;
      ptr = HelperInOrderTraversal(current->right, ptr);
    }
    return ptr;
  }
  static NodeAVL<T>* BuildAVL(const T* begin, const T* end) {
    if (begin == end) {
      return nullptr;
    }
    auto mid = begin + ((end - begin) / 2);
    auto p_node =
        new NodeAVL<T>{BuildAVL(begin, mid), BuildAVL(mid + 1, end), 1, *mid};
    if (p_node->left) {
      p_node->height = 1 + p_node->left->height;
    }
    return p_node;
  }
  static void HelperClear(NodeAVL<T>* p_node) {
    if (p_node) {
      HelperClear(p_node->left);
      HelperClear(p_node->right);
      delete p_node;
    }
  }

  static size_t Height(NodeAVL<T>* p_node) {
    return p_node ? p_node->height : 0;
  }
  static void SetHeight(NodeAVL<T>* p_node) {
    p_node->height = 1 + std::max(Height(p_node->left), Height(p_node->right));
  }
  static int BalanceFactor(NodeAVL<T>* p_node) {
    return Height(p_node->right) - Height(p_node->left);
  }
  static NodeAVL<T>* LocalBalance(NodeAVL<T>* p_node) {
    if (BalanceFactor(p_node) == 2) {
      if (BalanceFactor(p_node->right) == -1) {
        return RotateBigLeft(p_node);
      }
      return RotateLeft(p_node);
    }
    if (BalanceFactor(p_node) == -2) {
      if (BalanceFactor(p_node->left) == 1) {
        return RotateBigRight(p_node);
      }
      return RotateRight(p_node);
    }
    return p_node;
  }
  static NodeAVL<T>* RotateLeft(NodeAVL<T>* p_node) {
    auto p_child = p_node->right;
    p_node->right = p_child->left;
    p_child->left = p_node;
    SetHeight(p_node);
    SetHeight(p_child);
    return p_child;
  }
  static NodeAVL<T>* RotateRight(NodeAVL<T>* p_node) {
    auto p_child = p_node->left;
    p_node->left = p_child->right;
    p_child->right = p_node;
    SetHeight(p_node);
    SetHeight(p_child);
    return p_child;
  }
  static NodeAVL<T>* RotateBigLeft(NodeAVL<T>* p_node) {
    auto p_child = p_node->right;
    auto p_grandson = p_child->left;
    p_node->right = p_grandson->left;
    p_child->left = p_grandson->right;
    p_grandson->left = p_node;
    p_grandson->right = p_child;
    SetHeight(p_node);
    SetHeight(p_child);
    SetHeight(p_grandson);
    return p_grandson;
  }
  static NodeAVL<T>* RotateBigRight(NodeAVL<T>* p_node) {
    auto p_child = p_node->left;
    auto p_grandson = p_child->right;
    p_node->left = p_grandson->right;
    p_child->right = p_grandson->left;
    p_grandson->right = p_node;
    p_grandson->left = p_child;
    SetHeight(p_node);
    SetHeight(p_child);
    SetHeight(p_grandson);
    return p_grandson;
  }
};

#endif