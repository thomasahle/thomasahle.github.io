import ProvenHashes.UMASHModel

/-! Exact arithmetic for the bounds in the supplied lane files. These theorems
certify the constants, not the probabilistic premises of those files. -/
namespace ProvenHashes.UMASH

def weakA : ℚ≥0 := (718333281557 : ℚ≥0) / (q-561 : ℕ)
def weakComplement : ℚ≥0 := ((q-561-718333281557 : ℕ) : ℚ≥0) / (q-561 : ℕ)
def rootRate (L : ℕ) : ℚ≥0 := min 1 (2 * (((L+31)/32 : ℕ) : ℚ≥0) / (p-2 : ℕ))
def certifiedEnvelope (L : ℕ) : ℚ≥0 :=
  if L = 1 then (1 : ℚ≥0) / (q-561 : ℕ) else weakA + weakComplement*rootRate L
def certifiedSlope : ℚ≥0 :=
  1656363775600634047357796737589 / 85070591730234613168007331077920132390

theorem certifiedEnvelope_two : certifiedEnvelope 2 = 2*certifiedSlope := by
  apply NNRat.coe_inj.mp
  norm_num [certifiedEnvelope, weakA, weakComplement, rootRate, certifiedSlope, p, q, NNRat.coe_min]
-- CHECKPOINT

/-- An exact rational way to state that the score exceeds 25.6 bits:
the fifth power of the rate per word is below 2^-128. -/
theorem certifiedSlope_better_than_25_6 : certifiedSlope^5 < 1/(2:ℚ≥0)^128 := by
  apply NNRat.coe_lt_coe.mp
  norm_num [certifiedSlope]
-- CHECKPOINT

theorem mask_square : (852 : ℕ)^2 = 725904 := by norm_num
-- CHECKPOINT

theorem subcase_b_constant : (345763417 : ℚ≥0)/2^116 < 1/2^87 := by
  apply NNRat.coe_lt_coe.mp; norm_num
-- CHECKPOINT

theorem one_word_enh_constant : (852*32042 : ℚ≥0)/2^128 < 1/2^103 := by
  apply NNRat.coe_lt_coe.mp; norm_num
-- CHECKPOINT

theorem ph_enh_constant : (111924178297 : ℚ≥0)/2^128 < 1/2^91 := by
  apply NNRat.coe_lt_coe.mp; norm_num
-- CHECKPOINT

theorem tag_only_constant : (11946240 : ℚ≥0)/2^116 < 1/2^92 := by
  apply NNRat.coe_lt_coe.mp; norm_num
-- CHECKPOINT

theorem different_counts_constant : (81 : ℚ≥0)*(2*q-1 : ℕ)/(q:ℚ≥0)^2 < 162/q := by
  apply NNRat.coe_lt_coe.mp
  norm_num [q]
-- CHECKPOINT

theorem checksum_weight_constant : (101 : ℚ≥0)/2 + 909712/q < 51 := by
  apply NNRat.coe_lt_coe.mp
  norm_num [q]
-- CHECKPOINT

theorem weak_noPH_rounding :
    (13250910204502170674351448317121 : ℚ≥0)/2^128 ≤ 718333281557/q := by
  apply NNRat.coe_le_coe.mp
  norm_num [q]
-- CHECKPOINT

theorem weak_PH_rounding :
    (11989226195148851570875316890129934569436235759441 : ℚ≥0) /
      392318858461667547739736838950479151006397215279002157056 ≤ 563730706526/q := by
  apply NNRat.coe_le_coe.mp
  norm_num [q]
-- CHECKPOINT

theorem weakA_add_complement : weakA + weakComplement = 1 := by
  apply NNRat.coe_inj.mp
  norm_num [weakA, weakComplement, q]
-- CHECKPOINT

theorem certifiedEnvelope_per_word (L : ℕ) (hL : 1 ≤ L) :
    certifiedEnvelope L ≤ (L : ℚ≥0)*certifiedSlope := by
  by_cases h : L = 1
  · subst L
    apply NNRat.coe_le_coe.mp
    norm_num [certifiedEnvelope, certifiedSlope, q]
  have htwo : 2 ≤ L := by omega
  have hceilNat : 2*((L+31)/32) ≤ L := by omega
  have hceil : 2*((((L+31)/32 : ℕ) : ℚ)) ≤ (L : ℚ) := by exact_mod_cast hceilNat
  have htwoQ : (2 : ℚ) ≤ L := by exact_mod_cast htwo
  have hr : rootRate L ≤ 2 * (((L+31)/32 : ℕ) : ℚ≥0)/(p-2 : ℕ) := min_le_right _ _
  have hrQ := NNRat.coe_le_coe.mpr hr
  norm_num [p] at hrQ
  apply NNRat.coe_le_coe.mp
  norm_num [certifiedEnvelope, h, weakA, weakComplement, certifiedSlope, q]
  nlinarith
-- CHECKPOINT

end ProvenHashes.UMASH
