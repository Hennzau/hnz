#pragma once

#include <hnz/ecs/entity.hpp>
#include <hnz/ecs/component.hpp>

namespace hnz {
    struct System {
        enum class Usage {
            ON_REGISTRATION,    // Only once when an entity is registered.
            ON_DELETION,        // Only once when an entity is deleted from a system. Not always when it is killed.
            ON_NOTIFICATION,    // When notified in another system
            ON_TICK,            // Every tick
        };

        using Type = hnz::u64;

        static constexpr Type INVALID_TYPE = 0;
        static constexpr Type TYPE         = INVALID_TYPE;

        static constexpr auto REQUIREMENTS = { hnz::Component::INVALID_TYPE };
        static constexpr auto USAGE        = Usage::ON_NOTIFICATION;

        virtual void operator() (hnz::f32 delta,
                                 hnz::entity entity,
                                 const hnz::map<hnz::Component::Type, hnz::raw<hnz::Component>>& components,
                                 const hnz::vector<hnz::entity>& subscribers) {}
    };
}