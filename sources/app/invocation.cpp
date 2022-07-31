#include <hnz/app.hpp>

auto hnz::App::spawn () -> hnz::entity {
    auto entity = m_next_entity++;

    m_safe.entities.emplace_back (entity);

    return entity;
}

auto hnz::App::spawn_group (hnz::u64 count) -> hnz::vector<hnz::entity> {
    auto result = hnz::vector<hnz::entity> {};

    for (auto i = 0u; i < count; ++i) {
        result.emplace_back (spawn ());
    }

    return result;
}

auto hnz::App::spawn (hnz::entity& parent) -> hnz::entity {
    auto entity = spawn ();

    m_safe.commands.push (ParentingCommand {
            .entity = entity,
            .parent = parent
    });

    return entity;
}

auto hnz::App::spawn_group (hnz::entity& parent, hnz::u64 count) -> hnz::vector<hnz::entity> {
    auto result = spawn_group (count);

    for (const auto& entity: result) {
        m_safe.commands.push (ParentingCommand {
                .entity = entity,
                .parent = parent
        });
    }

    return result;
}

auto hnz::App::kill (hnz::entity& entity, bool genealogy) -> void {
    assert (exists (entity));

    m_safe.commands.push (KillCommand {
            .entity = entity,
            .genealogy = genealogy
    });

    m_safe.commands.push (UnParentingUnknownCommand {
            .entity = entity
    });
}
