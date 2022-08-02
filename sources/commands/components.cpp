#include <hnz/app.hpp>

auto hnz::App::retrieve (hnz::entity entity) const -> hnz::vector<hnz::Component::Type> {
    auto components = hnz::vector<hnz::Component::Type> {};

    for (auto& [type, pool]: m_pools) {
        if (pool.contains (entity)) {
            components.emplace_back (type);
        }
    }

    return components;

}