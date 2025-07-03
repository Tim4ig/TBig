#pragma once

#include <array>
#include <cstdint>
#include <string_view>

namespace t::big
{
    template<std::size_t N>
    class BigInt
    {
    public:
        using word_t = std::uint64_t;
        static constexpr std::size_t word_bits = sizeof(word_t) * 8;
        static constexpr std::size_t limb_count = (N + word_bits - 1) / word_bits;
        static constexpr std::size_t size = limb_count * sizeof(word_t);

        enum class Base { Dec = 10, Hex = 16, Bin = 2 };

        static_assert(limb_count % 4 == 0,
                      "BigInt<N>: number of 64-bit limbs must be divisible by 4 for AVX2 compatibility");

        // constructors
        BigInt() = default;
        BigInt(const BigInt &) = default;
        BigInt(BigInt &&) noexcept = default;
        BigInt(word_t value);

        // assignment operators
        BigInt &operator=(const BigInt &) = default;
        BigInt &operator=(BigInt &&) noexcept = default;
        BigInt &operator=(word_t value);

        // comparison operators
        bool operator<(const BigInt &other) const;
        bool operator>(const BigInt &other) const;
        bool operator<=(const BigInt &other) const;
        bool operator>=(const BigInt &other) const;
        bool operator==(const BigInt &other) const;
        bool operator!=(const BigInt &other) const;

        // arithmetic (pure)
        BigInt operator+(const BigInt &other) const;
        BigInt operator-(const BigInt &other) const;
        BigInt operator*(const BigInt &other) const;
        // BigInt operator/(const BigInt &other) const;

        // arithmetic (compound)
        BigInt &operator+=(const BigInt &other);
        BigInt &operator-=(const BigInt &other);
        BigInt &operator*=(const BigInt &other);
        // BigInt &operator/=(const BigInt &other);

        // increment / decrement
        BigInt &operator++(); // prefix
        BigInt operator++(int); // postfix
        BigInt &operator--(); // prefix
        BigInt operator--(int); // postfix

        // Serialization
        [[nodiscard]] std::string to_string(Base base = Base::Dec) const;
        [[nodiscard]] explicit operator std::string() const;

    private:
        alignas(32) std::array<word_t, limb_count> raw_;
    };
} // namespace t::big
