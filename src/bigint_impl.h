#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef ASM_ENABLED
extern uint64_t adcx_u64(uint64_t a, uint64_t b, uint64_t *carry);
extern uint64_t adox_u64(uint64_t a, uint64_t b, uint64_t *carry);
extern uint64_t sbb_u64(uint64_t a, uint64_t b, uint64_t *borrow);
extern void mulx_u64(uint64_t *h, uint64_t *l, uint64_t a, uint64_t b);
#else // move it into .c if you want i prefer keep it here for inline reasons
/// uint64_t adcx_u64(uint64_t a, uint64_t b, uint64_t *carry);
/// — performs a + b, writes the unsigned carry‐out (0 or 1) to *carry
static inline uint64_t adcx_u64(uint64_t a, uint64_t b, uint64_t *carry) {
    uint64_t sum = a + b;
    // carry-out occurs if unsigned overflow: sum < a
    *carry = (sum < a) ? 1 : 0;
    return sum;
}

/// uint64_t adox_u64(uint64_t a, uint64_t b, uint64_t *carry);
/// — performs a + b, writes the overflow flag (OF) of the signed add to *carry
static inline uint64_t adox_u64(uint64_t a, uint64_t b, uint64_t *carry) {
    uint64_t sum = a + b;
    // OF (signed overflow) is set if a and b have same sign, but sum has opposite sign:
    // OF = ((~(a ^ b) & (a ^ sum)) >> 63) & 1
    *carry = (uint64_t)((~(a ^ b) & (a ^ sum)) >> 63);
    return sum;
}

/// uint64_t sbb_u64(uint64_t a, uint64_t b, uint64_t *borrow);
/// — performs a − b − (*borrow), writes the unsigned borrow‐out (0 or 1) to *borrow
static inline uint64_t sbb_u64(uint64_t a, uint64_t b, uint64_t *borrow) {
    uint64_t in = *borrow;
    uint64_t sub = b + in;
    uint64_t res = a - sub;
    // borrow-out occurs if a < b + in
    *borrow = (sub > a) ? 1 : 0;
    return res;
}

/// void mulx_u64(uint64_t *hi, uint64_t *lo, uint64_t a, uint64_t b);
/// — full 128×64-bit multiply, hi ← high 64 bits, lo ← low 64 bits
static inline void mulx_u64(uint64_t *hi, uint64_t *lo, uint64_t a, uint64_t b) {
    __uint128_t prod = ( __uint128_t )a * b;
    *lo = (uint64_t)prod;
    *hi = (uint64_t)(prod >> 64);
}
#endif

bool bigint_lt_avx2(const uint64_t *a, const uint64_t *b, size_t limbs);

void bigint_add_avx2(const uint64_t *a, const uint64_t *b, uint64_t *out, size_t limbs);
void bigint_sub_avx2(const uint64_t *a, const uint64_t *b, uint64_t *out, size_t limbs);

void bigint_mul_school(uint64_t *dst, const uint64_t *a, const uint64_t *b, size_t n);
void bigint_mul_karatsuba(uint64_t *dst, const uint64_t *a, const uint64_t *b, size_t n, uint64_t *temp);
char bigint_div_basic(const uint64_t *numerator, const uint64_t *denominator, uint64_t *quotient, uint64_t *remainder,
                      size_t limbs);
