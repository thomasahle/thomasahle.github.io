// Independent oracle: real foldhash crate (path dependency = master checkout).
// vectors            : print the six values for the verifier's nine reference cases
// measure L T MODEL S: count collisions of 00*8 vs ff*8 over 2^L hidden seeds,
//                      MODEL 0 = per-hasher seed + six shared words uniform (transmute),
//                      MODEL 1 = per-hasher seed uniform, SharedSeed::from_u64(uniform)
use foldhash::{fast, quality, SharedSeed};
use std::hash::{Hash, Hasher};

fn sm(x: &mut u64) -> u64 {
    *x = x.wrapping_add(0x9E3779B97F4A7C15);
    let mut z = *x;
    z = (z ^ (z >> 30)).wrapping_mul(0xBF58476D1CE4E5B9);
    z = (z ^ (z >> 27)).wrapping_mul(0x94D049BB133111EB);
    z ^ (z >> 31)
}

fn leak(s: SharedSeed) -> &'static SharedSeed { Box::leak(Box::new(s)) }

fn six(phs: u64, ss: &'static SharedSeed, bytes: &[u8], utf8: bool) -> [u64; 6] {
    let mut fw = fast::FoldHasher::with_seed(phs, ss); fw.write(bytes);
    let mut qw = quality::FoldHasher::with_seed(phs, ss); qw.write(bytes);
    let mut fv = fast::FoldHasher::with_seed(phs, ss); bytes.to_vec().hash(&mut fv);
    let mut qv = quality::FoldHasher::with_seed(phs, ss); bytes.to_vec().hash(&mut qv);
    let (sf, sq) = if utf8 {
        let s = std::str::from_utf8(bytes).unwrap();
        let mut a = fast::FoldHasher::with_seed(phs, ss); s.hash(&mut a);
        let mut b = quality::FoldHasher::with_seed(phs, ss); s.hash(&mut b);
        (a.finish(), b.finish())
    } else { (0, 0) };
    [fw.finish(), qw.finish(), fv.finish(), qv.finish(), sf, sq]
}

fn unhex(h: &str) -> Vec<u8> { (0..h.len()/2).map(|i| u8::from_str_radix(&h[2*i..2*i+2], 16).unwrap()).collect() }

fn main() {
    let a: Vec<String> = std::env::args().collect();
    if a[1] == "vectors" {
        let cases: [(u64, u64, &str, bool); 9] = [
            (0, 0, "0000000000000000", true),
            (0, 0, "ffffffffffffffff", false),
            (0x0123456789abcdef, 0xfedcba9876543210, "0000000000000000", true),
            (0x0123456789abcdef, 0xfedcba9876543210, "ffffffffffffffff", false),
            (0xdeadbeefcafebabe, 0x1234567890abcdef, "68656c6c6f20776f", true),
            (0xdeadbeefcafebabe, 0x1234567890abcdef, "68656c6c6f20776f726c642c2074686973206973206c6f6e676572207468616e207369787465656e206279746573", true),
            (0x1111111111111111, 0x2222222222222222, "616263", true),
            (0x1111111111111111, 0x2222222222222222, "", true),
            (0x8000000000000001, 0x00000000ffffffff, "3031323334353637383961626364656667", true),
        ];
        for (i, (phs, sh, hx, utf8)) in cases.iter().enumerate() {
            let v = six(*phs, leak(SharedSeed::from_u64(*sh)), &unhex(hx), *utf8);
            println!("case {} {}", i, v.iter().map(|x| format!("{:016x}", x)).collect::<Vec<_>>().join(" "));
        }
        return;
    }
    let l: u32 = a[2].parse().unwrap();
    let t: u64 = a[3].parse().unwrap();
    let model: u32 = a[4].parse().unwrap();
    let seed: u64 = u64::from_str_radix(a[5].trim_start_matches("0x"), 16).unwrap();
    let nper = (1u64 << l) / t;
    let m1 = [0u8; 8]; let m2 = [0xffu8; 8];
    let totals: Vec<[u64; 4]> = std::thread::scope(|s| {
        let hs: Vec<_> = (0..t).map(|k| s.spawn(move || {
            let mut st = seed ^ k.wrapping_mul(0xA5A5A5A5A5A5A5A5);
            for _ in 0..8 { sm(&mut st); }
            let mut c = [0u64; 4];
            for _ in 0..nper {
                let phs = sm(&mut st);
                let ss: SharedSeed = if model == 0 {
                    let w = [sm(&mut st), sm(&mut st), sm(&mut st), sm(&mut st), sm(&mut st), sm(&mut st)];
                    assert_eq!(std::mem::size_of::<SharedSeed>(), 48);
                    unsafe { std::mem::transmute::<[u64; 6], SharedSeed>(w) }
                } else { SharedSeed::from_u64(sm(&mut st)) };
                // extend lifetime for the duration of this iteration only
                let ssr: &'static SharedSeed = unsafe { &*(&ss as *const SharedSeed) };
                let mut a1 = quality::FoldHasher::with_seed(phs, ssr); a1.write(&m1);
                let mut a2 = quality::FoldHasher::with_seed(phs, ssr); a2.write(&m2);
                let mut b1 = quality::FoldHasher::with_seed(phs, ssr); m1.to_vec().hash(&mut b1);
                let mut b2 = quality::FoldHasher::with_seed(phs, ssr); m2.to_vec().hash(&mut b2);
                let mut f1 = fast::FoldHasher::with_seed(phs, ssr); f1.write(&m1);
                let mut f2 = fast::FoldHasher::with_seed(phs, ssr); f2.write(&m2);
                let qw = a1.finish() == a2.finish();
                let fw = f1.finish() == f2.finish();
                c[0] += qw as u64; c[1] += (b1.finish() == b2.finish()) as u64;
                c[2] += fw as u64; c[3] += (qw && !fw) as u64;
            }
            c
        })).collect();
        hs.into_iter().map(|h| h.join().unwrap()).collect()
    });
    let mut tot = [0u64; 4];
    for c in totals { for i in 0..4 { tot[i] += c[i]; } }
    println!("N=2^{} threads={} model={} seed={:#x}: quality_raw={} quality_vec={} fast_raw={} quality_only={}", l, t, model, seed, tot[0], tot[1], tot[2], tot[3]);
}
