#pragma once
#include "parlay/parallel.h"
#include "parlay/primitives.h"
#include "parlay/sequence.h"
#include "parlay/utilities.h"

#include "get_time.hpp"
#include "utilities.h"
using namespace std;
using namespace parlay;

constexpr size_t MIN_BAG_SIZE = 1 << 10;
constexpr int MAX_PROBES = 100;
constexpr int EXP_SAMPLES = 64;

template <class ET>
class hashbag {
 protected:
  size_t n;
  double load_factor;
  int pointer;
  int num_hash_bag;
  size_t pool_size;
  ET empty;
  sequence<size_t> bag_size;
  sequence<uint32_t> counter;
  sequence<ET> pool;
  void set_values(ET u, int &local_pointer, size_t &len, size_t &idx,
                  size_t &rate) {
    local_pointer = pointer;
    // size of hash bag
    len = bag_size[local_pointer] - bag_size[local_pointer - 1];
    // insert into a random position between [2^i, 2^(i+1)]
    idx = (_hash(u) & (len - 1)) + bag_size[local_pointer - 1];
    // reciprocal sample rate
    rate = max(1.0, floor(len * load_factor / EXP_SAMPLES));
  }
  void clear() {
    size_t len = bag_size[pointer];
    parallel_for(
        0, len,
        [&](size_t i) {
          if (pool[i] != empty) {
            pool[i] = empty;
          }
        },
        BLOCK_SIZE);
    for (int i = 0; i <= pointer; i++) {
      counter[i] = 0;
    }
    pointer = 1;
  }

 public:
  // public:
  hashbag() = delete;
  hashbag(size_t _n, size_t _min_bag_size = MIN_BAG_SIZE,
          double _load_factor = 0.5, ET _empty = numeric_limits<ET>::max())
      : n(_n), load_factor(_load_factor), empty(_empty) {
    pointer = 1;
    size_t cur_size = _min_bag_size;
    pool_size = 0;
    bag_size = sequence<size_t>(1, 0);
    while (pool_size < n / load_factor) {
      bag_size.push_back(cur_size + bag_size.back());
      pool_size += cur_size;
      cur_size *= 2;
    }
    num_hash_bag = bag_size.size();
    counter = sequence<uint32_t>(num_hash_bag, 0);
    pool = sequence<ET>(pool_size, empty);
  }
  size_t get_bag_capacity() { return pool_size; }
  size_t insert(ET u) {
    int local_pointer;
    size_t len, idx, rate;
    set_values(u, local_pointer, len, idx, rate);
    while (idx % rate == 0) {
      bool succeed = false;
      uint32_t ret = counter[local_pointer];
      while (ret < EXP_SAMPLES && !succeed) {
        succeed = compare_and_swap(&counter[local_pointer], ret, ret + 1);
        ret = counter[local_pointer];
      }
      if (ret >= EXP_SAMPLES && local_pointer + 1 < num_hash_bag) {
        compare_and_swap(&pointer, local_pointer, local_pointer + 1);
      }
      if (local_pointer != pointer) {
        set_values(u, local_pointer, len, idx, rate);
      }
      if (succeed || local_pointer + 1 == num_hash_bag) {
        break;
      }
      // assert(local_pointer + 1 != num_hash_bag || ret < EXP_SAMPLES);
    }
    for (size_t i = 0;; i++) {
      if (compare_and_swap(&pool[idx], empty, u)) {
        break;
      }
      idx = (idx == bag_size[local_pointer] - 1 ? bag_size[local_pointer - 1]
                                                : idx + 1);
      if (i % MAX_PROBES == 0) {
        if (local_pointer != pointer) {
          set_values(u, local_pointer, len, idx, rate);
        }
      }
      if (i == len) {
        if (local_pointer == pointer) {
          compare_and_swap(&pointer, local_pointer, local_pointer + 1);
        }
        local_pointer = pointer;
        set_values(u, local_pointer, len, idx, rate);
      }
    }
    return idx;
  }

  template <typename Stamp_Seq>
  size_t insert(ET u, Stamp_Seq pool_stamp,
                // decltype(pool_stamp[0]) current_stamp) {
                uint32_t current_stamp) {
    // using stamp_type = decltype(pool_stamp[0]);
    int local_pointer;
    size_t len, idx, rate;
    set_values(u, local_pointer, len, idx, rate);
    while (idx % rate == 0) {
      bool succeed = false;
      uint32_t ret = counter[local_pointer];
      while (ret < EXP_SAMPLES && !succeed) {
        succeed = compare_and_swap(&counter[local_pointer], ret, ret + 1);
        ret = counter[local_pointer];
      }
      if (ret >= EXP_SAMPLES && local_pointer + 1 < num_hash_bag) {
        compare_and_swap(&pointer, local_pointer, local_pointer + 1);
      }
      if (local_pointer != pointer) {
        set_values(u, local_pointer, len, idx, rate);
      }
      if (succeed || local_pointer + 1 == num_hash_bag) {
        break;
      }
    }
    // stamp_type key_stamp = pool_stamp[idx];
    uint32_t key_stamp = pool_stamp[idx];
    for (size_t i = 0;; i++) {
      if (key_stamp != current_stamp &&
          compare_and_swap(&pool_stamp[idx], key_stamp, current_stamp)) {
        break;
      }
      idx = (idx == bag_size[local_pointer] - 1 ? bag_size[local_pointer - 1]
                                                : idx + 1);
      key_stamp = pool_stamp[idx];
      if (i % MAX_PROBES == 0) {
        if (local_pointer != pointer) {
          set_values(u, local_pointer, len, idx, rate);
        }
      }
      if (i == len) {
        if (local_pointer == pointer) {
          compare_and_swap(&pointer, local_pointer, local_pointer + 1);
        }
        local_pointer = pointer;
        set_values(u, local_pointer, len, idx, rate);
      }
    }
    pool[idx] = u;
    return idx;
  }
  bool lookup(ET u) {
    size_t val = hash32(u);
    for (int i = 1; i <= pointer; i++) {
      size_t len = bag_size[i] - bag_size[i - 1];
      size_t idx = (val & (len - 1)) + bag_size[i - 1];
      size_t num_probes = 0;
      while (pool[idx] != empty) {
        if (pool[idx] == u) {
          return true;
        } else {
          idx = (idx == bag_size[i] - 1 ? bag_size[i - 1] : idx + 1);
        }
        num_probes++;
        if (num_probes == len) {
          break;
        }
      }
    }
    return false;
  }
  template <typename Out_Seq>
  size_t pack(Out_Seq out) {
    size_t len = bag_size[pointer];
    auto pred =
        delayed_seq<bool>(len, [&](size_t i) { return pool[i] != empty; });
    size_t ret =
        pack_into_uninitialized(pool.cut(0, len), pred, make_slice(out));
    clear();
    return ret;
  }

  template <typename Out_Seq, typename Stamp_Seq>
  size_t pack(Out_Seq out, Stamp_Seq pool_stamp,
              // decltype(pool_stamp[0]) current_stamp) {
              uint32_t current_stamp) {
    size_t len = bag_size[pointer];
    auto pred = delayed_seq<bool>(
        len, [&](size_t i) { return pool_stamp[i] == current_stamp; });
    size_t ret =
        pack_into_uninitialized(pool.cut(0, len), pred, make_slice(out));
    for (int i = 0; i <= pointer; i++) {
      counter[i] = 0;
    }
    pointer = 1;
    return ret;
  }
  void report_load_factor() {
    for (int i = 1; i < num_hash_bag; i++) {
      size_t len = bag_size[i] - bag_size[i - 1];
      size_t occupied = 0;
      for (size_t j = bag_size[i - 1]; j < bag_size[i]; j++) {
        if (pool[j] != empty) {
          occupied++;
        }
      }
      printf("table[%d]: %f\n", i, 1.0 * occupied / len);
    }
  }
};
