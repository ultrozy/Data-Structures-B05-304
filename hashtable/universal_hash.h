#ifndef UNIVERSAL_HASH_H_
#define UNIVERSAL_HASH_H_

#include <random>

class UniversalHash {
 private:
  using ValueType = size_t;
  static constexpr inline ValueType kPrime = 4'294'967'231;
  ValueType multiplier;
  ValueType addendum;

 public:
  static constexpr inline ValueType kMaxValue = kPrime - 1;
  static inline std::uniform_int_distribution<size_t> mult_random{1, kMaxValue};
  static inline std::uniform_int_distribution<size_t> add_random{0, kMaxValue};
  template <class T>
  static UniversalHash GenerateHash(T&);

  explicit UniversalHash(ValueType = 1, ValueType = 0);
  ValueType operator()(ValueType);
};

#endif