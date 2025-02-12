# kOS

## Setup and Installation
Most of the toolchain is dockerized, so the required installs are quite limited:
- Ensure you have `nasm`, `qemu-system`, `docker`, (OPTIONAL) `cargo` and `rust`
- You can create the build environment for the first time with `make env`
- Use `make` to build without `rust` support, and `make RUST=1` for the inverse

You should be good to go at this point but honestly, shit happens so be prepared
to deal with some unexpected BS. Sorry, you're building a kernel, it kinda sucks.

## Screenshots:
<img width="720" alt="Screenshot 2024-03-09 at 2 46 07 PM" src="https://github.com/kevinkleiman/kOS/assets/36178104/ad756ebe-e555-4eb3-9603-c9b35ba8858c">


## Yes, kOS supports both C and Rust! 
I fucking hate `rust`, however I know quite a few people who love it. Not throwing any shade, just my opinion.
Figured I'd add support for those who are insane enough to want to use it. So, in short, yes kOS supports
`rust`.

## What kOS Is:
kOS (short for "kevin's OS") pronounced as "chaos", is a lightweight operating system intended to provide absolute freedom while retaining ~some~ safety. It started out of a desire to create
a ring0 math library where the developer has absolute control over optimization/hardware.

## What kOS Is NOT:
kOS is technically ~not~ an operating system... yet. For now it's just a shitty, lightweight kernel with IRQ1 (keyboard) and syscalls barely impelemented.

## Contributing:
For now, I (kevin) have been the sole contributor but I'm very open to reviewing any PRs from the OS community, even if that is a pipe dream lol.

## Have fun, create chaos!
