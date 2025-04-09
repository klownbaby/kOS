#pragma once
#include <stdint.h>  // For u32


// i had to make this shit because i remembered i needed to make RUST=0 compile too
#ifdef RUST_ENABLED
// theres rust, let rust make the function
uint32_t __init(void);
#else
// stub incase rust is disabled
static inline uint32_t __init(void) {
    return 0;  // we got no rust...
}
#endif