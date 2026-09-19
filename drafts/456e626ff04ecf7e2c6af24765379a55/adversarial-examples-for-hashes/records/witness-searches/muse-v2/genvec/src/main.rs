// Test-vector generator: links the museair crate at tag crate-0.6.0 and prints
// len seed sa sb hex(bytes) hash bfast::hash hash128 bfast::hash128, checking UNROLL parity.
use std::io::Write;
fn main() {
    let n: usize = std::env::args().nth(1).map(|s| s.parse().unwrap()).unwrap_or(200000);
    let mut st: u64 = 0x9E3779B97F4A7C15;
    let mut next = move || { st ^= st << 13; st ^= st >> 7; st ^= st << 17; st };
    let out = std::io::stdout();
    let mut w = std::io::BufWriter::new(out.lock());
    for t in 0..n {
        let len = if t < 512 { t } else if t % 3 == 0 { (next() % 512) as usize } else { (next() % 70) as usize };
        let mut bytes = vec![0u8; len];
        for b in bytes.iter_mut() { *b = next() as u8; }
        let seed = next(); let sa = next(); let sb = next();
        let h = museair::hash(&bytes, seed);
        let hb = museair::bfast::hash(&bytes, seed);
        let h2 = museair::hash128(&bytes, sa, sb);
        let h2b = museair::bfast::hash128(&bytes, sa, sb);
        assert_eq!(h, museair::impls::hash::<false, true>(&bytes, seed));
        assert_eq!(hb, museair::impls::hash::<true, true>(&bytes, seed));
        assert_eq!(h2, museair::impls::hash128::<false, true>(&bytes, sa, sb));
        assert_eq!(h2b, museair::impls::hash128::<true, true>(&bytes, sa, sb));
        let hex: String = bytes.iter().map(|b| format!("{:02x}", b)).collect();
        writeln!(w, "{} {:016x} {:016x} {:016x} {} {:016x} {:016x} {:032x} {:032x}", len, seed, sa, sb, if hex.is_empty() { "-".to_string() } else { hex }, h, hb, h2, h2b).unwrap();
    }
}
