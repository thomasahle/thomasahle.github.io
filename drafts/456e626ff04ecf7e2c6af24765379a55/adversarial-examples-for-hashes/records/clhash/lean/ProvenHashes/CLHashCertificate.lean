import ProvenHashes.CLHashModulus
import ProvenHashes.ModulusResidues

noncomputable section
namespace ProvenHashes.CLHash
open Polynomial ChainHash
set_option maxHeartbeats 0
set_option maxRecDepth 10000
set_option linter.unusedSimpArgs false
set_option linter.unusedTactic false
set_option linter.unreachableTactic false

def r0 : BitsPolynomial := sparse [1]
def r1 : BitsPolynomial := sparse [2]
def r2 : BitsPolynomial := sparse [4]
def r3 : BitsPolynomial := sparse [8]
def r4 : BitsPolynomial := sparse [16]
def r5 : BitsPolynomial := sparse [32]
def r6 : BitsPolynomial := sparse [64]
def r7 : BitsPolynomial := sparse [1, 2]
def r8 : BitsPolynomial := sparse [2, 4]
def r9 : BitsPolynomial := sparse [4, 8]
def r10 : BitsPolynomial := sparse [8, 16]
def r11 : BitsPolynomial := sparse [16, 32]
def r12 : BitsPolynomial := sparse [32, 64]
def r13 : BitsPolynomial := sparse [1, 2, 64]
def r14 : BitsPolynomial := sparse [1, 4]
def r15 : BitsPolynomial := sparse [2, 8]
def r16 : BitsPolynomial := sparse [4, 16]
def r17 : BitsPolynomial := sparse [8, 32]
def r18 : BitsPolynomial := sparse [16, 64]
def r19 : BitsPolynomial := sparse [1, 2, 32]
def r20 : BitsPolynomial := sparse [2, 4, 64]
def r21 : BitsPolynomial := sparse [1, 2, 4, 8]
def r22 : BitsPolynomial := sparse [2, 4, 8, 16]
def r23 : BitsPolynomial := sparse [4, 8, 16, 32]
def r24 : BitsPolynomial := sparse [8, 16, 32, 64]
def r25 : BitsPolynomial := sparse [1, 2, 16, 32, 64]
def r26 : BitsPolynomial := sparse [1, 4, 32, 64]
def r27 : BitsPolynomial := sparse [1, 8, 64]
def r28 : BitsPolynomial := sparse [1, 16]
def r29 : BitsPolynomial := sparse [2, 32]
def r30 : BitsPolynomial := sparse [4, 64]
def r31 : BitsPolynomial := sparse [1, 2, 8]
def r32 : BitsPolynomial := sparse [2, 4, 16]
def r33 : BitsPolynomial := sparse [4, 8, 32]
def r34 : BitsPolynomial := sparse [8, 16, 64]
def r35 : BitsPolynomial := sparse [1, 2, 16, 32]
def r36 : BitsPolynomial := sparse [2, 4, 32, 64]
def r37 : BitsPolynomial := sparse [1, 2, 4, 8, 64]
def r38 : BitsPolynomial := sparse [1, 4, 8, 16]
def r39 : BitsPolynomial := sparse [2, 8, 16, 32]
def r40 : BitsPolynomial := sparse [4, 16, 32, 64]
def r41 : BitsPolynomial := sparse [1, 2, 8, 32, 64]
def r42 : BitsPolynomial := sparse [1, 4, 16, 64]
def r43 : BitsPolynomial := sparse [1, 8, 32]
def r44 : BitsPolynomial := sparse [2, 16, 64]
def r45 : BitsPolynomial := sparse [1, 2, 4, 32]
def r46 : BitsPolynomial := sparse [2, 4, 8, 64]
def r47 : BitsPolynomial := sparse [1, 2, 4, 8, 16]
def r48 : BitsPolynomial := sparse [2, 4, 8, 16, 32]
def r49 : BitsPolynomial := sparse [4, 8, 16, 32, 64]
def r50 : BitsPolynomial := sparse [1, 2, 8, 16, 32, 64]
def r51 : BitsPolynomial := sparse [1, 4, 16, 32, 64]
def r52 : BitsPolynomial := sparse [1, 8, 32, 64]
def r53 : BitsPolynomial := sparse [1, 16, 64]
def r54 : BitsPolynomial := sparse [1, 32]
def r55 : BitsPolynomial := sparse [2, 64]
def r56 : BitsPolynomial := sparse [1, 2, 4]
def r57 : BitsPolynomial := sparse [2, 4, 8]
def r58 : BitsPolynomial := sparse [4, 8, 16]
def r59 : BitsPolynomial := sparse [8, 16, 32]
def r60 : BitsPolynomial := sparse [16, 32, 64]
def r61 : BitsPolynomial := sparse [1, 2, 32, 64]
def r62 : BitsPolynomial := sparse [1, 4, 64]
def r63 : BitsPolynomial := sparse [1, 8]
def r64 : BitsPolynomial := sparse [2, 16]
def r65 : BitsPolynomial := sparse [4, 32]
def r66 : BitsPolynomial := sparse [8, 64]
def r67 : BitsPolynomial := sparse [1, 2, 16]
def r68 : BitsPolynomial := sparse [2, 4, 32]
def r69 : BitsPolynomial := sparse [4, 8, 64]
def r70 : BitsPolynomial := sparse [1, 2, 8, 16]
def r71 : BitsPolynomial := sparse [2, 4, 16, 32]
def r72 : BitsPolynomial := sparse [4, 8, 32, 64]
def r73 : BitsPolynomial := sparse [1, 2, 8, 16, 64]
def r74 : BitsPolynomial := sparse [1, 4, 16, 32]
def r75 : BitsPolynomial := sparse [2, 8, 32, 64]
def r76 : BitsPolynomial := sparse [1, 2, 4, 16, 64]
def r77 : BitsPolynomial := sparse [1, 4, 8, 32]
def r78 : BitsPolynomial := sparse [2, 8, 16, 64]
def r79 : BitsPolynomial := sparse [1, 2, 4, 16, 32]
def r80 : BitsPolynomial := sparse [2, 4, 8, 32, 64]
def r81 : BitsPolynomial := sparse [1, 2, 4, 8, 16, 64]
def r82 : BitsPolynomial := sparse [1, 4, 8, 16, 32]
def r83 : BitsPolynomial := sparse [2, 8, 16, 32, 64]
def r84 : BitsPolynomial := sparse [1, 2, 4, 16, 32, 64]
def r85 : BitsPolynomial := sparse [1, 4, 8, 32, 64]
def r86 : BitsPolynomial := sparse [1, 8, 16, 64]
def r87 : BitsPolynomial := sparse [1, 16, 32]
def r88 : BitsPolynomial := sparse [2, 32, 64]
def r89 : BitsPolynomial := sparse [1, 2, 4, 64]
def r90 : BitsPolynomial := sparse [1, 4, 8]
def r91 : BitsPolynomial := sparse [2, 8, 16]
def r92 : BitsPolynomial := sparse [4, 16, 32]
def r93 : BitsPolynomial := sparse [8, 32, 64]
def r94 : BitsPolynomial := sparse [1, 2, 16, 64]
def r95 : BitsPolynomial := sparse [1, 4, 32]
def r96 : BitsPolynomial := sparse [2, 8, 64]
def r97 : BitsPolynomial := sparse [1, 2, 4, 16]
def r98 : BitsPolynomial := sparse [2, 4, 8, 32]
def r99 : BitsPolynomial := sparse [4, 8, 16, 64]
def r100 : BitsPolynomial := sparse [1, 2, 8, 16, 32]
def r101 : BitsPolynomial := sparse [2, 4, 16, 32, 64]
def r102 : BitsPolynomial := sparse [1, 2, 4, 8, 32, 64]
def r103 : BitsPolynomial := sparse [1, 4, 8, 16, 64]
def r104 : BitsPolynomial := sparse [1, 8, 16, 32]
def r105 : BitsPolynomial := sparse [2, 16, 32, 64]
def r106 : BitsPolynomial := sparse [1, 2, 4, 32, 64]
def r107 : BitsPolynomial := sparse [1, 4, 8, 64]
def r108 : BitsPolynomial := sparse [1, 8, 16]
def r109 : BitsPolynomial := sparse [2, 16, 32]
def r110 : BitsPolynomial := sparse [4, 32, 64]
def r111 : BitsPolynomial := sparse [1, 2, 8, 64]
def r112 : BitsPolynomial := sparse [1, 4, 16]
def r113 : BitsPolynomial := sparse [2, 8, 32]
def r114 : BitsPolynomial := sparse [4, 16, 64]
def r115 : BitsPolynomial := sparse [1, 2, 8, 32]
def r116 : BitsPolynomial := sparse [2, 4, 16, 64]
def r117 : BitsPolynomial := sparse [1, 2, 4, 8, 32]
def r118 : BitsPolynomial := sparse [2, 4, 8, 16, 64]
def r119 : BitsPolynomial := sparse [1, 2, 4, 8, 16, 32]
def r120 : BitsPolynomial := sparse [2, 4, 8, 16, 32, 64]
def r121 : BitsPolynomial := sparse [1, 2, 4, 8, 16, 32, 64]
def r122 : BitsPolynomial := sparse [1, 4, 8, 16, 32, 64]
def r123 : BitsPolynomial := sparse [1, 8, 16, 32, 64]
def r124 : BitsPolynomial := sparse [1, 16, 32, 64]
def r125 : BitsPolynomial := sparse [1, 32, 64]
def r126 : BitsPolynomial := sparse [1, 64]
def r127 : BitsPolynomial := sparse [1]

/-- A checked Frobenius chain and a checked Bezout identity. -/
theorem modulus127_certificate :
    modulus127 ∣ X ^ (2 ^ 127) - X ∧ IsCoprime modulus127 (X ^ 2 - X) := by
  have step (i : ℕ) (r s q : BitsPolynomial)
      (hs : r ^ 2 = s + modulus127 * q)
      (hr : AdjoinRoot.root modulus127 ^ (2 ^ i) = AdjoinRoot.mk modulus127 r) :
      AdjoinRoot.root modulus127 ^ (2 ^ (i + 1)) = AdjoinRoot.mk modulus127 s := by
    rw [pow_succ, pow_mul, hr, ← map_pow, hs, map_add, map_mul,
      AdjoinRoot.mk_self, zero_mul, add_zero]
  have h0 : AdjoinRoot.root modulus127 ^ (2 ^ 0) = AdjoinRoot.mk modulus127 r0 := by
    simp [r0, sparse, AdjoinRoot.mk_X]
  have s0 : r0 ^ 2 = r1 + modulus127 * (sparse []) := by
    unfold r0 r1 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h1 := step 0 _ _ _ s0 h0
  have s1 : r1 ^ 2 = r2 + modulus127 * (sparse []) := by
    unfold r1 r2 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h2 := step 1 _ _ _ s1 h1
  have s2 : r2 ^ 2 = r3 + modulus127 * (sparse []) := by
    unfold r2 r3 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h3 := step 2 _ _ _ s2 h2
  have s3 : r3 ^ 2 = r4 + modulus127 * (sparse []) := by
    unfold r3 r4 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h4 := step 3 _ _ _ s3 h3
  have s4 : r4 ^ 2 = r5 + modulus127 * (sparse []) := by
    unfold r4 r5 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h5 := step 4 _ _ _ s4 h4
  have s5 : r5 ^ 2 = r6 + modulus127 * (sparse []) := by
    unfold r5 r6 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h6 := step 5 _ _ _ s5 h5
  have s6 : r6 ^ 2 = r7 + modulus127 * (sparse [1]) := by
    unfold r6 r7 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h7 := step 6 _ _ _ s6 h6
  have s7 : r7 ^ 2 = r8 + modulus127 * (sparse []) := by
    unfold r7 r8 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h8 := step 7 _ _ _ s7 h7
  have s8 : r8 ^ 2 = r9 + modulus127 * (sparse []) := by
    unfold r8 r9 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h9 := step 8 _ _ _ s8 h8
  have s9 : r9 ^ 2 = r10 + modulus127 * (sparse []) := by
    unfold r9 r10 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h10 := step 9 _ _ _ s9 h9
  have s10 : r10 ^ 2 = r11 + modulus127 * (sparse []) := by
    unfold r10 r11 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h11 := step 10 _ _ _ s10 h10
  have s11 : r11 ^ 2 = r12 + modulus127 * (sparse []) := by
    unfold r11 r12 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h12 := step 11 _ _ _ s11 h11
  have s12 : r12 ^ 2 = r13 + modulus127 * (sparse [1]) := by
    unfold r12 r13 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h13 := step 12 _ _ _ s12 h12
  have s13 : r13 ^ 2 = r14 + modulus127 * (sparse [1]) := by
    unfold r13 r14 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h14 := step 13 _ _ _ s13 h13
  have s14 : r14 ^ 2 = r15 + modulus127 * (sparse []) := by
    unfold r14 r15 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h15 := step 14 _ _ _ s14 h14
  have s15 : r15 ^ 2 = r16 + modulus127 * (sparse []) := by
    unfold r15 r16 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h16 := step 15 _ _ _ s15 h15
  have s16 : r16 ^ 2 = r17 + modulus127 * (sparse []) := by
    unfold r16 r17 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h17 := step 16 _ _ _ s16 h16
  have s17 : r17 ^ 2 = r18 + modulus127 * (sparse []) := by
    unfold r17 r18 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h18 := step 17 _ _ _ s17 h17
  have s18 : r18 ^ 2 = r19 + modulus127 * (sparse [1]) := by
    unfold r18 r19 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h19 := step 18 _ _ _ s18 h18
  have s19 : r19 ^ 2 = r20 + modulus127 * (sparse []) := by
    unfold r19 r20 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h20 := step 19 _ _ _ s19 h19
  have s20 : r20 ^ 2 = r21 + modulus127 * (sparse [1]) := by
    unfold r20 r21 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h21 := step 20 _ _ _ s20 h20
  have s21 : r21 ^ 2 = r22 + modulus127 * (sparse []) := by
    unfold r21 r22 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h22 := step 21 _ _ _ s21 h21
  have s22 : r22 ^ 2 = r23 + modulus127 * (sparse []) := by
    unfold r22 r23 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h23 := step 22 _ _ _ s22 h22
  have s23 : r23 ^ 2 = r24 + modulus127 * (sparse []) := by
    unfold r23 r24 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h24 := step 23 _ _ _ s23 h23
  have s24 : r24 ^ 2 = r25 + modulus127 * (sparse [1]) := by
    unfold r24 r25 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h25 := step 24 _ _ _ s24 h24
  have s25 : r25 ^ 2 = r26 + modulus127 * (sparse [1]) := by
    unfold r25 r26 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h26 := step 25 _ _ _ s25 h25
  have s26 : r26 ^ 2 = r27 + modulus127 * (sparse [1]) := by
    unfold r26 r27 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h27 := step 26 _ _ _ s26 h26
  have s27 : r27 ^ 2 = r28 + modulus127 * (sparse [1]) := by
    unfold r27 r28 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h28 := step 27 _ _ _ s27 h27
  have s28 : r28 ^ 2 = r29 + modulus127 * (sparse []) := by
    unfold r28 r29 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h29 := step 28 _ _ _ s28 h28
  have s29 : r29 ^ 2 = r30 + modulus127 * (sparse []) := by
    unfold r29 r30 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h30 := step 29 _ _ _ s29 h29
  have s30 : r30 ^ 2 = r31 + modulus127 * (sparse [1]) := by
    unfold r30 r31 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h31 := step 30 _ _ _ s30 h30
  have s31 : r31 ^ 2 = r32 + modulus127 * (sparse []) := by
    unfold r31 r32 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h32 := step 31 _ _ _ s31 h31
  have s32 : r32 ^ 2 = r33 + modulus127 * (sparse []) := by
    unfold r32 r33 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h33 := step 32 _ _ _ s32 h32
  have s33 : r33 ^ 2 = r34 + modulus127 * (sparse []) := by
    unfold r33 r34 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h34 := step 33 _ _ _ s33 h33
  have s34 : r34 ^ 2 = r35 + modulus127 * (sparse [1]) := by
    unfold r34 r35 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h35 := step 34 _ _ _ s34 h34
  have s35 : r35 ^ 2 = r36 + modulus127 * (sparse []) := by
    unfold r35 r36 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h36 := step 35 _ _ _ s35 h35
  have s36 : r36 ^ 2 = r37 + modulus127 * (sparse [1]) := by
    unfold r36 r37 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h37 := step 36 _ _ _ s36 h36
  have s37 : r37 ^ 2 = r38 + modulus127 * (sparse [1]) := by
    unfold r37 r38 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h38 := step 37 _ _ _ s37 h37
  have s38 : r38 ^ 2 = r39 + modulus127 * (sparse []) := by
    unfold r38 r39 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h39 := step 38 _ _ _ s38 h38
  have s39 : r39 ^ 2 = r40 + modulus127 * (sparse []) := by
    unfold r39 r40 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h40 := step 39 _ _ _ s39 h39
  have s40 : r40 ^ 2 = r41 + modulus127 * (sparse [1]) := by
    unfold r40 r41 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h41 := step 40 _ _ _ s40 h40
  have s41 : r41 ^ 2 = r42 + modulus127 * (sparse [1]) := by
    unfold r41 r42 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h42 := step 41 _ _ _ s41 h41
  have s42 : r42 ^ 2 = r43 + modulus127 * (sparse [1]) := by
    unfold r42 r43 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h43 := step 42 _ _ _ s42 h42
  have s43 : r43 ^ 2 = r44 + modulus127 * (sparse []) := by
    unfold r43 r44 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h44 := step 43 _ _ _ s43 h43
  have s44 : r44 ^ 2 = r45 + modulus127 * (sparse [1]) := by
    unfold r44 r45 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h45 := step 44 _ _ _ s44 h44
  have s45 : r45 ^ 2 = r46 + modulus127 * (sparse []) := by
    unfold r45 r46 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h46 := step 45 _ _ _ s45 h45
  have s46 : r46 ^ 2 = r47 + modulus127 * (sparse [1]) := by
    unfold r46 r47 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h47 := step 46 _ _ _ s46 h46
  have s47 : r47 ^ 2 = r48 + modulus127 * (sparse []) := by
    unfold r47 r48 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h48 := step 47 _ _ _ s47 h47
  have s48 : r48 ^ 2 = r49 + modulus127 * (sparse []) := by
    unfold r48 r49 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h49 := step 48 _ _ _ s48 h48
  have s49 : r49 ^ 2 = r50 + modulus127 * (sparse [1]) := by
    unfold r49 r50 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h50 := step 49 _ _ _ s49 h49
  have s50 : r50 ^ 2 = r51 + modulus127 * (sparse [1]) := by
    unfold r50 r51 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h51 := step 50 _ _ _ s50 h50
  have s51 : r51 ^ 2 = r52 + modulus127 * (sparse [1]) := by
    unfold r51 r52 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h52 := step 51 _ _ _ s51 h51
  have s52 : r52 ^ 2 = r53 + modulus127 * (sparse [1]) := by
    unfold r52 r53 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h53 := step 52 _ _ _ s52 h52
  have s53 : r53 ^ 2 = r54 + modulus127 * (sparse [1]) := by
    unfold r53 r54 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h54 := step 53 _ _ _ s53 h53
  have s54 : r54 ^ 2 = r55 + modulus127 * (sparse []) := by
    unfold r54 r55 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h55 := step 54 _ _ _ s54 h54
  have s55 : r55 ^ 2 = r56 + modulus127 * (sparse [1]) := by
    unfold r55 r56 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h56 := step 55 _ _ _ s55 h55
  have s56 : r56 ^ 2 = r57 + modulus127 * (sparse []) := by
    unfold r56 r57 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h57 := step 56 _ _ _ s56 h56
  have s57 : r57 ^ 2 = r58 + modulus127 * (sparse []) := by
    unfold r57 r58 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h58 := step 57 _ _ _ s57 h57
  have s58 : r58 ^ 2 = r59 + modulus127 * (sparse []) := by
    unfold r58 r59 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h59 := step 58 _ _ _ s58 h58
  have s59 : r59 ^ 2 = r60 + modulus127 * (sparse []) := by
    unfold r59 r60 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h60 := step 59 _ _ _ s59 h59
  have s60 : r60 ^ 2 = r61 + modulus127 * (sparse [1]) := by
    unfold r60 r61 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h61 := step 60 _ _ _ s60 h60
  have s61 : r61 ^ 2 = r62 + modulus127 * (sparse [1]) := by
    unfold r61 r62 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h62 := step 61 _ _ _ s61 h61
  have s62 : r62 ^ 2 = r63 + modulus127 * (sparse [1]) := by
    unfold r62 r63 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h63 := step 62 _ _ _ s62 h62
  have s63 : r63 ^ 2 = r64 + modulus127 * (sparse []) := by
    unfold r63 r64 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h64 := step 63 _ _ _ s63 h63
  have s64 : r64 ^ 2 = r65 + modulus127 * (sparse []) := by
    unfold r64 r65 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h65 := step 64 _ _ _ s64 h64
  have s65 : r65 ^ 2 = r66 + modulus127 * (sparse []) := by
    unfold r65 r66 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h66 := step 65 _ _ _ s65 h65
  have s66 : r66 ^ 2 = r67 + modulus127 * (sparse [1]) := by
    unfold r66 r67 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h67 := step 66 _ _ _ s66 h66
  have s67 : r67 ^ 2 = r68 + modulus127 * (sparse []) := by
    unfold r67 r68 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h68 := step 67 _ _ _ s67 h67
  have s68 : r68 ^ 2 = r69 + modulus127 * (sparse []) := by
    unfold r68 r69 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h69 := step 68 _ _ _ s68 h68
  have s69 : r69 ^ 2 = r70 + modulus127 * (sparse [1]) := by
    unfold r69 r70 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h70 := step 69 _ _ _ s69 h69
  have s70 : r70 ^ 2 = r71 + modulus127 * (sparse []) := by
    unfold r70 r71 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h71 := step 70 _ _ _ s70 h70
  have s71 : r71 ^ 2 = r72 + modulus127 * (sparse []) := by
    unfold r71 r72 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h72 := step 71 _ _ _ s71 h71
  have s72 : r72 ^ 2 = r73 + modulus127 * (sparse [1]) := by
    unfold r72 r73 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h73 := step 72 _ _ _ s72 h72
  have s73 : r73 ^ 2 = r74 + modulus127 * (sparse [1]) := by
    unfold r73 r74 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h74 := step 73 _ _ _ s73 h73
  have s74 : r74 ^ 2 = r75 + modulus127 * (sparse []) := by
    unfold r74 r75 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h75 := step 74 _ _ _ s74 h74
  have s75 : r75 ^ 2 = r76 + modulus127 * (sparse [1]) := by
    unfold r75 r76 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h76 := step 75 _ _ _ s75 h75
  have s76 : r76 ^ 2 = r77 + modulus127 * (sparse [1]) := by
    unfold r76 r77 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h77 := step 76 _ _ _ s76 h76
  have s77 : r77 ^ 2 = r78 + modulus127 * (sparse []) := by
    unfold r77 r78 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h78 := step 77 _ _ _ s77 h77
  have s78 : r78 ^ 2 = r79 + modulus127 * (sparse [1]) := by
    unfold r78 r79 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h79 := step 78 _ _ _ s78 h78
  have s79 : r79 ^ 2 = r80 + modulus127 * (sparse []) := by
    unfold r79 r80 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h80 := step 79 _ _ _ s79 h79
  have s80 : r80 ^ 2 = r81 + modulus127 * (sparse [1]) := by
    unfold r80 r81 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h81 := step 80 _ _ _ s80 h80
  have s81 : r81 ^ 2 = r82 + modulus127 * (sparse [1]) := by
    unfold r81 r82 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h82 := step 81 _ _ _ s81 h81
  have s82 : r82 ^ 2 = r83 + modulus127 * (sparse []) := by
    unfold r82 r83 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h83 := step 82 _ _ _ s82 h82
  have s83 : r83 ^ 2 = r84 + modulus127 * (sparse [1]) := by
    unfold r83 r84 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h84 := step 83 _ _ _ s83 h83
  have s84 : r84 ^ 2 = r85 + modulus127 * (sparse [1]) := by
    unfold r84 r85 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h85 := step 84 _ _ _ s84 h84
  have s85 : r85 ^ 2 = r86 + modulus127 * (sparse [1]) := by
    unfold r85 r86 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h86 := step 85 _ _ _ s85 h85
  have s86 : r86 ^ 2 = r87 + modulus127 * (sparse [1]) := by
    unfold r86 r87 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h87 := step 86 _ _ _ s86 h86
  have s87 : r87 ^ 2 = r88 + modulus127 * (sparse []) := by
    unfold r87 r88 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h88 := step 87 _ _ _ s87 h87
  have s88 : r88 ^ 2 = r89 + modulus127 * (sparse [1]) := by
    unfold r88 r89 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h89 := step 88 _ _ _ s88 h88
  have s89 : r89 ^ 2 = r90 + modulus127 * (sparse [1]) := by
    unfold r89 r90 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h90 := step 89 _ _ _ s89 h89
  have s90 : r90 ^ 2 = r91 + modulus127 * (sparse []) := by
    unfold r90 r91 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h91 := step 90 _ _ _ s90 h90
  have s91 : r91 ^ 2 = r92 + modulus127 * (sparse []) := by
    unfold r91 r92 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h92 := step 91 _ _ _ s91 h91
  have s92 : r92 ^ 2 = r93 + modulus127 * (sparse []) := by
    unfold r92 r93 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h93 := step 92 _ _ _ s92 h92
  have s93 : r93 ^ 2 = r94 + modulus127 * (sparse [1]) := by
    unfold r93 r94 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h94 := step 93 _ _ _ s93 h93
  have s94 : r94 ^ 2 = r95 + modulus127 * (sparse [1]) := by
    unfold r94 r95 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h95 := step 94 _ _ _ s94 h94
  have s95 : r95 ^ 2 = r96 + modulus127 * (sparse []) := by
    unfold r95 r96 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h96 := step 95 _ _ _ s95 h95
  have s96 : r96 ^ 2 = r97 + modulus127 * (sparse [1]) := by
    unfold r96 r97 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h97 := step 96 _ _ _ s96 h96
  have s97 : r97 ^ 2 = r98 + modulus127 * (sparse []) := by
    unfold r97 r98 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h98 := step 97 _ _ _ s97 h97
  have s98 : r98 ^ 2 = r99 + modulus127 * (sparse []) := by
    unfold r98 r99 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h99 := step 98 _ _ _ s98 h98
  have s99 : r99 ^ 2 = r100 + modulus127 * (sparse [1]) := by
    unfold r99 r100 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h100 := step 99 _ _ _ s99 h99
  have s100 : r100 ^ 2 = r101 + modulus127 * (sparse []) := by
    unfold r100 r101 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h101 := step 100 _ _ _ s100 h100
  have s101 : r101 ^ 2 = r102 + modulus127 * (sparse [1]) := by
    unfold r101 r102 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h102 := step 101 _ _ _ s101 h101
  have s102 : r102 ^ 2 = r103 + modulus127 * (sparse [1]) := by
    unfold r102 r103 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h103 := step 102 _ _ _ s102 h102
  have s103 : r103 ^ 2 = r104 + modulus127 * (sparse [1]) := by
    unfold r103 r104 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h104 := step 103 _ _ _ s103 h103
  have s104 : r104 ^ 2 = r105 + modulus127 * (sparse []) := by
    unfold r104 r105 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h105 := step 104 _ _ _ s104 h104
  have s105 : r105 ^ 2 = r106 + modulus127 * (sparse [1]) := by
    unfold r105 r106 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h106 := step 105 _ _ _ s105 h105
  have s106 : r106 ^ 2 = r107 + modulus127 * (sparse [1]) := by
    unfold r106 r107 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h107 := step 106 _ _ _ s106 h106
  have s107 : r107 ^ 2 = r108 + modulus127 * (sparse [1]) := by
    unfold r107 r108 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h108 := step 107 _ _ _ s107 h107
  have s108 : r108 ^ 2 = r109 + modulus127 * (sparse []) := by
    unfold r108 r109 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h109 := step 108 _ _ _ s108 h108
  have s109 : r109 ^ 2 = r110 + modulus127 * (sparse []) := by
    unfold r109 r110 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h110 := step 109 _ _ _ s109 h109
  have s110 : r110 ^ 2 = r111 + modulus127 * (sparse [1]) := by
    unfold r110 r111 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h111 := step 110 _ _ _ s110 h110
  have s111 : r111 ^ 2 = r112 + modulus127 * (sparse [1]) := by
    unfold r111 r112 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h112 := step 111 _ _ _ s111 h111
  have s112 : r112 ^ 2 = r113 + modulus127 * (sparse []) := by
    unfold r112 r113 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h113 := step 112 _ _ _ s112 h112
  have s113 : r113 ^ 2 = r114 + modulus127 * (sparse []) := by
    unfold r113 r114 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h114 := step 113 _ _ _ s113 h113
  have s114 : r114 ^ 2 = r115 + modulus127 * (sparse [1]) := by
    unfold r114 r115 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h115 := step 114 _ _ _ s114 h114
  have s115 : r115 ^ 2 = r116 + modulus127 * (sparse []) := by
    unfold r115 r116 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h116 := step 115 _ _ _ s115 h115
  have s116 : r116 ^ 2 = r117 + modulus127 * (sparse [1]) := by
    unfold r116 r117 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h117 := step 116 _ _ _ s116 h116
  have s117 : r117 ^ 2 = r118 + modulus127 * (sparse []) := by
    unfold r117 r118 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h118 := step 117 _ _ _ s117 h117
  have s118 : r118 ^ 2 = r119 + modulus127 * (sparse [1]) := by
    unfold r118 r119 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h119 := step 118 _ _ _ s118 h118
  have s119 : r119 ^ 2 = r120 + modulus127 * (sparse []) := by
    unfold r119 r120 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h120 := step 119 _ _ _ s119 h119
  have s120 : r120 ^ 2 = r121 + modulus127 * (sparse [1]) := by
    unfold r120 r121 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h121 := step 120 _ _ _ s120 h120
  have s121 : r121 ^ 2 = r122 + modulus127 * (sparse [1]) := by
    unfold r121 r122 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h122 := step 121 _ _ _ s121 h121
  have s122 : r122 ^ 2 = r123 + modulus127 * (sparse [1]) := by
    unfold r122 r123 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h123 := step 122 _ _ _ s122 h122
  have s123 : r123 ^ 2 = r124 + modulus127 * (sparse [1]) := by
    unfold r123 r124 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h124 := step 123 _ _ _ s123 h123
  have s124 : r124 ^ 2 = r125 + modulus127 * (sparse [1]) := by
    unfold r124 r125 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h125 := step 124 _ _ _ s124 h124
  have s125 : r125 ^ 2 = r126 + modulus127 * (sparse [1]) := by
    unfold r125 r126 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h126 := step 125 _ _ _ s125 h125
  have s126 : r126 ^ 2 = r127 + modulus127 * (sparse [1]) := by
    unfold r126 r127 modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]
  have h127 := step 126 _ _ _ s126 h126
  constructor
  · apply AdjoinRoot.mk_eq_zero.mp
    rw [map_sub, map_pow, AdjoinRoot.mk_X, h127]
    simp [r127, sparse, AdjoinRoot.mk_X]
  · refine ⟨sparse [0], sparse [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125], ?_⟩
    rw [CharTwo.sub_eq_add]
    unfold modulus127
    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [poly_num_2, poly_num_3, poly_num_4, poly_num_5, poly_num_6, poly_num_7, poly_num_8, poly_num_9, poly_num_10, poly_num_11, poly_num_12, poly_num_13, poly_num_14, poly_num_15, poly_num_16, poly_num_17, poly_num_18, poly_num_19, poly_num_20, poly_num_21, poly_num_22, poly_num_23, poly_num_24, poly_num_25, poly_num_26, poly_num_27, poly_num_28, poly_num_29, poly_num_30, poly_num_31, poly_num_32, poly_num_33, poly_num_34, poly_num_35, poly_num_36, poly_num_37, poly_num_38, poly_num_39, poly_num_40, poly_num_41, poly_num_42, poly_num_43, poly_num_44, poly_num_45, poly_num_46, poly_num_47, poly_num_48, poly_num_49, poly_num_50, poly_num_51, poly_num_52, poly_num_53, poly_num_54, poly_num_55, poly_num_56, poly_num_57, poly_num_58, poly_num_59, poly_num_60, poly_num_61, poly_num_62, poly_num_63, poly_num_64,
      mul_zero, mul_one, zero_add, add_zero]

end ProvenHashes.CLHash
