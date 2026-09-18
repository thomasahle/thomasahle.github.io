import ProvenHashes.Highway.Model
namespace ProvenHashes.Highway

def stateWords (s : State) : List Word :=
  let ls := [s.ab.a, s.ab.b, s.cd.a, s.cd.b]
  ls.map Lane.v0 ++ ls.map Lane.v1 ++ ls.map Lane.mul0 ++ ls.map Lane.mul1

def vectorKeys : List Key := [![0, 0, 0, 0], ![1, 2, 3, 4],
  ![0xdbe6d5d58afad71e, 0xa0142b42de197939, 0x5bd2b2861106bd66, 0xb6c304527caad524],
  ![0xdbe6d5d58afad71f, 0xa0142b42de197939, 0x5bd2b2861106bd66, 0xb6c304527caad524]]

def vectorMessages : List (List Packet) := [messageA, messageB,
  [decodePacket (fun i => BitVec.ofNat 8 i.val)]]

def vectorRow (k : Key) (ps : List Packet) : List Nat :=
  let s := absorb k ps
  let h128 := finalize128 s
  let h256 := finalize256 s
  (stateWords s ++ [finalize64 s, h128.1, h128.2,
    h256.1.1, h256.1.2, h256.2.1, h256.2.2]).map BitVec.toNat

-- Executable comparisons, deliberately not presented as proofs.
#eval do
  for k in vectorKeys do
    for ps in vectorMessages do
      IO.println ("[" ++ String.intercalate ", " ((vectorRow k ps).map toString) ++ "]")

end ProvenHashes.Highway
