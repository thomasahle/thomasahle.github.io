import ProvenHashes.Halftime.Executable
open ProvenHashes.Halftime.Exec

def main (args : List String) : IO Unit := do
  if args.head! == "seeds" then
    for s in [0,1,18446744073709551615,3680580360085262969] do
      IO.println (String.intercalate " " ((seedExpand (UInt64.ofNat s)).toList.map (fun x => toString x.toNat)))
    return
  if args.head! == "encoders" then
    for k in [2,3] do
      let d := if k == 2 then 6 else 7
      let data := (List.range (3*d)).toArray.map (fun j => (1 : UInt64) <<< UInt64.ofNat j)
      IO.println (String.intercalate " " ((encode k data).toList.map (fun x => toString x.toNat)))
    return
  let path := args.head!
  let lines := (← IO.FS.readFile path).splitOn "\n"
  let mut count := 0
  for line in lines do
    if line.isEmpty then continue
    let a := (line.splitOn " ").toArray.map String.toNat!
    let b := a[1]!
    let (key,x) := fixture (UInt64.ofNat a[0]!) a[2]!
    let got := #[style b key x] ++ core b 3 key x
    let expected := (a.extract 3 7).map UInt64.ofNat
    unless got == expected do
      throw (IO.userError s!"mismatch case={count} b={b} n={x.size}: {got} != {expected}")
    count := count+1
  IO.println s!"PASS: {count} cases, four outputs each; Lean executable vs fixed C++"
