# PolyXOR128: the checked statement

Source: `proofs/PolyXORCheck.lean` at
[orlp/polyxor@3123eb6](https://github.com/orlp/polyxor/blob/3123eb6c0ac95b879a46b490ecda1de0184b14bd/proofs/PolyXORCheck.lean).
The printed signature below is copied verbatim from `lake env lean` (see
[lean/AUDIT.txt](lean/AUDIT.txt)).

```
PolyXOR.polyxor128_axu : ∀ (F : Type) [inst : Field F] [inst_1 : Fintype F] [inst_2 : Module PolyXOR.F4 F],
  Fintype.card F = 2 ^ 128 →
    ∀ (ι : Polynomial (ZMod 2) →+ F),
      (∀ (x : Polynomial (ZMod 2)), x.degree < 128 → ι x = 0 → x = 0) →
        ∀ (L : ℕ) (m m' : List PolyXOR.Byte),
          m ≠ m' →
            m.length ≤ L →
              m'.length ≤ L →
                L < 2 ^ 64 →
                  ∀ (γ : F),
                    ↑(Nat.card { k // PolyXOR.polyxor128 ι k m + PolyXOR.polyxor128 ι k m' = γ }) /
                        ↑(Nat.card (PolyXOR.Key F)) ≤
                      (↑L / 4096 + 3) / 2 ^ 128
```

`Key F = (Fin 32 → Fin 16 → Word) × F × F × F`: the 4096-byte block key and z, u, y.
`polyxor128` (in `PolyXORFull.lean`) is `finalize_raw`:

- zero-pad to 128-byte blocks;
- XOR each block with its block-key bytes and apply `hashBlock` (two `compress_axu128`
  calls);
- sum each 4096-byte chunk's block digests into (h₀, h₁);
- chain acc ← (acc+u)(h₁+y)+h₀ starting from z;
- output (acc+u)(len+y), with len the 64-bit byte count.

The companion `PolyXOR.compress_axu128_is_axu` states that the keyed compression function is
2⁻¹²⁸-almost-XOR-universal over a uniform 64-byte key.

## Reading it in the post's model

- Fixed messages, uniform key, no adaptivity. γ = 0 is equality of the full 128-bit output.
- All lengths up to L < 2⁶⁴ bytes are covered, including the empty message, partial blocks
  and unequal lengths.
- With L = ⌈max bytes/8⌉ words, n = 8L bytes and ε(L) = (8L/4096 + 3)/2¹²⁸. The ratio
  L/ε(L) increases with L, so the score is attained at L = 1:
  128 − log₂(3 + 1/512) = **126.414099 bits**.
  - L = 2 gives 127.41 bits; L = 512 gives 135.0; long inputs tend to 137.
  - At 1 GiB, ε ≈ 2⁻¹¹⁰.
  - Domain: 1 ≤ L ≤ 2⁶¹ − 1.
- Tightness: for 00/01 the exact probability is 2⁻¹²⁸·(3 − 2⁻⁶⁴ − 2⁻¹²⁷ + 2⁻¹⁹²), i.e.
  126.415037 bits. The certified score is therefore exact to 0.001 bits.

The statement quantifies over every field of size 2¹²⁸, every F4-module structure and every
ι injective below degree 128. It therefore covers the instantiation the code uses, which
[difftest/model.py](difftest/model.py) spells out: POLYVAL's Montgomery product, the bit
layout, and w acting as (lo, hi) ↦ (hi, lo+hi) on 32-bit subwords.
