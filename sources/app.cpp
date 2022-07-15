#include <iostream>
#include <thread>

#include <hnz/app.hpp>

void hnz::App::join () {
    m_ecs_thread.join ();
}

void hnz::App::run () {
    std::this_thread::sleep_for (std::chrono::milliseconds (100));

    std::cout << "Hello from " << std::this_thread::get_id () << std::endl;
}

void hnz::App::build () {
    m_running.set (true);

    m_ecs_thread = std::thread ([&] () {
        while (m_running.is ()) {
            if (m_safe.commands.empty ()) {
                std::this_thread::sleep_for (std::chrono::milliseconds (500));
                continue;
            }

            auto& command_var = m_safe.commands.front ();

            std::visit ([&] (auto&& command) {
                using command_type = std::decay_t<decltype (command)>;

                if constexpr (std::is_same_v<command_type, SpawnCommand>) {
                    std::this_thread::sleep_for (std::chrono::milliseconds (200));

                    const auto entity = m_next_entity++;
                    command.entity.set_value (entity);
                    m_safe.entities.insert (entity);

                    std::cout << "Spawning entity : " << entity << std::endl;

                } else if constexpr (std::is_same_v<command_type, ParentingCommand>) {
                    std::this_thread::sleep_for (std::chrono::milliseconds (100));

                    auto entity = command.entity.get ();
                    auto parent = command.parent.get ();

                    m_safe.parents[entity].insert (parent);

                    std::cout << "Parenting entity : " << entity << " to : " << parent << std::endl;
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

