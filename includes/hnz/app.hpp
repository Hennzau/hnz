#pragma once

#include <cassert>

#include <hnz/types.hpp>

#include <hnz/ecs/entity.hpp>
#include <hnz/ecs/component.hpp>
#include <hnz/ecs/system.hpp>
#include <hnz/ecs/group.hpp>

#include <iostream>

namespace hnz {
    class App {
        public:
            App () = default;

            ~App () = default;

            auto build () -> void;

            /* invocations */

            auto spawn () -> hnz::entity;

            /* assertions */

            [[nodiscard]] auto exists (hnz::entity entity) const -> bool;

            /* components */

            /* systems */

            template<typename T>
            auto record () -> void {
                static_assert (std::is_base_of_v<System, T>,
                               "T must be a System");

                static_assert (T::REQUIREMENTS.size () != 0,
                               "T must have at least one requirement");

                m_systems[T::TYPE] = AppSystem {
                        .ptr = std::make_unique<T> (),
                        .requirements = T::REQUIREMENTS,
                };

                // if T::REQUIREMENTS.size () == 1, so we link the pool of components
                if (T::REQUIREMENTS.size () == 1) {

                } else {
                    m_groups_required.emplace_back (T::REQUIREMENTS);
                }
            }

        private:
            hnz::set<hnz::entity> m_entities;
            hnz::u32              m_next_valid_entity = 0;;

            struct EntitiesInSystem {
                hnz::vector<hnz::entity>* entities;
                hnz::u32                * count;
            };

            struct AppSystem {
                hnz::owner<hnz::System> ptr;
                hnz::Component::Types   requirements;
            };

            hnz::map<hnz::System::Type, AppSystem>        m_systems;
            hnz::map<hnz::System::Type, EntitiesInSystem> m_entities_in_system;

            /* groups */
            static constexpr auto sort (hnz::vector<hnz::Component::Types>& wanted) -> void;

            hnz::vector<hnz::Component::Types> m_groups_required;
            hnz::vector<hnz::Group>            m_groups;
    };
}