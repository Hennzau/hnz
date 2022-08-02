#pragma once

#include <cassert>

#include <hnz/types.hpp>

#include <hnz/ecs/entity.hpp>
#include <hnz/ecs/component.hpp>
#include <hnz/ecs/system.hpp>
#include <hnz/ecs/pool.hpp>
#include <hnz/ecs/group.hpp>

#include <iostream>

namespace hnz {
    class App {
        public:
            App () = default;

            ~App () = default;

            [[nodiscard]] auto& pools () const noexcept { return m_pools; }

            /* invocations */

            auto spawn () -> hnz::entity;

            /* assertions */

            [[nodiscard]] auto exists (hnz::entity entity) const -> bool;

            /* components */

            template<typename T, typename... Args>
            auto add (hnz::entity entity, Args&& ... args) -> void {
                m_pools[T::TYPE].template add<T> (entity,
                                                  std::forward<Args> (args)...);
            }

            template<typename T>
            auto remove (hnz::entity entity) -> void {
                m_pools[T::TYPE].remove (entity);
            }

            // retrieve component's ids of a given entity, iterating over pools
            [[nodiscard]] auto retrieve (hnz::entity entity) const -> hnz::vector<hnz::Component::Type>;

            /* groups */

            void register_group (const hnz::vector<hnz::Component::Type>& types) {
                m_registered_groups.emplace_back (types);
            }

        private:
            hnz::set<hnz::entity> m_entities;
            hnz::u32              m_next_valid_entity = 0;

            hnz::map<hnz::Component::Type, hnz::Pool> m_pools;

            hnz::map<hnz::Component::Sum, bool>            m_groups;
            hnz::vector<hnz::vector<hnz::Component::Type>> m_registered_groups;
    };
}