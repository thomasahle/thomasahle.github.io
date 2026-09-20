# verify/go-maphash/Makefile -- build and check the Go map-hash reproduction on its own.
# The top-level verify/Makefile builds and checks this directory too (`make check DIRS=go-maphash`).
#
#   make            build go_maphash_verify with hardware AES when the host has it
#   make check      run it with 2^20 samples and compare the four collision counts with EXPECT
#   make portable   build go_maphash_verify_portable (table AES, no target flags)
#   make real       build the two real-runtime Go programs (needs the go1.27.1 toolchain on PATH)
#   make clean

CC     ?= cc
CFLAGS ?= -O2 -std=c11
LDLIBS ?= -lm
GO     ?= go
EXPECT ?= 65536 65536 0 0

UNAME_M := $(shell uname -m)
ifneq ($(filter arm64 aarch64,$(UNAME_M)),)
AESFLAGS := -march=armv8-a+crypto
else ifneq ($(filter x86_64 amd64,$(UNAME_M)),)
AESFLAGS := -maes
else
AESFLAGS :=
endif

.PHONY: all check portable real clean

all: go_maphash_verify

go_maphash_verify: go_maphash_verify.c
	$(CC) $(CFLAGS) $(AESFLAGS) -o $@ $< $(LDLIBS)

go_maphash_verify_portable: go_maphash_verify.c
	$(CC) $(CFLAGS) -DGO_PORTABLE -o $@ $< $(LDLIBS)

portable: go_maphash_verify_portable

check: go_maphash_verify
	@out=$$(./go_maphash_verify 20) || { printf '%s\n' "$$out" | tail -3; echo "FAIL: non-zero exit"; exit 1; }; \
	got=$$(printf '%s\n' "$$out" | grep -oE 'collisions = [0-9]+' | grep -oE '[0-9]+$$' | tr '\n' ' ' | sed 's/ $$//'); \
	if [ "$$got" = "$(EXPECT)" ]; then echo "ok    go-maphash: $$got"; else echo "FAIL  go-maphash: expected [$(EXPECT)], got [$$got]"; exit 1; fi

real: real_check vectors_go

real_check: real_check.go
	$(GO) build -ldflags=-checklinkname=0 -o $@ $<

vectors_go: vectors.go
	$(GO) build -ldflags=-checklinkname=0 -o $@ $<

clean:
	rm -f go_maphash_verify go_maphash_verify_portable real_check vectors_go
