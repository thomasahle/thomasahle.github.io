//! museair_long: MuseAir v2 (crate museair 0.6.0, unmodified) beyond 96 bytes.
//! Startup: SMHasher verification values from the crate's stability_v2 test for
//! museair::hash (0x7140CABC) and museair::bfast::hash (0xA4BFD093); exit 1 on mismatch.
//! M0: the 80-byte every-seed product-exchange pair (museair2_swap.c, museair2_check), sanity check.
//! M1: the same 80 bytes preceded by a common random 96-byte chunk (176 B): the
//!     exchange words now meet the finalize after one compress round.
//! M2: the same 80 bytes as the FIRST 80 bytes of a 192-byte message with a common
//!     random remainder: the exchange words now go through the compress round.
//! RNG: splitmix64 -> xoshiro256**.  Usage: museair_long [log2_seeds=24]
fn verif(f: &dyn Fn(&[u8], u64) -> u64) -> u32 {
    let mut key = [0u8; 256];
    let mut table = Vec::with_capacity(256 * 8);
    for i in 0..256usize {
        key[i] = i as u8;
        table.extend_from_slice(&f(&key[..i], (256 - i) as u64).to_le_bytes());
    }
    let h = f(&table, 0).to_le_bytes();
    u32::from_le_bytes([h[0], h[1], h[2], h[3]])
}
struct Rng { s: [u64; 4] }
impl Rng {
    fn new(mut x: u64) -> Rng {
        let mut s = [0u64; 4];
        for v in s.iter_mut() {
            x = x.wrapping_add(0x9e3779b97f4a7c15);
            let mut z = x;
            z = (z ^ (z >> 30)).wrapping_mul(0xbf58476d1ce4e5b9);
            z = (z ^ (z >> 27)).wrapping_mul(0x94d049bb133111eb);
            *v = z ^ (z >> 31);
        }
        Rng { s }
    }
    fn next(&mut self) -> u64 {
        let s = &mut self.s;
        let r = s[1].wrapping_mul(5).rotate_left(7).wrapping_mul(9);
        let t = s[1] << 17;
        s[2] ^= s[0]; s[3] ^= s[1]; s[1] ^= s[2]; s[0] ^= s[3]; s[2] ^= t; s[3] = s[3].rotate_left(45);
        r
    }
}
fn unhex(s: &str) -> Vec<u8> { (0..s.len()).step_by(2).map(|i| u8::from_str_radix(&s[i..i + 2], 16).unwrap()).collect() }
fn count(a: &[u8], b: &[u8], n: u64, rng: &mut Rng) -> (u64, u64) {
    let (mut c1, mut c2) = (0u64, 0u64);
    for _ in 0..n {
        let s = rng.next();
        c1 += (museair::hash(a, s) == museair::hash(b, s)) as u64;
        c2 += (museair::bfast::hash(a, s) == museair::bfast::hash(b, s)) as u64;
    }
    (c1, c2)
}
fn main() {
    let lg: u32 = std::env::args().nth(1).map(|s| s.parse().unwrap()).unwrap_or(24);
    let v1 = verif(&|m, s| museair::hash(m, s));
    let v2 = verif(&|m, s| museair::bfast::hash(m, s));
    println!("verification museair::hash {:08X} (expected 7140CABC), bfast::hash {:08X} (expected A4BFD093)", v1, v2);
    if v1 != 0x7140CABC || v2 != 0xA4BFD093 { std::process::exit(1); }
    let m = unhex("909dea05e80e02d243265f3567044597aa01c47742b811c67ad20fc2a1b5b144000000000000008017cedf86b534a0667d8a71f0e684fc7ff1bc7e2b4f655fdb3319d238bcff82080000000000000080");
    let mp = unhex("909dea05e80e025243265f3567044597aa01c47742b811467ad20fc2a1b5b1c4000000000000008017cedf86b534a0e67d8a71f0e684fc7ff1bc7e2b4f655fdb3319d238bcff82080000000000000080");
    let mut rng = Rng::new(0x4d7573654c6f6e67);
    let n0 = 1u64 << 20;
    let (a, b) = count(&m, &mp, n0, &mut rng);
    println!("M0 80 B pair: hash {}/{}, bfast {}/{} seeds collide", a, n0, b, n0);
    let n = 1u64 << lg;
    let mut pre = vec![0u8; 96];
    for x in pre.iter_mut() { *x = rng.next() as u8; }
    let a1: Vec<u8> = pre.iter().chain(m.iter()).cloned().collect();
    let b1: Vec<u8> = pre.iter().chain(mp.iter()).cloned().collect();
    let (a, b) = count(&a1, &b1, n, &mut rng);
    println!("M1 176 B (common 96-byte chunk + the 80-byte pair): hash {}/{}, bfast {}/{} seeds collide", a, n, b, n);
    let mut rest = vec![0u8; 112];
    for x in rest.iter_mut() { *x = rng.next() as u8; }
    let a2: Vec<u8> = m.iter().chain(rest.iter()).cloned().collect();
    let b2: Vec<u8> = mp.iter().chain(rest.iter()).cloned().collect();
    let (a, b) = count(&a2, &b2, n, &mut rng);
    println!("M2 192 B (the 80-byte pair as the start of the first compress chunk + common 112 B): hash {}/{}, bfast {}/{} seeds collide", a, n, b, n);
}
