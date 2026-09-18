// Benchmark adapters for the fixed upstream header. These hosts are little endian.
#include "Platform.h"
#include "Hashlib.h"
#define halftime_hash halftime_hash_fixed
#include "halftime-fixed.hpp"
#undef halftime_hash

#if defined(__AVX512F__)
#define HH_FIXED_IMPL "avx512f"
#elif defined(__AVX2__)
#define HH_FIXED_IMPL "avx2"
#elif defined(__ARM_NEON) || defined(__ARM_NEON__)
#define HH_FIXED_IMPL "neon"
#elif defined(__SSE2__)
#define HH_FIXED_IMPL "sse2"
#else
#define HH_FIXED_IMPL "portable"
#endif
alignas(64) static thread_local uint64_t fixed_entropy[9000];
static uint64_t fixed_splitmix(uint64_t& s){
 uint64_t z=(s+=UINT64_C(0x9e3779b97f4a7c15));
 z=(z^(z>>30))*UINT64_C(0xbf58476d1ce4e5b9);
 z=(z^(z>>27))*UINT64_C(0x94d049bb133111eb);return z^(z>>31);
}
static uintptr_t fixed_seed(const seed_t seed){
 uint64_t m=seed,w=fixed_splitmix(m),x=fixed_splitmix(m),y=fixed_splitmix(m),z=fixed_splitmix(m);
 for(unsigned i=0;i<10+9000;++i){
  const uint64_t wp=w,xp=x,yp=y,zp=z;
  w=zp*UINT64_C(15241094284759029579);x=zp+ROTL64(wp,52);y=yp-xp;z=ROTL64(yp+wp,19);
  if(i>=10)fixed_entropy[i-10]=xp;
 }
 return (uintptr_t)fixed_entropy;
}
static void fixed24(const void*in,size_t n,seed_t seed,void*out){
 uint64_t result[3];
 halftime_hash_fixed::advanced::V4<3>((const uint64_t*)(uintptr_t)seed,(const char*)in,n,result);
 memcpy(out,result,24);
}
static void fixed512(const void*in,size_t n,seed_t seed,void*out){
 uint64_t result=halftime_hash_fixed::HalftimeHashStyle512((const uint64_t*)(uintptr_t)seed,(const char*)in,n);
 memcpy(out,&result,8);
}
REGISTER_FAMILY(halftimefixed,
 $.src_url="https://github.com/jbapple/HalftimeHash",
 $.src_status=HashFamilyInfo::SRC_STABLEISH
);
// Both slots call the literal little-endian header, not a new endian port.
REGISTER_HASH(HalftimeHash24_fixed,
 $.desc="Fixed HalftimeHash24 (192-bit output, 512-bit blocks, length tagged)",
 $.impl=HH_FIXED_IMPL,
 $.hash_flags=0,
 $.impl_flags=FLAG_IMPL_MULTIPLY|FLAG_IMPL_LICENSE_MIT,
 $.bits=192,
 $.verification_LE=0,
 $.verification_BE=0,
 $.hashfn_native=fixed24,
 $.hashfn_bswap=fixed24,
 $.seedfn=fixed_seed
);
REGISTER_HASH(HalftimeHash_512_fixed,
 $.desc="Fixed HalftimeHash Style512 (64-bit output)",
 $.impl=HH_FIXED_IMPL,
 $.hash_flags=FLAG_HASH_LOOKUP_TABLE,
 $.impl_flags=FLAG_IMPL_MULTIPLY|FLAG_IMPL_LICENSE_MIT,
 $.bits=64,
 $.verification_LE=0x1E0F99EA,
 $.verification_BE=0x1E0F99EA,
 $.hashfn_native=fixed512,
 $.hashfn_bswap=fixed512,
 $.seedfn=fixed_seed
);
