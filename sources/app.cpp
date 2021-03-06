#include <thread>
#include <cassert>

#include <hnz/app.hpp>

#include <iostream>

hnz::App::App () {
    m_running.set (true);

    m_ecs_thread = std::thread ([&] () {
        while (m_running.is ()) {
            if (m_safe.commands.empty ()) {
                continue;
            }

            auto& command_var = m_safe.commands.front ();

            std::visit ([&] (auto&& command) {
                            using command_type = std::decay_t<decltype (command)>;

                            if constexpr (std::is_same_v<command_type, ParentingCommand>) {
                                auto entity = command.entity;
                                auto parent = command.parent;

                                m_safe.parents[parent].emplace_back (entity);

                                std::cout << "Parenting entity : " << entity << " is child of : " << parent << std::endl;
                            } else if constexpr (std::is_same_v<command_type, UnParentingUnknownCommand>) {
                                auto entity = command.entity;

                                for (auto& [parent, entities]: m_safe.parents) {
                                    for (auto& child: entities) {
                                        if (child == entity) {
                                            m_safe.commands.push (UnParentingKnownCommand {
                                                    .entity = entity,
                                                    .parent = parent
                                            });
                                        }
                                    }
                                }

                            } else if constexpr (std::is_same_v<command_type, UnParentingKnownCommand>) {
                                auto entity = command.entity;
                                auto parent = command.parent;

                                m_safe.parents[parent].erase (std::find (m_safe.parents[parent].cbegin (),
                                                                         m_safe.parents[parent].cend (),
                                                                         entity));

                                std::cout << "UnParenting entity : " << entity << " from : " << parent << std::endl;
                            } else if constexpr (std::is_same_v<command_type, KillCommand>) {
                                auto entity    = command.entity;
                                auto genealogy = command.genealogy;

                                if (genealogy) {
                                    for (auto child: m_safe.parents[entity]) {
                                        m_safe.commands.push (KillCommand {
                                                .entity = child,
                                                .genealogy = true
                                        });
                                    }
                                }

                                m_safe.entities.erase (std::find (m_safe.entities.cbegin (),
                                                                  m_safe.entities.cend (),
                                                                  entity));

                                m_safe.parents.erase (entity);
                                m_safe.components[entity].clear ();

                                std::cout << "Killing entity : " << entity << std::endl;
                            } else if constexpr (std::is_same_v<command_type, AddComponentCommand>) {
                                auto entity = command.entity;
                                auto type   = command.type;

                                m_safe.components[entity][type] = std::move (command.component);

                                std::cout << "Adding component : " << type << " to entity : " << entity << std::endl;
                            } else if constexpr (std::is_same_v<command_type, RemoveComponentCommand>) {
                                auto entity = command.entity;
                                auto type   = command.type;

                                m_safe.components[entity].erase (type);

                                std::cout << "Removing component : " << type << " from entity : " << entity << std::endl;
                            } else {
                                std::cout << "Unknown command " << std::endl;
                            }
                        },
                        command_var);
            m_safe.commands.pop ();
        }
    });
}

void hnz::App::join () {
    m_ecs_thread.join ();
}

void hnz::App::run () {
    std::this_thread::sleep_for (std::chrono::milliseconds (200));
    std::cout << "Hello, World!" << std::endl;

    /* Systems */
}

auto hnz::App::spawn () -> hnz::entity {
    auto entity = m_next_entity++;

    m_safe.entities.emplace_back (entity);

    return entity;
}

auto hnz::App::spawn_group (hnz::u64 count) -> hnz::vector<hnz::entity> {
    auto result = hnz::vector<hnz::entity> {};

    for (auto i = 0u; i < count; ++i) {
        result.emplace_back (spawn ());
    }

    return result;
}

auto hnz::App::spawn (hnz::entity& parent) -> hnz::entity {
    auto entity = spawn ();

    m_safe.commands.push (ParentingCommand {
            .entity = entity,
            .parent = parent
    });

    return entity;
}

auto hnz::App::spawn_group (hnz::entity& parent, hnz::u64 count) -> hnz::vector<hnz::entity> {
    auto result = spawn_group (count);

    for (const auto& entity: result) {
        m_safe.commands.push (ParentingCommand {
                .entity = entity,
                .parent = parent
        });
    }

    return result;
}

auto hnz::App::exists (hnz::entity entity) const -> bool {
    if (entity == hnz::INVALID_ENTITY) {
        return false;
    }

    return m_safe.entities.contains (entity);
}

auto hnz::App::exists (hnz::vector<hnz::entity> entities) const -> bool {
    return std::ranges::all_of (entities,
                                [&] (const auto entity) {
                                    return exists (entity);
                                });;
}

auto hnz::App::kill (hnz::entity& entity, bool genealogy) -> void {
    assert (exists (entity));

    m_safe.commands.push (KillCommand {
            .entity = entity,
            .genealogy = genealogy
    });

    m_safe.commands.push (UnParentingUnknownCommand {
            .entity = entity
    });
}