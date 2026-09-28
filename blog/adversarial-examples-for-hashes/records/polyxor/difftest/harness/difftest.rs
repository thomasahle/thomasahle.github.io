// Differential-test driver. stdin lines: "<entropy hex> <message hex> <split,split,...|->"
// stdout per line: "<raw hex> <avalanche hex> <stream hex> <resume hex> <reset hex>"
//   raw       = one-shot update + finalize_raw
//   avalanche = finalize_avalanche of the same state
//   stream    = finalize_raw after updates at the given split points
//   resume    = finalize_raw after interleaving finalize_raw calls between the split updates
//   reset     = hasher reused after reset() (garbage first), then one-shot
use polyxor::PolyXor128;
use std::io::{BufRead, Write};

fn unhex(s: &str) -> Vec<u8> {
    if s == "-" { return Vec::new(); }
    (0..s.len()).step_by(2).map(|i| u8::from_str_radix(&s[i..i + 2], 16).unwrap()).collect()
}

fn main() {
    let stdin = std::io::stdin();
    let mut out = std::io::BufWriter::new(std::io::stdout());
    for line in stdin.lock().lines() {
        let line = line.unwrap();
        let f: Vec<&str> = line.split(' ').collect();
        let ent = unhex(f[0]);
        let msg = unhex(f[1]);
        let splits: Vec<usize> = if f[2] == "-" { vec![] } else { f[2].split(',').map(|x| x.parse().unwrap()).collect() };
        let p = PolyXor128::from_entropy(&ent);
        let mut h = p.hasher();
        h.update(&msg);
        let raw = h.finalize_raw();
        let av = h.finalize_avalanche();
        assert_eq!(h.count(), msg.len() as u64);
        let mut s = p.hasher();
        let mut r = p.hasher();
        let mut prev = 0;
        for &cut in splits.iter().chain(std::iter::once(&msg.len())) {
            s.update(&msg[prev..cut]);
            r.update(&msg[prev..cut]);
            let _ = r.finalize_raw(); // pads the sponge in place; must not disturb later updates
            prev = cut;
        }
        let stream = s.finalize_raw();
        let resume = r.finalize_raw();
        let mut g = p.hasher();
        g.update(&[0xA5u8; 5000][..(msg.len() * 7 + 13) % 5000]);
        g.reset();
        g.update(&msg);
        let reset = g.finalize_raw();
        writeln!(out, "{raw:032x} {av:032x} {stream:032x} {resume:032x} {reset:032x}").unwrap();
    }
}
