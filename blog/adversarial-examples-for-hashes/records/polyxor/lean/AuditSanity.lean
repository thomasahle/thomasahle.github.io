import PolyXORCheck

/-!
# Independent audit checks (not part of polyxor)

1. The probability denominators in both theorems are the true key-space sizes,
   not `0` (`Nat.card` of an infinite type is `0`, which would make the ratio
   `0` and the theorem vacuous).
2. The field hypotheses are satisfiable: `GaloisField 2 128` is a field of
   cardinality `2^128`.
3. The exact statements and their axioms, re-printed.
-/

namespace PolyXOR.Audit

open Polynomial

lemma card_word : Nat.card (Word : Type) = 2 ^ 64 := by
  rw [Nat.card_congr (Polynomial.degreeLTEquiv (ZMod 2) 64).toEquiv, Nat.card_fun,
    Nat.card_zmod, Nat.card_eq_fintype_card, Fintype.card_fin]

lemma card_block : Nat.card Block = 2 ^ 512 := by
  have h2 : Nat.card (Prod Word Word) = 2 ^ 128 := by
    rw [Nat.card_prod, card_word]; norm_num
  rw [Nat.card_fun, h2, Nat.card_eq_fintype_card, Fintype.card_fin, ← pow_mul]

/-- `#Key F = 2^(64·512) · (2^128)^3 = 2^33152`, the true key-space size. -/
lemma card_key (F : Type) [Field F] [Fintype F] (hF : Fintype.card F = 2 ^ 128) :
    Nat.card (Key F) = 2 ^ (64 * 512) * (2 ^ 128 * (2 ^ 128 * 2 ^ 128)) := by
  have hw : Nat.card (Fin 16 → (Word : Type)) = 2 ^ (64 * 16) := by
    rw [Nat.card_fun, card_word, Nat.card_eq_fintype_card, Fintype.card_fin, ← pow_mul]
  have hb : Nat.card (Fin 32 → Fin 16 → (Word : Type)) = 2 ^ (64 * 512) := by
    rw [Nat.card_fun, hw, Nat.card_eq_fintype_card, Fintype.card_fin, ← pow_mul]
  have hF' : Nat.card F = 2 ^ 128 := by rw [Nat.card_eq_fintype_card, hF]
  simp only [Key, Nat.card_prod, hb, hF']

lemma card_key_ne_zero (F : Type) [Field F] [Fintype F] (hF : Fintype.card F = 2 ^ 128) :
    Nat.card (Key F) ≠ 0 := by
  rw [card_key F hF]; positivity

/-- A concrete field with the theorem's cardinality exists. -/
lemma galois_card : Nat.card (GaloisField 2 128) = 2 ^ 128 :=
  GaloisField.card 2 128 (by norm_num)

end PolyXOR.Audit

#check @PolyXOR.polyxor128_axu
#check @PolyXOR.compress_axu128_is_axu
#print axioms PolyXOR.polyxor128_axu
#print axioms PolyXOR.compress_axu128_is_axu
#print axioms PolyXOR.Audit.card_key_ne_zero
#print axioms PolyXOR.Audit.card_block
#print axioms PolyXOR.Audit.galois_card
