use ahash::RandomState;
use std::hash::BuildHasher;
fn build_tie(choice: u32) -> Vec<u8> {
    let mut m = vec![0u8; 448];
    let col0 = [[0x7cu8,0x06,0x7e,0x7b],[0x81,0x08,0x81,0x80]];
    let col5 = [[0x80u8,0x08,0x82,0x80],[0x7d,0x06,0x7d,0x7b]];
    let tb = [0x03u8, 0x08]; let b4 = [0x08u8, 0x03];
    for j in 0..4 { let c = ((choice >> j) & 1) as usize; let o = 16*j;
        m[384+o+8] = tb[c]; m[o+8..o+12].copy_from_slice(&col0[c]); m[256+o+8] = b4[c]; m[320+o+8..320+o+12].copy_from_slice(&col5[c]); }
    m
}
fn sm(x: &mut u64) -> u64 { *x = x.wrapping_add(0x9e3779b97f4a7c15); let mut z = *x; z = (z ^ (z>>30)).wrapping_mul(0xbf58476d1ce4e5b9); z = (z ^ (z>>27)).wrapping_mul(0x94d049bb133111eb); z ^ (z>>31) }
fn main() {
    let msgs: Vec<Vec<u8>> = (0..16).map(build_tie).collect();
    let rs = RandomState::with_seeds(0xd36cce1f36b157c4, 0xc21552b51f5c0ff3, 0xfcae49fe7892b205, 0x07726122279d53ee);
    let hs: Vec<u64> = msgs.iter().map(|m| rs.hash_one(&m[..])).collect();
    println!("explicit key: all 16 equal = {} ; h = {:016x}", hs.iter().all(|&h| h == hs[0]), hs[0]);
    for (i,m) in msgs.iter().enumerate() { if i==0 || i==15 { let s: String = m.iter().map(|b| format!("{:02x}", b)).collect(); println!("member {} = {}", i, s); } }
    let n: u64 = 1 << std::env::args().nth(1).map(|a| a.parse().unwrap()).unwrap_or(24u32);
    let mut st = 0x5eed_0928u64; let (mut p0, mut w4a, mut w4b, mut w16) = (0u64,0u64,0u64,0u64);
    for _ in 0..n {
        let rs = RandomState::with_seeds(sm(&mut st), sm(&mut st), sm(&mut st), sm(&mut st));
        let h0 = rs.hash_one(&msgs[0][..]);
        let h1 = rs.hash_one(&msgs[1][..]); let h4 = rs.hash_one(&msgs[4][..]); let h5 = rs.hash_one(&msgs[5][..]);
        if h1 == h0 { p0 += 1; }
        if h1 == h0 && h4 == h0 && h5 == h0 { w4a += 1; }
        let h2 = rs.hash_one(&msgs[2][..]); let h8 = rs.hash_one(&msgs[8][..]); let h10 = rs.hash_one(&msgs[10][..]);
        if h2 == h0 && h8 == h0 && h10 == h0 { w4b += 1; }
        if h1 == h0 && h4 == h0 && h5 == h0 && h2 == h0 && h8 == h0 && h10 == h0 {
            if msgs.iter().all(|m| rs.hash_one(&m[..]) == h0) { w16 += 1; } }
    }
    let l = |c: u64| (c as f64 / n as f64).log2();
    println!("N = 2^{} keys (with_seeds, four uniform words): slot0 pair {} (2^{:.3}); 4-way {{0,1,4,5}} {} (2^{:.3}); 4-way {{0,2,8,10}} {} (2^{:.3}); 16-way {}", n.trailing_zeros(), p0, l(p0), w4a, l(w4a), w4b, l(w4b), w16);
}
