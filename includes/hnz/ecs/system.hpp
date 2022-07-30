#pragma once

#include <hnz/ecs/entity.hpp>
#include <hnz/ecs/component.hpp>

namespace hnz {
    struct System {
        enum class Use {
            CREATION,       // Only once at after creation
            DESTRUCTION,    // Only once at before destruction
            EVERY_TICK,     // Every tick
            ON_NOTIFY,      // When notified in another system
        };

        using Type = hnz::u64;

        static constexpr Type INVALID_TYPE = 0;
        static constexpr Type TYPE         = INVALID_TYPE;

        static constexpr auto REQUIREMENTS = { hnz::Component::INVALID_TYPE };
        static constexpr auto USING        = Use::ON_NOTIFY;

        virtual void operator() (hnz::f32 delta,
                                 hnz::entity entity,
                                 const hnz::map<hnz::Component::Type, hnz::raw<hnz::Component>>& components,
                                 const hnz::vector<hnz::entity>& subscribers) {}
    };
}