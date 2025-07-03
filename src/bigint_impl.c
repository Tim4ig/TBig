#include "bigint_impl.h"

#include <immintrin.h>
#include <string.h>

#if !defined(__AVX2__)
#error "AVX2 is required. Please compile with -mavx2"
#endif

bool bigint_lt_avx2(const uint64_t *a, const uint64_t *b, const size_t limbs)
{
    constexpr size_t step = 4;
    size_t i = limbs;

    while (i >= step)
    {
        i -= step;

        const __m256i va = _mm256_load_si256((const __m256i *) (a + i));
        const __m256i vb = _mm256_load_si256((const __m256i *) (b + i));

        const __m256i cmp_eq = _mm256_cmpeq_epi64(va, vb);
        const uint32_t eq_mask = _mm256_movemask_epi8(cmp_eq);

        if (eq_mask != -1)
        {
            const uint32_t diff_mask = ~eq_mask;
            const uint32_t bit_pos = __tzcnt_u32(diff_mask);
            const uint32_t byte_index = bit_pos / 8;
            const uint32_t limb_index = byte_index / 8;
            const size_t idx = i + limb_index;

            return a[idx] < b[idx];
        }
    }

    while (i > 0)
    {
        --i;
        if (a[i] < b[i]) return true;
        if (a[i] > b[i]) return false;
    }

    return false;
}

void bigint_add_avx2(const uint64_t *a, const uint64_t *b, uint64_t *out, const size_t limbs)
{
    uint64_t cf = 0, of = 0;

    for (size_t i = 0; i < limbs; i += 4)
    {
        out[i + 0] = adcx_u64(a[i + 0], b[i + 0], &cf);
        out[i + 2] = adcx_u64(a[i + 2], b[i + 2], &cf);

        out[i + 1] = adox_u64(a[i + 1], b[i + 1], &of);
        out[i + 3] = adox_u64(a[i + 3], b[i + 3], &of);
    }
}

void bigint_sub_avx2(const uint64_t *a, const uint64_t *b, uint64_t *out, const size_t limbs)
{
    uint64_t borrow1 = 0;
    uint64_t borrow2 = 0;

    for (size_t i = 0; i < limbs; i += 4)
    {
        out[i + 0] = sbb_u64(a[i + 0], b[i + 0], &borrow1);
        out[i + 2] = sbb_u64(a[i + 2], b[i + 2], &borrow1);

        out[i + 1] = sbb_u64(a[i + 1], b[i + 1], &borrow2);
        out[i + 3] = sbb_u64(a[i + 3], b[i + 3], &borrow2);
    }
}

void bigint_mul_school(uint64_t *dst, const uint64_t *a, const uint64_t *b, const size_t n)
{
    memset(dst, 0, 2 * n * sizeof(uint64_t));

    for (size_t i = 0; i < n; ++i)
    {
        uint64_t cf = 0, of = 0;

        for (size_t j = 0; j < n; ++j)
        {
            const size_t k = i + j;
            uint64_t lo, hi;
            mulx_u64(&hi, &lo, a[i], b[j]);

            dst[k] = adcx_u64(dst[k], lo, &cf);
            dst[k + 1] = adox_u64(dst[k + 1], hi, &of);

            size_t l = k + 2;
            while (cf | of && l < 2 * n)
            {
                const uint64_t tmp = dst[l];
                dst[l] = adox_u64(tmp, 0, &of);
                ++l;
            }
        }
    }
}

void bigint_mul_karatsuba(
    uint64_t *dst,
    const uint64_t *a,
    const uint64_t *b,
    const size_t n,
    uint64_t *temp
)
{
    if (n <= 8)
    {
        bigint_mul_school(dst, a, b, n);
        return;
    }

    const size_t h = n / 2;

    const uint64_t *a0 = a;
    const uint64_t *a1 = a + h;
    const uint64_t *b0 = b;
    const uint64_t *b1 = b + h;

    uint64_t *z0 = temp;
    uint64_t *z1 = temp + 2 * h;
    uint64_t *z2 = temp + 4 * h;
    uint64_t *a_sum = temp + 6 * h;
    uint64_t *b_sum = temp + 7 * h;

    uint64_t carry = 0;
    for (size_t i = 0; i < h; ++i)
    {
        a_sum[i] = adcx_u64(a0[i], a1[i], &carry);
    }

    carry = 0;
    for (size_t i = 0; i < h; ++i)
    {
        b_sum[i] = adcx_u64(b0[i], b1[i], &carry);
    }

    bigint_mul_karatsuba(z0, a0, b0, h, temp + 8 * h);
    bigint_mul_karatsuba(z2, a1, b1, h, temp + 8 * h);
    bigint_mul_karatsuba(z1, a_sum, b_sum, h, temp + 8 * h);

    uint64_t tmp[2 * h];
    bigint_sub_avx2(z1, z0, tmp, n);
    bigint_sub_avx2(tmp, z2, z1, n);

    memset(dst, 0, 2 * n * sizeof(uint64_t));
    memcpy(dst, z0, n * sizeof(uint64_t));

    carry = 0;
    for (size_t i = 0; i < n; ++i)
    {
        dst[i + h] = adcx_u64(dst[i + h], z1[i], &carry);
    }

    carry = 0;
    for (size_t i = 0; i < n; ++i)
    {
        dst[i + 2 * h] = adcx_u64(dst[i + 2 * h], z2[i], &carry);
    }
}

// slow but it's only used for to_string()
char bigint_div_basic(
    const uint64_t *numerator,
    const uint64_t *denominator,
    uint64_t *quotient,
    uint64_t *remainder,
    const size_t limbs)
{
    bool is_zero = true;
    for (size_t i = 0; i < limbs; ++i)
    {
        if (denominator[i] != 0)
        {
            is_zero = false;
            break;
        }
    }

    if (is_zero)
    {
        return 1;
    }

    memset(quotient, 0, limbs * sizeof(uint64_t));
    memset(remainder, 0, limbs * sizeof(uint64_t));

    if (bigint_lt_avx2(numerator, denominator, limbs))
    {
        memcpy(remainder, numerator, limbs * sizeof(uint64_t));
        return 0;
    }
    if (memcmp(denominator, numerator, limbs * sizeof(uint64_t)) == 0)
    {
        quotient[0] = 1;
        memset(remainder, 0, limbs * sizeof(uint64_t));
        return 0;
    }

    uint64_t temp_rem[limbs];
    memset(temp_rem, 0, limbs * sizeof(uint64_t));

    for (ssize_t bit = (ssize_t) (limbs * 64) - 1; bit >= 0; --bit)
    {
        uint64_t carry = 0;
        for (size_t i = 0; i < limbs; ++i)
        {
            uint64_t new_carry = temp_rem[i] >> 63;
            temp_rem[i] = (temp_rem[i] << 1) | carry;
            carry = new_carry;
        }

        const size_t limb_idx = bit / 64;
        const size_t bit_idx = bit % 64;
        const uint64_t bit_val = (numerator[limb_idx] >> bit_idx) & 1;
        temp_rem[0] |= bit_val;

        if (!bigint_lt_avx2(temp_rem, denominator, limbs))
        {
            uint64_t tmp[limbs];
            bigint_sub_avx2(temp_rem, denominator, tmp, limbs);
            memcpy(temp_rem, tmp, limbs * sizeof(uint64_t));

            quotient[bit / 64] |= (1ULL << (bit % 64));
        }
    }

    memcpy(remainder, temp_rem, limbs * sizeof(uint64_t));

    return 0;
}
