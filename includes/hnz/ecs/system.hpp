#pragma once

#include <hnz/ecs/entity.hpp>
#include <hnz/ecs/component.hpp>

namespace hnz {
    struct System {
        using Type = hnz::u64;

        static constexpr Type INVALID_TYPE = 0;
        static constexpr Type TYPE         = INVALID_TYPE;

        static constexpr auto REQUIRED = { hnz::Component::INVALID_TYPE };

        virtual void operator() (hnz::f64 delta,
                                 hnz::entity entity,
                                 hnz::set<hnz::entity>& subscribers,
                                 hnz::map<hnz::Component::Type, hnz::raw<hnz::Component>>& components) {}
    };
}