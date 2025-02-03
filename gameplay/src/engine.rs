#[cfg(target_arch = "wasm32")]
extern "C" {
    fn log_wrapper(ptr: *const u8, len: u32);
    fn error_wrapper(ptr: *const u8, len: u32);
}

#[cfg(target_arch = "wasm32")]
pub fn log(msg: &str) {
    unsafe {
        log_wrapper(msg.as_ptr(), msg.len() as u32);
    }
}

#[cfg(target_arch = "wasm32")]
pub fn error(msg: &str) {
    unsafe {
        unsafe {
            error_wrapper(msg.as_ptr(), msg.len() as u32);
        }
    }
}

#[cfg(not(target_arch = "wasm32"))]
#[repr(C)]
pub struct EngineFunctions {
    pub log_wrapper: unsafe extern "C" fn(*const u8, u32),
    pub error_wrapper: unsafe extern "C" fn(*const u8, u32),
}
#[cfg(not(target_arch = "wasm32"))]
static mut ENGINE: Option<EngineFunctions> = None;
#[cfg(not(target_arch = "wasm32"))]
#[no_mangle]
pub extern "C" fn init_engine(engine: EngineFunctions) {
    unsafe {
        ENGINE = Some(engine);
    }
}

#[cfg(not(target_arch = "wasm32"))]
pub fn log(msg: &str) {
    unsafe {
        if let Some(ref engine) = ENGINE {
            (engine.log_wrapper)(msg.as_ptr(), msg.len() as u32);
        }
    }
}

#[cfg(not(target_arch = "wasm32"))]
pub fn error(msg: &str) {
    unsafe {
        if let Some(ref engine) = ENGINE {
            (engine.error_wrapper)(msg.as_ptr(), msg.len() as u32);
        }
    }
}