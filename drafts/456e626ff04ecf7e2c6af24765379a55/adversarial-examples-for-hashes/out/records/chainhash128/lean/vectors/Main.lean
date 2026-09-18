import Std

namespace ChainHash128Vectors

def mask : Nat := 2^128-1

def raw (a b : Nat) : Nat := Id.run do
  let mut r := 0
  for i in [:128] do
    if b.testBit i then r := r ^^^ (a <<< i)
  return r

def reduce (a : Nat) : Nat := Id.run do
  let mut r := a
  for j in [:128] do
    let i := 255-j
    if r.testBit i then r := r ^^^ (((2^128 : Nat) + 135) <<< (i-128))
  return r

def mul (a b : Nat) : Nat := reduce (raw a b)

def wordAt (m : Array Nat) (offset : Nat) : Nat := Id.run do
  let mut r := 0
  for i in [:16] do
    r := r + (m[offset+i]?.getD 0 <<< (8*i))
  return r

def hash (k m : Array Nat) : Nat := Id.run do
  let n := max 1 ((m.size+511)/512)
  let mut state := k[34]!
  for t in [:n] do
    let bytes := min 512 (m.size-512*t)
    let mut acc := 0
    for g in [:(bytes+63)/64] do
      let j := 4*g
      let a := wordAt m (512*t+16*j) ^^^ k[j]!
      let b := wordAt m (512*t+16*(j+1)) ^^^ k[j+1]!
      let c := wordAt m (512*t+16*(j+2)) ^^^ k[j+2]!
      let d := wordAt m (512*t+16*(j+3)) ^^^ k[j+3]!
      acc := acc ^^^ raw a c ^^^ raw b d
    let tag := if t+1=n then m.size else 0
    let a := (acc &&& mask) ^^^ tag
    let b := (acc >>> 128) ^^^ tag
    state := a ^^^ mul (b ^^^ k[33]!) (state ^^^ k[32]!)
  let v := (state+k[40]!) &&& mask
  let y := mul v v
  let z := mul (y ^^^ k[35]!) (v ^^^ y ^^^ k[36]!)
  return mul (v ^^^ k[37]!) (z ^^^ k[38]!) ^^^ k[39]!

def expand (k : Array Nat) : Array Nat := Id.run do
  let s := k[0]!
  let mut p := s
  let mut out := #[]
  for _ in [:32] do
    out := out.push p
    p := mul p s
  for i in [:9] do out := out.push k[i+1]!
  return out

end ChainHash128Vectors

def main (args : List String) : IO Unit := do
  let input ← IO.FS.readFile args[0]!
  let out ← IO.getStdout
  for line in input.splitOn "\n" do
    if line.isEmpty then continue
    let xs := (line.splitOn " ").map String.toNat!
    let a := xs.toArray
    let seeded := a[0]!
    let n := a[1]!
    let count := if seeded=1 then 10 else 41
    let mut key := #[]
    for i in [:count] do key := key.push (a[2+2*i]! + (a[3+2*i]! <<< 64))
    if seeded=1 then key := ChainHash128Vectors.expand key
    let msg := a.extract (2+2*count) (2+2*count+n)
    let h := ChainHash128Vectors.hash key msg
    out.putStrLn s!"{h &&& (2^64-1)} {h >>> 64}"
