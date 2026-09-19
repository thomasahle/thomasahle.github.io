/* Independent-implementation check: uses the hash function from the website's verify package
 * (t1ha2_64_verify.c, compiled with its main renamed) on the new pair and seeds found on the Xeon. */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>
uint64_t t1ha2_64_ext(const void *data, size_t len, uint64_t seed);
static int hex2bytes(const char *h, uint8_t *b) { int n = strlen(h) / 2; for (int i = 0; i < n; i++) { unsigned v; sscanf(h + 2 * i, "%2x", &v); b[i] = v; } return n; }
int main(int argc, char **argv) {
    if (argc < 4) { fprintf(stderr, "usage: indep_check m1hex m2hex seedhex...\n"); return 1; }
    uint8_t m1[32], m2[32]; int l1 = hex2bytes(argv[1], m1), l2 = hex2bytes(argv[2], m2);
    for (int i = 3; i < argc; i++) { uint64_t s = strtoull(argv[i], NULL, 16); uint64_t h1 = t1ha2_64_ext(m1, l1, s), h2 = t1ha2_64_ext(m2, l2, s);
        printf("seed %016" PRIx64 ": H(m1)=%016" PRIx64 " H(m2)=%016" PRIx64 " %s\n", s, h1, h2, h1 == h2 ? "COLLIDE" : "differ"); }
    return 0; }
