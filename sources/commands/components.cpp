#include <hnz/app.hpp>

#include <iostream>

auto hnz::App::process_add_component (hnz::App::AddComponentCommand& command) -> void {
    auto entity = command.entity;
    auto type   = command.type;

    assert (exists (entity));

    m_safe.components[entity][type].operator= (std::move (command.component));

    std::cout << "Adding component : " << type << " to entity : " << entity << std::endl;
}

auto hnz::App::process_remove_component (const hnz::App::RemoveComponentCommand& command) -> void {
    auto entity = command.entity;
    auto type   = command.type;

    assert (has (entity));

    m_safe.components[entity].erase (type);

    std::cout << "Removing component : " << type << " from entity : " << entity << std::endl;
}