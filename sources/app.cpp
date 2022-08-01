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
                                process_parent (command);
                            } else if constexpr (std::is_same_v<command_type, UnParentingUnknownCommand>) {
                                process_unparent_unknown (command);
                            } else if constexpr (std::is_same_v<command_type, UnParentingKnownCommand>) {
                                process_unparent_known (command);
                            } else if constexpr (std::is_same_v<command_type, KillCommand>) {
                                process_kill (command);
                            } else if constexpr (std::is_same_v<command_type, AddComponentCommand>) {
                                process_add_component (command);
                            } else if constexpr (std::is_same_v<command_type, RemoveComponentCommand>) {
                                process_remove_component (command);
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

