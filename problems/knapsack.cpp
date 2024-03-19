#include <cstddef>
#include <cstdint>

using WeightType = uint32_t;
using CostType = int32_t;
using QuantityType = uint32_t;

QuantityType* UnboundedKnapsack(size_t num, WeightType capacity, CostType* costs, WeightType* weights) {
  auto amounts = new CostType[capacity + 1]{};
  auto element = new size_t[capacity + 1];
  const size_t inf = -1;
  for (WeightType i = 0; i <= capacity; ++i) {
    element[i] = inf;
  }

  for (WeightType curr_cap = 1; curr_cap <= capacity; ++curr_cap) {
    for (size_t i = 0; i < num; ++i) {
      if (weights[i] > curr_cap) {
        continue;
      }
      if (amounts[curr_cap - weights[i]] + costs[i] > amounts[curr_cap]) {
        amounts[curr_cap] = amounts[curr_cap - weights[i]] + costs[i];
        element[curr_cap] = i;
      }
    }
  }

  auto answer = new QuantityType[num]{};
  while (element[capacity] != inf) {
    ++answer[element[capacity]];
    capacity -= weights[element[capacity]];
  }

  delete[] amounts;
  delete[] element;
  return answer;
}

QuantityType* Knapsack01(size_t num, WeightType capacity, CostType* costs, WeightType* weights) {
  auto amounts = new CostType*[num + 1];
  for (size_t i = 0; i <= num; ++i) {
    amounts[i] = new CostType[capacity + 1]{};
  }

  for (size_t i = 0; i < num; ++i) {
    for (WeightType curr_cap = 1; curr_cap <= capacity; ++curr_cap) {
      amounts[i + 1][curr_cap] = amounts[i][curr_cap];
      if (weights[i] > curr_cap) {
        continue;
      }
      if (amounts[i + 1][curr_cap] < costs[i] + amounts[i][curr_cap - weights[i]]) {
        amounts[i + 1][curr_cap] = costs[i] + amounts[i][curr_cap - weights[i]];
      }
    }
  }

  auto answer = new QuantityType[num]{};
  for (size_t i = num; i > 0; --i) {
    if (amounts[i][capacity] != amounts[i - 1][capacity]) {
      capacity -= weights[i - 1];
      answer[i - 1] = 1;
    }
  }

  for (size_t i = 0; i <= num; ++i) {
    delete[] amounts[i];
  }
  delete[] amounts;
  return answer;
}