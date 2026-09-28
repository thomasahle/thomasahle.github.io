// Tightness witnesses through the shipped public API (from_entropy + hasher).
// stdin: "<name> <4160-byte entropy hex>" per line.
use polyxor::PolyXor128;
use std::io::BufRead;
fn main() {
    for line in std::io::stdin().lock().lines() {
        let line = line.unwrap();
        let (name, hex) = line.split_once(' ').unwrap();
        let ent: Vec<u8> = (0..hex.len()).step_by(2).map(|i| u8::from_str_radix(&hex[i..i + 2], 16).unwrap()).collect();
        let p = PolyXor128::from_entropy(&ent);
        let h = |m: &[u8]| { let mut h = p.hasher(); h.update(m); (h.finalize_raw(), h.finalize_avalanche()) };
        let (r0, a0) = h(&[0x00]);
        let (r1, a1) = h(&[0x01]);
        println!("{name:22} raw {r0:032x} {r1:032x} collide={}  avalanche collide={}", r0 == r1, a0 == a1);
    }
}
