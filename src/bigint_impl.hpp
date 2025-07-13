#pragma once

#include <cstdint>

namespace t::big::impl
{
    using word_t = uint64_t;

    extern "C"
    {
    uint64_t tbig_add(const uint64_t *a, const uint64_t *b, uint64_t *out, size_t limbs);
    uint64_t tbig_sub(const uint64_t *a, const uint64_t *b, uint64_t *out, size_t limbs);

    void tbig_mul(const uint64_t *a, const uint64_t *b, uint64_t *out, size_t limbs);
    }
}
