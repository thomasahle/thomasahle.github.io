/* ABI is documented in DEMOS.md. All numeric results are validation-gated. */
#define DEMO_COUNT ((int)(sizeof(demo_pairs) / sizeof(demo_pairs[0])))
static int demo_validated;
static uint8_t demo_scratch[256] __attribute__((aligned(8)));

static uint64_t demo_next(uint64_t s[4]) {
    uint64_t x = s[1] * 5;
    uint64_t result = ((x << 7) | (x >> 57)) * 9;
    uint64_t t = s[1] << 17;
    s[2] ^= s[0]; s[3] ^= s[1]; s[1] ^= s[2]; s[0] ^= s[3];
    s[2] ^= t; s[3] = (s[3] << 45) | (s[3] >> 19);
    return result;
}
static void demo_hex(const uint64_t h[4], int bits, char *out) {
    const char *hex = "0123456789abcdef";
    int digits = bits / 4;
    for (int j = 0; j < digits; ++j) {
        int shift = (digits - 1 - j) * 4;
        out[j] = hex[(h[shift / 64] >> (shift % 64)) & 15];
    }
    out[digits] = 0;
}
static int demo_equal(int i, const uint64_t key[4], char *out) {
    uint64_t a[4] = {0}, b[4] = {0};
    const DemoPair *p = &demo_pairs[i];
    demo_digest(i, p->message[0], p->length[0], key, a);
    demo_digest(i, p->message[1], p->length[1], key, b);
    if (p->bits == 32) { a[0] = (uint32_t)a[0]; b[0] = (uint32_t)b[0]; }
    if (out) { demo_hex(a, p->bits, out); demo_hex(b, p->bits, out + 65); }
    return memcmp(a, b, (size_t)p->bits / 8) == 0;
}
int validate(void) {
    demo_validated = 0;
    if (!demo_source_validation()) return 0;
    for (int i = 0; i < DEMO_COUNT; ++i) {
        const DemoPair *p = &demo_pairs[i];
        char out[130];
        if (p->length[0] == p->length[1] &&
            !memcmp(p->message[0], p->message[1], p->length[0])) return 0;
        if (!demo_equal(i, p->example, out)) return 0;
        if (p->expected && strcmp(out, p->expected)) return 0;
    }
    demo_validated = 1;
    return 1;
}
int init(void) { return validate(); }
int pair_count(void) { return demo_validated ? DEMO_COUNT : 0; }
static int demo_index(int i) { return demo_validated && i >= 0 && i < DEMO_COUNT; }
const uint8_t *pair_bytes(int i, int which) {
    return demo_index(i) && which >= 0 && which < 2 ? demo_pairs[i].message[which] : NULL;
}
int pair_len(int i, int which) {
    return demo_index(i) && which >= 0 && which < 2 ? (int)demo_pairs[i].length[which] : -1;
}
int seed_bits(int i) { return demo_index(i) ? demo_pairs[i].seed_bits : -1; }
int key_words(int i) { return demo_index(i) ? demo_pairs[i].key_words : -1; }
int has_class(int i) { return demo_index(i) ? demo_pairs[i].class_mode : -1; }
void *scratch(void) { return demo_scratch; }
int example_key(int i, uint64_t *out) {
    if (!demo_index(i) || !out) return -1;
    memcpy(out, demo_pairs[i].example, 32);
    return demo_pairs[i].key_words;
}
int hash_key(int i, const uint64_t *key, char *out) {
    if (!demo_index(i) || !key || !out) return -1;
    if (demo_pairs[i].seed_bits == 32 && key[0] > UINT32_MAX) return -1;
    return demo_equal(i, key, out);
}
int hash_pair(int i, uint32_t seed_lo, uint32_t seed_hi, char *out) {
    /* This API follows the package's SMHasher3 seed expansion. The UI and batch
       sampler use hash_key with independent words for the full-key models. */
    if (!demo_index(i)) return -1;
    uint64_t key[4] = {0};
    demo_seed_key((uint64_t)seed_lo | (uint64_t)seed_hi << 32, key);
    return hash_key(i, key, out);
}
void rng_init(uint32_t lo, uint32_t hi, uint64_t *state) {
    uint64_t s = (uint64_t)lo | (uint64_t)hi << 32;
    for (int j = 0; j < 4; ++j) {
        uint64_t z = (s += UINT64_C(0x9e3779b97f4a7c15));
        z = (z ^ (z >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
        z = (z ^ (z >> 27)) * UINT64_C(0x94d049bb133111eb);
        state[j] = z ^ (z >> 31);
    }
}
int sample_seed(int i, int mode, uint64_t *state, uint64_t *out) {
    if (!demo_index(i) || !state || !out || (mode != 0 && mode != 1) ||
        (mode && !demo_pairs[i].class_mode) || !(state[0] | state[1] | state[2] | state[3])) return -1;
    memset(out, 0, 32);
    for (int j = 0; j < demo_pairs[i].key_words; ++j) out[j] = demo_next(state);
    if (demo_pairs[i].seed_bits == 32) out[0] = (uint32_t)out[0];
    if (mode && !demo_class_key(i, state, out)) return -1;
    return demo_pairs[i].key_words;
}
int run_batch(int i, uint32_t n, uint64_t *state, int mode) {
    if (!demo_index(i) || n > 1000000) return -1;
    int collisions = 0;
    for (uint32_t j = 0; j < n; ++j) {
        uint64_t key[4];
        if (sample_seed(i, mode, state, key) < 0) return -1;
        collisions += demo_equal(i, key, NULL);
    }
    return collisions;
}
