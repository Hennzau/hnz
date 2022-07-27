#pragma once

#include <string_view>
#include <mutex>
#include <variant>

#include <hnz/types.hpp>

#include <hnz/ecs/entity.hpp>
#include <hnz/ecs/component.hpp>

#include <hnz/safe/safe_bool.hpp>
#include <hnz/safe/safe_set.hpp>
#include <hnz/safe/safe_map.hpp>
#include <hnz/safe/safe_queue.hpp>
#include <hnz/safe/safe_vector.hpp>

namespace hnz {
    class App {
        public:

            App () = default;

            ~App () = default;

            void join ();

            void build ();

            void run ();

            [[nodiscard]] auto& running () const { return m_running; }

            [[nodiscard]] auto& running () { return m_running; }

            [[nodiscard]] auto& entities () const { return m_safe.entities; }

            [[nodiscard]] auto& parents () const { return m_safe.parents; }

            /* entities */

            auto spawn () -> hnz::entity;

            auto spawn (hnz::entity& parent) -> hnz::entity;

            auto exists (hnz::entity entity) const -> bool;

            auto kill (hnz::entity& entity, bool genealogy = false) -> void;

            template<typename T, typename... Args>
            auto add_component (hnz::entity& entity, Args&& ... args) -> void {
                m_safe.commands.push (AddComponentCommand {
                        .entity = entity,
                        .type = T::TYPE,
                        .component = std::make_unique<T> (std::forward<Args> (args)...)
                });
            }

            template<typename T>
            auto remove_component (hnz::entity& entity) -> void {
                m_safe.commands.push (RemoveComponentCommand {
                        .entity = entity,
                        .type = T::TYPE
                });
            }

        private:

            /* App */

            hnz::safe::safe_bool m_running;

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

            using commands = std::variant<ParentingCommand, UnParentingUnknownCommand, UnParentingKnownCommand, KillCommand, AddComponentCommand, RemoveComponentCommand>;

            /* Safe objects */

            struct safe {
                hnz::safe::queue<commands> commands;

                hnz::safe::set<hnz::entity>                        entities;
                hnz::safe::map<hnz::entity, hnz::set<hnz::entity>> parents;

                hnz::safe::map<hnz::entity, hnz::map<hnz::Component::Type, hnz::owner<hnz::Component>>> components;
            }                    m_safe;

            hnz::u32 m_next_entity = 1;

            std::thread m_ecs_thread;
    };
}

