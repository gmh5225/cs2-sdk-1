#pragma once

#include "utility/fnv1a.hpp"

namespace schemas {
    bool initialize();

    std::uint16_t get_offset_impl(fnv1a::hash_t hash);

    template <std::size_t N>
    __forceinline constexpr std::uint16_t get_offset(const char(&name)[N]) {
        return get_offset_impl(fnv1a::fnv_hash_const(name));
    }
}