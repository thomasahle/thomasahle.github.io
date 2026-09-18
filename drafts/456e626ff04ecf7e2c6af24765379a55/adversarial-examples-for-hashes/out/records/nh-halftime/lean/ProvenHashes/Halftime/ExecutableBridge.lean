import ProvenHashes.Halftime.Executable
import ProvenHashes.Halftime.WordPacking

namespace ProvenHashes.Halftime

/-- The unsigned halves used by the executable scalarization. -/
def machineHalves (x : UInt64) : Bool → ZMod (2 ^ 32)
  | false => x.toUInt32.toNat
  | true => (x >>> 32).toUInt32.toNat

theorem machine_mix_agrees_nh32 (x k : UInt64) :
    ((Exec.mix x k).toNat : ZMod (2 ^ 64)) =
      nh32 (fun p : Fin 1 × Bool => machineHalves x p.2)
        (fun p => machineHalves k p.2) := by
  simp [Exec.mix, nh32, nh32Equiv, nh, machineHalves, ZMod.val_add,
    ZMod.val_natCast, Nat.add_mod, ZMod.natCast_mod, Nat.cast_mul]

end ProvenHashes.Halftime
