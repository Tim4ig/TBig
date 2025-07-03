#pragma once

#include <stddef.h>
#include <stdint.h>

extern uint64_t adcx_u64(uint64_t a, uint64_t b, uint64_t *carry);
extern uint64_t adox_u64(uint64_t a, uint64_t b, uint64_t *carry);
extern uint64_t sbb_u64(uint64_t a, uint64_t b, uint64_t *borrow);
extern void mulx_u64(uint64_t *h, uint64_t *l, uint64_t a, uint64_t b);

bool bigint_lt_avx2(const uint64_t *a, const uint64_t *b, size_t limbs);

void bigint_add_avx2(const uint64_t *a, const uint64_t *b, uint64_t *out, size_t limbs);
void bigint_sub_avx2(const uint64_t *a, const uint64_t *b, uint64_t *out, size_t limbs);

void bigint_mul_school(uint64_t *dst, const uint64_t *a, const uint64_t *b, size_t n);
void bigint_mul_karatsuba(uint64_t *dst, const uint64_t *a, const uint64_t *b, size_t n, uint64_t *temp);
char bigint_div_basic(const uint64_t *numerator, const uint64_t *denominator, uint64_t *quotient, uint64_t *remainder,
                      size_t limbs);
