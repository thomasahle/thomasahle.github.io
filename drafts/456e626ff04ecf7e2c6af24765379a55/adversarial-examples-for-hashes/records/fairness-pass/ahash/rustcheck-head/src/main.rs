// Native Rust check of the blog post's aHash pair A against the real crate.
// Hashes &[u8] through RandomState::hash_one (write_usize(len) then write(bytes)).
use ahash::RandomState;
use std::hash::BuildHasher;

const PI2: [u64; 4] = [0x452821e638d01377, 0xbe5466cf34e90c6c, 0xc0ac29b7c97c50dd, 0x3f84d5b5b5470917];
const A_HEX: &str = "40313233343536373839253b3c3d3e3f408142094445464748494a4b4c4d4e4f8d896d065c5d5e5f606162636465666768696a6b6c6d6e6f";
const B_HEX: &str = "3e313233343536373839403b3c3d3e3f405e42204445464748494a4b4c4d4e4f727290085c5d5e5f606162636465666768696a6b6c6d6e6f";

fn hex(s: &str) -> Vec<u8> { (0..s.len()).step_by(2).map(|i| u8::from_str_radix(&s[i..i + 2], 16).unwrap()).collect() }

struct Xo([u64; 4]);
impl Xo {
    fn new(seed: u64) -> Self {
        let mut s = seed; let mut st = [0u64; 4];
        for i in 0..4 { s = s.wrapping_add(0x9e3779b97f4a7c15); let mut z = s; z = (z ^ (z >> 30)).wrapping_mul(0xbf58476d1ce4e5b9); z = (z ^ (z >> 27)).wrapping_mul(0x94d049bb133111eb); st[i] = z ^ (z >> 31); }
        Xo(st)
    }
    fn next(&mut self) -> u64 {
        let s = &mut self.0; let r = s[1].wrapping_mul(5).rotate_left(7).wrapping_mul(9); let t = s[1] << 17;
        s[2] ^= s[0]; s[3] ^= s[1]; s[1] ^= s[2]; s[0] ^= s[3]; s[2] ^= t; s[3] = s[3].rotate_left(45); r
    }
}

fn main() {
    let args: Vec<String> = std::env::args().collect();
    let lg: u32 = args.get(1).map(|s| s.parse().unwrap()).unwrap_or(24);
    let seed: u64 = args.get(2).map(|s| s.parse().unwrap()).unwrap_or(1);
    let threads: usize = args.get(3).map(|s| s.parse().unwrap()).unwrap_or(8);
    let a = hex(A_HEX); let b = hex(B_HEX);
    println!("ahash crate path build; cfg target_feature aes = {}, ssse3 = {}", cfg!(target_feature = "aes"), cfg!(target_feature = "ssse3"));
    let k = [0x711096b41e1b7d99u64, 0x59af3b7ae3b7d595, 0xd2fb75eb5658a771, 0x5a0f91d8ed1ffe38];
    let rs = RandomState::with_seeds(k[0] ^ PI2[0], k[1] ^ PI2[1], k[2] ^ PI2[2], k[3] ^ PI2[3]);
    let (ha, hb) = (rs.hash_one(&a[..]), rs.hash_one(&b[..]));
    println!("explicit internal key {:016x} {:016x} {:016x} {:016x}: H(A)={:016x} H(B)={:016x} {} (post: 45ef7682d72b19b6)", k[0], k[1], k[2], k[3], ha, hb, if ha == hb { "COLLIDE" } else { "differ" });
    let s = 0xf04f16d5122a6a3au64; let rs = RandomState::with_seeds(s, s, s, s);
    let (ha, hb) = (rs.hash_one(&a[..]), rs.hash_one(&b[..]));
    println!("smh seed {:016x} (with_seeds(s,s,s,s)): H(A)={:016x} H(B)={:016x} {} (README: 2dc0adbf4570e056)", s, ha, hb, if ha == hb { "COLLIDE" } else { "differ" });
    let n: u64 = 1u64 << lg; let per = n / threads as u64;
    let t0 = std::time::Instant::now();
    let handles: Vec<_> = (0..threads).map(|t| { let a = a.clone(); let b = b.clone(); std::thread::spawn(move || {
        let mut r = Xo::new(seed.wrapping_mul(0x100000001b3).wrapping_add(t as u64)); let mut c = 0u64; let mut first = None;
        for _ in 0..per {
            let (k0, k1, k2, k3) = (r.next(), r.next(), r.next(), r.next());
            let rs = RandomState::with_seeds(k0, k1, k2, k3);
            let h = rs.hash_one(&a[..]);
            if h == rs.hash_one(&b[..]) { c += 1; if first.is_none() { first = Some(([k0, k1, k2, k3], h)); } }
        }
        (c, first) }) }).collect();
    let mut c = 0u64; let mut firsts = vec![];
    for h in handles { let (x, f) = h.join().unwrap(); c += x; if let Some(f) = f { firsts.push(f); } }
    let tot = per * threads as u64; let rate = c as f64 / tot as f64;
    let lo = (c as f64 - 1.96 * (c as f64).sqrt()).max(0.5) / tot as f64; let hi = (c as f64 + 1.96 * (c as f64).sqrt()) / tot as f64;
    println!("native Rust, RandomState::with_seeds(uniform a,b,c,d) [= four independent uniform internal words], rng seed {}: {}/2^{} = 2^{:.3}; 95% CI [2^{:.3}, 2^{:.3}]; score cap log2(7)-log2(rate) = {:.3} bits, CI [{:.3}, {:.3}]; {:.1} s on {} threads",
        seed, c, lg, rate.log2(), lo.log2(), hi.log2(), 7f64.log2() - rate.log2(), 7f64.log2() - hi.log2(), 7f64.log2() - lo.log2(), t0.elapsed().as_secs_f64(), threads);
    if let Some((k, h)) = firsts.first() { println!("first colliding with_seeds args {:016x} {:016x} {:016x} {:016x} (internal k_i = arg_i ^ PI2[i]), H = {:016x}", k[0], k[1], k[2], k[3], h); }
}
