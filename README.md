# kOS

## Design Principles
kOS is __not__ designed with ANY security in mind. It's quite obviously incredibly easy to break.

kOS is designed with, __freedom__, __speed__, and __learning__ as it's core principles. Modern operating systems treat the end-user as one would treat a cat, walking across a keyboard.

Terry Davis once compared the Linux kernel to a semi, Windows to a car, and TempleOS to a motorcycle. Well, by that same analogy, kOS is akin to a dune buggy, with no brakes, and certainly no airbags...

Coolio, now go write a fucking driver. 

## Setup and Installation
Most of the toolchain is containerized, so the required installs are quite limited:
- Ensure you have `nasm`, `qemu`, `docker`, (OPTIONAL) `cargo` and `rust`
- You can create the build environment for the first time with `make env`
- Use `make` to build without `rust` support, and `make RUST=1` for the inverse

You should be good to go at this point but honestly, shit happens so be prepared
to deal with some unexpected BS. Sorry, you're compiling a kernel, it kinda sucks.

## Screenshots:
<img width="720" alt="Screenshot 2024-03-09 at 2 46 07 PM" src="https://github.com/kevinkleiman/kOS/assets/36178104/ad756ebe-e555-4eb3-9603-c9b35ba8858c">


## Yes, kOS supports both C and Rust
I fucking hate `rust`, however I know quite a few people who love it. Not throwing any shade, just my opinion.
Figured I'd add support for those who are insane enough to want to use it. So, in short, yes kOS supports
`rust`.

## Contributing:
For now, I (kevin) have been the sole contributor but I'm very open to any contributions, no matter how stupid.

## Have fun, create chaos
