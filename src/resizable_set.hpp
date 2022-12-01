// This code is part of the Problem Based Benchmark Suite (PBBS)
// Copyright (c) 2010-2016 Guy Blelloch and the PBBS team
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights (to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#pragma once


#include "parlay/primitives.h"
#include "parlay/sequence.h"

#include <tuple>
#include <unordered_set>
#include <vector>

#include "utilities.h"

using namespace parlay;

namespace gbbs {
// TODO: see if striding by an entire page improves times further.
constexpr size_t kResizableTableCacheLineSz = 128;

inline size_t hashToRange(const size_t& h, const size_t& mask) {
  return h & mask;
}
inline size_t incrementIndex(const size_t& h, const size_t& mask) {
  return hashToRange(h + 1, mask);
}

template <class T, class KeyHash>
class resizable_set {
 public:
  size_t m;
  size_t mask;
  size_t ne;
  T empty;
  sequence<T> table;
  KeyHash key_hash;
  sequence<size_t> cts;

  inline size_t firstIndex(T& k) { return hashToRange(key_hash(k), mask); }

  void init_counts() {
    size_t workers = num_workers();
    cts = sequence<size_t>::uninitialized(kResizableTableCacheLineSz * workers);
    for (size_t i = 0; i < workers; i++) {
      cts[i * kResizableTableCacheLineSz] = 0;
    }
  }

  void update_nelms() {
    size_t workers = num_workers();
    for (size_t i = 0; i < workers; i++) {
      ne += cts[i * kResizableTableCacheLineSz];
      cts[i * kResizableTableCacheLineSz] = 0;
    }
  }

  resizable_set() : m(0), ne(0) {
    mask = 0;
    init_counts();
  }

  resizable_set(size_t _m, T _empty, KeyHash _key_hash)
      // : m((size_t)1 << parlay::log2_up((size_t)(1.1 * _m))),
      : m((size_t)1 << parlay::log2_up(_m)),
        mask(m - 1),
        ne(0),
        empty(_empty),
        key_hash(_key_hash) {
    table = sequence<T>::uninitialized(m);
    clear();
    init_counts();
  }

  void maybe_resize(size_t n_inc) {
    size_t nt = ne + n_inc;
    if (nt > (0.25 * m)) {
      size_t old_m = m;
      auto old_t = std::move(table);
      m = ((size_t)1 << parlay::log2_up((size_t)(10 * nt)));
      if (m == old_m) {
        return;
      }
      // cout << "#### resize ne " << ne << " m " << m<< endl;
      mask = m - 1;
      ne = 0;
      table = sequence<T>::uninitialized(m);
      clear();
      parallel_for(0, old_m, [&](size_t i) {
        if (old_t[i] != empty) {
          insert(old_t[i]);
        }
      });
      update_nelms();
    }
  }

  bool insert(T kv) {
    size_t h = firstIndex(kv);
    size_t count = 0;
    while (1) {
      // for (size_t i = 0; i<m; i++){
      if (table[h] == empty && atomic_compare_and_swap(&table[h], empty, kv)) {
        size_t wn = worker_id();
        cts[wn * kResizableTableCacheLineSz]++;
        return 1;
      }
      if (table[h] == kv) {
        return false;
      }
      h = incrementIndex(h, mask);
      if (count++ == m + 2) {
        cout << "table full m " << m << endl;
      }
    }
    // cout << "table full m " << m << endl;
    return 0;
  }

  void clear() {
    parallel_for(0, m, [&](size_t i) { table[i] = empty; });
  }
};

}  // namespace gbbs