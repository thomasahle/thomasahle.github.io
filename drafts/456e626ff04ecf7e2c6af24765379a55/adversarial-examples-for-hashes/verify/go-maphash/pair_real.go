// pair_real.go -- independent confirmation against the REAL Go 1.27.1 runtime: re-randomise the per-process AES key
// (internal/runtime/maps.aeskeysched, all 128 bytes) and the 64-bit seed for every sample, hash both messages with
// runtime.memhash (the function hash/maphash and map[string] use), count equal outputs.
// build: go build -ldflags=-checklinkname=0 -o pair_real pair_real.go
// usage: ./pair_real show                      print the process key bytes (run twice: per-process randomness)
//        ./pair_real <hexA> <hexB> <log2N>     sample
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

func main() {
	if os.Args[1] == "show" {
		fmt.Printf("UseAeshash=%v aeskeysched[0:16]=%x\n", useAeshash, aeskeysched[:16])
		return
	}
	a, _ := hex.DecodeString(os.Args[1])
	b, _ := hex.DecodeString(os.Args[2])
	lg, _ := strconv.Atoi(os.Args[3])
	if !useAeshash { fmt.Println("UseAeshash=false: AES path not active"); os.Exit(2) }
	var ks [32]byte
	crand.Read(ks[:])
	rng := rand.NewChaCha8(ks)
	N := uint64(1) << uint(lg)
	var coll uint64
	shown := 0
	for i := uint64(0); i < N; i++ {
		rng.Read(aeskeysched[:])
		seed := rng.Uint64()
		if seed == 0 { seed = 1 }
		ha := memhash(unsafe.Pointer(&a[0]), uintptr(seed), uintptr(len(a)))
		hb := memhash(unsafe.Pointer(&b[0]), uintptr(seed), uintptr(len(b)))
		if ha == hb {
			coll++
			if shown < 2 {
				shown++
				s2 := rng.Uint64() | 1
				fmt.Printf("collision: key[8,10,12,14]=%02x%02x%02x%02x seed=%016x  maphash.Bytes: a=%016x b=%016x | other seed %016x: a=%016x b=%016x\n",
					aeskeysched[8], aeskeysched[10], aeskeysched[12], aeskeysched[14], seed,
					maphash.Bytes(seedOf(seed), a), maphash.Bytes(seedOf(seed), b), s2,
					maphash.Bytes(seedOf(s2), a), maphash.Bytes(seedOf(s2), b))
			}
		}
	}
	fmt.Printf("real runtime go1.27.1 memhash, fresh key+seed per sample: collisions=%d N=2^%d rate=2^%.3f\n", coll, lg, math.Log2(float64(coll)/float64(N)))
}
