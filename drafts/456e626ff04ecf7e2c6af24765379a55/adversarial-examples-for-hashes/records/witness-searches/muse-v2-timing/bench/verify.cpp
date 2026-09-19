#include "Platform.h"
#include "Hashlib.h"
#include "museair2.h"
#include <cstdio>
#include <cstdlib>
#include <cinttypes>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
static const HashInfo *h;
static std::vector<uint8_t> unhex(const std::string &s) {
    std::vector<uint8_t> b;
    for (size_t i=0; i+1<s.size(); i+=2) b.push_back(strtoul(s.substr(i,2).c_str(),nullptr,16));
    return b;
}
static uint64_t call(const uint8_t *p,size_t n,uint64_t seed) {
    uint64_t out[2]={0,0x123456789abcdef0ULL};
    if(h->Seed(seed)!=seed) abort();
    h->hashfn_native(p,n,h->Seed(seed),out);
    if(out[1]!=0x123456789abcdef0ULL) abort();
    return out[0];
}
int main(int argc,char **argv) {
    h=findHash("MuseAir-v2"); if(!h || h->bits!=64 || !h->Init()) abort();
    printf("REGISTRATION name=%s backend=%s bits=%u verify=%08x\n",h->name,h->impl,h->bits,h->ComputedVerify(HashInfo::ENDIAN_NATIVE));
    std::ifstream f(argv[1]); if(!f) abort();
    std::string line; size_t count=0,bad=0,maxlen=0;
    while(std::getline(f,line)) {
        std::istringstream s(line);size_t n;uint64_t seed,sa,sb,want;std::string hex;
        if(!(s>>std::dec>>n>>std::hex>>seed>>sa>>sb>>hex>>want)) abort();
        auto b=unhex(hex);if(b.size()!=n) abort();
        // Exercise both aligned and unaligned registered inputs.
        for(size_t off: {size_t(0),size_t(1),size_t(7)}) {
            std::vector<uint8_t> storage(n+off+1);memcpy(storage.data()+off,b.data(),n);
            if(call(storage.data()+off,n,seed)!=want) bad++;
        }
        ++count; if(n>maxlen)maxlen=n;
    }
    printf("VECTORS count=%zu registered_calls=%zu mismatches=%zu maxlen=%zu\n",count,count*3,bad,maxlen);
    if(bad || !count) return 1;
    auto a=unhex(std::string(64,'0'));
    auto b=unhex("00000000000000404a048402a910048a00000000000000a80000000000000000");
    for(auto seed:{0x040963434fe5e368ULL,0xacac861647d5f7f2ULL,0ULL}) {
        auto x=call(a.data(),32,seed),y=call(b.data(),32,seed);
        printf("EXAMPLE seed=%016llx a=%016" PRIx64 " b=%016" PRIx64 "\n",seed,x,y);
        if((x==y)!=(seed!=0))abort();
        if(seed==0x040963434fe5e368ULL && x!=0xef46cda19c0bbc67ULL)abort();
        if(seed==0xacac861647d5f7f2ULL && x!=0xb1f37635e72a255fULL)abort();
    }
    if(argc>2) {
        uint64_t n=1ULL<<atoi(argv[2]),state=0x763220260919ULL,hits=0;
        for(uint64_t i=0;i<n;i++) {auto seed=ma_splitmix(&state);hits+=call(a.data(),32,seed)==call(b.data(),32,seed);}
        double k=hits,lo=k*pow(1-1/(9*k)-1.96/(3*sqrt(k)),3),hi=(k+1)*pow(1-1/(9*(k+1))+1.96/(3*sqrt(k+1)),3);
        printf("PAIR trials=%" PRIu64 " collisions=%" PRIu64 " rng=splitmix64 master=0x763220260919 log2_rate=%.9f poisson95_log2=[%.9f,%.9f]\n",n,hits,log2(k/n),log2(lo/n),log2(hi/n));
        if(!hits || fabs(log2(k/n)+17.447)>0.2)return 1;
    }
    puts("PASS");
}
