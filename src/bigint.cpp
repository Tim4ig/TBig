#include "bigint.hpp"

#include <string>
#include <cstring>
#include <algorithm>
#include <immintrin.h>

namespace t::big
{
    template<std::size_t N>
    BigInt<N> &BigInt<N>::operator=(const word_t value)
    {
        std::fill(std::begin(raw_), std::end(raw_), 0);
        raw_[0] = value;
        return *this;
    }

    template<std::size_t N>
    bool BigInt<N>::operator<(const BigInt &other) const
    {
        const auto *lhs = raw_.data();
        const auto *rhs = other.raw_.data();

        auto i = limb_count;
        while (i >= 4)
        {
            i -= 4;
            const auto va = _mm256_load_si256(reinterpret_cast<const __m256i *>(lhs + i));
            const auto vb = _mm256_load_si256(reinterpret_cast<const __m256i *>(rhs + i));
            const auto cmp_eq = _mm256_cmpeq_epi64(va, vb);

            if (const auto eq_mask = _mm256_movemask_epi8(cmp_eq); eq_mask != 0xFFFFFFFFu)
            {
                const auto diff_mask = ~eq_mask;
                const auto bit_pos = _tzcnt_u32(diff_mask);
                const auto lane = bit_pos >> 3;
                const auto idx = i + lane;
                return lhs[idx] < rhs[idx];
            }
        }

        return false;
    }

    template<std::size_t N>
    bool BigInt<N>::operator>(const BigInt &other) const
    {
        return other < *this;
    }

    template<std::size_t N>
    bool BigInt<N>::operator<=(const BigInt &other) const
    {
        return !(other < *this);
    }

    template<std::size_t N>
    bool BigInt<N>::operator>=(const BigInt &other) const
    {
        return !(*this < other);
    }

    template<size_t N>
    bool BigInt<N>::operator==(const BigInt &other) const
    {
        return std::memcmp(raw_.data(), other.raw_.data(), size) == 0;
    }

    template<size_t N>
    bool BigInt<N>::operator!=(const BigInt &other) const
    {
        return !operator==(other);
    }

    template<std::size_t N>
    BigInt<N> BigInt<N>::operator+(const BigInt &other) const
    {
        BigInt res{};
        unsigned char carry = 0;

        for (std::size_t i = 0; i < limb_count; i += 4)
        {
            carry = _addcarryx_u64(carry, raw_[i + 0], other.raw_[i + 0],
                                   reinterpret_cast<unsigned long long *>(&res.raw_[i + 0]));
            carry = _addcarryx_u64(carry, raw_[i + 1], other.raw_[i + 1],
                                   reinterpret_cast<unsigned long long *>(&res.raw_[i + 1]));
            carry = _addcarryx_u64(carry, raw_[i + 2], other.raw_[i + 2],
                                   reinterpret_cast<unsigned long long *>(&res.raw_[i + 2]));
            carry = _addcarryx_u64(carry, raw_[i + 3], other.raw_[i + 3],
                                   reinterpret_cast<unsigned long long *>(&res.raw_[i + 3]));
        }

        return res;
    }

    template<std::size_t N>
    std::string BigInt<N>::to_string(Base base) const
    {
        return std::to_string(raw_[0]);
    }
} // namespace t::big

template class t::big::BigInt<256>;
template class t::big::BigInt<512>;
template class t::big::BigInt<1024>;
template class t::big::BigInt<2048>;
template class t::big::BigInt<4096>;
