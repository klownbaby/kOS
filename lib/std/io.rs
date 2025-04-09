pub mod alias {
    // i had to port vgacolor from the c code
    pub enum VgaColor {
    	Black = 0,
    	Blue = 1,
    	Green = 2,
    	Cyan = 3,
    	Red = 4,
    	Magenta = 5,
    	Brown = 6,
    	LightGrey = 7,
    	DarkGrey = 8,
    	LightBlue = 9,
    	LightGreen = 10,
    	LightCyan = 11,
    	LightRed = 12,
    	LightMagenta = 13,
    	LightBrown = 14,
    	White = 15,
    }
    // import stuff from C
    extern "C" {
        pub fn printk(fmt: *const u8, ...); // import printk from C
        pub fn tty_write(fmt: *const u8); // tty_write from C
        pub fn tty_writecolor(fmt: *const u8, color: VgaColor, bg_color: VgaColor);
    }
}



pub mod io {
    pub fn print() -> i32 {
        1
    }
    pub fn printk(fmt: &str) { // printk rust edition
        unsafe {
            super::alias::printk(fmt.as_ptr());
        }
    }
    pub fn BOOT_LOG(fmt: &str) { // essentially printk() but BOOT_LOG
        unsafe {
            super::alias::tty_write("[ \0".as_ptr()); 
            super::alias::tty_writecolor("OK\0".as_ptr(), super::alias::VgaColor::Green, super::alias::VgaColor::Black);
            super::alias::tty_write(" ] \0".as_ptr());
            super::alias::tty_write(fmt.as_ptr());
            super::alias::tty_write("\n\0".as_ptr());
        }
    }
}
