#pragma once

namespace hnz {
    struct Group {
        using Type = hnz::Component::Sum;

        hnz::vector<hnz::entity> entities;
        hnz::u32                 first_not = 0;
        hnz::map<Type, hnz::u32> nested;
    };

    static constexpr auto is_included (const hnz::Component::Types& a, const hnz::Component::Types& b) -> bool {
        for (auto& type: b) {
            if (std::find (a.begin (),
                           a.end (),
                           type) == a.end ()) {
                return false;
            }
        }

        return a.size () > b.size ();
    }

    static constexpr auto count (const hnz::Component::Types& a, const hnz::vector<hnz::Component::Types>& b) -> hnz::u64 {
        auto result = hnz::u64 { 0 };

        for (auto& group: b) {
            if (is_included (a,
                             group)) {
                result += 1;
            }
        }

        return result;
    }
}