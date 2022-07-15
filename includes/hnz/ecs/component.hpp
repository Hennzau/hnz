#pragma once

#include <hnz/types.hpp>

namespace hnz {
    struct Component {
        using Type = hnz::u64;

        static constexpr Type INVALID_TYPE = 0;
        static constexpr Type TYPE         = INVALID_TYPE;
    };
}