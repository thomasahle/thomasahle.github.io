import Std

/- Executable source-level scalarization. Agreement tests are not a proof of
   refinement to the noncomputable probability model. All arithmetic is unsigned. -/
namespace ProvenHashes.Halftime.Exec

def mix (x k : UInt64) : UInt64 :=
  (x.toUInt32 + k.toUInt32).toUInt64 *
    ((x >>> 32).toUInt32 + (k >>> 32).toUInt32).toUInt64

def load (x : Array UInt8) (i : Nat) : UInt64 := Id.run do
  let mut w : UInt64 := 0
  for j in [:8] do
    w := w ||| (x[i+j]!.toUInt64 <<< UInt64.ofNat (8*j))
  return w

def t2 : Array (Array UInt64) := #[#[1,0,1,1,2,1,4], #[0,1,1,2,1,4,1]]
def t3 : Array (Array UInt64) :=
  #[#[0,0,1,4,1,1,2,2,1], #[1,1,0,0,1,4,1,2,2], #[1,4,1,1,0,0,2,1,2]]

-- For each data row 1..6 and source column, a mask of parity-8 destinations.
def parityMasks : Array (Array Nat) :=
  #[#[4,5,2], #[5,7,6], #[2,6,5], #[3,4,1], #[6,3,7], #[7,1,3]]

def encode (k : Nat) (data : Array UInt64) : Array UInt64 := Id.run do
  let d := if k == 2 then 6 else 7
  let mut a := data ++ Array.replicate (if k == 2 then 3 else 6) 0
  for c in [:3] do
    let mut p : UInt64 := 0
    for i in [:d] do p := p ^^^ a[3*i+c]!
    a := a.set! (3*d+c) p
  if k == 3 then
    for c in [:3] do a := a.set! (24+c) a[c]!
    for i in [:6] do
      for c in [:3] do
        for j in [:3] do
          if (parityMasks[i]![c]! >>> j) % 2 == 1 then
            a := a.set! (24+j) (a[24+j]! ^^^ a[3*(i+1)+c]!)
  return a

def leaf (b k : Nat) (key : Array UInt64) (x : Array UInt8) (start : Nat) : Array UInt64 := Id.run do
  let d := if k == 2 then 6 else 7
  let e := if k == 2 then 7 else 9
  let mat := if k == 2 then t2 else t3
  let mut out := Array.replicate (k*b) 0
  for lane in [:b] do
    let a := encode k ((List.range (3*d)).toArray.map (fun j => load x (start+8*(j*b+lane))))
    for s in [:e] do
      let mut h : UInt64 := 0
      for j in [:3] do h := h + mix a[3*s+j]! key[3*s+j]!
      for c in [:k] do out := out.set! (c*b+lane) (out[c*b+lane]! + mat[c]![s]! * h)
  return out

def node (b k level : Nat) (key : Array UInt64) (children : Array (Array UInt64)) : Array UInt64 := Id.run do
  let e := if k == 2 then 7 else 9
  let mut out := children[0]!
  for c in [:k] do
    for lane in [:b] do
      let mut h := out[c*b+lane]!
      for j in [1:8] do h := h + mix children[j]![c*b+lane]! key[3*e+7*k*level+7*c+j-1]!
      out := out.set! (c*b+lane) h
  return out

/-- Promotion before insertion; roots flattened low level first. -/
def forest (b k : Nat) (key : Array UInt64) (x : Array UInt8) : Array (Array UInt64) := Id.run do
  let group := (if k == 2 then 144 else 168)*b
  let mut stack : Array (Array (Array UInt64)) := Array.replicate 9 #[]
  for g in [:x.size / group] do
    let mut level := 0
    for j in [:8] do
      if level == j && stack[j]!.size == 8 then level := level+1
    for r in [:level] do
      let j := level-1-r
      let v := node b k j key stack[j]!
      stack := stack.set! (j+1) (stack[j+1]!.push v)
      stack := stack.set! j #[]
    stack := stack.set! 0 (stack[0]!.push (leaf b k key x (g*group)))
  return stack.foldl (· ++ ·) #[]

def core (b k : Nat) (key : Array UInt64) (x : Array UInt8) : Array UInt64 := Id.run do
  let d := if k == 2 then 6 else 7
  let e := if k == 2 then 7 else 9
  let start := 3*e+63*k
  let roots := forest b k key x
  let mut out := Array.replicate k (0 : UInt64)
  for r in [:roots.size] do
    for c in [:k] do
      for lane in [:b] do
        out := out.set! c (out[c]! + mix roots[r]![c*b+lane]! key[start+b*(k*r+c)+lane]!)
  let used := x.size / (24*d*b) * (24*d*b)
  let tail := x.extract used x.size
  let blocks := tail.size / (8*b) + 1
  let padded := tail ++ Array.replicate (blocks*8*b-tail.size) 0
  let tailStart := if k == 3 then start+192*b+(21-blocks)*b else start+roots.size*k*b
  for j in [:blocks*b] do
    for c in [:k] do
      out := out.set! c (out[c]! + mix (load padded (8*j)) key[tailStart+j+c*b]!)
  if k == 3 then
    for c in [:3] do out := out.set! c (out[c]! + mix (UInt64.ofNat x.size) key[start+213*b+c*b]!)
  return out

def tabulate (key : Array UInt64) (start : Nat) (x : UInt64) : UInt64 := Id.run do
  let mut h : UInt64 := 0
  for j in [:8] do h := h ^^^ key[start+256*j+((x >>> UInt64.ofNat (8*j)) &&& 255).toNat]!
  return h

def style (b : Nat) (key : Array UInt64) (x : Array UInt8) : UInt64 :=
  let h := core b 2 (key.extract 512 key.size) x
  tabulate key 0 (UInt64.ofNat x.size) ^^^ tabulate key 2048 h[0]! ^^^ tabulate key 4096 h[1]!

def next (s : UInt64) : UInt64 := s * 6364136223846793005 + 1442695040888963407

def fixture (seed : UInt64) (n : Nat) : Array UInt64 × Array UInt8 := Id.run do
  let mut s := seed
  let mut key := #[]
  for _ in [:8192] do
    s := next s
    key := key.push s
  let mut x := #[]
  for _ in [:n] do
    s := next s
    x := x.push (s >>> 56).toUInt8
  return (key,x)

def rotl (x : UInt64) (n : UInt64) : UInt64 := (x <<< n) ||| (x >>> (64-n))

def splitmix (s : UInt64) : UInt64 × UInt64 := Id.run do
  let s := s + 0x9e3779b97f4a7c15
  let mut z := s
  z := (z ^^^ (z >>> 30)) * 0xbf58476d1ce4e5b9
  z := (z ^^^ (z >>> 27)) * 0x94d049bb133111eb
  return (s, z ^^^ (z >>> 31))

/-- Literal deterministic expansion in the supplied fixed SMHasher adapter.
This function has only 64 seed bits, not independent uniform output words. -/
def seedExpand (seed : UInt64) : Array UInt64 := Id.run do
  let (m,w0) := splitmix seed
  let (m,x0) := splitmix m
  let (m,y0) := splitmix m
  let (_,z0) := splitmix m
  let mut w := w0
  let mut x := x0
  let mut y := y0
  let mut z := z0
  let mut key := #[]
  for i in [:9010] do
    let wp := w
    let xp := x
    let yp := y
    let zp := z
    w := zp * 15241094284759029579
    x := zp + rotl wp 52
    y := yp - xp
    z := rotl (yp + wp) 19
    if i >= 10 then key := key.push xp
  return key

end ProvenHashes.Halftime.Exec
