#[allow(dead_code)]

// define extern kernel (C) functions
mod alias {
    extern "C" {
        pub fn TTYWrite(buf: *const u8);
        pub fn __itoa(value: i32, buf: *const u8, base: i32);
        pub fn __panic(buf: *const u8);
    }
}

pub fn TTYWrite(buf: &str) {
    unsafe { alias::TTYWrite(buf.as_ptr()); }
}

pub fn __itoa(value: i32, buf: &str, base: i32) {
    unsafe { alias::__itoa(value, buf.as_ptr(), base); }
}

pub fn __panic(buf: &str) -> ! {
    unsafe { alias::__panic(buf.as_ptr()); }
    loop {}
}
