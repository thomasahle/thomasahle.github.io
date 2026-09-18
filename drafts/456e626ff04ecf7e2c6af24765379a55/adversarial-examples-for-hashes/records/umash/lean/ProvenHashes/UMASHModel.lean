import ProvenHashes.ClassicCore
import ProvenHashes.UMASHPrime

/-! Literal word model of the supplied `umash_reference.py`.
The ideal reference key samples 34 distinct OH words and a multiplier in
`{2,...,p-1}`.  Fingerprinting below has TWO independent multipliers, as in C;
`referenceFingerprint` separately records the shared-multiplier Python API.
No probabilistic or projection claim is part of these definitions. -/
namespace ProvenHashes.UMASH

def q : ℕ := 2 ^ 64
abbrev Word := BitVec 64
abbrev Wide := BitVec 128
abbrev Chunk := Word × Word
abbrev OHKey := Fin 34 → Word
abbrev Byte := Fin 256
abbrev Message := List Byte
abbrev Field := ZMod p

def xorChunk (x y : Chunk) : Chunk := (x.1 ^^^ y.1, x.2 ^^^ y.2)
def split (x : Wide) : Chunk := (x.setWidth 64, (x >>> 64).setWidth 64)
def join (x : Chunk) : Wide := x.1.zeroExtend 128 ||| (x.2.zeroExtend 128 <<< 64)

/-- Full carry-less 64 by 64 product, without field reduction. -/
def clmul (a b : Word) : Wide :=
  (List.range 64).foldl (fun acc i =>
    acc ^^^ (if a.getLsbD i then b.zeroExtend 128 <<< i else 0)) 0

def ph (key data : Chunk) : Chunk :=
  split (clmul (data.1 ^^^ key.1) (data.2 ^^^ key.2))

/-- The tag occupies the HIGH word; the high word is then XORed with the low.
Both additions of input words wrap at 64 bits; the tagged product wraps at 128. -/
def enh (key data : Chunk) (tag : Word) : Chunk :=
  let a := (data.1 + key.1).zeroExtend 128
  let b := (data.2 + key.2).zeroExtend 128
  let v := split (a * b + (tag.zeroExtend 128 <<< 64))
  (v.1, v.2 ^^^ v.1)

def keyWord (k : OHKey) (i : ℕ) : Word := k ⟨i % 34, Nat.mod_lt _ (by decide)⟩
def keyPair (k : OHKey) (i : ℕ) : Chunk := (keyWord k (2*i), keyWord k (2*i+1))

structure Block where
  chunks : List Chunk
  byteSize : ℕ
  deriving DecidableEq

def Block.Valid (b : Block) : Prop :=
  0 < b.byteSize ∧ b.byteSize ≤ 256 ∧
  b.chunks.length = (b.byteSize + 15) / 16

def blockTag (seed : Word) (b : Block) : Word :=
  seed ^^^ BitVec.ofNat 64 (b.byteSize % 256)

def checksum (k : OHKey) (b : Block) : Chunk :=
  (b.chunks.mapIdx (fun i x => xorChunk x (keyPair k i))).foldl xorChunk (0, 0)

def mixed (k : OHKey) (b : Block) (seed : Word) : List Chunk :=
  b.chunks.mapIdx fun i x =>
    if i+1 < b.chunks.length then ph (keyPair k i) x
    else enh (keyPair k i) x (blockTag seed b)

def laneShift (x : Chunk) (s : ℕ) : Chunk := (x.1 <<< s, x.2 <<< s)

/-- The reference passes the ONE-based index of an original chunk. -/
def shuffle (x : Chunk) (i n : ℕ) : Chunk :=
  let s := n-i
  if s = 0 then x else if s = 1 then laneShift x 1
  else xorChunk (laneShift x s) (laneShift x 1)

def oh (k : OHKey) (b : Block) (seed : Word) : Chunk :=
  (mixed k b seed).foldl xorChunk (0, 0)

def ohSecondary (k : OHKey) (b : Block) (seed : Word) : Chunk :=
  let twist := ph (keyWord k 32, keyWord k 33) (checksum k b)
  ((mixed k b seed).mapIdx (fun i v => shuffle v (i+1) b.chunks.length)).foldl
    xorChunk twist

def project (x : Chunk) : Field × Field := (x.1.toNat, x.2.toNat)

def loadLE (xs : List Byte) : ℕ :=
  xs.foldr (fun b acc => b.val + 256 * acc) 0

def readWord (xs : List Byte) : Word := BitVec.ofNat 64 (loadLE xs)

/-- Overlap the last full 16 bytes, except that 9--15 bytes overlap the first
and last eight-byte words, exactly as in `chunk_bytes`. -/
def chunkAt (m : Message) (i : ℕ) : Chunk :=
  if m.length < 16 then
    (readWord (m.take 8), readWord (m.drop (m.length-8)))
  else
    let start := min (16*i) (m.length-16)
    (readWord ((m.drop start).take 8), readWord ((m.drop (start+8)).take 8))

def encodeBlock (m : Message) (i : ℕ) : Block :=
  let size := min 256 (m.length - 256*i)
  ⟨(List.range ((size+15)/16)).map (fun j => chunkAt m (16*i+j)), size⟩

def encode (m : Message) : List Block :=
  (List.range ((m.length+255)/256)).map (encodeBlock m)

def compress (secondary : Bool) (k : OHKey) (seed : Word) (m : Message) : List Chunk :=
  (encode m).map (fun b => if secondary then ohSecondary k b seed else oh k b seed)

/-- The actual double-pumped recurrence uses representatives modulo q-8,
and reduces the SQUARE of the multiplier modulo p. -/
def polyStep (f acc : ℕ) (x : Chunk) : ℕ :=
  ((f*f % p) * (acc + x.1.toNat) + f*x.2.toNat) % (q-8)

def polyReduce (f : ℕ) (xs : List Chunk) (initial : ℕ := 0) : ℕ :=
  xs.foldl (polyStep f) initial

/-- Field-level positive-power polynomial: low coefficient precedes high
coefficient in Horner order, and each coefficient is followed by multiplication. -/
noncomputable def blockPolynomial (xs : List Chunk) : Polynomial Field :=
  Classic.positive ((xs.flatMap fun x => [project x |>.1, project x |>.2]).reverse)

def finalize (x : Word) : Word := x ^^^ x.rotateLeft 8 ^^^ x.rotateLeft 33

def shortPack (m : Message) : Word :=
  let lo := if 4 ≤ m.length then loadLE (m.take 4)
    else if m.length % 2 = 1 then loadLE (m.take 1) else 0
  let hi := if 4 ≤ m.length then loadLE (m.drop (m.length-4))
    else if 2 ≤ m.length then loadLE (m.drop (m.length-2)) else 0
  BitVec.ofNat 64 (hi * 2^32 + ((hi+lo) % 2^32))

def shortHash (k : OHKey) (seed : Word) (m : Message) (secondary : Bool) : Word :=
  let noise := seed + keyWord k (m.length + if secondary then 4 else 0)
  let h := shortPack m
  let h := h ^^^ (h >>> 30)
  let h := h * 0xBF58476D1CE4E5B9
  let h := h ^^^ (h >>> 27) ^^^ noise
  let h := h * 0x94D049BB133111EB
  h ^^^ (h >>> 31)

abbrev PolyKey := {f : Fin p // 1 < f.val}
abbrev DistinctOHKey := {k : OHKey // Function.Injective k}
abbrev Key64 := DistinctOHKey × PolyKey
abbrev Key128 := DistinctOHKey × (PolyKey × PolyKey)

def hashWith (k : OHKey) (f : ℕ) (seed : Word) (m : Message)
    (secondary : Bool := false) : Word :=
  if m.length ≤ 8 then shortHash k seed m secondary
  else finalize (BitVec.ofNat 64 (polyReduce f (compress secondary k seed m)))

def hash64 (k : Key64) (seed : Word) (m : Message) : Word :=
  hashWith k.1.val k.2.val.val seed m

def hash128 (k : Key128) (seed : Word) (m : Message) : Word × Word :=
  (hashWith k.1.val k.2.1.val.val seed m,
   hashWith k.1.val k.2.2.val.val seed m true)

def referenceFingerprint (k : Key64) (seed : Word) (m : Message) : Word × Word :=
  (hash64 k seed m, hashWith k.1.val k.2.val.val seed m true)

end ProvenHashes.UMASH
