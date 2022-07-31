#include <hnz/app.hpp>

#include <iostream>

auto hnz::App::process_parent (const hnz::App::ParentingCommand& command) -> void {
    assert (exists ({ command.entity, command.parent }));

    m_safe.parents[command.parent].emplace_back (command.entity);

    std::cout << "Parenting entity : " << command.entity << " is child of : " << command.parent << std::endl;
}

auto hnz::App::process_unparent_unknown (const hnz::App::UnParentingUnknownCommand& command) -> void {
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
}

auto hnz::App::process_unparent_known (const hnz::App::UnParentingKnownCommand& command) -> void {
    auto entity = command.entity;
    auto parent = command.parent;

    assert (exists ( parent ));

    m_safe.parents[parent].erase (std::find (m_safe.parents[parent].cbegin (),
                                             m_safe.parents[parent].cend (),
                                             entity));

    std::cout << "UnParenting entity : " << entity << " from : " << parent << std::endl;
}