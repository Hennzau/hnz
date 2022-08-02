#include <hnz/app.hpp>

auto hnz::App::spawn () -> hnz::entity {
    auto entity = m_next_valid_entity++;

    m_entities.insert (entity);

    return entity;
}
