import ProvenHashes.Highway.Trail
import ProvenHashes.Highway.WordEquations
namespace ProvenHashes.Highway
set_option maxRecDepth 10000
set_option maxHeartbeats 500000

def firstU0 (s : PairState) : Word :=
  s.a.v0+s.a.mul1+zip0 s.a.v1 (s.b.v1+s.b.mul0)
def firstU1 (s : PairState) : Word :=
  s.b.v0+s.b.mul1+zip1 s.a.v1 (s.b.v1+s.b.mul0)

theorem first_pair_data (s : PairState) (p : Word)
    (hz : hi s.a.v0 = 0) (hp : s.a.mul0+p=0) :
    (pairStep p 0 s).a.v0 = firstU0 s ∧
    (pairStep p 0 s).b.v0 = firstU1 s ∧
    (pairStep p 0 s).a.v1 = s.a.v1+zip0 (firstU0 s) (firstU1 s) ∧
    (pairStep p 0 s).a.mul0 = s.a.mul0 := by
  have ha : s.a.v1+s.a.mul0+p = s.a.v1 := by rw [add_assoc, hp, add_zero]
  simp only [pairStep, zipper, laneStep, firstU0, firstU1, ha, hz, add_zero]
  simp [widen]

theorem e2_first_formula (k : Key) (hk : Class k) :
    E2 k ↔ lo ((reset k).ab.a.v1 +
      zip0 (firstU0 (reset k).ab) (firstU1 (reset k).ab) + 0xb3000100) -
      hi (firstU0 (reset k).ab) = 256 := by
  have h := first_pair_data (reset k).ab 0x24192a2a01b331d1
    (reset_halves k hk).1 first_injection.1
  change lo ((pairStep 0x24192a2a01b331d1 0 (reset k).ab).a.v1 +
    (pairStep 0x24192a2a01b331d1 0 (reset k).ab).a.mul0 + 0x24192a2ab4b332d1) -
    hi (pairStep 0x24192a2a01b331d1 0 (reset k).ab).a.v0 = 256 ↔ _
  rw [h.1, h.2.2.1, h.2.2.2, add_assoc]
  change (lo (_ + ((0xdbe6d5d5fe4cce2f : Word)+0x24192a2ab4b332d1)) - _) = 256 ↔ _
  rw [first_injection.2]

def firstW (k : Key) : Word := (reset k).ab.b.v1+(reset k).ab.b.mul0
def keyGamma (k : Key) : Nat :=
  firstGamma (reset k).ab.a.v0.toNat (byte (reset k).ab.a.v1 5).toNat
    (byte (firstW k) 4).toNat

def ByteConditions (k : Key) : Prop :=
  let w := firstW k
  let u := firstU0 (reset k).ab
  let v := firstU1 (reset k).ab
  let g := keyGamma k
  (byte w 6).toNat+g ≤ 239 ∧ 21 ≤ (byte w 7).toNat ∧
  (byte u 3).toNat = (202+(byte w 6).toNat+g)%256 ∧
  (byte u 2).toNat = (235+(byte w 7).toNat)%256 ∧
  (byte v 4).toNat = 157 + if 54 ≤ (byte w 6).toNat+g then 1 else 0

/-- An exact byte characterization of the actual key predicate, under the key class. -/
theorem e2_byte_conditions (k : Key) (hk : Class k) : E2 k ↔ ByteConditions k := by
  obtain ⟨hz, ha⟩ := reset_halves k hk
  have H := initial_summary (reset k).ab.a.v0 (reset k).ab.a.mul1
    (reset k).ab.a.v1 (firstW k) hz rfl ha
  have hNat : (lo (reset k).ab.a.v1).toNat = 0x10e82046 := by
    rw [ha]
    rfl
  rw [e2_first_formula k hk]
  exact word_e2_equations (reset k).ab.a.v1 (0xb3000100 : Word)
    (firstU0 (reset k).ab) (firstU1 (reset k).ab)
    (byte (firstW k) 6) (byte (firstW k) 7) (keyGamma k)
    hNat rfl H.1 H.2.1 H.2.2

end ProvenHashes.Highway
