fn hex(s: &str) -> Vec<u8> { (0..s.len()).step_by(2).map(|i| u8::from_str_radix(&s[i..i+2],16).unwrap()).collect() }
fn main() {
    let a: Vec<u8> = vec![0u8; 15];
    let b: Vec<u8> = vec![0xffu8; 16];
    let c: Vec<u8> = vec![0u8; 24];
    let d: Vec<u8> = hex("0100000000000000a803d3a0b6cb85eb1120e4f3a270c9a6");
    let s2: i64 = 0xf556ecbfcbfee3adu64 as i64;
    let s1: i64 = 0xd73a9a3d941e7ec7u64 as i64;
    println!("pair2 seed {:016x}: gxhash64(A)={:016x} gxhash64(B)={:016x}", s2 as u64, gxhash::gxhash64(&a, s2), gxhash::gxhash64(&b, s2));
    println!("pair2 seed {:016x}: gxhash128(A)={:032x} gxhash128(B)={:032x}", s2 as u64, gxhash::gxhash128(&a, s2), gxhash::gxhash128(&b, s2));
    println!("pair1 seed {:016x}: gxhash64(C)={:016x} gxhash64(D)={:016x}", s1 as u64, gxhash::gxhash64(&c, s1), gxhash::gxhash64(&d, s1));
    println!("pair1 seed 0123456789abcdef: {:016x} {:016x}", gxhash::gxhash64(&c, 0x0123456789abcdef), gxhash::gxhash64(&d, 0x0123456789abcdef));
    let mut x: u64 = 0x9e3779b97f4a7c15; let n = 1u64<<22; let (mut c2, mut c1, mut c2h, mut c1h) = (0u64,0u64,0u64,0u64);
    for _ in 0..n { x ^= x<<13; x ^= x>>7; x ^= x<<17; let s = x as i64;
        if gxhash::gxhash64(&a,s)==gxhash::gxhash64(&b,s) { c2+=1; }
        if gxhash::gxhash128(&a,s)==gxhash::gxhash128(&b,s) { c2h+=1; }
        if gxhash::gxhash64(&c,s)==gxhash::gxhash64(&d,s) { c1+=1; }
        if gxhash::gxhash128(&c,s)==gxhash::gxhash128(&d,s) { c1h+=1; } }
    println!("random seeds n={}: pair2 64={} 128={}  pair1 64={} 128={}", n, c2, c2h, c1, c1h);
    let e: Vec<u8> = vec![0u8; 16]; let mut ctl=0u64; for i in 0..1000i64 { if gxhash::gxhash64(&a,i)==gxhash::gxhash64(&e,i) {ctl+=1;} }
    println!("control 15x00 vs 16x00 over 1000 seeds: {} collisions", ctl);
}
