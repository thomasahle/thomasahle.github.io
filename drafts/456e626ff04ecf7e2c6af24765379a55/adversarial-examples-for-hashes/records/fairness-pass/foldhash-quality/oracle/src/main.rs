use std::hash::{Hash, Hasher};
use foldhash::SharedSeed;

fn unhex(s: &str) -> Vec<u8> { (0..s.len()/2).map(|i| u8::from_str_radix(&s[2*i..2*i+2], 16).unwrap()).collect() }

fn six(phs: u64, shared: &SharedSeed, b: &[u8], utf8: bool) -> [u64; 6] {
    let mut h = foldhash::fast::FoldHasher::with_seed(phs, shared); h.write(b); let fw = h.finish();
    let mut h = foldhash::quality::FoldHasher::with_seed(phs, shared); h.write(b); let qw = h.finish();
    let v = b.to_vec();
    let mut h = foldhash::fast::FoldHasher::with_seed(phs, shared); v.hash(&mut h); let fv = h.finish();
    let mut h = foldhash::quality::FoldHasher::with_seed(phs, shared); v.hash(&mut h); let qv = h.finish();
    let (fs, qs) = if utf8 {
        let s = std::str::from_utf8(b).unwrap();
        let mut h = foldhash::fast::FoldHasher::with_seed(phs, shared); s.hash(&mut h); let fs = h.finish();
        let mut h = foldhash::quality::FoldHasher::with_seed(phs, shared); s.hash(&mut h); let qs = h.finish();
        (fs, qs)
    } else { (0, 0) };
    [fw, qw, fv, qv, fs, qs]
}

struct Rng(u64);
impl Rng { fn next(&mut self) -> u64 { self.0 = self.0.wrapping_add(0x9e3779b97f4a7c15); let mut z = self.0; z = (z ^ (z >> 30)).wrapping_mul(0xbf58476d1ce4e5b9); z = (z ^ (z >> 27)).wrapping_mul(0x94d049bb133111eb); z ^ (z >> 31) } }

fn main() {
    let cases: &[(u64, u64, &str, bool)] = &[
        (0x0000000000000000, 0x0000000000000000, "0000000000000000", true),
        (0x0000000000000000, 0x0000000000000000, "ffffffffffffffff", false),
        (0x0123456789abcdef, 0xfedcba9876543210, "0000000000000000", true),
        (0x0123456789abcdef, 0xfedcba9876543210, "ffffffffffffffff", false),
        (0xdeadbeefcafebabe, 0x1234567890abcdef, "68656c6c6f20776f", true),
        (0xdeadbeefcafebabe, 0x1234567890abcdef, "68656c6c6f20776f726c642c2074686973206973206c6f6e676572207468616e207369787465656e206279746573", true),
        (0x1111111111111111, 0x2222222222222222, "616263", true),
        (0x1111111111111111, 0x2222222222222222, "", true),
        (0x8000000000000001, 0x00000000ffffffff, "3031323334353637383961626364656667", true),
    ];
    for (i, (phs, sh, hex, utf8)) in cases.iter().enumerate() {
        let shared = SharedSeed::from_u64(*sh);
        let r = six(*phs, &shared, &unhex(hex), *utf8);
        println!("case {} {}", i, r.iter().map(|x| format!("{:016x}", x)).collect::<Vec<_>>().join(" "));
    }
    // Pair sampling with the real crate: quality::FoldHasher, HashMap<Vec<u8>> framing (write_usize(8); write; finish) and raw write.
    let args: Vec<String> = std::env::args().collect();
    let log2n: u32 = args.get(1).map(|s| s.parse().unwrap()).unwrap_or(20);
    let threads: u64 = args.get(2).map(|s| s.parse().unwrap()).unwrap_or(1);
    let base: u64 = args.get(3).map(|s| u64::from_str_radix(s.trim_start_matches("0x"), 16).unwrap()).unwrap_or(0xf01d);
    let per = (1u64 << log2n) / threads;
    let m1 = vec![0u8; 8]; let m2 = vec![0xffu8; 8];
    let t0 = std::time::Instant::now();
    let handles: Vec<_> = (0..threads).map(|t| { let m1 = m1.clone(); let m2 = m2.clone(); std::thread::spawn(move || {
        let mut rng = Rng(base ^ (t.wrapping_mul(0x5851f42d4c957f2d)));
        let (mut qraw, mut qvec, mut fraw) = (0u64, 0u64, 0u64);
        for _ in 0..per {
            let phs = rng.next();
            // model 0: six independent uniform shared words; SharedSeed has no public word constructor,
            // so use from_u64 for the shared seed (model 1) -- rate is the same within noise per the records.
            let shared = SharedSeed::from_u64(rng.next());
            let mut h = foldhash::quality::FoldHasher::with_seed(phs, &shared); h.write(&m1); let a = h.finish();
            let mut h = foldhash::quality::FoldHasher::with_seed(phs, &shared); h.write(&m2); let b = h.finish();
            qraw += (a == b) as u64;
            let mut h = foldhash::quality::FoldHasher::with_seed(phs, &shared); m1.hash(&mut h); let a = h.finish();
            let mut h = foldhash::quality::FoldHasher::with_seed(phs, &shared); m2.hash(&mut h); let b = h.finish();
            qvec += (a == b) as u64;
            let mut h = foldhash::fast::FoldHasher::with_seed(phs, &shared); h.write(&m1); let a = h.finish();
            let mut h = foldhash::fast::FoldHasher::with_seed(phs, &shared); h.write(&m2); let b = h.finish();
            fraw += (a == b) as u64;
        }
        (qraw, qvec, fraw)
    })}).collect();
    let (mut q, mut v, mut f) = (0, 0, 0);
    for h in handles { let (a, b, c) = h.join().unwrap(); q += a; v += b; f += c; }
    println!("real-crate pair sample: N=2^{} threads={} model=from_u64  quality_raw={} quality_vec={} fast_raw={}  ({:.1} s)", log2n, threads, q, v, f, t0.elapsed().as_secs_f64());
}
