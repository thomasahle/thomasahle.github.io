import ProvenHashes.CLHashBytes
import ProvenHashes.CLHashModulus
import ProvenHashes.ConcreteWords

noncomputable section
namespace ProvenHashes.CLHash
open ChainHash Polynomial
open scoped BigOperators

/-- 128 CLNH words, a 126-bit polynomial key, two finalizer words,
and one independent length word. All coordinates are uniformly sampled. -/
abbrev IdealKey := BlockKey × (Word 126 × ((Word 64 × Word 64) × Word 64))

/-- Algorithm 4's lazy modulus is X times its irreducible modulus. -/
def lazyModulus : BitsPolynomial := modulus127 * X

/-- Horner's polynomial, written as a sum in decreasing coefficient order.
One final remainder equals reducing after every multiplication and addition. -/
def aggregate {n : ℕ} (a : Fin n → BitsPolynomial) (k : Word 126) : BitsPolynomial :=
  (∑ i, a i * (pack 126 k) ^ (n - 1 - i.val)) %ₘ lazyModulus

def longAccumulator (k : IdealKey) (m : Message) : BitsPolynomial :=
  aggregate (fun i : Fin (blockCount m) => rawBlock m k.1 i.val) k.2.1

/-- The final two-word unreduced CLNH product. -/
def finalProduct (p : BitsPolynomial) (k : Word 64 × Word 64) : BitsPolynomial :=
  pack 64 (lowWord p + k.1) * pack 64 (highWord p + k.2)

/-- Literal ideal-key Algorithm 4, without the optional invertible bit mixer. -/
def hash (k : IdealKey) (m : Message) : Word 64 :=
  let core := if m.length ≤ 1024 then rawBlock m k.1 0
    else finalProduct (longAccumulator k m) k.2.2.1
  lowWord ((core + pack 64 k.2.2.2 * pack 64 (lengthWord m.length)) %ₘ modulus)

/-- L is a common upper bound measured in 64-bit words, rounded up. -/
def epsilon (L : ℕ) : ℚ≥0 :=
  if L ≤ 128 then 1 / (2 : ℚ≥0) ^ 64
  else 2 / (2 : ℚ≥0) ^ 64 + (((L + 127) / 128 - 1 : ℕ) : ℚ≥0) / (2 : ℚ≥0) ^ 126

/-- Requested end-to-end specification, proved by `collision_bound`. -/
def CollisionBound : Prop :=
  ∀ (L : ℕ) (m m' : Message),
    m.length < 2 ^ 64 → m'.length < 2 ^ 64 →
    m.length ≤ 8 * L → m'.length ≤ 8 * L → m ≠ m' →
    uniformProb (fun k : IdealKey => hash k m = hash k m') ≤ epsilon L

/-- Stronger bound for different lengths, proved by `unequal_lengths_bound`. -/
def UnequalLengthsBound : Prop :=
  ∀ (m m' : Message), m.length < 2 ^ 64 → m'.length < 2 ^ 64 →
    m.length ≠ m'.length →
    uniformProb (fun k : IdealKey => hash k m = hash k m') ≤ 1 / (2 : ℚ≥0) ^ 64

/-- Restricted 126-bit-key root bound, before the final reduction. -/
def AggregateCollisionBound : Prop :=
  ∀ (n : ℕ) (a b : Fin n → BitsPolynomial),
    (∀ i, (a i).natDegree < 127) → (∀ i, (b i).natDegree < 127) → a ≠ b →
    uniformProb (fun k : Word 126 => aggregate a k = aggregate b k) ≤
      ((n - 1 : ℕ) : ℚ≥0) / (2 : ℚ≥0) ^ 126

end ProvenHashes.CLHash
