#pragma once

#include <hnz/types.hpp>

namespace hnz {
    struct Component {
        using Type = hnz::u32;
        using Types = hnz::vector<Type>;

        using Sum = hnz::u64;

        static constexpr Type INVALID_TYPE = 0;
        static constexpr Type TYPE         = INVALID_TYPE;
    };

    [[maybe_unused]] static constexpr auto sum (const hnz::vector<Component::Type>& types) noexcept -> Component::Sum {
        auto sum = Component::Sum { 0 };

        for (auto type: types) {
            sum += type;
        }

        return sum;
    }

}