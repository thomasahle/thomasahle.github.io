// Pair check against the LATEST upstream foldhash source (path dep = git HEAD).
// Model A (real crate API): per-hasher seed uniform u64, SharedSeed::from_u64(uniform u64).
// Counts fast raw / fast Vec<u8>-framed / quality Vec<u8>-framed collisions of
// m = 00*8 vs m' = ff*8.
use core::hash::{Hash, Hasher};
use foldhash::{fast, quality, SharedSeed};
use std::thread;

fn splitmix(x: &mut u64) -> u64 {
    *x = x.wrapping_add(0x9e3779b97f4a7c15);
    let mut z = *x;
    z = (z ^ (z >> 30)).wrapping_mul(0xbf58476d1ce4e5b9);
    z = (z ^ (z >> 27)).wrapping_mul(0x94d049bb133111eb);
    z ^ (z >> 31)
}

fn main() {
    let args: Vec<String> = std::env::args().collect();
    let log2n: u32 = args.get(1).map(|s| s.parse().unwrap()).unwrap_or(30);
    let nthr: u64 = args.get(2).map(|s| s.parse().unwrap()).unwrap_or(8);
    let base: u64 = args.get(3).map(|s| u64::from_str_radix(s.trim_start_matches("0x"), 16).unwrap()).unwrap_or(0xc0ffee1234567890);

    // Reference vectors baked into verify/foldhash-fast/foldhash_verify.c (emitted by crates.io foldhash =0.2.0):
    // (per_hasher_seed, from_u64 arg, input) -> fast_raw, fast_vec, fast_str, qual_raw, qual_vec
    let cases: [(u64, u64, &[u8], [u64; 5]); 3] = [
        (0x0123456789abcdef, 0xfedcba9876543210, &[0u8; 8], [0x76515ea1a03101fb, 0x66ca112256c774fa, 0x9ac395840828b9ae, 0x6608bc5df43f5b74, 0x647e5b7098fa10de]),
        (0x0123456789abcdef, 0xfedcba9876543210, &[0xffu8; 8], [0x84165e99364de5f4, 0x1126871eba2b7a20, 0xa612287b16657ff7, 0x757e226dd898e948, 0xc9a03fd89c1af7fa]),
        (0xdeadbeefcafebabe, 0x0000000000000001, b"hello world", [0x9cac4433811168a2, 0x9e731322f0728b3f, 0x92d7b847ace9dd11, 0x1748bcd35d9a46d4, 0xc4f0e6fce3f1277d]),
    ];
    let mut bad = 0;
    for (i, (phs, su, inp, exp)) in cases.iter().enumerate() {
        let shared = SharedSeed::from_u64(*su);
        let mut h = fast::FoldHasher::with_seed(*phs, &shared); h.write(inp); let fr = h.finish();
        let mut h = fast::FoldHasher::with_seed(*phs, &shared); inp.to_vec().hash(&mut h); let fv = h.finish();
        let mut h = fast::FoldHasher::with_seed(*phs, &shared); h.write(inp); h.write_u8(0xff); let fs = h.finish();
        let mut h = quality::FoldHasher::with_seed(*phs, &shared); h.write(inp); let qr = h.finish();
        let mut h = quality::FoldHasher::with_seed(*phs, &shared); inp.to_vec().hash(&mut h); let qv = h.finish();
        let got = [fr, fv, fs, qr, qv];
        for (k, nm) in ["fast_raw","fast_vec","fast_str","qual_raw","qual_vec"].iter().enumerate() {
            let ok = got[k] == exp[k]; if !ok { bad += 1; }
            println!("  case {} {:<8} HEAD={:016x} v0.2.0-vector={:016x} {}", i, nm, got[k], exp[k], if ok {"OK"} else {"MISMATCH"});
        }
    }
    println!("  -> {} mismatches against the 0.2.0 reference vectors", bad);
    if bad > 0 { std::process::exit(1); }

    let total: u64 = 1u64 << log2n;
    let per = total / nthr;
    let mut hs = Vec::new();
    for t in 0..nthr {
        hs.push(thread::spawn(move || {
            let mut sm = base ^ (t + 1).wrapping_mul(0xa0761d6478bd642f);
            let (mut hr, mut hv, mut hq) = (0u64, 0u64, 0u64);
            for _ in 0..per {
                let phs = splitmix(&mut sm);
                let su = splitmix(&mut sm);
                let shared = SharedSeed::from_u64(su);
                let mut a = fast::FoldHasher::with_seed(phs, &shared);
                let mut b = fast::FoldHasher::with_seed(phs, &shared);
                a.write(&m1); b.write(&m2);
                let (ra, rb) = (a.finish(), b.finish());
                if ra == rb { hr += 1; }
                let mut a = fast::FoldHasher::with_seed(phs, &shared);
                let mut b = fast::FoldHasher::with_seed(phs, &shared);
                (&m1[..]).hash(&mut a); (&m2[..]).hash(&mut b);
                if a.finish() == b.finish() { hv += 1; }
                let mut a = quality::FoldHasher::with_seed(phs, &shared);
                let mut b = quality::FoldHasher::with_seed(phs, &shared);
                (&m1[..]).hash(&mut a); (&m2[..]).hash(&mut b);
                if a.finish() == b.finish() { hq += 1; }
            }
            (hr, hv, hq)
        }));
    }
    let (mut hr, mut hv, mut hq) = (0u64, 0u64, 0u64);
    for h in hs { let (a, b, c) = h.join().unwrap(); hr += a; hv += b; hq += c; }
    let n = per * nthr;
    println!("seeds = {} (2^{}), model: per-hasher uniform, SharedSeed::from_u64(uniform)", n, log2n);
    println!("hits fast raw    = {}", hr);
    println!("hits fast &[u8]  = {}", hv);
    println!("hits quality &[u8]= {}", hq);
    let eps = hr as f64 / n as f64;
    println!("eps(raw) = {:.4e} = 2^{:.4}", eps, eps.log2());
}
