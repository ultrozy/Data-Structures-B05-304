#include <iostream>
#include <utility>

template <class T>
void Print(T* array, size_t size) {
  for (size_t i = 0; i < size; ++i) {
    std::cout << array[i] << ' ';
  }
  std::cout << '\n';
}

template <class Container>
void Print(const Container& cont) {
  for (const auto& x : cont) {
    std::cout << x << ' ';
  }
  std::cout << '\n';
}

template <class T>
std::pair<T*, size_t> LongestCommonSubsequence(const T* first_array, size_t first_size, const T* second_array,
                                               size_t second_size) {
  // Dynamic Programming
  auto lsp_size = new size_t*[first_size + 1];
  for (size_t i = 0; i <= first_size; ++i) {
    lsp_size[i] = new size_t[second_size + 1];
    lsp_size[i][0] = 0;
  }
  for (size_t j = 0; j <= second_size; ++j) {
    lsp_size[0][j] = 0;
  }
  for (size_t i = 1; i <= first_size; ++i) {
    for (size_t j = 1; j <= second_size; ++j) {
      if (first_array[i - 1] == second_array[j - 1]) {
        lsp_size[i][j] = lsp_size[i - 1][j - 1] + 1;
      } else {
        lsp_size[i][j] = std::max(lsp_size[i][j - 1], lsp_size[i - 1][j]);
      }
    }
  }

  // Answer Recovery
  auto answer = new T[lsp_size[first_size][second_size]];
  auto i = first_size;
  auto j = second_size;
  auto k = lsp_size[first_size][second_size];

  while (k) {
    if (first_array[i - 1] == second_array[j - 1]) {
      answer[--k] = first_array[i - 1];
      --i;
      --j;
    } else if (lsp_size[i][j] == lsp_size[i - 1][j]) {
      --i;
    } else {
      --j;
    }
  }
  size_t answer_size = lsp_size[first_size][second_size];

  for (size_t i = 0; i <= first_size; ++i) {
    delete[] lsp_size[i];
  }
  delete[] lsp_size;
  return {answer, answer_size};
}

template <class T>
std::pair<T*, size_t> LongestIncreasingSubsequence(const T* array, size_t size) {
  if (size == 0) {
    return {nullptr, 0};
  }
  T m_inf = *array;
  T inf = *array;
  for (size_t i = 1; i < size; ++i) {
    m_inf = std::min(m_inf, array[i]);
    inf = std::max(inf, array[i]);
  }
  m_inf -= 1000;
  inf += 1000;

  auto last_elem = new T[size + 1]{m_inf};
  for (size_t i = 1; i <= size; ++i) {
    last_elem[i] = inf;
  }
  auto index_curr = new size_t[size + 1];
  auto index_prev = new size_t[size + 1];

  // Dynamic Programming
  for (size_t i = 0; i < size; ++i) {
    size_t left = 1;
    size_t right = i + 1;

    // Строго   возр. -> lower bound (last_elem[mid] >= array[i])
    // Нестрого возр. -> upper bound (last_elem[mid] >  array[i])
    while (left < right) {
      size_t mid = (left + right) >> 1;
      if (last_elem[mid] >= array[i]) {
        right = mid;
      } else {
        left = mid + 1;
      }
    }

    if (last_elem[left] > array[i]) {
      last_elem[left] = array[i];
      index_curr[left] = i;
      index_prev[i] = index_curr[left - 1];
    }
  }

  // Answer Recovery
  size_t left = 1;
  size_t right = size + 1;
  while (left < right) {
    size_t mid = (left + right) >> 1;
    if (last_elem[mid] >= inf) {
      right = mid;
    } else {
      left = mid + 1;
    }
  }

  auto lis = new T[left - 1];
  auto curr = index_curr[left - 1];
  lis[left - 2] = array[curr];
  for (size_t i = left - 2; i; --i) {
    curr = index_prev[curr];
    lis[i - 1] = array[curr];
  }

  delete[] index_curr;
  delete[] index_prev;
  delete[] last_elem;
  return {lis, left - 1};
}