#pragma once
#include "utilities.h"

/* Union-Find options */
enum FindOption {
  find_compress,
  find_naive,
  find_split,
  find_halve,
  find_atomic_split,
  find_atomic_halve
};
enum UniteOption { unite, unite_early, unite_rem_cas };
// unite_nd, unite_rem_lock,

/* RemCAS-specific options */
enum SpliceOption {
  split_atomic_one,
  halve_atomic_one,
  splice_simple,
  splice_atomic
};

namespace gbbs {
using parent = NodeId;
namespace find_variants {
inline NodeId find_naive(NodeId i, sequence<parent>& parents) {
  while (i != parents[i]) {
    i = parents[i];
  }
  return i;
}

inline NodeId find_compress(NodeId i, sequence<parent>& parents) {
  parent j = i;
  if (parents[j] == j) return j;
  do {
    j = parents[j];
  } while (parents[j] != j);
  parent tmp;
  while ((tmp = parents[i]) > j) {
    parents[i] = j;
    i = tmp;
  }
  return j;
}

inline NodeId find_atomic_split(NodeId i, sequence<parent>& parents) {
  while (1) {
    parent v = parents[i];
    parent w = parents[v];
    if (v == w) {
      return v;
    } else {
      CAS(&parents[i], v, w);
      // i = its parents
      i = v;
    }
  }
}

inline NodeId find_atomic_halve(NodeId i, sequence<parent>& parents) {
  while (1) {
    parent v = parents[i];
    parent w = parents[v];
    if (v == w) {
      return v;
    } else {
      CAS(&parents[i], (parent)v, (parent)w);
      // i = its grandparent
      i = parents[i];
    }
  }
}
}  // namespace find_variants

namespace splice_variants {

/* Used in Rem-CAS variants for splice */
inline NodeId split_atomic_one(NodeId i, NodeId, sequence<parent>& parents) {
  parent v = parents[i];
  parent w = parents[v];
  if (v == w)
    return v;
  else {
    CAS(&parents[i], v, w);
    i = v;
    return i;
  }
}

/* Used in Rem-CAS variants for splice */
inline NodeId halve_atomic_one(NodeId i, NodeId, sequence<parent>& parents) {
  parent v = parents[i];
  parent w = parents[v];
  if (v == w)
    return v;
  else {
    CAS(&parents[i], v, w);
    i = w;
    return i;
  }
}

/* Used in Rem-CAS variants for splice */
inline NodeId splice_atomic(NodeId u, NodeId v, sequence<parent>& parents) {
  parent z = parents[u];
  CAS(&parents[u], z, parents[v]);
  return z;
}
}  // namespace splice_variants

namespace unite_variants {

template <class Find>
struct Unite {
  Find& find;
  Unite(Find& find) : find(find) {}

  inline NodeId operator()(NodeId u_orig, NodeId v_orig,
                           sequence<parent>& parents) {
    parent u = u_orig;
    parent v = v_orig;
    while (1) {
      u = find(u, parents);
      v = find(v, parents);
      if (u == v)
        break;
      else if (u > v && parents[u] == u && CAS(&parents[u], u, v)) {
        return u;
      } else if (v > u && parents[v] == v && CAS(&parents[v], v, u)) {
        return v;
      }
    }
    return UINT_N_MAX;
  }
};

// template <class Splice, class Compress, FindOption find_option>
// struct UniteRemLock {
//   NodeId n;
//   pbbs::sequence<bool> locks;
//   Compress& compress;
//   Splice& splice;
//   UniteRemLock(Compress& compress, Splice& splice, NodeId n) : n(n),
//   compress(compress), splice(splice) {
//     locks = pbbs::sequence<bool>(n, false);
//   }

//   inline void fence() {
//     std::atomic_thread_fence(std::memory_order_seq_cst);
//   }

//   bool acquire_lock(NodeId u) {
//     if (!locks[u] && pbbs::atomic_compare_and_swap(&locks[u], false, true)) {
//       return true;
//     }
//     return false;
//   }

//   void release_lock(NodeId u) {
//     locks[u] = false;
//   }

//   inline NodeId operator()(NodeId u_orig, NodeId v_orig,
//   pbbs::sequence<parent>& parents) {
//     parent rx = u_orig;
//     parent ry = v_orig;
//     while (parents[rx] != parents[ry]) {
//       /* link from high -> low */
//       if (parents[rx] < parents[ry]) std::swap(rx,ry);
//       if (rx == parents[rx]) {
//         if (acquire_lock(rx)) {
//           parent py = parents[ry];
//           if (rx == parents[rx] && rx > py) {
//             parents[rx] = py;
//           }
//           release_lock(rx);
//         }
//         /* link high -> low */
//       } else {
//         rx = splice(rx, ry, parents);
//       }
//     }
//     if constexpr (find_option != find_naive) { /* aka find_none */
//       compress(u_orig, parents);
//       compress(v_orig, parents);
//     }
//     return UINT_N_MAX;  // TODO (ret-value)
//   }
// };

template <class Splice, class Compress, FindOption find_option>
struct UniteRemCAS {
  Compress& compress;
  Splice& splice;
  UniteRemCAS(Compress& compress, Splice& splice)
      : compress(compress), splice(splice) {}

  inline NodeId operator()(NodeId x, NodeId y, sequence<parent>& parents) {
    NodeId rx = x;
    NodeId ry = y;
    while (parents[rx] != parents[ry]) {
      /* link high -> low */
      parent p_ry = parents[ry];
      parent p_rx = parents[rx];
      if (p_rx < p_ry) {
        std::swap(rx, ry);
        std::swap(p_rx, p_ry);
      }
      if (rx == parents[rx] && CAS(&parents[rx], rx, p_ry)) {
        // if (rx == parents[rx] && (parents[rx] = p_ry)) {
        if constexpr (find_option != find_naive) { /* aka find_none */
          compress(x, parents);
          compress(y, parents);
        }
        return rx;
      } else {
        // failure: locally compress by splicing and try again
        rx = splice(rx, ry, parents);
      }
    }
    return UINT_N_MAX;
  }
};

template <class Find, FindOption find_option>
struct UniteEarly {
  Find& find;
  UniteEarly(Find& find) : find(find) {}
  inline NodeId operator()(NodeId u, NodeId v, sequence<parent>& parents) {
    [[maybe_unused]] NodeId u_orig = u, v_orig = v;
    NodeId ret = UINT_N_MAX;
    while (u != v) {
      /* link high -> low */
      if (v > u) std::swap(u, v);
      if (parents[u] == u && CAS(&parents[u], u, v)) {
        ret = u;
        break;
      }
      parent z = parents[u];
      parent w = parents[z];
      CAS(&parents[u], z, w);
      u = w;
    }
    if constexpr (find_option != find_naive) {
      u = find(u_orig, parents); /* force */
      v = find(v_orig, parents); /* force */
    }
    return ret;
  }
};

// template <class Find>
// struct UniteND {
//   Find find;
//   pbbs::sequence<NodeId> hooks;
//   UniteND(size_t n, Find& find) : find(find) {
//     hooks = pbbs::sequence<NodeId>(n, UINT_N_MAX);
//   }

//   inline NodeId operator()(NodeId u_orig, NodeId v_orig,
//   pbbs::sequence<parent>& parents) {
//     parent u = u_orig;
//     parent v = v_orig;
//     while(1) {
//       u = find(u,parents);
//       v = find(v,parents);
//       if(u == v) break;
//       /* link high -> low */
//       if(u < v) std::swap(u,v);
//       if (hooks[u] == UINT_N_MAX && pbbs::atomic_compare_and_swap(&hooks[u],
//       UINT_N_MAX,v)) {
//         parents[u] = v;
//         return u;
//       }
//     }
//     return UINT_N_MAX;
//   }
// };

}  // namespace unite_variants
}  // namespace gbbs
