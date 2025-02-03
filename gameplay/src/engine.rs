#[repr(C)]
pub struct EngineFunctions {
    pub log_wrapper: unsafe extern "C" fn(*const u8, u32),
    pub error_wrapper: unsafe extern "C" fn(*const u8, u32),
}
static mut ENGINE: Option<EngineFunctions> = None;
#[no_mangle]
pub extern "C" fn init_engine(engine: EngineFunctions) {
    unsafe {
        ENGINE = Some(engine);
    }
}

pub fn log(msg: &str) {
    unsafe {
        if let Some(ref engine) = ENGINE {
            (engine.log_wrapper)(msg.as_ptr(), msg.len() as u32);
        }
    }
}

pub fn error(msg: &str) {
    unsafe {
        if let Some(ref engine) = ENGINE {
            (engine.error_wrapper)(msg.as_ptr(), msg.len() as u32);
        }
    }
}
