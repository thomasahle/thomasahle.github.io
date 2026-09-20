// Independent cross-check: do the recorded MuseAir v0.3 key-free pairs collide
// on the upstream crates at tags v0.3, v0.4, v1 and crate-0.6.0 (algorithm v2)?
fn unhex(s: &str) -> Vec<u8> { (0..s.len()).step_by(2).map(|i| u8::from_str_radix(&s[i..i+2], 16).unwrap()).collect() }
fn splitmix(x: &mut u64) -> u64 { *x = x.wrapping_add(0x9E3779B97F4A7C15); let mut z = *x; z = (z ^ (z >> 30)).wrapping_mul(0xBF58476D1CE4E5B9); z = (z ^ (z >> 27)).wrapping_mul(0x94D049BB133111EB); z ^ (z >> 31) }
fn main() {
    let log2n: u32 = std::env::args().nth(1).map(|s| s.parse().unwrap()).unwrap_or(24);
    let n = 1u64 << log2n;
    let pairs = [
        ("17B", "0000000000000000000000000000000000", "8079763bb19a00001a9a1100642d3a3f01"),
        ("24B", "000000000000000000000000000000000000000000000000", "7cd5c18245c15e8ef47bfef8b79181a80101010101010101"),
        ("32B", "000000000000000000000000000000005a5a5a5a5a5a5a5a5a5a5a5a5a5a5a5a", "dd4bd4006a79dcff6a28a852aefcfbbca5a5a5a5a5a5a5a5a5a5a5a5a5a5a5a5"),
    ];
    // Published example seed for the 17-byte pair: expect 0xd4ed417ecc529ae4 on v0.3 hash().
    let ex = 0x2cb0f69f4abea221u64;
    let (a, b) = (unhex(pairs[0].1), unhex(pairs[0].2));
    println!("v0.3 hash(A,ex)={:#018x} hash(B,ex)={:#018x} (published 0xd4ed417ecc529ae4)", m03::hash(&a, ex), m03::hash(&b, ex));
    println!("v0.3 bfast::hash(A,ex)={:#018x} bfast::hash(B,ex)={:#018x}", m03::bfast::hash(&a, ex), m03::bfast::hash(&b, ex));
    println!("v0.3 hash_128(A,ex)={:#034x}", m03::hash_128(&a, ex));
    println!("v2   hash(A,ex)={:#018x} hash(B,ex)={:#018x}", m2::hash(&a, ex), m2::hash(&b, ex));
    println!("seeds: {} = 2^{} (splitmix64 from 0x1234, second seed independent)", n, log2n);
    for (name, ah, bh) in pairs.iter() {
        let (a, b) = (unhex(ah), unhex(bh));
        let mut c = [[0u64; 4]; 4]; // [version][api]
        let mut st = 0x1234u64;
        for _ in 0..n {
            let s = splitmix(&mut st); let s2 = splitmix(&mut st);
            c[0][0] += (m03::hash(&a, s) == m03::hash(&b, s)) as u64;
            c[0][1] += (m03::bfast::hash(&a, s) == m03::bfast::hash(&b, s)) as u64;
            c[0][2] += (m03::hash_128(&a, s) == m03::hash_128(&b, s)) as u64;
            c[0][3] += (m03::bfast::hash_128(&a, s) == m03::bfast::hash_128(&b, s)) as u64;
            c[1][0] += (m04::hash(&a, s) == m04::hash(&b, s)) as u64;
            c[1][1] += (m04::bfast::hash(&a, s) == m04::bfast::hash(&b, s)) as u64;
            c[1][2] += (m04::hash_128(&a, s) == m04::hash_128(&b, s)) as u64;
            c[1][3] += (m04::bfast::hash_128(&a, s) == m04::bfast::hash_128(&b, s)) as u64;
            c[2][0] += (m1::hash(&a, s) == m1::hash(&b, s)) as u64;
            c[2][1] += (m1::bfast::hash(&a, s) == m1::bfast::hash(&b, s)) as u64;
            c[2][2] += (m1::hash128(&a, s, s2) == m1::hash128(&b, s, s2)) as u64;
            c[2][3] += (m1::bfast::hash128(&a, s, s2) == m1::bfast::hash128(&b, s, s2)) as u64;
            c[3][0] += (m2::hash(&a, s) == m2::hash(&b, s)) as u64;
            c[3][1] += (m2::bfast::hash(&a, s) == m2::bfast::hash(&b, s)) as u64;
            c[3][2] += (m2::hash128(&a, s, s2) == m2::hash128(&b, s, s2)) as u64;
            c[3][3] += (m2::bfast::hash128(&a, s, s2) == m2::bfast::hash128(&b, s, s2)) as u64;
        }
        for (vi, vn) in ["v0.3", "v0.4", "v1  ", "v2  "].iter().enumerate() {
            println!("{} {}: hash {}/{}  bfast::hash {}/{}  hash128 {}/{}  bfast::hash128 {}/{}", name, vn, c[vi][0], n, c[vi][1], n, c[vi][2], n, c[vi][3], n);
        }
    }
}
