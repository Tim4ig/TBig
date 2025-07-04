#include "bigint.hpp"

extern "C"
{
#include "bigint_impl.h"
}

#include <string>
#include <cstring>
#include <algorithm>
#include <iostream>

namespace t::big
{
    template<std::size_t N>
    BigInt<N>::BigInt(word_t value)
    {
        *this = value;
    }

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
        return bigint_lt_avx2(raw_.data(), other.raw_.data(), limb_count);
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
        bigint_add_avx2(raw_.data(), other.raw_.data(), res.raw_.data(), limb_count);
        return res;
    }

    template<std::size_t N>
    BigInt<N> BigInt<N>::operator-(const BigInt &other) const
    {
        BigInt res{};
        bigint_sub_avx2(raw_.data(), other.raw_.data(), res.raw_.data(), limb_count);
        return res;
    }

    template<std::size_t N>
    BigInt<N> BigInt<N>::operator*(const BigInt &other) const
    {
        constexpr std::size_t R = 2 * limb_count;
        alignas(32) uint64_t result[R];
        alignas(32) uint64_t temp[4 * R];

        if (N > 512)
        {
            bigint_mul_karatsuba(result, raw_.data(), other.raw_.data(), limb_count, temp);
        } else
        {
            bigint_mul_school(result, raw_.data(), other.raw_.data(), limb_count);
        }

        BigInt trimmed{};
        std::copy_n(result, limb_count, trimmed.raw_.begin());
        return trimmed;
    }

    template<std::size_t N>
    BigInt<N> &BigInt<N>::operator+=(const BigInt &other)
    {
        *this = *this + other;
        return *this;
    }

    template<std::size_t N>
    BigInt<N> &BigInt<N>::operator-=(const BigInt &other)
    {
        *this = *this - other;
        return *this;
    }

    template<std::size_t N>
    BigInt<N> &BigInt<N>::operator*=(const BigInt &other)
    {
        *this = *this * other;
        return *this;
    }

    template<std::size_t N>
    BigInt<N> &BigInt<N>::operator++()
    {
        *this = *this + 1;
        return *this;
    }

    template<std::size_t N>
    BigInt<N> BigInt<N>::operator++(int)
    {
        BigInt old = *this;
        ++*this;
        return old;
    }


    template<std::size_t N>
    BigInt<N> &BigInt<N>::operator--()
    {
        *this = *this - 1;
        return *this;
    }

    template<std::size_t N>
    BigInt<N> BigInt<N>::operator--(int)
    {
        BigInt old = *this;
        --*this;
        return old;
    }

    template<std::size_t N>
    std::string BigInt<N>::to_string(const BigIntBase base) const
    {
        if (*this == BigInt{})
        {
            return "0";
        }

        if (base == BigIntBase::Hex)
        {
            std::string result;
            result.reserve(limb_count * 16 + 2);
            result += "0x";

            bool started = false;
            for (ssize_t i = limb_count - 1; i >= 0; --i)
            {
                if (!started)
                {
                    if (raw_[i] == 0) continue;
                    char buf[17];
                    std::snprintf(buf, sizeof(buf), "%lx", raw_[i]);
                    result += buf;
                    started = true;
                } else
                {
                    char buf[17];
                    std::snprintf(buf, sizeof(buf), "%016lx", raw_[i]);
                    result += buf;
                }
            }

            return result;
        }

        if (base == BigIntBase::Bin)
        {
            std::string result;
            result.reserve(limb_count * 64 + 2);
            result += "0b";

            auto started = false;
            for (ssize_t i = limb_count - 1; i >= 0; --i)
            {
                for (auto bit = 63; bit >= 0; --bit)
                {
                    const auto bit_set = raw_[i] >> bit & 1;
                    if (!started && !bit_set) continue;
                    result += bit_set ? '1' : '0';
                    started = true;
                }
            }

            return result;
        }

        // Slow fallback for dec (use slow div)
        char buffer[N * 20]{};
        auto pos = sizeof(buffer);

        BigInt value = *this;
        BigInt q{}, remainder{};
        BigInt b = 10;

        while (value > BigInt{})
        {
            bigint_div_basic(
                value.raw_.data(),
                b.raw_.data(),
                q.raw_.data(),
                remainder.raw_.data(),
                limb_count
            );

            buffer[--pos] = '0' + static_cast<char>(remainder.raw_[0]);
            value = q;
        }

        return std::string(buffer + pos, buffer + sizeof(buffer));
    }

    template<std::size_t N>
    BigInt<N>::operator std::string() const
    {
        return this->to_string();
    }
} // namespace t::big

template class t::big::BigInt<256>;
template class t::big::BigInt<512>;
template class t::big::BigInt<1024>;
template class t::big::BigInt<2048>;
template class t::big::BigInt<4096>;
template class t::big::BigInt<8192>;
