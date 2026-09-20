// Independent native check of proposed aHash trail pairs against the upstream crate ahash 0.8.12.
// Seed protocol = the blog row's rs4 model: RandomState::with_seeds(a,b,c,d) with four uniform
// u64 arguments (internal words k_i = arg_i ^ PI2[i], a bijection, so the internal words are
// four independent uniform words).  Hash = RandomState::hash_one(&[u8]) (write_usize(len), write(bytes)).
// usage: ahash_trail_verify pairs.txt log2N rngseed threads
//   pairs.txt: "m1hex m2hex label" per line.  All pairs are evaluated on the SAME key stream.
// RNG: xoshiro256** per thread, seeded by splitmix64 from (rngseed * 0x9E3779B97F4A7C15) ^ (thread * 0xD1B54A32D192ED03)
// -- a seeding distinct from both the searcher's C sampler (splitmix(rngseed+i)) and the row's
// native program (rngseed*0x100000001b3 + t), so the stream is fresh by construction.
use ahash::RandomState;
use std::hash::BuildHasher;

const PI2: [u64; 4] = [0x452821e638d01377, 0xbe5466cf34e90c6c, 0xc0ac29b7c97c50dd, 0x3f84d5b5b5470917];

fn hex(s: &str) -> Vec<u8> { (0..s.len()).step_by(2).map(|i| u8::from_str_radix(&s[i..i + 2], 16).unwrap()).collect() }

fn splitmix(s: &mut u64) -> u64 {
    *s = s.wrapping_add(0x9e3779b97f4a7c15); let mut z = *s;
    z = (z ^ (z >> 30)).wrapping_mul(0xbf58476d1ce4e5b9); z = (z ^ (z >> 27)).wrapping_mul(0x94d049bb133111eb); z ^ (z >> 31)
}
struct Xo([u64; 4]);
impl Xo {
    fn new(seed: u64) -> Self { let mut s = seed; let mut st = [0u64; 4]; for i in 0..4 { st[i] = splitmix(&mut s); } Xo(st) }
    #[inline] fn next(&mut self) -> u64 {
        let s = &mut self.0; let r = s[1].wrapping_mul(5).rotate_left(7).wrapping_mul(9); let t = s[1] << 17;
        s[2] ^= s[0]; s[3] ^= s[1]; s[1] ^= s[2]; s[0] ^= s[3]; s[2] ^= t; s[3] = s[3].rotate_left(45); r
    }
}

fn main() {
    let args: Vec<String> = std::env::args().collect();
    if args.len() < 3 { eprintln!("usage: {} pairs.txt log2N [rngseed] [threads]", args[0]); std::process::exit(2); }
    let text = std::fs::read_to_string(&args[1]).expect("pairs file");
    let lg: u32 = args[2].parse().unwrap();
    let seed: u64 = args.get(3).map(|s| s.parse().unwrap()).unwrap_or(1);
    let threads: usize = args.get(4).map(|s| s.parse().unwrap()).unwrap_or(8);
    let mut pairs: Vec<(Vec<u8>, Vec<u8>, String)> = vec![];
    for line in text.lines() {
        let f: Vec<&str> = line.split_whitespace().collect();
        if f.len() < 2 || line.starts_with('#') { continue; }
        pairs.push((hex(f[0]), hex(f[1]), f.get(2).unwrap_or(&"?").to_string()));
    }
    println!("ahash crate 0.8.12 (crates.io); cfg target_feature aes = {}, ssse3 = {}; {} pairs; N = 2^{} keys; rngseed {}; threads {}",
        cfg!(target_feature = "aes"), cfg!(target_feature = "ssse3"), pairs.len(), lg, seed, threads);
    // sanity: the row's explicit colliding internal key for the shipped pair
    let k = [0x711096b41e1b7d99u64, 0x59af3b7ae3b7d595, 0xd2fb75eb5658a771, 0x5a0f91d8ed1ffe38];
    let rs = RandomState::with_seeds(k[0] ^ PI2[0], k[1] ^ PI2[1], k[2] ^ PI2[2], k[3] ^ PI2[3]);
    for (a, b, lab) in &pairs {
        let (ha, hb) = (rs.hash_one(&a[..]), rs.hash_one(&b[..]));
        println!("  pair {:<20} len {}/{} B  xor {}  row-key H(m)={:016x} H(m')={:016x} {}", lab, a.len(), b.len(),
            a.iter().zip(b.iter()).map(|(x, y)| format!("{:02x}", x ^ y)).collect::<String>(), ha, hb, if ha == hb { "COLLIDE" } else { "differ" });
    }
    assert!(cfg!(target_feature = "aes"), "This witness requires the AES backend");
    let selected = pairs.iter().find(|(_,_,label)| label.starts_with("A_2")).expect("selected A_2 pair");
    let witness = RandomState::with_seeds(0xa9438ebec17194e4, 0x0bb688b615fedc93, 0x499051d9d7fdc675, 0x9b696bdcaa2076a5);
    assert_eq!(witness.hash_one(&selected.0[..]), 0x21a01e6ddf40c84a);
    assert_eq!(witness.hash_one(&selected.1[..]), 0x21a01e6ddf40c84a);
    println!("A_2 explicit with_seeds witness: 21a01e6ddf40c84a PASS");
    let n: u64 = 1u64 << lg; let per = n / threads as u64;
    let t0 = std::time::Instant::now();
    let np = pairs.len();
    let handles: Vec<_> = (0..threads).map(|t| { let pairs = pairs.clone(); std::thread::spawn(move || {
        let mut r = Xo::new(seed.wrapping_mul(0x9E3779B97F4A7C15) ^ (t as u64).wrapping_mul(0xD1B54A32D192ED03));
        let mut c = vec![0u64; np]; let mut first: Vec<Option<([u64; 4], u64)>> = vec![None; np];
        for _ in 0..per {
            let (a0, a1, a2, a3) = (r.next(), r.next(), r.next(), r.next());
            let rs = RandomState::with_seeds(a0, a1, a2, a3);
            for (i, (a, b, _)) in pairs.iter().enumerate() {
                let h = rs.hash_one(&a[..]);
                if h == rs.hash_one(&b[..]) { c[i] += 1; if first[i].is_none() { first[i] = Some(([a0, a1, a2, a3], h)); } }
            }
        }
        (c, first) }) }).collect();
    let mut c = vec![0u64; np]; let mut firsts: Vec<Option<([u64; 4], u64)>> = vec![None; np];
    for h in handles { let (x, f) = h.join().unwrap(); for i in 0..np { c[i] += x[i]; if firsts[i].is_none() { firsts[i] = f[i]; } } }
    let tot = per * threads as u64;
    println!("done: {} keys ({} per thread x {}), {:.1} s", tot, per, threads, t0.elapsed().as_secs_f64());
    for i in 0..np {
        let (a, _, lab) = &pairs[i];
        let l = (a.len() as f64 / 8.0).ceil();
        let cnt = c[i] as f64; let rate = cnt / tot as f64;
        let lo = (cnt - 1.96 * cnt.sqrt()).max(0.5) / tot as f64; let hi = (cnt + 1.96 * cnt.sqrt()) / tot as f64;
        println!("{:<20} {:>7}/2^{} = 2^{:.3}  95% CI [2^{:.3}, 2^{:.3}]  cap log2({})-log2(rate) = {:.3} bits, CI [{:.3}, {:.3}]",
            lab, c[i], lg, rate.log2(), lo.log2(), hi.log2(), l, l.log2() - rate.log2(), l.log2() - hi.log2(), l.log2() - lo.log2());
        if let Some((kk, h)) = firsts[i] {
            println!("    first colliding with_seeds args {:016x} {:016x} {:016x} {:016x} (internal k_i = arg_i ^ PI2[i]), H = {:016x}", kk[0], kk[1], kk[2], kk[3], h);
        }
    }
}
