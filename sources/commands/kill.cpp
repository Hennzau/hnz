#include <hnz/app.hpp>

#include <iostream>

auto hnz::App::process_kill (const hnz::App::KillCommand& command) -> void {
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

    for (auto& [type, ptr]: m_safe.components[entity]) {
        m_safe.commands.push (RemoveComponentCommand {
                .entity = entity,
                .type   = type
        });
    }

    std::cout << "Killing entity : " << entity << std::endl;
}