// Independent verifier for the muse-v2 witness: links the published crate museair 0.6.0
// (crates.io tarball) directly. Counts seeds s (uniform 64-bit) with hash(M,s)==hash(M2,s)
// for the four public functions. RNG: xoshiro256** seeded by splitmix64 (own stream).
use std::env;
use std::thread;

fn hex(s: &str) -> Vec<u8> {
    (0..s.len()).step_by(2).map(|i| u8::from_str_radix(&s[i..i + 2], 16).unwrap()).collect()
}
fn splitmix(x: &mut u64) -> u64 {
    *x = x.wrapping_add(0x9E3779B97F4A7C15);
    let mut z = *x;
    z = (z ^ (z >> 30)).wrapping_mul(0xBF58476D1CE4E5B9);
    z = (z ^ (z >> 27)).wrapping_mul(0x94D049BB133111EB);
    z ^ (z >> 31)
}
struct Xo([u64; 4]);
impl Xo {
    fn new(master: u64, t: u64) -> Xo {
        let mut x = master ^ t.wrapping_mul(0xD1B54A32D192ED03).wrapping_add(0x2545F4914F6CDD1D);
        Xo([splitmix(&mut x), splitmix(&mut x), splitmix(&mut x), splitmix(&mut x)])
    }
    fn next(&mut self) -> u64 {
        let s = &mut self.0;
        let r = s[1].wrapping_mul(5).rotate_left(7).wrapping_mul(9);
        let t = s[1] << 17;
        s[2] ^= s[0]; s[3] ^= s[1]; s[1] ^= s[2]; s[0] ^= s[3]; s[2] ^= t; s[3] = s[3].rotate_left(45);
        r
    }
}
fn ci(c: u64, n: f64, l: f64) -> String {
    // Poisson 95% CI on the count (Wilson-Hilferty), converted to log2 rate and to bits = log2(L/eps)
    let z = 1.959964;
    let lo = if c == 0 { 0.0 } else { let k = c as f64; k * (1.0 - 1.0 / (9.0 * k) - z / (3.0 * k.sqrt())).powi(3) };
    let k1 = (c + 1) as f64;
    let hi = k1 * (1.0 - 1.0 / (9.0 * k1) + z / (3.0 * k1.sqrt())).powi(3);
    let rate = |x: f64| if x > 0.0 { (x / n).log2() } else { f64::NEG_INFINITY };
    format!("rate 2^{:.3} [2^{:.3}, 2^{:.3}]  bits {:.3} [{:.3}, {:.3}]",
        rate(c as f64), rate(lo), rate(hi),
        l.log2() - rate(c as f64), l.log2() - rate(hi), l.log2() - rate(lo))
}
fn main() {
    let a: Vec<String> = env::args().collect();
    if a.len() < 6 { eprintln!("usage: verify M_hex M2_hex log2N master_rng threads [example_seed_hex...]"); std::process::exit(2); }
    let m = hex(&a[1]); let m2 = hex(&a[2]);
    assert_eq!(m.len(), m2.len()); assert_ne!(m, m2);
    let log2n: u32 = a[3].parse().unwrap();
    let master = u64::from_str_radix(a[4].trim_start_matches("0x"), 16).unwrap();
    let threads: u64 = a[5].parse().unwrap();
    let n: u64 = 1u64 << log2n;
    let l = ((m.len() + 7) / 8) as f64;
    println!("museair crate 0.6.0 (crates.io tarball) len {} L {} N 2^{} master 0x{:x} threads {}", m.len(), l, log2n, master, threads);
    for s in &a[6..] {
        let seed = u64::from_str_radix(s.trim_start_matches("0x"), 16).unwrap();
        println!("seed 0x{:016x}: hash {:016x} {:016x} eq={}  bfast {:016x} {:016x} eq={}", seed,
            museair::hash(&m, seed), museair::hash(&m2, seed), museair::hash(&m, seed) == museair::hash(&m2, seed),
            museair::bfast::hash(&m, seed), museair::bfast::hash(&m2, seed), museair::bfast::hash(&m, seed) == museair::bfast::hash(&m2, seed));
    }
    let per = n / threads;
    let mask: u64 = env::var("VERIFY_MASK").ok().map(|v| u64::from_str_radix(v.trim_start_matches("0x"),16).unwrap()).unwrap_or(0);
    if mask != 0 { println!("seed class: seed &= !0x{:016x}", mask); }
    let hs: Vec<_> = (0..threads).map(|t| {
        let (m, m2) = (m.clone(), m2.clone());
        thread::spawn(move || {
            let mut rng = Xo::new(master, t);
            let mut c = [0u64; 4];
            let mut first: [Option<(u64, u64, u64)>; 4] = [None; 4];
            for _ in 0..per {
                let s = rng.next() & !mask; let sb = rng.next();
                let h = [
                    (museair::hash(&m, s) as u128, museair::hash(&m2, s) as u128),
                    (museair::bfast::hash(&m, s) as u128, museair::bfast::hash(&m2, s) as u128),
                    (museair::hash128(&m, s, sb), museair::hash128(&m2, s, sb)),
                    (museair::bfast::hash128(&m, s, sb), museair::bfast::hash128(&m2, s, sb)),
                ];
                for k in 0..4 { if h[k].0 == h[k].1 { c[k] += 1; if first[k].is_none() { first[k] = Some((s, sb, h[k].0 as u64)); } } }
            }
            (c, first)
        })
    }).collect();
    let mut c = [0u64; 4]; let mut first: [Option<(u64, u64, u64)>; 4] = [None; 4];
    for h in hs { let (cc, f) = h.join().unwrap(); for k in 0..4 { c[k] += cc[k]; if first[k].is_none() { first[k] = f[k]; } } }
    let names = ["hash", "bfast::hash", "hash128", "bfast::hash128"];
    let total = (per * threads) as f64;
    for k in 0..4 {
        println!("{:16} {:>8} / {}  {}", names[k], c[k], per * threads, ci(c[k], total, l));
        if let Some((s, sb, h)) = first[k] { println!("{:16}   first colliding seed 0x{:016x} (seed_b 0x{:016x}) -> low64 {:016x}", "", s, sb, h); }
    }
}
