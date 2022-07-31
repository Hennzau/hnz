#pragma once

#include <hnz/types.hpp>

namespace hnz {
    struct Component {
        using Type = hnz::u32; // Must be a prime number

        static constexpr Type INVALID_TYPE = 0;
        static constexpr Type TYPE         = INVALID_TYPE;
    };

    // Returns the product of all the components in the vector
    constexpr auto product (const hnz::vector<Component::Type>& components) -> hnz::u64 {
        auto product = hnz::u64 { 1 };

        for (const auto& component: components) {
            product *= component;
        }

        return product;
    }

    // Returns true if 'key' divide 'product'
    constexpr auto divides (hnz::u64 key, hnz::u64 product) -> bool {
        return product % key == 0;
    }
}