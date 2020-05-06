#include "vm.h"

// Marsaglia's xorshift32
static inline uint32_t xorshift32(uint32_t* state)
{
    /* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */
    uint32_t x = *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return *state = x;
}

uint32_t vm_rand(vm_state_t* vm)
{
    return xorshift32(&vm->rand_seed);
}
