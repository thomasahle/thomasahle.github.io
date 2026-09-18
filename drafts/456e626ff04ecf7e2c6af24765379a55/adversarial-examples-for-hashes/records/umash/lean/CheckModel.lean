import ProvenHashes.UMASHModel
open ProvenHashes.UMASH

def checkKey (profile : ℕ) : OHKey := fun i => BitVec.ofNat 64
  (if profile = 0 then i.val else if profile = 1 then q-1-i.val
   else i.val*0x9E3779B97F4A7C15)
def checkMessage (profile n : ℕ) : Message :=
  (List.range n).map fun i =>
    ⟨(if profile = 0 then 13*i+7 else if profile = 1 then 255
      else if i%3 = 0 then 0 else 255)%256, Nat.mod_lt _ (by decide)⟩

def main : IO Unit := do
  for profile in [0,1,2] do
    let seed : Word := if profile = 0 then 42 else if profile = 1 then -1
      else 0xDEADBEEF12345678
    let f0 := if profile = 0 then 1234567 else if profile = 1 then 2 else p-1
    let f1 := if profile = 0 then 9876543 else if profile = 1 then p-1 else p-2
    for n in [0, 1, 2, 3, 4, 7, 8, 9, 15, 16, 17, 31, 32, 255, 256, 257, 511, 512, 513, 1025] do
      let m := checkMessage profile n
      let h0 := hashWith (checkKey profile) f0 seed m false
      let h1 := hashWith (checkKey profile) f1 seed m true
      IO.println s!"{profile},{n},{h0.toNat},{h1.toNat}"
