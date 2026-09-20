/* Browser adapter only. The included verification program is never modified. */
#include <stdint.h>
#include <stddef.h>
#include <string.h>

typedef struct {
    const uint8_t *message[2];
    uint32_t length[2];
    int bits, seed_bits, key_words, variant, class_mode;
    uint64_t example[4];
    const char *expected; /* Numeric hex: most significant word first. */
} DemoPair;

static int demo_source_validation(void);
static void demo_digest(int i, const uint8_t *m, size_t n,
                        const uint64_t key[4], uint64_t out[4]);
static int demo_class_key(int i, uint64_t state[4], uint64_t key[4]);
static void demo_seed_key(uint64_t seed, uint64_t key[4]);
static uint64_t demo_next(uint64_t state[4]);

static uint64_t demo_read64(const uint8_t *p) {
    uint64_t x = 0;
    for (int j = 0; j < 8; ++j) x |= (uint64_t)p[j] << (8 * j);
    return x;
}
