/* Prints which polyxor hash_blocks backend the staticlib dispatches to. */
#include <stdio.h>
const char *polyxor_backend_probe(void);
int main(void) { puts(polyxor_backend_probe()); return 0; }
