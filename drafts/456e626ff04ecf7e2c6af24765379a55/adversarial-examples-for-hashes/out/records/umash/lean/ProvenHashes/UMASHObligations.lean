import ProvenHashes.UMASHPolynomial
import ProvenHashes.UMASHConstants

/-! Named obligations, deliberately DEFINITIONS OF PROPOSITIONS. No declaration
in this module postulates any case bound.  Bounds below are for the literal
word model, with exact finite uniform probabilities. -/
namespace ProvenHashes.UMASH

def primaryEvent (seed : Word) (x y : Block) (k : OHKey) : Prop :=
  project (oh k x seed) = project (oh k y seed)
def jointEvent (seed : Word) (x y : Block) (k : OHKey) : Prop :=
  primaryEvent seed x y k ∧ project (ohSecondary k x seed) = project (ohSecondary k y seed)

def sameCount (x y : Block) : Prop := x.chunks.length = y.chunks.length
def dataChecksum (b : Block) : Chunk := b.chunks.foldl xorChunk (0,0)
def lastChunk (b : Block) : Chunk := b.chunks.getLastD (0,0)
def phDiffCount (x y : Block) : ℕ :=
  ((x.chunks.dropLast.zip y.chunks.dropLast).filter fun xy => xy.1 != xy.2).length
def enhChanges (x y : Block) : ℕ :=
  (if (lastChunk x).1 = (lastChunk y).1 then 0 else 1) +
  (if (lastChunk x).2 = (lastChunk y).2 then 0 else 1)
def wordValuation (x y : Word) : ℕ := padicValNat 2 (x ^^^ y).toNat
def enhValuation (x y : Block) : ℕ :=
  min (wordValuation (lastChunk x).1 (lastChunk y).1)
      (wordValuation (lastChunk x).2 (lastChunk y).2)
def oddPH (x y : Block) : Prop := ∃ i, i+1 < x.chunks.length ∧
  (((x.chunks.getD i (0,0)).1 ^^^ (y.chunks.getD i (0,0)).1).toNat % 2 = 1 ∨
   ((x.chunks.getD i (0,0)).2 ^^^ (y.chunks.getD i (0,0)).2).toNat % 2 = 1)

def PrimaryPHBound : Prop := ∀ (seed : Word) (x y : Block),
  x.Valid → y.Valid → sameCount x y → 0 < phDiffCount x y →
  uniformProb (primaryEvent seed x y) ≤ (852:ℚ≥0)^2/q

def DifferentChunkCountsBound : Prop := ∀ (seed : Word) (x y : Block),
  x.Valid → y.Valid → ¬sameCount x y →
  uniformProb (primaryEvent seed x y) < (162:ℚ≥0)/q

def OddPHBound : Prop := ∀ (seed : Word) (x y : Block),
  x.Valid → y.Valid → sameCount x y → oddPH x y →
  uniformProb (primaryEvent seed x y) ≤ (17:ℚ≥0)/q

def SubcaseBBound : Prop := ∀ (seed : Word) (x y : Block),
  x.Valid → y.Valid → sameCount x y → dataChecksum x = dataChecksum y →
  2 ≤ phDiffCount x y →
  uniformProb (jointEvent seed x y) ≤ (345763417:ℚ≥0)/2^116

def DifferentChecksumsBound : Prop := ∀ (seed : Word) (x y : Block),
  x.Valid → y.Valid → sameCount x y → dataChecksum x ≠ dataChecksum y →
  0 < phDiffCount x y →
  uniformProb (jointEvent seed x y) ≤ (852:ℚ≥0)^4/q^2

def OneWordENHBound : Prop := ∀ (seed : Word) (x y : Block),
  x.Valid → y.Valid → sameCount x y → phDiffCount x y = 0 → enhChanges x y = 1 →
  uniformProb (jointEvent seed x y) < (1:ℚ≥0)/2^103

def PHOneWordENHBound : Prop := ∀ (seed : Word) (x y : Block),
  x.Valid → y.Valid → sameCount x y → dataChecksum x = dataChecksum y →
  phDiffCount x y = 1 → enhChanges x y = 1 →
  uniformProb (jointEvent seed x y) < (1:ℚ≥0)/2^91

def TagOnlyBound : Prop := ∀ (seed : Word) (x y : Block),
  x.Valid → y.Valid → x.chunks = y.chunks → blockTag seed x ≠ blockTag seed y →
  uniformProb (jointEvent seed x y) < (1:ℚ≥0)/2^92

def ClosedTwoWordENHBound : Prop := ∀ (seed : Word) (x y : Block),
  x.Valid → y.Valid → sameCount x y → phDiffCount x y = 0 → enhChanges x y = 2 →
  enhValuation x y ≤ 31 → uniformProb (jointEvent seed x y) < (1:ℚ≥0)/2^87

def ClosedPHTwoWordENHBound : Prop := ∀ (seed : Word) (x y : Block),
  x.Valid → y.Valid → sameCount x y → dataChecksum x = dataChecksum y →
  phDiffCount x y = 1 → enhChanges x y = 2 →
  (enhValuation x y = 0 ∨ (4 ≤ enhValuation x y ∧ enhValuation x y ≤ 35)) →
  uniformProb (jointEvent seed x y) < (1:ℚ≥0)/2^87

/-- Open on paper: high-valuation two-word ENH-only changes. -/
def OpenENHOnly : Prop := ∀ (seed : Word) (x y : Block),
  x.Valid → y.Valid → sameCount x y → phDiffCount x y = 0 → enhChanges x y = 2 →
  32 ≤ enhValuation x y → enhValuation x y ≤ 63 →
  uniformProb (jointEvent seed x y) < (1:ℚ≥0)/2^87

/-- Open on paper: exactly one PH change, equal checksums, two ENH words change. -/
def OpenPHENH : Prop := ∀ (seed : Word) (x y : Block),
  x.Valid → y.Valid → sameCount x y → dataChecksum x = dataChecksum y →
  phDiffCount x y = 1 → enhChanges x y = 2 →
  (enhValuation x y = 1 ∨ enhValuation x y = 2 ∨ enhValuation x y = 3 ∨
    (36 ≤ enhValuation x y ∧ enhValuation x y ≤ 63)) →
  uniformProb (jointEvent seed x y) < (1:ℚ≥0)/2^87

noncomputable def comparisonPolynomial (k : OHKey) (seed : Word) (m : Message) :
    Polynomial Field :=
  if m.length ≤ 8 then Polynomial.C ((unfinalize (shortHash k seed m false)).toNat : Field)
  else blockPolynomial (compress false k seed m)

def DifferentBlockCountsBound : Prop := ∀ (seed : Word) (x y : Message),
  8 < x.length → 8 < y.length → (encode x).length ≠ (encode y).length →
  uniformProb (fun k : OHKey => comparisonPolynomial k seed x = comparisonPolynomial k seed y) <
    (162:ℚ≥0)/q

def WeakENHBlockBound : Prop := ∀ (seed : Word) (x y : Block),
  x.Valid → y.Valid → sameCount x y → phDiffCount x y = 0 → lastChunk x ≠ lastChunk y →
  uniformProb (primaryEvent seed x y) ≤ (718333281557:ℚ≥0)/q

/-- Includes short/long and unequal-length comparisons.  This is a primary
marginal obligation, distinct from the open JOINT propositions above. -/
def PrimaryIdentityBound (B : ℚ≥0) : Prop := ∀ (seed : Word) (x y : Message),
  x ≠ y → uniformProb (fun k : DistinctOHKey =>
    comparisonPolynomial k.val seed x = comparisonPolynomial k.val seed y) ≤ B

def SharpPrimaryProjection : Prop := PrimaryIdentityBound ((162:ℚ≥0)/(q-561 : ℕ))
def WeakPrimaryProjection : Prop := PrimaryIdentityBound weakA

def CertifiedAllPairs64 : Prop := ∀ (L : ℕ) (seed : Word) (x y : Message),
  1 ≤ L → x.length ≤ 8*L → y.length ≤ 8*L → x ≠ y →
  uniformProb (fun k : Key64 => hash64 k seed x = hash64 k seed y) ≤ certifiedEnvelope L

def CertifiedAllPairs128 : Prop := ∀ (L : ℕ) (seed : Word) (x y : Message),
  1 ≤ L → x.length ≤ 8*L → y.length ≤ 8*L → x ≠ y →
  uniformProb (fun k : Key128 => hash128 k seed x = hash128 k seed y) ≤ certifiedEnvelope L

def Published64 : Prop := ∀ (L : ℕ) (seed : Word) (x y : Message),
  1 ≤ L → x.length ≤ 8*L → y.length ≤ 8*L → x ≠ y →
  uniformProb (fun k : Key64 => hash64 k seed x = hash64 k seed y) ≤
    ((L+511)/512 : ℕ) / (2:ℚ≥0)^55

def Published128 : Prop := ∀ (L : ℕ) (seed : Word) (x y : Message),
  1 ≤ L → x.length ≤ 8*L → y.length ≤ 8*L → x ≠ y →
  uniformProb (fun k : Key128 => hash128 k seed x = hash128 k seed y) ≤
    (((L+2^23-1)/2^23 : ℕ) : ℚ≥0)^2 / (2:ℚ≥0)^83

def PHXorSliceInjective : Prop := ∀ (u v delta : Word), u ≠ v →
  Function.Injective (fun k : Word => clmul u k ^^^ clmul v (k ^^^ delta))

def RankLemma64 : Prop := ∀ (d : Word), d ≠ 0 → ∀ (h : ℕ), h ≤ 64 → ∀ (z : Chunk),
  (Finset.univ.filter (fun k : Word => laneShift (split (clmul d k)) h = z)).card ≤ 2^h

def DistinctKeyAcceptance : Prop :=
  ((q-561 : ℕ) : ℚ≥0)/q ≤ uniformProb (fun k : OHKey => Function.Injective k)

def EncodingInjectiveLong : Prop := ∀ (x y : Message),
  8 < x.length → 8 < y.length → encode x = encode y → x = y

def EncodedBlocksValid : Prop := ∀ (x : Message),
  8 < x.length → ∀ b ∈ encode x, b.Valid

def RequestedConditional55 : Prop := OpenENHOnly → OpenPHENH → Published64
def Conditional55WithPrimaryPremise : Prop := SharpPrimaryProjection → Published64

/-- Logical inheritance is unconditional; its primary premise is explicitly
the still-unproved all-pairs certification, not an imported paper claim. -/
theorem certifiedAllPairs128_of_64 (h : CertifiedAllPairs64) : CertifiedAllPairs128 := by
  intro L seed x y hL hx hy hxy
  exact (fingerprint_collision_le_primary seed x y).trans (h L seed x y hL hx hy hxy)
-- CHECKPOINT

end ProvenHashes.UMASH
