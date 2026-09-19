# verify/Makefile -- build the 24 reproduction programs and check their output.
#
#   make             build every directory's program in place (same as `make all`)
#   make check       run every program with 2^20 seeds and compare the collision counts it
#                    prints with the "make check reference" table in README.md
#   make clean       remove the binaries
#
# Variables: CC, CFLAGS, LDLIBS as usual.  DIRS=<subset> restricts all/check/clean to those
# directories (e.g. `make check DIRS=komihash`).  CHECK_TOL=<n> accepts |got - expected| <= n
# for every count; the default 0 is exact, because every program seeds its RNG from a fixed
# constant (or from argv), so the counts are deterministic.
#
# Needs GNU make (3.81 or later), a C11 compiler, a POSIX shell, awk, grep -E and sed -E.

CC        ?= cc
CFLAGS    ?= -O2 -std=c11
LDLIBS    ?= -lm
CHECK_TOL ?= 0

DIRS ?= a5hash cityhash-64 farmhash-64 gxhash-64 highwayhash komihash murmurhash3-128 museair museair-v2 rust-ahash spookyhash2-64 t1ha2-64 pengyhash nmhash32 nmhash32x mx3 mir fasthash mum rapidhash-v3 wyhash rapidhash-v1 xxh3-64 xxh3-128 dotnet-marvin abseil-hash go-maphash

# Program name per directory; the source is <name>.c in the same directory.
PROG_a5hash          := a5hash_verify
PROG_cityhash-64     := cityhash64_verify
PROG_farmhash-64     := farmhash64_pairs
PROG_gxhash-64       := gxhash64_verify
PROG_highwayhash     := highwayhash_verify
PROG_komihash        := komihash_pair
PROG_murmurhash3-128 := murmurhash3_128_verify
PROG_museair         := museair_verify
PROG_rust-ahash      := ahash_pairs
PROG_spookyhash2-64  := spookyhash2_64_pair
PROG_t1ha2-64        := t1ha2_64_verify

PROG_pengyhash := pengyhash_verify
PROG_nmhash32 := nmhash32_verify
PROG_nmhash32x := nmhash32x_verify
PROG_mx3 := mx3_verify
PROG_mir := mir_verify
PROG_fasthash := fasthash_verify
PROG_mum := mum_verify
PROG_rapidhash-v3 := rapidhash_v3_verify

PROG_wyhash := wyhash_verify
PROG_rapidhash-v1 := rapidhash_v1_verify
PROG_xxh3-64 := xxh3_64_pair_check
PROG_museair-v2 := museair_v2_verify
EXTRA_museair-v2 := -pthread
PROG_xxh3-128 := xxh3_128_verify
PROG_go-maphash := go_maphash_verify
PROG_dotnet-marvin := marvin32_verify
PROG_abseil-hash := abseil_hash_verify

# gxhash, aHash and the Go map hash have a hardware-AES path that the source enables when the compiler
# advertises the target feature.  All three programs also build and validate without these flags
# (portable table AES, slower); the first line of their output names the backend in use.
UNAME_M := $(shell uname -m)
ifneq ($(filter arm64 aarch64,$(UNAME_M)),)
AESFLAGS := -march=armv8-a+crypto
else
ifneq ($(filter x86_64 amd64,$(UNAME_M)),)
AESFLAGS := -maes
else
AESFLAGS :=
endif
endif
EXTRA_gxhash-64  := $(AESFLAGS)
EXTRA_rust-ahash := $(AESFLAGS)
EXTRA_go-maphash := $(AESFLAGS)

BINS := $(foreach d,$(DIRS),$(d)/$(PROG_$(d)))

.PHONY: all check clean

all: $(BINS)

define build-rule
$(1)/$(PROG_$(1)): $(1)/$(PROG_$(1)).c
	$$(CC) $$(CFLAGS) $$(EXTRA_$(1)) -o $$@ $$< $$(LDLIBS)
endef
$(foreach d,$(DIRS),$(eval $(call build-rule,$(d))))

# For each directory: read its row of the README table "make check reference" (column 3 is
# the command, which must start with `./`, and column 4 the expected counts; the index
# table above it has no such row), run the command in that directory, pull the
# collision counts out of the output with the directory's pattern below (every match ends
# in the count; a trailing word such as " collisions" is stripped first), and compare the
# two lists element by element.  A non-zero exit status of the program is a failure too.
check: all
	@fail=0; \
	for d in $(DIRS); do \
	  case $$d in \
	    a5hash)          key='collisions = [0-9]+|conditional rate: [0-9]+' ;; \
	    cityhash-64)     key='collisions = [0-9]+' ;; \
	    farmhash-64)     key='collisions = [0-9]+' ;; \
	    gxhash-64)       key='collisions = [0-9]+' ;; \
	    highwayhash)     key='HighwayHash64 collisions: [0-9]+|E_2 hits: [0-9]+|full state: [0-9]+' ;; \
	    komihash)        key='collisions: [0-9]+|\(predicted [^)]*\): [0-9]+' ;; \
	    murmurhash3-128) key='collisions/N = [0-9]+|agree: [0-9]+' ;; \
	    museair)         key='collisions [0-9]+' ;; \
	    museair-v2)      key='(hash|bfast::hash|hash128|bfast::hash128) +[0-9]+' ;; \
	    xxh3-64)        key='collisions=[0-9]+' ;; \
	    rust-ahash)      key='collisions/N = [0-9]+' ;; \
	    spookyhash2-64)  key='32-bit collisions [0-9]+|64-bit collisions [0-9]+|128-bit collisions [0-9]+|right for [0-9]+' ;; \
	    t1ha2-64)        key='[0-9]+ collisions' ;; \
	    go-maphash)      key='collisions = [0-9]+' ;; \
	    pengyhash|nmhash32|nmhash32x|mx3|mir|fasthash|mum|rapidhash-v3|wyhash|rapidhash-v1|xxh3-64|xxh3-128|dotnet-marvin|abseil-hash) key='collisions = [0-9]+' ;; \
	    *) echo "FAIL  $$d: no count pattern in the Makefile"; fail=1; continue ;; \
	  esac; \
	  row=$$(awk -F'|' -v d="$$d" '$$2 ~ ("^ *`" d "` *$$") && $$3 ~ "^ *`[.]/" { print; exit }' README.md); \
	  if [ -z "$$row" ]; then echo "FAIL  $$d: no row in the README check table"; fail=1; continue; fi; \
	  run=$$(printf '%s\n' "$$row" | awk -F'|' '{ gsub(/`/, "", $$3); gsub(/^ +| +$$/, "", $$3); print $$3 }'); \
	  want=$$(printf '%s\n' "$$row" | awk -F'|' '{ gsub(/`/, "", $$4); gsub(/^ +| +$$/, "", $$4); print $$4 }'); \
	  out=$$(cd $$d && $$run 2>&1); rc=$$?; \
	  got=$$(printf '%s\n' "$$out" | grep -oE "$$key" | sed -E 's/[^0-9]*$$//' | grep -oE '[0-9]+$$' | tr '\n' ' ' | sed 's/ $$//'); \
	  if [ $$rc -ne 0 ]; then \
	    echo "FAIL  $$d: '$$run' exited with status $$rc"; printf '%s\n' "$$out" | tail -3; fail=1; \
	  elif printf '%s\n%s\n' "$$want" "$$got" | awk -v tol='$(CHECK_TOL)' \
	       'NR == 1 { nw = split($$0, w, " ") } NR == 2 { ng = split($$0, g, " ") } \
	        END { if (nw != ng || nw == 0) exit 1; for (i = 1; i <= nw; i++) { x = g[i] - w[i]; if (x < 0) x = -x; if (x > tol) exit 1 } }'; then \
	    echo "ok    $$d: $$got"; \
	  else \
	    echo "FAIL  $$d: expected [$$want], got [$$got] (tolerance $(CHECK_TOL))"; fail=1; \
	  fi; \
	done; \
	if [ $$fail -eq 0 ]; then echo "make check: every program reproduces the README counts"; else echo "make check: FAILED"; fi; \
	exit $$fail

clean:
	rm -f $(BINS) gxhash-64/gxhash64_verify_portable

museair-v2/museair_v2_verify: museair-v2/measure.c museair-v2/museair2.h
