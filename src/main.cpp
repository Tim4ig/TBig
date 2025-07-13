#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>

#include "bigint_impl.hpp"

#include "bigint_test_data.h"


void from_string(const char *str, size_t limbs, uint64_t *out)
{
    size_t len = strlen(str);
    const size_t capacity = limbs * 64;

    if (len > capacity)
    {
        throw std::invalid_argument("binary string too long for bigint");
    }

    // zero the output buffer
    memset(out, 0, limbs * sizeof *out);

    // read MSB first, map str[len-1] → bit 0 (LSB), str[0] → bit (len-1)
    for (size_t i = 0; i < len; ++i)
    {
        char c = str[len - 1 - i];
        if (c == '1')
        {
            out[i / 64] |= (uint64_t(1) << (i % 64));
        } else if (c != '0')
        {
            throw std::invalid_argument("invalid character in binary string");
        }
    }
}

std::string to_string(const uint64_t *value, size_t limbs)
{
    const size_t total_bits = limbs * 64;
    std::string result;
    result.reserve(total_bits);

    bool seen_one = false; // skip leading zeros

    for (size_t i = total_bits; i-- > 0;)
    {
        size_t limb = i / 64;
        size_t bit = i % 64;
        bool bit_val = (value[limb] >> bit) & 1;

        if (bit_val) seen_one = true;

        if (seen_one)
            result += bit_val ? '1' : '0';
    }

    return result.empty() ? "0" : result;
}

#include <immintrin.h>

int bigint_cmp(const t::big::impl::word_t *a, const t::big::impl::word_t *b, const size_t limbs)
{
    for (size_t i = limbs - 4;; i -= 4)
    {
        const __m256i va = _mm256_load_si256((__m256i *) (a + i));
        const __m256i vb = _mm256_load_si256((__m256i *) (b + i));

        const __m256i cmp = _mm256_cmpeq_epi64(va, vb);
        const uint32_t mask = _mm256_movemask_epi8(cmp);
        if (mask != ~0)
        {
            const uint32_t diff = _tzcnt_u32(~mask);
            const uint32_t byte_in_block = diff;
            const uint32_t limb_offset = byte_in_block >> 3;
            const size_t idx = i + limb_offset;

            return a[idx] > b[idx] ? 1 : -1;
        }

        if (i == 0) break;
    }

    return 0;
}

int main()
{
    constexpr size_t limbs = 4096 / 64;
    alignas(64) uint64_t a[limbs];
    alignas(64) uint64_t b[limbs];
    alignas(64) uint64_t c[limbs * 2];

    from_string("10", limbs, a);
    from_string("10", limbs, b);

    t::big::impl::tbig_mul(a, b, c, limbs);

    std::string res = to_string(c, limbs);

    std::cout << res << '\n';

    return 0;
}

/*
    for (int i = 0; i < 100; ++i)
    {
        constexpr size_t limbs = 4096 / 64;
        alignas(64) uint64_t a[limbs];
        alignas(64) uint64_t b[limbs];
        alignas(64) uint64_t c[limbs * 2];
        alignas(64) uint64_t d[limbs * 2];

        from_string(test_a[i].c_str(), limbs, a);
        from_string(test_b[i].c_str(), limbs, b);

        const auto carry = t::big::impl::tbig_add(a, b, c, limbs);
        from_string(test_add[i].substr(carry).c_str(), limbs, d);

        if (bigint_cmp(c, d, limbs) != 0)
        {
            std::cout << "add err: " << i << '\n';
            return 1;
        }

        t::big::impl::tbig_sub(a, b, c, limbs);
        from_string(test_sub[i].substr(0).c_str(), limbs, d);

        if (bigint_cmp(c, d, limbs) != 0)
        {
            std::cout << "sub err: " << i << '\n';
            return 1;
        }


        t::big::impl::tbig_mul(a, b, c, limbs);
        from_string(test_mul[i].substr(0).c_str(), limbs * 2, d);

        std::string res_a = to_string(c, limbs * 2);
        std::string res_b = to_string(d, limbs * 2);

        if (bigint_cmp(c, d, limbs) != 0)
        {
            std::cout << "mul err: " << i << '\n' << res_a << '\n' << res_b << '\n';
            return 1;
        }
    }

    return 0;
 */
