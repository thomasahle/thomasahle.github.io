// Independent check of the MuseAir v2 (crate museair 0.6.0) 80-byte product-exchange pair.
// Uses the crate source unmodified; checks the crate's stability_v2 values first.
// Usage: museair2_check <log2 seeds> <rng seed>
use museair::{bfast, hash, hash128, hash128_folded, hash_folded};

struct Rng { s: [u64; 4] }
fn splitmix(x: &mut u64) -> u64 {
    *x = x.wrapping_add(0x9e3779b97f4a7c15);
    let mut z = *x;
    z = (z ^ (z >> 30)).wrapping_mul(0xbf58476d1ce4e5b9);
    z = (z ^ (z >> 27)).wrapping_mul(0x94d049bb133111eb);
    z ^ (z >> 31)
}
impl Rng {
    fn new(seed: u64) -> Self { let mut x = seed; Rng { s: [splitmix(&mut x), splitmix(&mut x), splitmix(&mut x), splitmix(&mut x)] } }
    fn next(&mut self) -> u64 {
        let s = &mut self.s;
        let r = s[1].wrapping_mul(5).rotate_left(7).wrapping_mul(9);
        let t = s[1] << 17;
        s[2] ^= s[0]; s[3] ^= s[1]; s[1] ^= s[2]; s[0] ^= s[3]; s[2] ^= t; s[3] = s[3].rotate_left(45);
        r
    }
}
fn unhex(s: &str) -> Vec<u8> { (0..s.len()).step_by(2).map(|i| u8::from_str_radix(&s[i..i + 2], 16).unwrap()).collect() }

fn main() {
    let expected = [0x7140CABCu32, 0x0B8F0243, 0x38028C88, 0xB9CD57B7, 0xA4BFD093, 0xDCCDD53A, 0x81863E77, 0x9BAAAF63];
    let calc = [
        hashverify::compute(64, |b, s, o| o.copy_from_slice(&hash(b, s).to_le_bytes())),
        hashverify::compute(32, |b, s, o| o.copy_from_slice(&hash_folded(b, s).to_le_bytes())),
        hashverify::compute(128, |b, s, o| o.copy_from_slice(&hash128(b, s, s).to_le_bytes())),
        hashverify::compute(64, |b, s, o| o.copy_from_slice(&hash128_folded(b, s, s).to_le_bytes())),
        hashverify::compute(64, |b, s, o| o.copy_from_slice(&bfast::hash(b, s).to_le_bytes())),
        hashverify::compute(32, |b, s, o| o.copy_from_slice(&bfast::hash_folded(b, s).to_le_bytes())),
        hashverify::compute(128, |b, s, o| o.copy_from_slice(&bfast::hash128(b, s, s).to_le_bytes())),
        hashverify::compute(64, |b, s, o| o.copy_from_slice(&bfast::hash128_folded(b, s, s).to_le_bytes())),
    ];
    if calc != expected { eprintln!("stability_v2 MISMATCH {:08X?}", calc); std::process::exit(1); }
    println!("stability_v2 values match");
    let args: Vec<String> = std::env::args().collect();
    let lg: u32 = args[1].parse().unwrap();
    let rs: u64 = args[2].parse().unwrap();
    let m = unhex("909dea05e80e02d243265f3567044597aa01c47742b811c67ad20fc2a1b5b144000000000000008017cedf86b534a0667d8a71f0e684fc7ff1bc7e2b4f655fdb3319d238bcff82080000000000000080");
    let mp = unhex("909dea05e80e025243265f3567044597aa01c47742b811467ad20fc2a1b5b1c4000000000000008017cedf86b534a0e67d8a71f0e684fc7ff1bc7e2b4f655fdb3319d238bcff82080000000000000080");
    assert_eq!(m.len(), 80); assert!(m != mp);
    let diff: Vec<usize> = (0..80).filter(|&i| m[i] != mp[i]).collect();
    println!("differing bytes: {:?} (xor {:?})", diff, diff.iter().map(|&i| m[i] ^ mp[i]).collect::<Vec<_>>());
    let mut rng = Rng::new(rs);
    let (mut c64, mut cb, mut c128, mut cb128) = (0u64, 0u64, 0u64, 0u64);
    let special = [0u64, u64::MAX, 0xAAAAAAAAAAAAAAAA, 0x5555555555555555, 1, 1 << 63];
    let n = 1u64 << lg;
    for t in 0..(n + special.len() as u64) {
        let s = if (t as usize) < special.len() { special[t as usize] } else { rng.next() };
        let h = hash(&m, s) == hash(&mp, s);
        let hb = bfast::hash(&m, s) == bfast::hash(&mp, s);
        if !h || !hb { println!("FAIL seed {:016x} hash {} bfast {}", s, h, hb); }
        c64 += h as u64; cb += hb as u64;
        if t < 1 << 16 {
            let s2 = rng.next();
            c128 += (hash128(&m, s, s2) == hash128(&mp, s, s2)) as u64;
            cb128 += (bfast::hash128(&m, s, s2) == bfast::hash128(&mp, s, s2)) as u64;
        }
    }
    let tot = n + special.len() as u64;
    println!("hash: {}/{}  bfast::hash: {}/{}  (incl. {} special seeds)", c64, tot, cb, tot, special.len());
    println!("hash128 (random seed_a, seed_b): {} / 65536; bfast::hash128: {} / 65536", c128, cb128);
    println!("seed 0: {:016x} {:016x}", hash(&m, 0), hash(&mp, 0));
    // 96-byte variant built from the stated relations: r4 = 2^63, B2 = A1, t0 = 0, B4 = A3 ^ 2^63;
    // flip bit 63 of r3, r6, t1, t2 (t_i = r_{8+i}).
    const C: [u64; 6] = [0x5ae31e589c56e17a, 0x96d7bb04e64f6da9, 0x7ab1006b26f9eb64, 0x21233394220b8457, 0x047cb9557c9f3b43, 0xd24f2590c0bcee28];
    let tb = 1u64 << 63;
    let mut r: Vec<u64> = (0..12).map(|_| rng.next()).collect();
    r[4] = tb;
    r[5] = C[3] ^ C[1] ^ r[1] ^ r[2];
    r[8] = 0;
    r[9] = C[5] ^ C[3] ^ r[5] ^ r[6] ^ tb;
    let mut rp = r.clone();
    for &w in &[3usize, 6, 9, 10] { rp[w] ^= tb; }
    let to_b = |v: &Vec<u64>| v.iter().flat_map(|x| x.to_le_bytes()).collect::<Vec<u8>>();
    let (m96, mp96) = (to_b(&r), to_b(&rp));
    println!("M96  = {}", m96.iter().map(|b| format!("{:02x}", b)).collect::<String>());
    println!("M96' = {}", mp96.iter().map(|b| format!("{:02x}", b)).collect::<String>());
    let (mut d1, mut d2) = (0u64, 0u64);
    let n2 = 1u64 << (lg.min(22));
    for _ in 0..n2 { let s = rng.next(); d1 += (hash(&m96, s) == hash(&mp96, s)) as u64; d2 += (bfast::hash(&m96, s) == bfast::hash(&mp96, s)) as u64; }
    println!("96-byte variant: hash {}/{}  bfast {}/{}", d1, n2, d2, n2);
}
