#include <hnz/app.hpp>

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

auto hnz::App::has (hnz::entity entity) const -> bool {
    return m_safe.components.contains (entity);
}

auto hnz::App::has (hnz::entity entity, hnz::Component::Type type) const -> bool {
    return m_safe.components[entity].contains (type);
}