import Mathlib
namespace ProvenHashes.Highway
set_option maxRecDepth 100000
set_option maxHeartbeats 0

theorem nat_byte_add40 (n : Nat) (hn : n < 18446744073709551616)
    (hb : n / 1099511627776 % 256 ≠ 255) (i : Fin 8) :
    ((n + 1099511627776) % 18446744073709551616) / 2^(8*i.val) % 256 =
      n / 2^(8*i.val) % 256 + if i.val = 5 then 1 else 0 := by
  fin_cases i <;> norm_num at * <;> omega

def packN (x0 x1 x2 x3 x4 x5 x6 x7 : Nat) : Nat :=
  x0 + 256*x1 + 65536*x2 + 16777216*x3 + 4294967296*x4 +
    1099511627776*x5 + 281474976710656*x6 + 72057594037927936*x7

theorem packN_inc5 (x0 x1 x2 x3 x4 x5 x6 x7 : Nat)
    (h0 : x0 < 256) (h1 : x1 < 256) (h2 : x2 < 256) (h3 : x3 < 256)
    (h4 : x4 < 256) (h5 : x5 < 255) (h6 : x6 < 256) (h7 : x7 < 256) :
    packN x0 x1 x2 x3 x4 (x5+1) x6 x7 =
      (packN x0 x1 x2 x3 x4 x5 x6 x7 + 1099511627776) % 18446744073709551616 := by
  unfold packN
  omega

theorem packN_inc3 (x0 x1 x2 x3 x4 x5 x6 x7 : Nat)
    (h0 : x0 < 256) (h1 : x1 < 256) (h2 : x2 < 256) (h3 : x3 < 255)
    (h4 : x4 < 256) (h5 : x5 < 256) (h6 : x6 < 256) (h7 : x7 < 256) :
    packN x0 x1 x2 (x3+1) x4 x5 x6 x7 =
      (packN x0 x1 x2 x3 x4 x5 x6 x7 + 16777216) % 18446744073709551616 := by
  unfold packN
  omega

theorem nat_halves_add40 (n : Nat) (_hn : n < 18446744073709551616) :
    (n + 1099511627776) % 18446744073709551616 % 4294967296 = n % 4294967296 ∧
    (n + 1099511627776) % 18446744073709551616 / 4294967296 =
      (n / 4294967296 + 256) % 4294967296 := by omega

theorem nat_sub8 (n : Nat) (hn : n < 18446744073709551616)
    (hb : n / 256 % 256 ≠ 0) :
    (18446744073709551360 + n) % 18446744073709551616 / 256 % 256 ≠ 255 ∧
    (18446744073709551360 + n) % 18446744073709551616 / 4294967296 = n / 4294967296 := by
  omega

theorem nat_lo_sub8 (n : Nat) (_hn : n < 18446744073709551616) :
    (18446744073709551360 + n) % 18446744073709551616 % 4294967296 =
    (4294967040 + n % 4294967296) % 4294967296 := by omega

theorem nat_e2_byte (u v : Nat) (_hu : u < 18446744073709551616)
    (_hv : v < 18446744073709551616) (hb : u / 1099511627776 % 256 ≠ 255)
    (he : v % 4294967296 = (u / 4294967296 + 256) % 4294967296) :
    v / 256 % 256 ≠ 0 := by omega

theorem nat_lowbytes (n : Nat) (h : n % 4294967296 = 0x10e82046) :
    n % 256 = 70 ∧ n / 256 % 256 = 32 ∧
      n / 65536 % 256 = 232 ∧ n / 16777216 % 256 = 16 := by omega

theorem nat_first_byte5 (n c w4 a5 w6 w7 : Nat) (hc : c = 0x3bd39e10cb0ef593)
    (hn : n < 4294967296) (h4 : w4 < 256) (h5 : a5 < 256)
    (h6 : w6 < 256) (_h7 : w7 < 256) :
    (((n + c) % 18446744073709551616 + packN 16 w4 232 a5 w6 32 w7 70) %
      18446744073709551616) / 1099511627776 % 256 ≠ 255 := by
  unfold packN
  subst c
  omega

end ProvenHashes.Highway
