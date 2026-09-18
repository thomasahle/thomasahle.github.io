import ProvenHashes.Polymur.Prefix
import ProvenHashes.Polymur.Tail

noncomputable section
namespace ProvenHashes.Polymur
open Polynomial

/-- The loop stops with 1..49 bytes; the empty message has no prefix. -/
def blockCount (m : Bytes) : ℕ := (m.length-1)/49

def blockWords (m : Bytes) (b : ℕ) : Words := fun j => window m (49*b+7*j.val) 7

def blocks (m : Bytes) : List Words := List.ofFn (fun b : Fin (blockCount m) => blockWords m b.val)

def tail (m : Bytes) : Bytes := m.drop (49*blockCount m)

/-- Polymur's message polynomial, including the final length term and X^14 shift. -/
def encode (m : Bytes) : F[X] := prefixPoly (blocks m)*X^14 + tailPoly (tail m)

def coefficients (m : Bytes) : ℕ → F := (encode m).coeff

def core (k : F) (m : Bytes) : F := (encode m).eval k

lemma tail_length_le (m : Bytes) : (tail m).length ≤ 49 := by
  simp [tail, blockCount]
  omega

lemma message_length (m : Bytes) : m.length = 49*blockCount m + (tail m).length := by
  simp [tail, blockCount]
  omega

lemma blocks_good (m : Bytes) : ∀ w ∈ blocks m, w 6+3 ≠ 0 := by
  intro w hw
  obtain ⟨b, rfl⟩ := List.mem_ofFn.mp hw
  have hb := pack_lt ((m.drop (49*b.val+7*6)).take 7) (by simp)
  norm_num at hb
  intro he
  have hz : pack ((m.drop (49*b.val+7*6)).take 7)+3 = 0 :=
    cast_inj (by dsimp [p]; omega) (by norm_num [p]) (by simpa [blockWords, window] using he)
  omega

lemma blocks_length (m : Bytes) : (blocks m).length = blockCount m := by simp [blocks]

theorem encode_injective : Function.Injective encode := by
  intro a b h
  have hp := separate_prefix_tail (tail_degree_le (tail a)) (tail_degree_le (tail b)) h
  have hb := prefix_inj (blocks_good a) (blocks_good b) hp.1
  have hc : blockCount a = blockCount b := by simpa [blocks_length] using congrArg List.length hb
  have ht : tail a = tail b := tail_inj (tail_length_le a) (tail_length_le b) hp.2
  have hl : a.length = b.length := by
    have := message_length a
    have := message_length b
    have := congrArg List.length ht
    omega
  apply List.ext_getElem hl
  intro i hi hi'
  by_cases hiq : i < 49*blockCount a
  · let bi : Fin (blockCount a) := ⟨i/49, by omega⟩
    let j : Fin 7 := ⟨(i%49)/7, by omega⟩
    have hw : blockWords a bi.val = blockWords b bi.val := by
      have he := congrArg (fun l : List Words => l[bi.val]?) hb
      have hbi : i/49 < blockCount b := by omega
      simpa [blocks, hc, bi, hbi] using he
    have he := congrFun hw j
    exact window_byte hl (by omega) (by dsimp [bi,j]; omega) hi (by dsimp [bi,j]; omega) he
  · have he := congrArg (fun l : Bytes => l[i-49*blockCount a]?) ht
    simpa [tail, ← hc, List.getElem?_drop, show 49*blockCount a+(i-49*blockCount a)=i by omega,
      List.getElem?_eq_getElem hi, List.getElem?_eq_getElem hi'] using he

theorem coefficients_injective : Function.Injective coefficients := by
  intro a b h
  apply encode_injective
  exact Polynomial.ext (congrFun h)

lemma encode_degree_le (m : Bytes) : (encode m).natDegree ≤ 7*blockCount m+14 := by
  apply (natDegree_add_le _ _).trans
  apply max_le
  · exact (natDegree_mul_le).trans (by
      have := prefix_degree_le (blocks m)
      simp only [blocks_length, natDegree_X_pow] at *
      omega)
  · exact (tail_degree_le _).trans (by omega)

lemma encode_short (m : Bytes) (h : m.length ≤ 49) : encode m = tailPoly m := by
  have hc : blockCount m = 0 := by unfold blockCount; omega
  simp [encode, blocks, tail, hc, prefixPoly]

/-- Review's safe degree bound for pairs of messages of at most n bytes.
The harmless n=0 extension is 2; there are no distinct messages at that cap. -/
def D (n : ℕ) : ℕ :=
  if n ≤ 7 then 2 else if n ≤ 21 then 9 else if n ≤ 49 then 13 else 7*((n-1)/49)+14

theorem difference_degree {n : ℕ} (a b : Bytes) (ha : a.length ≤ n) (hb : b.length ≤ n) :
    (encode a - encode b).natDegree ≤ D n := by
  unfold D
  split_ifs with h7 h21 h49
  · rw [encode_short a (by omega), encode_short b (by omega)]
    simpa [tailPoly, show a.length ≤ 7 by omega, show b.length ≤ 7 by omega] using
      short_difference_degree (a.length : F) (b.length : F) (tailWords a) (tailWords b)
  · rw [encode_short a (by omega), encode_short b (by omega)]
    exact (natDegree_sub_le _ _).trans (max_le (tail_small_degree a (by omega)) (tail_small_degree b (by omega)))
  · rw [encode_short a (by omega), encode_short b (by omega)]
    exact (natDegree_sub_le _ _).trans (max_le (tail_degree_le a) (tail_degree_le b))
  · apply (natDegree_sub_le _ _).trans
    exact max_le ((encode_degree_le a).trans (by unfold blockCount; omega))
      ((encode_degree_le b).trans (by unfold blockCount; omega))

end ProvenHashes.Polymur
