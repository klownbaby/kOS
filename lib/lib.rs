
#![no_std]
#![no_main]

/**
 *
 * @copyright Copyright (c) 2024, Kevin Kleiman, All Rights Reserved
 * 
 * Yes, kOS supports Rust. This is linked as a static library to the kernel
 * Feel free to write drivers in whichever language you choose (C, asm, Rust) 
 *
 * All rust functions are prefixed with __ to improve kernel readability
 */

#[path = "../src/rs/cdef.rs"] mod cdef;
#[path = "../src/drivers/rs/driver.rs"] mod driver;
#[path = "std/io.rs"] mod io;
use core::panic::PanicInfo;
use crate::io::io::BOOT_LOG;
// Check rust has been linked properly
#[no_mangle]
pub extern "C" fn __init() -> u32 {
    BOOT_LOG("Rust initialized.\0"); // we better know it initialized
    return 1
}

#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    cdef::__panic("RPANIC!\0");
}
