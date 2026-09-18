# Go maphash: amd64 AES fixed pair

This package preserves the independent transcription of go1.27.1's
[amd64 runtime assembly](https://github.com/golang/go/blob/go1.27.1/src/internal/runtime/maps/memhash_amd64.s),
a real-runtime checker, and the original measurement log. The result covers
string map keys and `maphash.Bytes/String`, not integer map keys or other processor paths.
Byte slices are accepted by `maphash.Bytes`; they are not legal Go map keys.

The messages are 15 zero bytes (`000000000000000000000000000000`) and the 16 bytes
`a3fe5da3a3fe5da342a3bcfe42a3bcfe`. Their exact sufficient trail has probability
(4/256)^4 = 2^-24 over the process AES key, independent of the map seed. With
L = 2 this gives a score cap of 25 bits. The independent sample is 66/2^30
(95% Poisson score interval [24.61, 25.33]); the real-runtime sample is 20/2^28.

## Independent check

On an x86-64 machine with AES-NI and GCC/OpenMP:

```sh
gcc -O2 -maes -msse4.1 -mssse3 -fopenmp -o gomh gomh.c -lm
./gomh vectors v_random_aes.txt
./gomh vectors v_fixed_aes.txt
./gomh delta 15
./gomh measure 15 30 8
./gomh seedfree 15 24
```

The vector files contain real-runtime outputs; the recorded check has 2,883/2,883
matches. `delta` derives the pair from the AES substitution difference table.
`measure` redraws the 128-byte process key and a 64-bit map seed for each trial.
`seedfree` finds a qualifying process key and checks additional map seeds.
The generator is xoshiro256**, initialized from `/dev/urandom`.
A short sample can have zero hits at this rare rate and does not establish safety.

## Real-runtime check

With the audited Go 1.27.1 toolchain on amd64 AES hardware:

```sh
go build -ldflags=-checklinkname=0 -o pair_real pair_real.go
./pair_real 000000000000000000000000000000 a3fe5da3a3fe5da342a3bcfe42a3bcfe 28
```

The checker redraws the runtime's global process AES key in a single test process,
simulating fresh process starts. See [verify.log](verify.log) for recorded output.
This integration pass packages those audited results; it does not claim to have
repeated the billion-trial measurement. The ARM64 observation is qemu-only and
is not scored as an ARM hardware result.
