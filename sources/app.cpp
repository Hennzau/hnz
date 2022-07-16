#include <iostream>
#include <thread>

#include <hnz/app.hpp>

void hnz::App::join () {
    m_ecs_thread.join ();
}

void hnz::App::run () {
}

void hnz::App::build () {
    m_running.set (true);

    m_ecs_thread = std::thread ([&] () {
        while (m_running.is ()) {
            if (m_safe.commands.empty ()) {
                continue;
            }

            auto& command_var = m_safe.commands.front ();

            std::visit ([&] (auto&& command) {
                using command_type = std::decay_t<decltype (command)>;

                if constexpr (std::is_same_v<command_type, SpawnCommand>) {
                    const auto entity = m_next_entity++;
                    command.entity.set_value (entity);
                    m_safe.entities.insert (entity);

                    std::cout << "Spawning entity : " << entity << std::endl;

                } else if constexpr (std::is_same_v<command_type, ParentingCommand>) {
                    auto entity = command.entity.get ();
                    auto parent = command.parent.get ();

                    m_safe.parents[parent].insert (entity);

                    std::cout << "Parenting entity : " << entity << " is child of : " << parent << std::endl;
                } else if constexpr (std::is_same_v<command_type, KillCommand>) {

                    auto entity    = command.entity.get ();
                    auto genealogy = command.genealogy;

                    auto list    = genealogy ? m_safe.parents[entity] : hnz::set<hnz::entity> {};
                    auto deja_vu = hnz::set<hnz::entity> {};

                    while (not list.empty ()) {
                        auto current = *list.begin ();
                        list.erase (current);

                        deja_vu.insert (current);

                        for (auto& child: m_safe.parents[current]) {
                            if (std::find (deja_vu.begin (), deja_vu.end (), child) == deja_vu.end ()) {
                                list.insert (child);
                            }
                        }

                        m_safe.entities.erase (current);
                        m_safe.parents.erase (current);
                        m_safe.components[current].clear ();

                        std::cout << "Killing entity : " << current << std::endl;
                    }

                    m_safe.entities.erase (entity);
                    m_safe.parents.erase (entity);
                    m_safe.components[entity].clear ();

                    std::cout << "Killing entity : " << entity << std::endl;
                } else if constexpr (std::is_same_v<command_type, AddComponentCommand>) {
                    auto entity = command.entity.get ();
                    auto type   = command.type;


                    m_safe.components[entity][type] = std::move (command.component);
                    m_safe.updated_entities.insert (entity);

                    std::cout << "Adding component : " << type << " to entity : " << entity << std::endl;
                } else if constexpr (std::is_same_v<command_type, RemoveComponentCommand>) {
                    auto entity = command.entity.get ();
                    auto type   = command.type;

                    m_safe.components[entity].erase (type);
                    m_safe.updated_entities.insert (entity);

                    std::cout << "Removing component : " << type << " from entity : " << entity << std::endl;
                } else {
                    std::cout << "Unknown command " << std::endl;
                }
            }, command_var);

            m_safe.commands.pop ();
        }
    });
}

auto hnz::App::spawn () -> hnz::fentity {
    auto promise = hnz::pentity {};
    auto future  = promise.get_future ().share ();

    m_safe.commands.push (SpawnCommand {
            .entity = std::move (promise)
    });

    return future;
}

auto hnz::App::spawn (hnz::fentity& parent) -> hnz::fentity {
    auto entity = spawn ();

    m_safe.commands.push (ParentingCommand {
            .entity = entity,
            .parent = parent
    });

    return entity;
}

auto hnz::App::kill (hnz::fentity& entity) -> void {
    m_safe.commands.push (KillCommand {
            .entity     = entity,
            .genealogy  = false
    });
}

auto hnz::App::kill_all (hnz::fentity& entity) -> void {
    m_safe.commands.push (KillCommand {
            .entity     = entity,
            .genealogy  = true
    });
}

