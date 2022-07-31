#pragma once

#include <hnz/ecs/entity.hpp>
#include <hnz/ecs/component.hpp>

namespace hnz {
    struct System {
        enum class Usage {
            ON_REGISTRATION,
            ON_DELETION,
            ON_NOTIFICATION,
            ON_TICK
        };

        using Type = hnz::u32;

        static constexpr Type INVALID_TYPE = 0;
        static constexpr Type TYPE         = INVALID_TYPE;

        static constexpr auto REQUIRED = { hnz::Component::INVALID_TYPE };
        static constexpr auto USAGE    = Usage::ON_NOTIFICATION;

        virtual void operator() (hnz::f32 delta,
                                 hnz::entity entity,
                                 const hnz::map<hnz::Component::Type, hnz::owner<hnz::Component>>& components,
                                 const hnz::vector<hnz::entity>& subscribers) {}
    };
}