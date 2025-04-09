#[allow(dead_code)]

// define extern kernel (C) functions
mod alias {
    extern "C" {
        pub fn tty_write(buf: *const u8);
        pub fn __itoa(value: i32, buf: *const u8, base: i32);
        pub fn kpanic(buf: *const u8);
    }
}



pub fn tty_write(buf: &str) {
    unsafe { alias::tty_write(buf.as_ptr()); }
}

pub fn __itoa(value: i32, buf: &str, base: i32) {
    unsafe { alias::__itoa(value, buf.as_ptr(), base); }
}

pub fn __panic(buf: &str) -> ! {
    unsafe { alias::kpanic(buf.as_ptr()); }
    loop {}
}
