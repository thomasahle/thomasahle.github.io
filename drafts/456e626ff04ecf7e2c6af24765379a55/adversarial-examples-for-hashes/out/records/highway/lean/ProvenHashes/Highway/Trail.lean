import ProvenHashes.Highway.TrailSteps
import ProvenHashes.Highway.InitialBound
namespace ProvenHashes.Highway
set_option maxRecDepth 10000
set_option maxHeartbeats 50000

theorem lo_rot32 (x : Word) : lo (rot32 x) = hi x := by
  unfold lo hi rot32
  simp only [HShiftLeft.hShiftLeft, HShiftRight.hShiftRight]
  bv_normalize

theorem reset_halves (k : Key) (hk : Class k) :
    hi (reset k).ab.a.v0 = 0 ∧ lo (reset k).ab.a.v1 = 0x10e82046 := by
  constructor
  · change hi ((0xdbe6d5d5fe4cce2f : Word) ^^^ k 0) = 0
    unfold hi
    rw [BitVec.extractLsb'_xor]
    change (0xdbe6d5d5 : Half) ^^^ hi (k 0) = 0
    rw [hk]
    exact BitVec.xor_self
  · change lo ((0x3bd39e10cb0ef593 : Word) ^^^ rot32 (k 0)) = 0x10e82046
    rw [show lo ((0x3bd39e10cb0ef593 : Word) ^^^ rot32 (k 0)) =
      lo (0x3bd39e10cb0ef593 : Word) ^^^ lo (rot32 (k 0)) from BitVec.extractLsb'_xor]
    rw [lo_rot32, hk]
    decide

theorem first_byte1 (a : Word) (ha : lo a = 0x10e82046) : byte a 1 ≠ 255 := by
  have haN := congrArg BitVec.toNat ha
  simp only [lo_toNat, BitVec.ofNat_eq_ofNat, BitVec.toNat_ofNat] at haN
  have H := (nat_lowbytes a.toNat haN).2.1
  intro hb
  have hbN := congrArg BitVec.toNat hb
  simp only [byte_toNat, BitVec.ofNat_eq_ofNat, BitVec.toNat_ofNat] at hbN
  omega

theorem packet1_carry (k : Key) (hk : Class k) :
    byte (pairStep 0x24192a2a01b331d1 0 (reset k).ab).a.v0 5 ≠ 255 := by
  obtain ⟨hz, ha⟩ := reset_halves k hk
  have hp : (reset k).ab.a.mul0 + (0x24192a2a01b331d1 : Word) = 0 := first_injection.1
  have hv : (laneStep 0x24192a2a01b331d1 (reset k).ab.a).v1 = (reset k).ab.a.v1 := by
    change (reset k).ab.a.v1 + (reset k).ab.a.mul0 + 0x24192a2a01b331d1 = _
    rw [add_assoc, hp, add_zero]
  change byte ((reset k).ab.a.v0 + (reset k).ab.a.mul1 +
    zip0 (laneStep 0x24192a2a01b331d1 (reset k).ab.a).v1
      (laneStep 0 (reset k).ab.b).v1) 5 ≠ 255
  rw [hv]
  exact first_byte5_bound _ _ _ _ hz rfl ha

theorem pair_trail (s : PairState) (p q r t : Word)
    (h1 : pairStep r 0 s = bump1 (pairStep p 0 s))
    (h2 : pairStep t 0 (bump1 (pairStep p 0 s)) = bump2 (pairStep q 0 (pairStep p 0 s))) :
    pairStep 0 0 (pairStep q 0 (pairStep p 0 s)) =
      pairStep 256 0 (pairStep t 0 (pairStep r 0 s)) := by
  rw [h1, h2]
  exact (third_packet_cancellation _).symm

theorem first_packet_trail (k : Key) (hk : Class k) :
    pairStep 0x24192a2a01b332d1 0 (reset k).ab =
      bump1 (pairStep 0x24192a2a01b331d1 0 (reset k).ab) := by
  obtain ⟨hz, hlo⟩ := reset_halves k hk
  have hb := first_byte1 _ hlo
  have hp : (reset k).ab.a.mul0 + (0x24192a2a01b331d1 : Word) = 0 := first_injection.1
  have hc := packet1_carry k hk
  have hfirst := pair_first (reset k).ab 0x24192a2a01b331d1 hz hp hb hc
  exact (congrArg (fun p => pairStep p 0 (reset k).ab) packet_differences.1).trans hfirst

theorem second_packet_trail (k : Key) (hk : Class k) (he : E2 k) :
    pairStep 0x24192a2ab3b330d1 0
      (bump1 (pairStep 0x24192a2a01b331d1 0 (reset k).ab)) =
      bump2 (pairStep 0x24192a2ab4b332d1 0
        (pairStep 0x24192a2a01b331d1 0 (reset k).ab)) := by
  have hsecond := pair_second
    (pairStep 0x24192a2a01b331d1 0 (reset k).ab) 0x24192a2ab4b332d1 he (packet1_carry k hk)
  exact (congrArg (fun p => pairStep p 0
    (bump1 (pairStep 0x24192a2a01b331d1 0 (reset k).ab))) packet_differences.2).trans hsecond

def threeSteps (p q r : Word) (s : State) : State :=
  update ![r,0,0,0] (update ![q,0,0,0] (update ![p,0,0,0] s))

theorem threeSteps_eq (p q r : Word) (s : State) :
    threeSteps p q r s = ⟨pairStep r 0 (pairStep q 0 (pairStep p 0 s.ab)),
      pairStep 0 0 (pairStep 0 0 (pairStep 0 0 s.cd))⟩ := by rfl

theorem state_trail (s : State) (p q r t : Word)
    (h1 : pairStep r 0 s.ab = bump1 (pairStep p 0 s.ab))
    (h2 : pairStep t 0 (bump1 (pairStep p 0 s.ab)) =
      bump2 (pairStep q 0 (pairStep p 0 s.ab))) :
    threeSteps p q 0 s = threeSteps r t 256 s := by
  rw [threeSteps_eq, threeSteps_eq]
  exact congrArg (fun a : PairState => State.mk a
    (pairStep 0 0 (pairStep 0 0 (pairStep 0 0 s.cd)))) (pair_trail s.ab p q r t h1 h2)

theorem absorb_three (k : Key) (p q r : Word) :
    absorb k [![p,0,0,0], ![q,0,0,0], ![r,0,0,0]] = threeSteps p q r (reset k) := rfl

/-- The three-packet trail, for the actual full 1024-bit state and actual keys. -/
theorem trail_state_collision (k : Key) (hk : Class k) (he : E2 k) :
    StateCollision k := by
  unfold StateCollision messageA messageB
  rw [absorb_three, absorb_three]
  exact state_trail (reset k) 0x24192a2a01b331d1 0x24192a2ab4b332d1
    0x24192a2a01b332d1 0x24192a2ab3b330d1
    (first_packet_trail k hk) (second_packet_trail k hk he)

theorem trail_outputs (k : Key) (h : Trail k) :
    finalize64 (absorb k messageA) = finalize64 (absorb k messageB) ∧
    finalize128 (absorb k messageA) = finalize128 (absorb k messageB) ∧
    finalize256 (absorb k messageA) = finalize256 (absorb k messageB) :=
  deterministic_outputs (absorb k messageA) (absorb k messageB)
    (trail_state_collision k h.1 h.2)

end ProvenHashes.Highway
