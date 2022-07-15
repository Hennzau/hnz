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

            auto spawn () -> hnz::fentity;

            auto spawn (hnz::fentity& parent) -> hnz::fentity;

        private:

            /* App */

            hnz::safe::safe_bool m_running;

            /* These are the commands that are sent to the app. */

            struct SpawnCommand {
                hnz::pentity entity;
            };

            struct ParentingCommand {
                hnz::fentity entity;
                hnz::fentity parent;
            };

            using commands = std::variant<SpawnCommand, ParentingCommand>;

            /* Safe objects */

            struct safe {
                hnz::safe::queue<commands> commands;

                hnz::safe::set<hnz::entity>                        entities;
                hnz::safe::map<hnz::entity, hnz::set<hnz::entity>> parents;
            }                    m_safe;

            hnz::u32 m_next_entity = 1;

            std::thread m_ecs_thread;
    };
}

