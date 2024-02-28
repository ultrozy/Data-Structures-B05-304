#include "universal_hash.h"

template <class T>
UniversalHash UniversalHash::GenerateHash(T& rng) {
  return {mult_random(rng), add_random(rng)};
}

UniversalHash::UniversalHash(ValueType a, ValueType b) : multiplier{a}, addendum{b} {
}

UniversalHash::ValueType UniversalHash::operator()(UniversalHash::ValueType x) {
  return ((multiplier * x) % kPrime + addendum) % kPrime;
}