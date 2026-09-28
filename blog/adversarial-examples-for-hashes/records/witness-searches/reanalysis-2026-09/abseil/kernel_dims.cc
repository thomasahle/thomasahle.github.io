// kernel_dims.cc -- for each string length L in [9,16] (single CRC round, two
// 8-byte reads at offsets 0 and L-8), compute the GF(2) dimension of the space
// of byte-difference vectors d in {0,1}^(8L) with C8(d[0:8]) = 0 and
// C8(d[L-8:L]) = 0, where C8 : GF(2)^64 -> GF(2)^32 is the CRC32C map
// crc32c_u64(0, .).  That dimension is log2 of the seed-independent collision
// family at length L (every member differs from a base string only inside the
// L bytes, and both 8-byte CRC reads are unchanged, hence the hash is equal
// for every seed).  Also lengths 4..8 and 1..3 for the report.
// Build: c++ -O2 -std=c++17 -msse4.2 kernel_dims.cc -o kernel_dims  (arm: -march=armv8-a+crc)
#include <cstdint>
#include <cstdio>
#include <vector>
#if defined(__x86_64__)
#include <nmmintrin.h>
static inline uint32_t C64(uint64_t d){return (uint32_t)_mm_crc32_u64(0,d);}
static inline uint32_t C32(uint32_t d){return _mm_crc32_u32(0,d);}
static inline uint32_t C8b(uint8_t d){return _mm_crc32_u8(0,d);}
#else
#include <arm_acle.h>
static inline uint32_t C64(uint64_t d){return __crc32cd(0,d);}
static inline uint32_t C32(uint32_t d){return __crc32cw(0,d);}
static inline uint32_t C8b(uint8_t d){return __crc32cb(0,d);}
#endif

// Gaussian elimination over GF(2): rows are (constraint bits over N vars)->0.
// Returns nullspace dimension = N - rank.
static int nullspace_dim(std::vector<std::vector<uint64_t>>& rows, int words){
  int rank=0; int R=rows.size();
  for(int col=0; col<words*64; col++){
    int w=col/64, b=col%64; int piv=-1;
    for(int r=rank;r<R;r++) if(rows[r][w]>>b&1){piv=r;break;}
    if(piv<0) continue;
    std::swap(rows[rank],rows[piv]);
    for(int r=0;r<R;r++) if(r!=rank && (rows[r][w]>>b&1))
      for(int k=0;k<words;k++) rows[r][k]^=rows[rank][k];
    rank++;
  }
  return rank;
}

int main(){
  // len 9..16: variables are 8L bits, laid out byte 0 = bits 0..7, etc.
  // Two reads: read0 = bytes[0..7] as UnalignedLoad64 (LE), read1 = bytes[L-8..L-1].
  // A constraint says a linear functional of the 64-bit read equals 0.
  // Build C8's 32x64 matrix by images of unit inputs.
  for(int L=9; L<=16; L++){
    int nb=8*L, words=(nb+63)/64;
    std::vector<std::vector<uint64_t>> rows;
    // For each of the two reads, and each of 32 output-basis directions:
    // simplest: add all 32 CRC output bits as constraints (rank auto-handles).
    // output bit j of C8 applied to read = sum over input bits i of M[j][i]*var.
    // Build M once (32x64), then for read at byte offset off map input bit i -> var bit 8*off+i.
    uint32_t M[64];
    for(int i=0;i<64;i++) M[i]=C64(1ull<<i); // column i = image of e_i
    int offs[2]={0, L-8};
    for(int ridx=0;ridx<2;ridx++){
      int off=offs[ridx];
      for(int j=0;j<32;j++){
        std::vector<uint64_t> row(words,0);
        bool any=false;
        for(int i=0;i<64;i++) if(M[i]>>j&1){ int var=8*off+i; row[var/64]^=1ull<<(var%64); any=true;}
        if(any) rows.push_back(row);
      }
    }
    int rank=nullspace_dim(rows,words); int dim=nb-rank;
    printf("L=%2d bytes, 2 reads at off 0 and %2d, overlap %2d bytes: family dim = %2d  (2^%d members)\n",
           L, L-8, 16-L, dim, dim);
  }
  return 0;
}
