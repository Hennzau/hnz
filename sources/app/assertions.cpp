#include <hnz/app.hpp>

auto hnz::App::exists (hnz::entity entity) const -> bool {
    return m_entities.find (entity) != m_entities.cend ();
}
