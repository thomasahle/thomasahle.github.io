// Pair check: the post's MuseAir v0.3 pairs against every upstream algorithm version (Rust crates at tags v0.3, v0.4, v1, crate-0.6.0 = algorithm v2).
fn unhex(s: &str) -> Vec<u8> { (0..s.len()/2).map(|i| u8::from_str_radix(&s[2*i..2*i+2],16).unwrap()).collect() }
fn splitmix(s: &mut u64) -> u64 { *s = s.wrapping_add(0x9E3779B97F4A7C15); let mut z=*s; z=(z^(z>>30)).wrapping_mul(0xBF58476D1CE4E5B9); z=(z^(z>>27)).wrapping_mul(0x94D049BB133111EB); z^(z>>31) }
fn main() {
    let log2n: u32 = std::env::args().nth(1).map(|a| a.parse().unwrap()).unwrap_or(24);
    let n = 1u64 << log2n;
    let pairs = [
        ("17B", "0000000000000000000000000000000000", "8079763bb19a00001a9a1100642d3a3f01"),
        ("24B", "000000000000000000000000000000000000000000000000", "7cd5c18245c15e8ef47bfef8b79181a80101010101010101"),
        ("32B", "000000000000000000000000000000005a5a5a5a5a5a5a5a5a5a5a5a5a5a5a5a", "dd4bd4006a79dcff6a28a852aefcfbbca5a5a5a5a5a5a5a5a5a5a5a5a5a5a5a5"),
    ];
    // v2 must reproduce its documented verification codes? Not available offline; we validate v0.3 against the SMHasher3 values instead via the known example seed.
    let ex = 0x2cb0f69f4abea221u64;
    let a = unhex(pairs[0].1); let b = unhex(pairs[0].2);
    println!("v0.3 crate, 17B pair, seed 0x{:016x}: hash(M)=0x{:016x} hash(M2)=0x{:016x} (published 0xd4ed417ecc529ae4)", ex, m03::hash(&a,ex), m03::hash(&b,ex));
    println!("N = 2^{} uniformly random 64-bit seeds (splitmix64 from 0x243F6A8885A308D3); 128-bit two-seed APIs get an independent second seed", log2n);
    for (name, ma, mb) in pairs.iter() {
        let a = unhex(ma); let b = unhex(mb);
        let labels = ["v0.3 hash","v0.3 bfast::hash","v0.3 hash_128","v0.3 bfast::hash_128",
                      "v0.4 hash","v0.4 bfast::hash","v0.4 hash_128","v0.4 bfast::hash_128",
                      "v1 hash","v1 bfast::hash","v1 hash128","v1 bfast::hash128",
                      "v2 hash","v2 bfast::hash","v2 hash128","v2 bfast::hash128"];
        let mut c = [0u64; 16];
        let mut st = 0x243F6A8885A308D3u64;
        for _ in 0..n {
            let s = splitmix(&mut st); let s2 = splitmix(&mut st);
            c[0]  += (m03::hash(&a,s)==m03::hash(&b,s)) as u64;
            c[1]  += (m03::bfast::hash(&a,s)==m03::bfast::hash(&b,s)) as u64;
            c[2]  += (m03::hash_128(&a,s)==m03::hash_128(&b,s)) as u64;
            c[3]  += (m03::bfast::hash_128(&a,s)==m03::bfast::hash_128(&b,s)) as u64;
            c[4]  += (m04::hash(&a,s)==m04::hash(&b,s)) as u64;
            c[5]  += (m04::bfast::hash(&a,s)==m04::bfast::hash(&b,s)) as u64;
            c[6]  += (m04::hash_128(&a,s)==m04::hash_128(&b,s)) as u64;
            c[7]  += (m04::bfast::hash_128(&a,s)==m04::bfast::hash_128(&b,s)) as u64;
            c[8]  += (m1::hash(&a,s)==m1::hash(&b,s)) as u64;
            c[9]  += (m1::bfast::hash(&a,s)==m1::bfast::hash(&b,s)) as u64;
            c[10] += (m1::hash128(&a,s,s2)==m1::hash128(&b,s,s2)) as u64;
            c[11] += (m1::bfast::hash128(&a,s,s2)==m1::bfast::hash128(&b,s,s2)) as u64;
            c[12] += (m2::hash(&a,s)==m2::hash(&b,s)) as u64;
            c[13] += (m2::bfast::hash(&a,s)==m2::bfast::hash(&b,s)) as u64;
            c[14] += (m2::hash128(&a,s,s2)==m2::hash128(&b,s,s2)) as u64;
            c[15] += (m2::bfast::hash128(&a,s,s2)==m2::bfast::hash128(&b,s,s2)) as u64;
        }
        println!("== pair {}", name);
        for k in 0..16 { println!("  {:22} collisions {} / {}", labels[k], c[k], n); }
    }
}
