//! C ABI shim over the `polyxor` crate (0.1.0, default features) for SMHasher3.
use polyxor::PolyXor128;

/// Build an instance from `PolyXor128::entropy_needed()` (4160) uniformly random bytes.
#[unsafe(no_mangle)]
pub unsafe extern "C" fn polyxor_new_from_entropy(entropy: *const u8) -> *mut PolyXor128 {
    let e = unsafe { core::slice::from_raw_parts(entropy, PolyXor128::entropy_needed()) };
    Box::into_raw(Box::new(PolyXor128::from_entropy(e)))
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn polyxor_free(p: *mut PolyXor128) {
    if !p.is_null() { drop(unsafe { Box::from_raw(p) }); }
}

#[unsafe(no_mangle)]
pub extern "C" fn polyxor_entropy_needed() -> usize { PolyXor128::entropy_needed() }

#[inline(always)]
unsafe fn input<'a>(data: *const u8, len: usize) -> &'a [u8] {
    if len == 0 { &[] } else { unsafe { core::slice::from_raw_parts(data, len) } }
}

/// Default output: finalize_avalanche (a bijection of finalize_raw), little-endian bytes.
#[unsafe(no_mangle)]
pub unsafe extern "C" fn polyxor_hash(p: *const PolyXor128, data: *const u8, len: usize, out: *mut u8) {
    let p = unsafe { &*p };
    let mut h = p.hasher();
    h.update(unsafe { input(data, len) });
    let r = h.finalize_avalanche().to_le_bytes();
    unsafe { core::ptr::copy_nonoverlapping(r.as_ptr(), out, 16) };
}

/// Raw polynomial output: finalize_raw, little-endian bytes.
#[unsafe(no_mangle)]
pub unsafe extern "C" fn polyxor_hash_raw(p: *const PolyXor128, data: *const u8, len: usize, out: *mut u8) {
    let p = unsafe { &*p };
    let mut h = p.hasher();
    h.update(unsafe { input(data, len) });
    let r = h.finalize_raw().to_le_bytes();
    unsafe { core::ptr::copy_nonoverlapping(r.as_ptr(), out, 16) };
}

/// Probe: compile-time target features (which fix the backend polyxor's dispatch
/// selects when they are all present) and runtime-detected features. Returns a
/// NUL-terminated static string.
#[unsafe(no_mangle)]
pub extern "C" fn polyxor_backend_probe() -> *const core::ffi::c_char {
    use std::sync::OnceLock;
    static S: OnceLock<std::ffi::CString> = OnceLock::new();
    S.get_or_init(|| {
        let mut s = String::new();
        #[cfg(target_arch = "x86_64")]
        {
            let ct = [
                ("avx512f", cfg!(target_feature = "avx512f")),
                ("avx512vl", cfg!(target_feature = "avx512vl")),
                ("vpclmulqdq", cfg!(target_feature = "vpclmulqdq")),
                ("avx2", cfg!(target_feature = "avx2")),
                ("pclmulqdq", cfg!(target_feature = "pclmulqdq")),
            ];
            let rt = [
                ("avx512f", std::arch::is_x86_feature_detected!("avx512f")),
                ("avx512vl", std::arch::is_x86_feature_detected!("avx512vl")),
                ("vpclmulqdq", std::arch::is_x86_feature_detected!("vpclmulqdq")),
                ("avx2", std::arch::is_x86_feature_detected!("avx2")),
                ("pclmulqdq", std::arch::is_x86_feature_detected!("pclmulqdq")),
            ];
            let backend = if cfg!(all(target_feature = "avx512f", target_feature = "avx512vl", target_feature = "vpclmulqdq")) {
                "hash_blocks_avx512 (compile-time)"
            } else if rt[0].1 && rt[1].1 && rt[2].1 { "hash_blocks_avx512 (runtime)" }
            else if rt[3].1 && rt[2].1 { "hash_blocks_avx2 (runtime)" }
            else if rt[4].1 { "hash_blocks_avx (runtime)" } else { "reference" };
            s += &format!("arch=x86_64 backend={backend} compile_time={ct:?} runtime={rt:?}");
        }
        #[cfg(target_arch = "aarch64")]
        {
            let ct = [("neon", cfg!(target_feature = "neon")), ("aes", cfg!(target_feature = "aes"))];
            let rt = [
                ("neon", std::arch::is_aarch64_feature_detected!("neon")),
                ("aes", std::arch::is_aarch64_feature_detected!("aes")),
            ];
            let backend = if ct[0].1 && ct[1].1 { "neon::hash_blocks (compile-time)" }
                else if rt[0].1 && rt[1].1 { "neon::hash_blocks (runtime)" } else { "reference" };
            s += &format!("arch=aarch64 backend={backend} compile_time={ct:?} runtime={rt:?}");
        }
        std::ffi::CString::new(s).unwrap()
    }).as_ptr()
}
