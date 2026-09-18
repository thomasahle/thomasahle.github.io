// vectors.go -- dump Go runtime map-hash test vectors with the process key.
// build: go build -ldflags=-checklinkname=0 -o vectors_go vectors.go
// usage: ./vectors_go [random|fixed] [aes|fallback]
package main

import (
	"hash/maphash"
	"os"
	"runtime"
	"strconv"
	"unsafe"
)

//go:linkname aeskeysched internal/runtime/maps.aeskeysched
var aeskeysched [128]byte

//go:linkname hashkey internal/runtime/maps.hashkey
var hashkey [4]uintptr

//go:linkname useAeshash internal/runtime/maps.UseAeshash
var useAeshash bool

//go:linkname memhash runtime.memhash
//go:noescape
func memhash(p unsafe.Pointer, h, s uintptr) uintptr

//go:linkname memhash64 runtime.memhash64
//go:noescape
func memhash64(p unsafe.Pointer, h uintptr) uintptr

//go:linkname memhash32 runtime.memhash32
//go:noescape
func memhash32(p unsafe.Pointer, h uintptr) uintptr

var smState uint64

func splitmix() uint64 {
	smState += 0x9E3779B97F4A7C15
	z := smState
	z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9
	z = (z ^ (z >> 27)) * 0x94D049BB133111EB
	return z ^ (z >> 31)
}

var out []byte

func emit(tag string, a uint64, seed uint64, h uint64) {
	out = append(out, tag...)
	out = append(out, ' ')
	out = strconv.AppendUint(out, a, 10)
	out = append(out, ' ')
	out = append(out, hex16(seed)...)
	out = append(out, ' ')
	out = append(out, hex16(h)...)
	out = append(out, '\n')
}
func hex16(v uint64) string {
	s := strconv.FormatUint(v, 16)
	for len(s) < 16 {
		s = "0" + s
	}
	return s
}
func mkseed(s uint64) maphash.Seed {
	var sd maphash.Seed
	*(*uint64)(unsafe.Pointer(&sd)) = s
	return sd
}

func main() {
	mode, path := "random", "aes"
	if len(os.Args) > 1 {
		mode = os.Args[1]
	}
	if len(os.Args) > 2 {
		path = os.Args[2]
	}
	if mode == "fixed" {
		smState = 0xABCDEF
		for i := 0; i < 128; i += 8 {
			v := splitmix()
			for j := 0; j < 8; j++ {
				aeskeysched[i+j] = byte(v >> (8 * j))
			}
		}
		for i := range hashkey {
			hashkey[i] = uintptr(splitmix())
		}
	}
	if path == "fallback" {
		useAeshash = false
	}
	out = append(out, "KS "...)
	for _, b := range aeskeysched {
		out = append(out, "0123456789abcdef"[b>>4], "0123456789abcdef"[b&15])
	}
	out = append(out, "\nHK "...)
	for i := range hashkey {
		out = append(out, hex16(uint64(hashkey[i]))...)
		out = append(out, ' ')
	}
	out = append(out, "\nARCH "+runtime.GOARCH+"\nAES "...)
	if useAeshash {
		out = append(out, '1')
	} else {
		out = append(out, '0')
	}
	out = append(out, "\nGOVERSION "+runtime.Version()+"\n"...)

	data := make([]byte, 8192+4096)
	smState = 12345
	for i := 0; i < 8192; i += 8 {
		v := splitmix()
		for j := 0; j < 8; j++ {
			data[i+j] = byte(v >> (8 * j))
		}
	}
	lens := []uint64{}
	for l := uint64(0); l <= 300; l++ {
		lens = append(lens, l)
	}
	lens = append(lens, 511, 512, 513, 1000, 1023, 1024, 1025, 4096, 4097, 8000, 8192)
	smState = 999
	seeds := []uint64{0, 1, 0x123456789abcdef0, splitmix(), splitmix()}
	for _, l := range lens {
		for _, s := range seeds {
			emit("M", l, s, uint64(memhash(unsafe.Pointer(&data[0]), uintptr(s), uintptr(l))))
			if s != 0 {
				emit("B", l, s, maphash.Bytes(mkseed(s), data[:l]))
				// streaming: byte-at-a-time writes must equal Bytes
				var hh maphash.Hash
				hh.SetSeed(mkseed(s))
				for i := uint64(0); i < l; i++ {
					hh.WriteByte(data[i])
				}
				emit("S", l, s, hh.Sum64())
			}
		}
	}
	// page-boundary loads (amd64 endofpage branch): place data[4096-l:4096] so that it ends exactly at a page end
	pg := make([]byte, 3*4096)
	off := (4096 - uintptr(unsafe.Pointer(&pg[0]))%4096) % 4096
	for l := uint64(1); l <= 15; l++ {
		copy(pg[off+4096-uintptr(l):off+4096], data[4096-l:4096])
		for _, s := range seeds {
			emit("E", l, s, uint64(memhash(unsafe.Pointer(&pg[off+4096-uintptr(l)]), uintptr(s), uintptr(l))))
		}
	}
	smState = 777
	for i := 0; i < 50; i++ {
		k := splitmix()
		for _, s := range seeds {
			emit("H64", k, s, uint64(memhash64(unsafe.Pointer(&k), uintptr(s))))
			k32 := uint32(k)
			emit("H32", uint64(k32), s, uint64(memhash32(unsafe.Pointer(&k32), uintptr(s))))
			if s != 0 {
				emit("C64", k, s, maphash.Comparable(mkseed(s), k))
				emit("C32", uint64(k32), s, maphash.Comparable(mkseed(s), k32))
			}
		}
	}
	os.Stdout.Write(out)
}
