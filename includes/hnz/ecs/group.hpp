#pragma once

namespace hnz {
    struct Group {
        hnz::vector<hnz::entity> entities;

        hnz::u32 first_not = 0;

        hnz::map<hnz::Component::Sum, hnz::u32> sparse_to_dense;
        hnz::vector<hnz::u32>                   nested;

        auto swap (hnz::u32 i, hnz::u32 j) noexcept -> void {
            std::swap (entities[i],
                       entities[j]);
        }
    };
}