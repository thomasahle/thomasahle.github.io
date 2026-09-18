// Independent native-Rust check of the blog's aHash pair A against the real crate.
// Hash of &[u8] via RandomState::hash_one (Hash for [u8] = write_usize(len) then write(bytes)).
// RNG: splitmix64 per thread (different from the C verifier's xoshiro and the reviewer's program).
use ahash::RandomState;
use std::hash::BuildHasher;

const PI2: [u64; 4] = [0x452821e638d01377, 0xbe5466cf34e90c6c, 0xc0ac29b7c97c50dd, 0x3f84d5b5b5470917];
const A: &str = "40313233343536373839253b3c3d3e3f408142094445464748494a4b4c4d4e4f8d896d065c5d5e5f606162636465666768696a6b6c6d6e6f";
const B: &str = "3e313233343536373839403b3c3d3e3f405e42204445464748494a4b4c4d4e4f727290085c5d5e5f606162636465666768696a6b6c6d6e6f";

fn hex(s: &str) -> Vec<u8> { (0..s.len()).step_by(2).map(|i| u8::from_str_radix(&s[i..i+2], 16).unwrap()).collect() }
fn sm(x: &mut u64) -> u64 { *x = x.wrapping_add(0x9e3779b97f4a7c15); let mut z = *x; z = (z ^ (z >> 30)).wrapping_mul(0xbf58476d1ce4e5b9); z = (z ^ (z >> 27)).wrapping_mul(0x94d049bb133111eb); z ^ (z >> 31) }

fn main() {
    let args: Vec<String> = std::env::args().collect();
    let lg: u32 = args.get(1).map(|s| s.parse().unwrap()).unwrap_or(24);
    let seed: u64 = args.get(2).map(|s| s.parse().unwrap()).unwrap_or(1);
    let threads: usize = args.get(3).map(|s| s.parse().unwrap()).unwrap_or(8);
    let (a, b) = (hex(A), hex(B));
    assert_eq!(a.len(), 56); assert_eq!(b.len(), 56);
    println!("cfg aes={} ssse3={} sse4.1={}", cfg!(target_feature = "aes"), cfg!(target_feature = "ssse3"), cfg!(target_feature = "sse4.1"));
    // Explicit key from the post: internal words k_i; with_seeds XORs each argument with PI2[i].
    let k = [0x711096b41e1b7d99u64, 0x59af3b7ae3b7d595, 0xd2fb75eb5658a771, 0x5a0f91d8ed1ffe38];
    let rs = RandomState::with_seeds(k[0]^PI2[0], k[1]^PI2[1], k[2]^PI2[2], k[3]^PI2[3]);
    println!("explicit key: H(A)={:016x} H(B)={:016x} (post says 45ef7682d72b19b6)", rs.hash_one(&a[..]), rs.hash_one(&b[..]));
    // Sanity: a key where they should differ.
    let rs0 = RandomState::with_seeds(1, 2, 3, 4);
    println!("control key (1,2,3,4): H(A)={:016x} H(B)={:016x}", rs0.hash_one(&a[..]), rs0.hash_one(&b[..]));
    // SMHasher3-style seed
    let s = 0xf04f16d5122a6a3au64; let rs = RandomState::with_seeds(s, s, s, s);
    println!("smh seed {:016x}: H(A)={:016x} H(B)={:016x} (README says 2dc0adbf4570e056)", s, rs.hash_one(&a[..]), rs.hash_one(&b[..]));
    let n = 1u64 << lg; let per = n / threads as u64;
    let t0 = std::time::Instant::now();
    let hs: Vec<_> = (0..threads).map(|t| { let a = a.clone(); let b = b.clone(); std::thread::spawn(move || {
        let mut st = seed.wrapping_mul(0x9E3779B97F4A7C15).rotate_left(17) ^ (t as u64).wrapping_mul(0xD1B54A32D192ED03);
        let mut c = 0u64; let mut first = None;
        for _ in 0..per {
            let (k0, k1, k2, k3) = (sm(&mut st), sm(&mut st), sm(&mut st), sm(&mut st));
            let rs = RandomState::with_seeds(k0, k1, k2, k3);
            let h = rs.hash_one(&a[..]);
            if h == rs.hash_one(&b[..]) { c += 1; if first.is_none() { first = Some(([k0,k1,k2,k3], h)); } }
        }
        (c, first) }) }).collect();
    let mut c = 0u64; let mut first = None;
    for h in hs { let (x, f) = h.join().unwrap(); c += x; if first.is_none() { first = f; } }
    let tot = per * threads as u64; let r = c as f64 / tot as f64;
    let (lo, hi) = (((c as f64) - 1.96*(c as f64).sqrt()).max(0.5)/tot as f64, ((c as f64) + 1.96*(c as f64).sqrt())/tot as f64);
    println!("with_seeds(uniform a,b,c,d), rng seed {}: {}/2^{} = 2^{:.3} [2^{:.3}, 2^{:.3}]; cap = log2(7)-log2(rate) = {:.3} [{:.3},{:.3}]; {:.0}s {} threads", seed, c, lg, r.log2(), lo.log2(), hi.log2(), 7f64.log2()-r.log2(), 7f64.log2()-hi.log2(), 7f64.log2()-lo.log2(), t0.elapsed().as_secs_f64(), threads);
    if let Some((k, h)) = first { println!("first colliding with_seeds args {:016x} {:016x} {:016x} {:016x} H={:016x}", k[0], k[1], k[2], k[3], h); }
}
