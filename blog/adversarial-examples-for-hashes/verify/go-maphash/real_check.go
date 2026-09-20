// real_check.go -- the same pair on the real go1.27.1 runtime (not the C port).
//
// The per-process AES key internal/runtime/maps.aeskeysched and runtime.memhash (the function behind
// hash/maphash and map[string]) are reached through go:linkname, which is why the build needs
// -ldflags=-checklinkname=0.  Nothing else is patched: memhash reads the global on every call, so
// re-filling aeskeysched between samples is the same as starting a fresh process with that key.
//
// build: go build -ldflags=-checklinkname=0 -o real_check real_check.go
// usage: ./real_check show
//            print UseAeshash and the first 16 key bytes of this process (run twice: they differ)
//        ./real_check witness <aeskeysched, 256 hex chars> <seed, 16 hex chars> <hexA> <hexB>
//            install that key, hash both messages with maphash.Bytes, maphash.String and runtime.memhash
//            under that seed and under a second seed
//        ./real_check sample <hexA> <hexB> <log2N>
//            per sample: all 128 key bytes and the 64-bit seed fresh from a ChaCha8 stream seeded by
//            crypto/rand (the runtime's own generator family); count memhash(A) == memhash(B)
package main

import (
	crand "crypto/rand"
	"encoding/hex"
	"fmt"
	"hash/maphash"
	"math"
	"math/rand/v2"
	"os"
	"strconv"
	"unsafe"
)

//go:linkname aeskeysched internal/runtime/maps.aeskeysched
var aeskeysched [128]byte

//go:linkname useAeshash internal/runtime/maps.UseAeshash
var useAeshash bool

//go:linkname memhash runtime.memhash
//go:noescape
func memhash(p unsafe.Pointer, h, s uintptr) uintptr

func seedOf(s uint64) maphash.Seed { var sd maphash.Seed; *(*uint64)(unsafe.Pointer(&sd)) = s; return sd }

func mustHex(s string) []byte {
	b, err := hex.DecodeString(s)
	if err != nil {
		fmt.Fprintln(os.Stderr, "bad hex:", s)
		os.Exit(2)
	}
	return b
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: real_check show | witness <key256hex> <seed16hex> <hexA> <hexB> | sample <hexA> <hexB> <log2N>")
	os.Exit(2)
}

func main() {
	if len(os.Args) < 2 {
		usage()
	}
	if !useAeshash {
		fmt.Println("UseAeshash=false: this host does not take the AES path; nothing to check")
		os.Exit(2)
	}
	switch os.Args[1] {
	case "show":
		fmt.Printf("UseAeshash=%v aeskeysched[0:16]=%x\n", useAeshash, aeskeysched[:16])
	case "witness":
		if len(os.Args) != 6 {
			usage()
		}
		key := mustHex(os.Args[2])
		if len(key) != 128 {
			usage()
		}
		seed, err := strconv.ParseUint(os.Args[3], 16, 64)
		if err != nil || seed == 0 {
			usage()
		}
		a, b := mustHex(os.Args[4]), mustHex(os.Args[5])
		copy(aeskeysched[:], key)
		for _, s := range []uint64{seed, seed ^ 0x5555555555555555} {
			ha := uint64(memhash(unsafe.Pointer(&a[0]), uintptr(s), uintptr(len(a))))
			hb := uint64(memhash(unsafe.Pointer(&b[0]), uintptr(s), uintptr(len(b))))
			verdict := "DIFFER"
			if maphash.Bytes(seedOf(s), a) == maphash.Bytes(seedOf(s), b) && maphash.String(seedOf(s), string(a)) == maphash.String(seedOf(s), string(b)) && ha == hb {
				verdict = "COLLIDE"
			}
			fmt.Printf("real go1.27.1 runtime, key[8,10,12,14]=%02x%02x%02x%02x seed=%016x: maphash.Bytes(m)=%016x maphash.Bytes(m')=%016x maphash.String(m)=%016x maphash.String(m')=%016x runtime.memhash(m)=%016x runtime.memhash(m')=%016x -> %s\n",
				aeskeysched[8], aeskeysched[10], aeskeysched[12], aeskeysched[14], s,
				maphash.Bytes(seedOf(s), a), maphash.Bytes(seedOf(s), b),
				maphash.String(seedOf(s), string(a)), maphash.String(seedOf(s), string(b)), ha, hb, verdict)
		}
	case "sample":
		if len(os.Args) != 5 {
			usage()
		}
		a, b := mustHex(os.Args[2]), mustHex(os.Args[3])
		lg, err := strconv.Atoi(os.Args[4])
		if err != nil || lg < 0 || lg > 40 {
			usage()
		}
		var ks [32]byte
		crand.Read(ks[:])
		rng := rand.NewChaCha8(ks)
		N := uint64(1) << uint(lg)
		var coll uint64
		shown := 0
		for i := uint64(0); i < N; i++ {
			rng.Read(aeskeysched[:])
			seed := rng.Uint64()
			if seed == 0 {
				seed = 1
			}
			ha := memhash(unsafe.Pointer(&a[0]), uintptr(seed), uintptr(len(a)))
			hb := memhash(unsafe.Pointer(&b[0]), uintptr(seed), uintptr(len(b)))
			if ha == hb {
				coll++
				if shown < 2 {
					shown++
					s2 := rng.Uint64() | 1
					fmt.Printf("collision: aeskeysched[0:16]=%x (bytes 8,10,12,14 = %02x %02x %02x %02x) seed=%016x  maphash.Bytes: m=%016x m'=%016x | another seed %016x: m=%016x m'=%016x\n",
						aeskeysched[:16], aeskeysched[8], aeskeysched[10], aeskeysched[12], aeskeysched[14], seed,
						maphash.Bytes(seedOf(seed), a), maphash.Bytes(seedOf(seed), b), s2,
						maphash.Bytes(seedOf(s2), a), maphash.Bytes(seedOf(s2), b))
				}
			}
		}
		fmt.Printf("real go1.27.1 runtime memhash, fresh 128-byte key + fresh seed per sample: collisions = %d / %d (2^%d); rate = 2^%.3f\n",
			coll, N, lg, math.Log2(float64(coll)/float64(N)))
	default:
		usage()
	}
}
