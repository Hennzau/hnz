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

                                assert (exists ({ entity, parent }));

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

                                assert (exists ({ parent }));

                                m_safe.parents[parent].erase (std::find (m_safe.parents[parent].cbegin (),
                                                                         m_safe.parents[parent].cend (),
                                                                         entity));

                                std::cout << "UnParenting entity : " << entity << " from : " << parent << std::endl;
                            } else if constexpr (std::is_same_v<command_type, KillCommand>) {
                                auto entity    = command.entity;
                                auto genealogy = command.genealogy;

                                assert (exists (entity));

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

                                for (auto& [key, system]: m_safe.systems) {
                                    if (std::find_if (m_safe.entities_in_systems[key].cbegin (),
                                                      m_safe.entities_in_systems[key].cend (),
                                                      [&] (auto&& entity_in_system) {
                                                          return entity_in_system.entity == entity;
                                                      }) != m_safe.entities_in_systems[key].cend ()) {

                                    }
                                }

                                m_safe.components[entity].clear ();

                                m_safe.commands.push (UpdateEntityInUnknownSystemsCommand {
                                        .entity = entity,
                                        .added = false
                                });

                                std::cout << "Killing entity : " << entity << std::endl;
                            } else if constexpr (std::is_same_v<command_type, AddComponentCommand>) {
                                auto entity = command.entity;
                                auto type   = command.type;

                                assert (exists (entity));

                                m_safe.components[entity][type] = std::move (command.component);

                                m_safe.commands.push (UpdateEntityInUnknownSystemsCommand {
                                        .entity = entity,
                                        .added = true
                                });

                                std::cout << "Adding component : " << type << " to entity : " << entity << std::endl;
                            } else if constexpr (std::is_same_v<command_type, RemoveComponentCommand>) {
                                auto entity = command.entity;
                                auto type   = command.type;

                                assert (exists (entity));

                                m_safe.components[entity].erase (type);

                                m_safe.commands.push (UpdateEntityInUnknownSystemsCommand {
                                        .entity = entity,
                                        .added = false
                                });

                                std::cout << "Removing component : " << type << " from entity : " << entity << std::endl;
                            } else if constexpr (std::is_same_v<command_type, UpdateEntityInUnknownSystemsCommand>) {
                                auto entity = command.entity;
                                auto added  = command.added;

                                for (auto& [key, system]: m_safe.systems) {
                                    m_safe.commands.push (UpdateEntityInKnownSystemCommand {
                                            .entity = entity,
                                            .key = key,
                                            .added = added
                                    });
                                }
                            } else if constexpr (std::is_same_v<command_type, UpdateEntityInKnownSystemCommand>) {
                                auto entity = command.entity;
                                auto key    = command.key;
                                auto added  = command.added;
                                auto& requirements = m_safe.systems[key].requirements;

                                auto valid = std::all_of (requirements.begin (),
                                                          requirements.end (),
                                                          [&] (auto&& requirement) {
                                                              return m_safe.components[entity].count (requirement) > 0;
                                                          });

                                auto registered = is_registered (entity,
                                                                 key);

                                if (added and valid and not registered) {
                                    auto components = hnz::map<hnz::Component::Type, hnz::raw<hnz::Component>> {};
                                    for (auto& requirement: requirements) {
                                        components[requirement] = m_safe.components[entity][requirement].get ();
                                    }

                                    if (m_safe.systems[key].usage == hnz::System::Usage::ON_REGISTRATION) {
                                        m_safe.systems[key].ptr->operator() (0.0f,
                                                                             entity,
                                                                             components,
                                                                             {});
                                    }

                                    m_safe.entities_in_systems[key].emplace_back (EntityInSystem {
                                            .entity = entity,
                                            .components = std::move (components)
                                    });

                                    std::cout << "Updating entity: " << entity << " in system: " << key << std::endl;
                                } else if (added and registered and not valid) {
                                    const auto& entity_in_system = std::find_if (m_safe.entities_in_systems[key].cbegin (),
                                                                                 m_safe.entities_in_systems[key].cend (),
                                                                                 [&] (const auto& entity_in_system) {
                                                                                     return entity_in_system.entity ==
                                                                                            entity;
                                                                                 });

                                    m_safe.entities_in_systems[key].erase (entity_in_system);

                                    std::cout << "Updating entity: " << entity << " in system: " << key << std::endl;
                                }
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

auto hnz::App::exists (const hnz::vector<hnz::entity>& entities) const -> bool {
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

auto hnz::App::view (const std::vector<hnz::Component::Type>& components) const -> hnz::vector<hnz::entity> {
    auto result = hnz::vector<hnz::entity> {};

    for (const auto& [entity, components_map]: m_safe.components) {
        if (std::ranges::all_of (components,
                                 [&] (const auto component) {
                                     return components_map.contains (component);
                                 })) {
            result.emplace_back (entity);
        }
    }

    return result;
}

auto hnz::App::is_registered (hnz::entity entity, hnz::System::Type type) -> bool {
    return not std::ranges::all_of (m_safe.entities_in_systems[type],
                                    [&] (const auto& entity_in_system) {
                                        return entity_in_system.entity != entity;
                                    });
}
