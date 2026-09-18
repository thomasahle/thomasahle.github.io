#include "farmhash.cc"
#include <cstdio>
#include <fstream>
#include <string>
int main(){ std::ifstream f("s11.bin"); std::string s((std::istreambuf_iterator<char>(f)), {}); printf("%s len=%zu Hash64=%llu\n", s.c_str(), s.size(), (unsigned long long)farmhashna::Hash64(s.data(), s.size())); }
