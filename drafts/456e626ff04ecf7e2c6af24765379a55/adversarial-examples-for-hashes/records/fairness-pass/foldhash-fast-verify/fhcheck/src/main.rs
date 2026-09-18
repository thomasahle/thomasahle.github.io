use foldhash::{SharedSeed, fast, quality};
use std::hash::{Hash, Hasher};
fn sm(x: &mut u64) -> u64 { *x = x.wrapping_add(0x9E3779B97F4A7C15); let mut z = *x; z = (z ^ (z >> 30)).wrapping_mul(0xBF58476D1CE4E5B9); z = (z ^ (z >> 27)).wrapping_mul(0x94D049BB133111EB); z ^ (z >> 31) }
fn main() {
    let a: Vec<String> = std::env::args().collect();
    let lg: u32 = a[1].parse().unwrap(); let th: u64 = a[2].parse().unwrap();
    let base = u64::from_str_radix(a[3].trim_start_matches("0x"), 16).unwrap();
    let m0 = [0u8; 8]; let m1 = [0xffu8; 8];
    let per = (1u64 << lg) / th;
    let hs: Vec<_> = (0..th).map(|t| std::thread::spawn(move || {
        let mut st = base ^ t.wrapping_mul(0xD1B54A32D192ED03);
        let (mut cr, mut cs, mut cq) = (0u64, 0u64, 0u64);
        for _ in 0..per {
            let ph = sm(&mut st); let shared_local = SharedSeed::from_u64(sm(&mut st));
            let shared: &'static SharedSeed = unsafe { std::mem::transmute(&shared_local) };
            let mut h0 = fast::FoldHasher::with_seed(ph, shared); h0.write(&m0);
            let mut h1 = fast::FoldHasher::with_seed(ph, shared); h1.write(&m1);
            if h0.finish() == h1.finish() { cr += 1; }
            let mut h0 = fast::FoldHasher::with_seed(ph, shared); (&m0[..]).hash(&mut h0);
            let mut h1 = fast::FoldHasher::with_seed(ph, shared); (&m1[..]).hash(&mut h1);
            if h0.finish() == h1.finish() { cs += 1; }
            let mut h0 = quality::FoldHasher::with_seed(ph, shared); (&m0[..]).hash(&mut h0);
            let mut h1 = quality::FoldHasher::with_seed(ph, shared); (&m1[..]).hash(&mut h1);
            if h0.finish() == h1.finish() { cq += 1; }
        }
        (cr, cs, cq)
    })).collect();
    let (mut r, mut s, mut q) = (0, 0, 0);
    for h in hs { let (a, b, c) = h.join().unwrap(); r += a; s += b; q += c; }
    let n = (per * th) as f64;
    println!("trials=2^{} fast_raw={} fast_slice={} quality_slice={} eps_raw=2^{:.4} bits={:.4}", lg, r, s, q, (r as f64 / n).log2(), (n / r as f64).log2());
}
