extern "C" {
    fn log_wrapper(ptr: *const u8, len: u32);
    fn error_wrapper(ptr: *const u8, len: u32);
}

#[no_mangle]
pub fn log(msg: &str) {
    unsafe {
        log_wrapper(msg.as_ptr(), msg.len() as u32);
    }
}

#[no_mangle]
pub fn error(msg: &str) {
    unsafe {
        error_wrapper(msg.as_ptr(), msg.len() as u32);
    }
}