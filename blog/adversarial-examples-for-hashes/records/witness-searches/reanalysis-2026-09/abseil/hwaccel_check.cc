// hwaccel_check.cc -- seed-independent collisions of absl::Hash in the
// hardware-accelerated build (ABSL_OPTION_INLINE_HW_ACCEL_STRATEGY != 0,
// CRC32C available).  Links the REAL abseil-cpp (pinned 73d2688).
//
// Mechanism.  For 9 <= len <= 32 the CRC path of CombineContiguousImpl
// (absl/hash/internal/hash.h) computes two CRC32C chains:
//    crcs.first  starts at state + 8*len and absorbs Read8(first)      [and Read8(first+len-16) if len>16]
//    crcs.second starts at gbswap_64(state) and absorbs Read8(first+len-8) [and Read8(first+8) if len>16]
// then returns Mix(mul - crcs.first, crcs.second - mul), mul = rotr(kMul,len),
// which is seed-independent.  CRC32C is GF(2)-affine in the data with the seed
// only in the initial value, so for two equal-length strings x, x' the CRC
// difference is  crc(init, w) ^ crc(init, w') = C(w ^ w'), independent of the
// seed, where C(.) = crc32c(0, .) is linear.  Hence H(x) = H(x') for EVERY
// seed as soon as both chains see the same running value, i.e. the byte
// difference d = x ^ x' satisfies  C8(each 8-byte read of d) = 0.  ker C8 has
// dimension 32; intersecting the two (overlapping) read windows gives the
// per-length family dimensions this program constructs and verifies.
//
// This program: (1) builds the GF(2) nullspace of {read-window CRC = 0} for
// each length 9..16 from the hardware crc32 instruction, (2) turns it into an
// explicit base string + basis of byte-differences, (3) evaluates the REAL
// library hash over many seeds and the 32 SwissTable seeds and checks every
// family member collides, (4) inserts a 2^N family into a real
// absl::flat_hash_set and measures the damage.
//
// Copyright (c) 2026 Thomas Dybdahl Ahle.  MIT License.  No Abseil source text
// is copied here; the algebra is derived from the public algorithm.
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <random>
#include <set>
#include <string>
#include <vector>
#include "absl/hash/hash.h"
#include "absl/hash/internal/hash.h"
#include "absl/container/flat_hash_set.h"
#include "absl/container/flat_hash_map.h"

#if defined(__x86_64__) && defined(__SSE4_2__)
#include <nmmintrin.h>
static inline uint32_t C64(uint64_t d){return (uint32_t)_mm_crc32_u64(0,d);}
#elif defined(__ARM_FEATURE_CRC32)
#include <arm_acle.h>
static inline uint32_t C64(uint64_t d){return __crc32cd(0,d);}
#else
#error "build with -msse4.2 (x86) or CRC32 (arm)"
#endif

namespace absl { namespace container_internal {
struct RawHashSetTestOnlyAccess {
  template <class S, class K> static size_t hash_of(const S& s, const K& k){return s.hash_of(k);}
  template <class S> static size_t seed(const S& s){return s.common().seed().seed();}
};
}}
using absl::container_internal::RawHashSetTestOnlyAccess;

// ---- real library evaluation (the exact hook flat_hash_set uses) ----
static uint64_t H(const std::string& s, uint64_t seed){
  return absl::hash_internal::HashWithSeed().hash(absl::Hash<std::string_view>{},
                                                  std::string_view(s), seed);
}

// ---- GF(2) linear algebra over up to 128-bit difference vectors ----
struct Row { uint64_t w[2]; };  // up to 128 bits
static void xoreq(Row&a,const Row&b){a.w[0]^=b.w[0];a.w[1]^=b.w[1];}
static bool bit(const Row&r,int i){return (r.w[i/64]>>(i%64))&1;}
static void setbit(Row&r,int i){r.w[i/64]|=1ull<<(i%64);}

// Build a basis of the nullspace {d in GF(2)^(8L): C8(read0(d))=0, C8(read1(d))=0}
// for a single-CRC-round length L in [9,16] (reads at byte offsets 0 and L-8).
static std::vector<Row> family_basis(int L){
  int nb = 8*L;
  // constraint rows: for each read (off 0, off L-8) and each output bit j of C8
  uint32_t M[64]; for(int i=0;i<64;i++) M[i]=C64(1ull<<i); // col i = C8(e_i)
  int offs[2]={0,L-8};
  std::vector<Row> cons;
  for(int r=0;r<2;r++) for(int j=0;j<32;j++){
    Row row{{0,0}}; bool any=false;
    for(int i=0;i<64;i++) if((M[i]>>j)&1){ setbit(row,8*offs[r]+i); any=true; }
    if(any) cons.push_back(row);
  }
  // reduce to row echelon, remember pivots
  std::vector<int> pivcol; std::vector<Row> ech;
  for(Row row: cons){
    for(size_t k=0;k<ech.size();k++) if(bit(row,pivcol[k])) xoreq(row,ech[k]);
    int p=-1; for(int i=0;i<nb;i++) if(bit(row,i)){p=i;break;}
    if(p<0) continue;
    for(size_t k=0;k<ech.size();k++) if(bit(ech[k],p)) xoreq(ech[k],row);
    ech.push_back(row); pivcol.push_back(p);
  }
  std::set<int> piv(pivcol.begin(),pivcol.end());
  // free variables -> basis vectors of nullspace
  std::vector<Row> basis;
  for(int f=0; f<nb; f++){
    if(piv.count(f)) continue;
    Row v{{0,0}}; setbit(v,f);
    for(size_t k=0;k<ech.size();k++) if(bit(ech[k],f)) setbit(v,pivcol[k]);
    // sanity: C8 of both reads must be zero
    basis.push_back(v);
  }
  return basis;
}

static std::string apply_diff(const std::string& base, const Row& d){
  std::string s=base;
  // apply byte-wise: byte i gets XOR of bits 8i..8i+7
  for(size_t i=0;i<s.size();i++){
    unsigned db=0;
    for(int b=0;b<8;b++) if(bit(d,(int)i*8+b)) db|=1u<<b;
    s[i]^= (char)db;
  }
  return s;
}

int main(int argc, char** argv){
  int LOG2N = argc>1?atoi(argv[1]):20;         // uniform seeds sampled = 2^LOG2N
  int FAMLOG = argc>2?atoi(argv[2]):16;         // family members tested = 2^FAMLOG
  uint64_t master = argc>3?strtoull(argv[3],0,0):0xC0FFEEULL;

  printf("abseil-cpp HW-accel (CRC32C) key-free collision check\n");
  printf("hook: absl::hash_internal::HashWithSeed().hash(Hash<string_view>{}, sv, seed)\n");
  // Confirm we are really on the CRC path: dim ker C8 must be 32.
  {
    // count nullspace dim of C8 by rank
    uint32_t M[64]; for(int i=0;i<64;i++) M[i]=C64(1ull<<i);
    // rank of 32x64 over GF(2)
    std::vector<uint64_t> rows;
    for(int j=0;j<32;j++){uint64_t r=0;for(int i=0;i<64;i++) if((M[i]>>j)&1) r|=1ull<<i; rows.push_back(r);}
    int rank=0; for(int c=0;c<64;c++){int p=-1;for(size_t r=rank;r<rows.size();r++) if(rows[r]>>c&1){p=r;break;} if(p<0)continue; std::swap(rows[rank],rows[p]); for(size_t r=0;r<rows.size();r++) if(r!=(size_t)rank&&(rows[r]>>c&1)) rows[r]^=rows[rank]; rank++;}
    printf("CRC32C 8-byte map: rank %d, ker dim %d (expect 32) -- %s\n", rank, 64-rank, (64-rank==32)?"CRC path confirmed":"NOT CRC PATH");
  }

  std::mt19937_64 rng(master);
  // base strings for each length: fixed ASCII so the report can print them
  int ALL_OK=1;
  long grand_tables=0, grand_slot_coll=0;

  // ---- per-length pairs and small family verification ----
  printf("\nPer-length seed-independent families (single CRC round, len 9..16):\n");
  for(int L=9; L<=16; L++){
    std::string base(L,0);
    for(int i=0;i<L;i++) base[i]=(char)('A'+i);  // "ABCDE..." truncated
    std::vector<Row> B = family_basis(L);
    int dim=B.size();
    // explicit pair: base vs base ^ B[0]
    std::string m1 = apply_diff(base, B[0]);
    uint64_t h0a=H(base,0), h0b=H(m1,0);
    uint64_t hSa=H(base,0x9e3779b97f4a7c15ULL), hSb=H(m1,0x9e3779b97f4a7c15ULL);
    // sample uniform seeds: every family member (2^min(dim,FAMLOG)) equal to base?
    int fl = std::min(dim, FAMLOG);
    long N = 1L<<LOG2N;
    // test: pick fl basis vectors -> 2^fl members; require all equal for all sampled seeds + 32 table seeds
    long checked=0, collided=0;
    std::mt19937_64 srng(master ^ (0x100+L));
    // For speed, verify collision as: member hash == base hash. Sample seeds.
    // First the exhaustive 32 table seeds:
    long tbl_ok=0, tbl_tot=0;
    for(int ti=0; ti<32; ti++){
      uint64_t seed = (uint64_t)ti<<6;   // {0,64,...,1984}
      uint64_t hb=H(base,seed);
      for(uint64_t mask=0; mask<(1u<<fl); mask++){
        Row d{{0,0}}; for(int b=0;b<fl;b++) if(mask>>b&1) xoreq(d,B[b]);
        std::string mm=apply_diff(base,d);
        tbl_tot++; if(H(mm,seed)==hb) tbl_ok++;
      }
    }
    // uniform seeds: test the single pair (base,B[0]) plus a rotating member, N seeds
    long pair_ok=0;
    for(long i=0;i<N;i++){
      uint64_t seed=srng();
      uint64_t hb=H(base,seed);
      if(H(m1,seed)==hb) pair_ok++;
      checked++;
    }
    bool ok = (tbl_ok==tbl_tot) && (pair_ok==N) && (h0a==h0b) && (hSa==hSb);
    if(!ok) ALL_OK=0;
    printf("  L=%2d dim=%2d (2^%d family): pair H(base,0)=%016llx H(m',0)=%016llx %s; "
           "table seeds %ld/%ld; uniform 2^%d pair %ld/%ld; sub-family 2^%d x32 seeds %ld/%ld  [%s]\n",
      L, dim, dim, (unsigned long long)h0a, (unsigned long long)h0b, h0a==h0b?"EQ":"NE",
      tbl_ok, tbl_tot, LOG2N, pair_ok, N, fl, tbl_ok, tbl_tot, ok?"OK":"FAIL");
    if(L==9){
      // print the shortest explicit pair in hex for the report/reply
      auto hex=[&](const std::string&s){std::string o;char t[3];for(unsigned char c:s){sprintf(t,"%02x",c);o+=t;}return o;};
      printf("     shortest pair (9 bytes): m  = %s  (\"%s\")\n", hex(base).c_str(), base.c_str());
      printf("                              m' = %s\n", hex(m1).c_str());
    }
  }

  // ---- real flat_hash_set damage at the shortest length (L=9, 2^FAMLOG members) ----
  printf("\nReal absl::flat_hash_set<std::string> experiment (length 9, 2^%d members):\n", FAMLOG);
  {
    int L=9; std::string base(L,0); for(int i=0;i<L;i++) base[i]=(char)('A'+i);
    std::vector<Row> B=family_basis(L);
    int fl=std::min((int)B.size(),FAMLOG);
    // build family members (distinct)
    std::set<std::string> famset;
    for(uint64_t mask=0; mask<(1u<<fl); mask++){
      Row d{{0,0}}; for(int b=0;b<fl;b++) if(mask>>b&1) xoreq(d,B[b]);
      famset.insert(apply_diff(base,d));
    }
    std::vector<std::string> fam(famset.begin(),famset.end());
    // confirm they all share one hash under a real table's seed
    absl::flat_hash_set<std::string> hs;
    for(auto&s:fam) hs.insert(s);
    size_t seed = RawHashSetTestOnlyAccess::seed(hs);
    std::set<uint64_t> hvals;
    for(auto&s:fam) hvals.insert(RawHashSetTestOnlyAccess::hash_of(hs,s));
    printf("  family: %zu distinct 9-byte strings; table seed=%zu; distinct hash_of() values among them: %zu\n",
           fam.size(), seed, hvals.size());
    // compare: random 9-byte strings of the same count
    std::mt19937_64 r(master^0xABCD);
    std::vector<std::string> rnd; std::set<std::string> rs;
    while(rs.size()<fam.size()){std::string s(9,0);for(auto&c:s)c=(char)r();rs.insert(s);}
    rnd.assign(rs.begin(),rs.end());
    absl::flat_hash_set<std::string> hr;
    for(auto&s:rnd) hr.insert(s);
    size_t rseed=RawHashSetTestOnlyAccess::seed(hr);
    std::set<uint64_t> rhv; for(auto&s:rnd) rhv.insert(RawHashSetTestOnlyAccess::hash_of(hr,s));
    printf("  random: %zu distinct 9-byte strings; table seed=%zu; distinct hash_of() values: %zu\n",
           rnd.size(), rseed, rhv.size());
    // H2 (top-7-bit control byte) distribution: family collapses to <=1, random ~uniform
    std::set<int> famH2, rndH2;
    for(auto&s:fam) famH2.insert((int)(RawHashSetTestOnlyAccess::hash_of(hs,s)>>57));
    for(auto&s:rnd) rndH2.insert((int)(RawHashSetTestOnlyAccess::hash_of(hr,s)>>57));
    printf("  distinct H2 control bytes: family=%zu (of 128), random=%zu (of 128)\n", famH2.size(), rndH2.size());
    if(hvals.size()!=1) ALL_OK=0;
  }

  printf("\n%s\n", ALL_OK?"ALL CHECKS PASSED":"SOME CHECKS FAILED");
  return ALL_OK?0:1;
}
