#!/usr/bin/env python3
from pathlib import Path
import sys,shutil,json,hashlib
root=Path(sys.argv[1]);here=Path(__file__).resolve().parent
for dest,src in [('hashes/halftimefixed.cpp',here/'halftimefixed.cpp'),('hashes/halftime-fixed.hpp',Path(sys.argv[2]))]:
 shutil.copy2(src,root/dest)
p=root/'hashes/Hashsrc.cmake';s=p.read_text();s+='\nlist(APPEND HASH_SRC_FILES hashes/halftimefixed.cpp)\n';p.write_text(s)
p=root/'CMakeLists.txt';s=p.read_text();s+='\nset_source_files_properties(hashes/halftimefixed.cpp PROPERTIES COMPILE_OPTIONS "-std=c++17;-flax-vector-conversions;-Wno-ignored-attributes")\n';p.write_text(s)
p=root/'main.cpp';s=p.read_text();needle='    // If you extend these statements by adding a new bitcount/type, you\n';assert needle in s
s=s.replace(needle,'''    // The Speed implementation is independent of hashtype and writes the full
    // registered output. Add 192-bit Speed/Sanity without changing any timer,
    // sample count, loop, buffer, or template instantiations of other tests.
    if (hInfo->bits == 192) {
        if (g_testAll || (!g_testSpeed && !g_testSanity)) {
            printf("192-bit adapter supports --test=Speed or --test=Sanity only\\n");
            return false;
        }
        if (!hInfo->Init()) return false;
        printf("--- Testing %s \\"%s\\" [%s]\\n\\n", hInfo->name, hInfo->desc, hInfo->impl);
        bool result = true;
        if (g_testSanity) {
            result &= HashSelfTest(hInfo);
            result &= SanityTest(hInfo, flags);
        }
        if (g_testSpeed) {
            SpeedTestInit(findHash("donothing-32"), flags);
            result &= SpeedTest(hInfo, flags, g_testSpeedSmall, g_testSpeedBulk);
        }
        return result;
    }

'''+needle)
p.write_text(s)
