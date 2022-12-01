#pragma once
#include "parlay/utilities.h"


#include <climits>
#include <cstring>
#include <iostream>
using namespace std;
using namespace parlay;
constexpr int BLOCK_SIZE = 1024;

#define ELONG

#ifdef NLONG
using NodeId = uint64_t;
#else
using NodeId = uint32_t;
#endif

#ifdef ELONG
using EdgeId = uint64_t;
#else
using EdgeId = uint32_t;
#endif

constexpr NodeId UINT_N_MAX = numeric_limits<NodeId>::max();
constexpr EdgeId UINT_E_MAX = numeric_limits<EdgeId>::max();

inline size_t num_blocks(size_t n, size_t block_size) {
  if (n == 0)
    return 0;
  else
    return (1 + ((n)-1) / (block_size));
}

template <typename ET>
inline bool atomic_compare_and_swap(ET *a, ET oldval, ET newval) {
  static_assert(sizeof(ET) <= 8, "Bad CAS length");
  if (sizeof(ET) == 1) {
    uint8_t r_oval, r_nval;
    std::memcpy(&r_oval, &oldval, sizeof(ET));
    std::memcpy(&r_nval, &newval, sizeof(ET));
    return __sync_bool_compare_and_swap(reinterpret_cast<uint8_t *>(a), r_oval,
                                        r_nval);
  } else if (sizeof(ET) == 4) {
    uint32_t r_oval, r_nval;
    std::memcpy(&r_oval, &oldval, sizeof(ET));
    std::memcpy(&r_nval, &newval, sizeof(ET));
    return __sync_bool_compare_and_swap(reinterpret_cast<uint32_t *>(a), r_oval,
                                        r_nval);
  } else {  // if (sizeof(ET) == 8) {
    uint64_t r_oval, r_nval;
    std::memcpy(&r_oval, &oldval, sizeof(ET));
    std::memcpy(&r_nval, &newval, sizeof(ET));
    return __sync_bool_compare_and_swap(reinterpret_cast<uint64_t *>(a), r_oval,
                                        r_nval);
  }
}

template <class ET>
inline bool compare_and_swap(ET *a, ET oldval, ET newval) {
  return (*a) == oldval && atomic_compare_and_swap(a, oldval, newval);
}

template <typename E, typename EV>
inline E fetch_and_add(E *a, EV b) {
  volatile E newV, oldV;
  do {
    oldV = *a;
    newV = oldV + b;
  } while (!atomic_compare_and_swap(a, oldV, newV));
  return oldV;
}

template <typename E, typename EV>
inline void write_add(E *a, EV b) {
  // volatile E newV, oldV;
  E newV, oldV;
  do {
    oldV = *a;
    newV = oldV + b;
  } while (!atomic_compare_and_swap(a, oldV, newV));
}

template <typename ET, typename F>
inline bool write_min(ET *a, ET b, F less) {
  ET c;
  bool r = 0;
  do c = *a;
  while (less(b, c) && !(r = atomic_compare_and_swap(a, c, b)));
  return r;
}

template <typename ET, typename F>
inline bool write_max(ET *a, ET b, F less) {
  ET c;
  bool r = 0;
  do c = *a;
  while (less(c, b) && !(r = atomic_compare_and_swap(a, c, b)));
  return r;
}

template <typename ET>
inline bool CAS(ET *a, ET oldval, ET newval) {
  return *a == oldval && atomic_compare_and_swap(a, oldval, newval);
}

template <class ET>
inline ET _hash(ET a) {
  if (sizeof(ET) == 4) {
    return hash32(a);
  } else if (sizeof(ET) == 8) {
    return hash64(a);
  } else {
    std::cout << "hash bad length: " << sizeof(ET) << std::endl;
    abort();
  }
}

template <class ET>
inline ET _hash_2(ET a) {
  if (sizeof(ET) == 4) {
    return hash32_2(a);
  } else if (sizeof(ET) == 8) {
    return hash64_2(a);
  } else {
    std::cout << "hash bad length: " << sizeof(ET) << std::endl;
    abort();
  }
}
