#include "farmhash.cc"
#include <cstdio>
int main(){ const char* s[]={"orlp-farmhash64-&?J3rZ_8gsuieaaa","orlp-farmhash64-/v^CqdPvziuheaaa","orlp-farmhash64-?VrJ@L7ytzwheaaa","orlp-farmhash64-B?&l::hxqmfjeaaa","orlp-farmhash64-IbY`xAG&ibkieaaa","orlp-farmhash64-LOBWtm5Szyuieaaa","orlp-farmhash64-Mptaa^g^ytvieaaa","orlp-farmhash64-QiY!clz]bttieaaa","orlp-farmhash64-[_LU!d1hwmkieaaa","orlp-farmhash64-p3`!SQb}fmxheaaa","orlp-farmhash64-pdt\x27cuI\\gvxheaaa"};
for(auto x: s) printf("%s len=%zu Hash64=%llu\n", x, strlen(x), (unsigned long long)farmhashna::Hash64(x, strlen(x))); }
