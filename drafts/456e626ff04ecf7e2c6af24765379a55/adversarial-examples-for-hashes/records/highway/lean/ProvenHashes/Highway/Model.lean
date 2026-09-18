import Mathlib

/-! The frozen portable C HighwayHash algorithm, on complete 32-byte packets.
`zip0 a b` and `zip1 a b` use the low lane `a` and high lane `b`.
All word operations are unsigned operations modulo 2^64. No remainder is used
by the fixed 96-byte pair. -/
namespace ProvenHashes.Highway

abbrev Word := BitVec 64
abbrev Half := BitVec 32
abbrev Lanes := Fin 4 → Word
abbrev Key := Lanes
abbrev Packet := Lanes

def lo (x : Word) : Half := x.extractLsb' 0 32
def hi (x : Word) : Half := x.extractLsb' 32 32
def widen (x : Half) : Word := x.zeroExtend 64
def rot32 (x : Word) : Word := (x >>> 32) ||| (x <<< 32)

def init0 : Lanes := ![0xdbe6d5d5fe4cce2f, 0xa4093822299f31d0,
  0x13198a2e03707344, 0x243f6a8885a308d3]
def init1 : Lanes := ![0x3bd39e10cb0ef593, 0xc0acf169b5f18a8c,
  0xbe5466cf34e90c6c, 0x452821e638d01377]

@[ext] structure Lane where
  v0 : Word
  v1 : Word
  mul0 : Word
  mul1 : Word
  deriving DecidableEq, Repr

@[ext] structure PairState where
  a : Lane
  b : Lane
  deriving DecidableEq, Repr

/-- Two independent lane pairs during absorption; sixteen 64-bit words. -/
@[ext] structure State where
  ab : PairState
  cd : PairState
  deriving DecidableEq, Repr

def resetLane (c0 c1 k : Word) : Lane :=
  ⟨c0 ^^^ k, c1 ^^^ rot32 k, c0, c1⟩

def reset (k : Key) : State :=
  ⟨⟨resetLane (init0 0) (init1 0) (k 0),
     resetLane (init0 1) (init1 1) (k 1)⟩,
   ⟨resetLane (init0 2) (init1 2) (k 2),
     resetLane (init0 3) (init1 3) (k 3)⟩⟩

/-- The C ZipperMergeAndAdd expression for the low output. -/
def zip0 (a b : Word) : Word :=
  (((a &&& 0xff000000) ||| (b &&& 0xff00000000)) >>> 24) |||
  (((a &&& 0xff0000000000) ||| (b &&& 0xff000000000000)) >>> 16) |||
  (a &&& 0xff0000) ||| ((a &&& 0xff00) <<< 32) |||
  ((b &&& 0xff00000000000000) >>> 8) ||| (a <<< 56)

/-- The C ZipperMergeAndAdd expression for the high output. -/
def zip1 (a b : Word) : Word :=
  (((b &&& 0xff000000) ||| (a &&& 0xff00000000)) >>> 24) |||
  (b &&& 0xff0000) ||| ((b &&& 0xff0000000000) >>> 16) |||
  ((b &&& 0xff00) <<< 24) ||| ((a &&& 0xff000000000000) >>> 8) |||
  ((b &&& 0xff) <<< 48) ||| (a &&& 0xff00000000000000)

/-- Packet addition and both 32 × 32 → 64 products, in C evaluation order. -/
def laneStep (p : Word) (s : Lane) : Lane :=
  let v1 := s.v1 + s.mul0 + p
  let m0 := s.mul0 ^^^ (widen (lo v1) * widen (hi s.v0))
  let v0 := s.v0 + s.mul1
  let m1 := s.mul1 ^^^ (widen (lo v0) * widen (hi v1))
  ⟨v0, v1, m0, m1⟩

def zipper (s : PairState) : PairState :=
  let a0 := s.a.v0 + zip0 s.a.v1 s.b.v1
  let b0 := s.b.v0 + zip1 s.a.v1 s.b.v1
  ⟨{ s.a with v0 := a0, v1 := s.a.v1 + zip0 a0 b0 },
   { s.b with v0 := b0, v1 := s.b.v1 + zip1 a0 b0 }⟩

def pairStep (p q : Word) (s : PairState) : PairState :=
  zipper ⟨laneStep p s.a, laneStep q s.b⟩

def update (p : Packet) (s : State) : State :=
  ⟨pairStep (p 0) (p 1) s.ab, pairStep (p 2) (p 3) s.cd⟩

def absorb (k : Key) (ps : List Packet) : State := ps.foldl (fun s p => update p s) (reset k)

/-- Little-endian byte decoding of a single complete packet. -/
def read64 (b : Fin 8 → BitVec 8) : Word :=
  (List.finRange 8).foldl (fun w j => w ||| ((b j).zeroExtend 64 <<< (8 * j.val))) 0

def decodePacket (b : Fin 32 → BitVec 8) : Packet :=
  fun i => read64 (fun j => b ⟨i.val * 8 + j.val, by omega⟩)

def messageA : List Packet := [![0x24192a2a01b331d1, 0, 0, 0],
  ![0x24192a2ab4b332d1, 0, 0, 0], ![0, 0, 0, 0]]
def messageB : List Packet := [![0x24192a2a01b332d1, 0, 0, 0],
  ![0x24192a2ab3b330d1, 0, 0, 0], ![0x100, 0, 0, 0]]

def permuteUpdate (s : State) : State :=
  update ![rot32 s.cd.a.v0, rot32 s.cd.b.v0, rot32 s.ab.a.v0, rot32 s.ab.b.v0] s

def rounds : Nat → State → State
  | 0, s => s
  | n + 1, s => rounds n (permuteUpdate s)

def finalize64 (s : State) : Word :=
  let t := (rounds 4 s).ab.a
  t.v0 + t.v1 + t.mul0 + t.mul1

def finalize128 (s : State) : Word × Word :=
  let t := rounds 6 s
  (t.ab.a.v0 + t.ab.a.mul0 + t.cd.a.v1 + t.cd.a.mul1,
   t.ab.b.v0 + t.ab.b.mul0 + t.cd.b.v1 + t.cd.b.mul1)

def modularReduction (a3 a2 a1 a0 : Word) : Word × Word :=
  let a3 := a3 &&& 0x3fffffffffffffff
  (a0 ^^^ (a2 <<< 1) ^^^ (a2 <<< 2),
   a1 ^^^ ((a3 <<< 1) ||| (a2 >>> 63)) ^^^ ((a3 <<< 2) ||| (a2 >>> 62)))

def reducePair (s : PairState) : Word × Word :=
  modularReduction (s.b.v1 + s.b.mul1) (s.a.v1 + s.a.mul1)
    (s.b.v0 + s.b.mul0) (s.a.v0 + s.a.mul0)

def finalize256 (s : State) : (Word × Word) × (Word × Word) :=
  let t := rounds 10 s
  (reducePair t.ab, reducePair t.cd)

def Class (k : Key) : Prop := hi (k 0) = 0xdbe6d5d5

/-- Event evaluated after packet-2 addition, before its first multiply. -/
def E2 (k : Key) : Prop :=
  let s := update (![0x24192a2a01b331d1, 0, 0, 0]) (reset k)
  lo (s.ab.a.v1 + s.ab.a.mul0 + 0x24192a2ab4b332d1) - hi s.ab.a.v0 = 256

def Trail (k : Key) : Prop := Class k ∧ E2 k
def StateCollision (k : Key) : Prop := absorb k messageA = absorb k messageB

theorem message_lengths : messageA.length * 4 = 12 ∧ messageB.length * 4 = 12 := by
  decide

theorem messages_distinct : messageA ≠ messageB := by
  intro h
  have h0 := congrArg (fun ps => (ps.headD (fun _ => 0)) 0) h
  exact (by decide : (0x24192a2a01b331d1 : Word) ≠ 0x24192a2a01b332d1) h0

theorem deterministic_outputs (s t : State) (h : s = t) :
    finalize64 s = finalize64 t ∧ finalize128 s = finalize128 t ∧
      finalize256 s = finalize256 t := by
  subst t
  exact ⟨rfl, rfl, rfl⟩

theorem common_suffix (s t : State) (h : s = t) (ps : List Packet) :
    ps.foldl (fun s p => update p s) s = ps.foldl (fun s p => update p s) t := by
  rw [h]

end ProvenHashes.Highway
