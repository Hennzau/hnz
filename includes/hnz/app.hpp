#pragma once

#include <cassert>
#include <string_view>
#include <mutex>
#include <variant>
#include <memory>

#include <hnz/types.hpp>

#include <hnz/ecs/entity.hpp>
#include <hnz/ecs/component.hpp>
#include <hnz/ecs/system.hpp>

#include <hnz/safe/safe_bool.hpp>
#include <hnz/safe/safe_map.hpp>
#include <hnz/safe/safe_queue.hpp>
#include <hnz/safe/safe_vector.hpp>

namespace hnz {
    class App {
        public:

            App ();

            ~App () = default;

            void join ();

            void run ();

            [[nodiscard]] auto& running () const { return m_running; }

            [[nodiscard]] auto& running () { return m_running; }

            [[nodiscard]] auto& entities () const { return m_safe.entities; }

            [[nodiscard]] auto& parents () const { return m_safe.parents; }

            [[nodiscard]] auto& systems () const { return m_safe.systems; }

            [[nodiscard]] auto& entities_in_systems () const { return m_safe.entities_in_systems; }

            /* entities */

            auto spawn () -> hnz::entity;

            auto spawn_group (hnz::u64 count) -> hnz::vector<hnz::entity>;

            auto spawn (hnz::entity& parent) -> hnz::entity;

            auto spawn_group (hnz::entity& parent, hnz::u64 count) -> hnz::vector<hnz::entity>;

            auto exists (hnz::entity entity) const -> bool;

            auto exists (const hnz::vector<hnz::entity>& entities) const -> bool;

            auto kill (hnz::entity& entity, bool genealogy = false) -> void;

            /* components */

            template<typename T, typename... Args>
            auto add (hnz::entity& entity, Args&& ... args) -> void {
                static_assert (std::is_base_of<hnz::Component, T>::value,
                               "T must be a Component");

                static_assert (T::TYPE != hnz::Component::INVALID_TYPE,
                               "T must have T::type defined");

                assert (exists (entity));

                m_safe.commands.push (AddComponentCommand {
                        .entity = entity,
                        .type = T::TYPE,
                        .component = std::make_unique<T> (std::forward<Args> (args)...)
                });
            }

            template<typename T, typename... Args>
            auto add (const hnz::vector<entity>& entities, Args&& ... args) -> void {
                static_assert (std::is_base_of<hnz::Component, T>::value,
                               "T must be a Component");

                static_assert (T::TYPE != hnz::Component::INVALID_TYPE,
                               "T must have T::type defined");

                assert (exists (entities));

                for (const auto& entity: entities) {
                    m_safe.commands.push (AddComponentCommand {
                            .entity = entity,
                            .type = T::TYPE,
                            .component = std::make_unique<T> (std::forward<Args> (args)...)
                    });
                }
            }

            template<typename T>
            auto remove (hnz::entity& entity) -> void {
                static_assert (std::is_base_of<hnz::Component, T>::value,
                               "T must be a Component");

                static_assert (T::TYPE != hnz::Component::INVALID_TYPE,
                               "T must have T::type defined");

                assert (exists (entity));

                m_safe.commands.push (RemoveComponentCommand {
                        .entity = entity,
                        .type = T::TYPE
                });
            }

            template<typename T>
            auto remove (const hnz::vector<entity>& entities) -> void {
                static_assert (std::is_base_of<hnz::Component, T>::value,
                               "T must be a Component");

                static_assert (T::TYPE != hnz::Component::INVALID_TYPE,
                               "T must have T::type defined");

                assert (exists (entities));

                for (const auto& entity: entities) {
                    m_safe.commands.push (RemoveComponentCommand {
                            .entity = entity,
                            .type = T::TYPE
                    });
                }
            }

            template<typename T>
            auto has (hnz::entity entity) const -> bool {
                static_assert (std::is_base_of<hnz::Component, T>::value,
                               "T must be a Component");

                static_assert (T::TYPE != hnz::Component::INVALID_TYPE,
                               "T must have T::type defined");

                assert (exists (entity));

                return m_safe.components[entity].contains (T::TYPE);
            }

            template<typename T>
            auto component (hnz::entity entity) const -> T& {
                assert (has<T> (entity));

                return *static_cast<T*> (m_safe.components[entity][T::TYPE].get ());
            }

            template<typename T>
            auto component (hnz::entity entity) -> T& {
                assert (has<T> (entity));

                return *static_cast<T*> (m_safe.components[entity][T::TYPE].get ());
            }

            /* views */

            auto view (const hnz::vector<hnz::Component::Type>& components) const -> hnz::vector<hnz::entity>;

            /* systems */

            auto is_registered (hnz::entity entity, hnz::System::Type type) -> bool;

            template<typename T>
            auto record () -> void {
                static_assert (std::is_base_of<hnz::System, T>::value,
                               "T must be a System");

                static_assert (T::TYPE != hnz::System::INVALID_TYPE,
                               "T must have T::type defined");

                m_safe.systems[T::TYPE] = AppSystem {
                        .ptr = std::make_unique<T> (),
                        .requirements = T::REQUIREMENTS,
                };

                for (const auto& entity: m_safe.entities) {
                    m_safe.commands.push (UpdateEntityInKnownSystemCommand {
                            .entity = entity,
                            .key = T::TYPE,
                            .added = true
                    });
                }
            }

        private:

            /* App */

            hnz::safe::safe_bool m_running;

            struct AppSystem {
                hnz::owner<hnz::System>          ptr;
                hnz::ilist<hnz::Component::Type> requirements;
            };

            struct EntityInSystem {
                hnz::entity entity;

                hnz::map<hnz::Component::Type, hnz::raw<hnz::Component>> components;
                hnz::vector<hnz::entity>                                 subscribers;
            };

            /* These are the commands that are sent to the app. */

            struct ParentingCommand {
                hnz::entity entity;
                hnz::entity parent;
            };

            struct UnParentingUnknownCommand {
                hnz::entity entity;
            };

            struct UnParentingKnownCommand {
                hnz::entity entity;
                hnz::entity parent;
            };

            struct KillCommand {
                hnz::entity entity;
                bool        genealogy;
            };

            struct AddComponentCommand {
                hnz::entity                entity;
                hnz::Component::Type       type;
                hnz::owner<hnz::Component> component;
            };

            struct RemoveComponentCommand {
                hnz::entity          entity;
                hnz::Component::Type type;
            };

            struct UpdateEntityInUnknownSystemsCommand {
                hnz::entity entity;
                bool        added;
            };

            struct UpdateEntityInKnownSystemCommand {
                hnz::entity       entity;
                hnz::System::Type key;
                bool              added;
            };

            using commands = std::variant<
                    ParentingCommand,
                    UnParentingUnknownCommand,
                    UnParentingKnownCommand,
                    KillCommand,
                    AddComponentCommand,
                    RemoveComponentCommand,
                    UpdateEntityInUnknownSystemsCommand,
                    UpdateEntityInKnownSystemCommand>;

            /* Safe objects */

            struct safe {
                hnz::safe::queue<commands> commands;

                hnz::safe::vector<hnz::entity>                        entities;
                hnz::safe::map<hnz::entity, hnz::vector<hnz::entity>> parents;

                hnz::safe::map<hnz::entity, hnz::map<hnz::Component::Type, hnz::owner<hnz::Component>>> components;

                hnz::safe::map<hnz::System::Type, AppSystem>                   systems;
                hnz::safe::map<hnz::System::Type, hnz::vector<EntityInSystem>> entities_in_systems;

            } m_safe;

            hnz::u32 m_next_entity = 1;

            std::thread m_ecs_thread;
    };
}

