// swiss_check.cc -- the seed protocol of a real absl::flat_hash_set / flat_hash_map
// (abseil-cpp 73d2688): which seed does the table feed absl::Hash, and do the
// three pairs collide inside real tables?  Uses the RawHashSetTestOnlyAccess
// friend hook declared in raw_hash_set.h (its definition lives in Abseil's
// unit test, so it is provided here) to read hash_of() and the per-table seed.
// Copyright (c) 2026 Thomas Dybdahl Ahle.  MIT License.
// Build: see CMakeLists.txt (../Makefile target `swiss`).
#include <cstdio>
#include <cstring>
#include <set>
#include <string>
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/hash/hash.h"
namespace absl { namespace container_internal {
struct RawHashSetTestOnlyAccess {
  template <class S, class K> static size_t hash_of(const S& s, const K& k) { return s.hash_of(k); }
  template <class S> static size_t seed(const S& s) { return s.common().seed().seed(); }
};
}}  // namespace absl::container_internal
using absl::container_internal::RawHashSetTestOnlyAccess;
static std::string unhex(const char* s) {
  std::string v;
  for (size_t i = 0; s[i] && s[i + 1]; i += 2) { unsigned b; sscanf(s + i, "%2x", &b); v.push_back((char)b); }
  return v;
}
int main() {
  const char* P[3][2] = {{"", "a6e02637c07bd386"}, {"61", "44b53d572db1948b"},
                         {"6162636465666768f58c1edee0f9d579", "4142434445464748f58c1edee0f9d579"}};
  std::set<size_t> seeds;
  long tables = 0, bad_seed = 0, checks = 0, coll = 0, bad_hws = 0;
  for (int t = 0; t < 400; t++) {
    absl::flat_hash_set<std::string> s;
    for (int i = 0; i < 2 + (t % 50); i++) s.insert("k" + std::to_string(t * 1000 + i));
    size_t seed = RawHashSetTestOnlyAccess::seed(s); seeds.insert(seed); tables++;
    if (seed % 64 != 0 || seed / 64 >= 32) bad_seed++;
    for (int p = 0; p < 3; p++) {
      std::string a = unhex(P[p][0]), b = unhex(P[p][1]);
      size_t ha = RawHashSetTestOnlyAccess::hash_of(s, a), hb = RawHashSetTestOnlyAccess::hash_of(s, b);
      // hash_of(k) must be HashWithSeed().hash(Hash<string>{}, k, seed): the protocol the row states
      size_t hw = absl::hash_internal::HashWithSeed().hash(absl::Hash<std::string>{}, a, seed);
      if (hw != ha) bad_hws++;
      checks++; coll += (ha == hb);
      s.insert(a); s.insert(b);
      if (!s.contains(a) || !s.contains(b)) { printf("lookup failure\n"); return 1; }
    }
    absl::flat_hash_map<std::string, int> m;
    for (int i = 0; i < 3 + (t % 7); i++) m["m" + std::to_string(t * 7 + i)] = i;
    size_t ms = RawHashSetTestOnlyAccess::seed(m); seeds.insert(ms); tables++;
    if (ms % 64 != 0 || ms / 64 >= 32) bad_seed++;
    for (int p = 0; p < 3; p++) {
      std::string a = unhex(P[p][0]), b = unhex(P[p][1]);
      checks++; coll += (RawHashSetTestOnlyAccess::hash_of(m, a) == RawHashSetTestOnlyAccess::hash_of(m, b));
    }
  }
  printf("real flat_hash_set/flat_hash_map<std::string> tables: %ld; distinct per-table seeds: %zu; seeds outside {0, 64, ..., 1984}: %ld\n",
         tables, seeds.size(), bad_seed);
  printf("hash_of(k) != HashWithSeed().hash(Hash<string>{}, k, seed): %ld of %ld\n", bad_hws, checks / 2);
  printf("pair checks inside real tables: %ld; collided: %ld\n", checks, coll);
  printf("seeds seen:");
  for (size_t x : seeds) printf(" %zu", x);
  printf("\n%s\n", (bad_seed || bad_hws || coll != checks) ? "FAILED" : "OK: every pair collides in every real table");
  return (bad_seed || bad_hws || coll != checks);
}
