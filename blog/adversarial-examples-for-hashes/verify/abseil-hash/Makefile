# abseil-hash/Makefile
#
#   make            build abseil_hash_verify (C11, no dependencies; what ../Makefile does too)
#   make check      ./abseil_hash_verify 20  (the 2^20 smoke run of ../README.md)
#   make native     fetch abseil-cpp at the pinned commit into native/abseil-cpp, build the
#                   real-library check program (scalar, and x86 AES-NI on x86-64), diff its
#                   vectors against the C program's, run the seed-protocol and pair modes
#   make swiss      build native/swiss_check against the real containers (CMake) and run it
#   make clean      remove binaries, build directories and the checkout
#
# Variables: CC, CXX, CFLAGS, CXXFLAGS, LOG2N (default 20 for the native pair run).
CC       ?= cc
CXX      ?= c++
CFLAGS   ?= -O2 -std=c11
CXXFLAGS ?= -O2 -std=c++17
LOG2N    ?= 20
ABSL_COMMIT := 73d2688300440c8af028eec865ee0dcd85e93025
ABSL_URL    := https://github.com/abseil/abseil-cpp.git
ABSL        := native/abseil-cpp
ABSL_SRC    := $(ABSL)/absl/hash/internal/hash.cc $(ABSL)/absl/hash/internal/city.cc
# sha256 of the two files that define the hash at the pinned commit
SHA_HASH_H  := 65f71f11a2726a570d94610b494cde0592ae0a40d67d25f05559da1443d8e288
SHA_HASH_CC := f2b6084bffd569da77eb2822176c5d71b31bddcdde961cd8fad9cfb12db8857e
UNAME_M := $(shell uname -m)

.PHONY: all check native swiss clean

all: abseil_hash_verify

abseil_hash_verify: abseil_hash_verify.c
	$(CC) $(CFLAGS) -o $@ $< -lm

check: abseil_hash_verify
	./abseil_hash_verify 20

$(ABSL)/absl/hash/internal/hash.cc:
	rm -rf $(ABSL); mkdir -p $(ABSL); cd $(ABSL) && git init -q && git remote add origin $(ABSL_URL) \
	  && git fetch -q --depth 1 origin $(ABSL_COMMIT) && git checkout -q FETCH_HEAD
	@cd $(ABSL) && test "$$(git rev-parse HEAD)" = "$(ABSL_COMMIT)" || { echo "wrong commit"; exit 1; }
	@echo "$(SHA_HASH_H)  $(ABSL)/absl/hash/internal/hash.h"  | (sha256sum -c - 2>/dev/null || shasum -a 256 -c -)
	@echo "$(SHA_HASH_CC)  $(ABSL)/absl/hash/internal/hash.cc" | (sha256sum -c - 2>/dev/null || shasum -a 256 -c -)

native/absl_check_scalar: native/absl_check.cc $(ABSL)/absl/hash/internal/hash.cc
	$(CXX) $(CXXFLAGS) -I$(ABSL) $< $(ABSL_SRC) -o $@

native/absl_check_aes: native/absl_check.cc $(ABSL)/absl/hash/internal/hash.cc
	$(CXX) $(CXXFLAGS) -msse4.2 -maes -I$(ABSL) $< $(ABSL_SRC) -o $@

ifneq ($(filter x86_64 amd64,$(UNAME_M)),)
NATIVE_BINS := native/absl_check_scalar native/absl_check_aes
else
NATIVE_BINS := native/absl_check_scalar
endif

# Both sides are built with the same flags, so they select the same LowLevelHash backend
# (default flags: scalar on x86-64, ARM crypto on arm64 macOS; -msse4.2 -maes: x86 AES-NI).
native: abseil_hash_verify $(NATIVE_BINS)
	@for b in $(NATIVE_BINS); do \
	  echo "== $$b vectors vs the C program's --vectors"; \
	  case $$b in *_aes) flags="-msse4.2 -maes";; *) flags="";; esac; \
	  $(CC) $(CFLAGS) $$flags -o abseil_hash_verify_tmp abseil_hash_verify.c -lm || exit 1; \
	  ./$$b vectors > native/vectors_real.tmp; ./abseil_hash_verify_tmp --vectors > native/vectors_c.tmp; \
	  if cmp -s native/vectors_real.tmp native/vectors_c.tmp; then echo "identical ($$(wc -l < native/vectors_real.tmp) vectors)"; else echo "DIFFER"; exit 1; fi; \
	  rm -f native/vectors_real.tmp native/vectors_c.tmp abseil_hash_verify_tmp; \
	  echo "== $$b seed"; ./$$b seed || exit 1; \
	  echo "== $$b pairs $(LOG2N)"; ./$$b pairs $(LOG2N) || exit 1; \
	done

swiss: $(ABSL)/absl/hash/internal/hash.cc
	cmake -S native -B native/build -DCMAKE_BUILD_TYPE=Release > native/build_cmake.log 2>&1 || { tail -20 native/build_cmake.log; exit 1; }
	cmake --build native/build --target swiss_check -j 4 >> native/build_cmake.log 2>&1 || { tail -20 native/build_cmake.log; exit 1; }
	./native/build/swiss_check

clean:
	rm -f abseil_hash_verify abseil_hash_verify_tmp native/absl_check_scalar native/absl_check_aes native/*.tmp native/build_cmake.log
	rm -rf native/build $(ABSL)
