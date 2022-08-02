#pragma once

#include <robin_hood/robin_hood.hpp>

#include <string_view>
#include <cstdint>
#include <array>
#include <queue>
#include <span>
#include <bitset>

namespace hnz {
    using u8 = std::uint8_t;
    using u16 = std::uint16_t;
    using u32 = std::uint32_t;
    using u64 = std::uint64_t;

    using i8 = std::int8_t;
    using i16 = std::int16_t;
    using i32 = std::int32_t;
    using i64 = std::int64_t;

    using f32 = float;
    using f64 = double;

    template<typename T>
    using owner = std::unique_ptr<T>;

    template<typename T, typename... Args>
    static constexpr hnz::owner<T> make (Args&& ... args) {
        return std::make_unique<T> (std::forward<Args> (args)...);
    }

    template<typename T>
    using raw = T*;

    template<typename T, typename O>
    static constexpr T as (const O& v) noexcept {
        return static_cast<T> (v);
    }

    template<typename T, typename O>
    static constexpr T reinterpret (const O& v) noexcept {
        return reinterpret_cast<T> (v);
    }

    template<typename Key, typename Val>
    using map = robin_hood::unordered_map<Key, Val>;

    template<typename T>
    using set = robin_hood::unordered_set<T>;

    template<typename T>
    using queue = std::queue<T>;

    template<typename T>
    using vector = std::vector<T>;

    template<typename T>
    using span = std::span<T>;

    constexpr auto hash_32 (std::string_view str) noexcept -> hnz::u32 {
        return std::empty (str) ? 2216829733
                                : (hnz::as<hnz::u32> (str[0])
                                   ^ hash_32 (str.substr (1,
                                                          std::size (str) - 1))) * 0x100000001b3UL;

    }
}