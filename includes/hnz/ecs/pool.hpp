#pragma once

#include <hnz/types.hpp>

#include <hnz/ecs/entity.hpp>
#include <hnz/ecs/component.hpp>

#include <iostream>

namespace hnz {
    struct Pool {
        hnz::vector<hnz::entity>                entities;
        hnz::vector<hnz::owner<hnz::Component>> components;
        hnz::map<hnz::entity, hnz::u32>         sparse_to_dense;

        auto retrieve (hnz::entity entity) -> hnz::Component* {
            return components[sparse_to_dense.at (entity)].get ();
        }

        [[nodiscard]] auto retrieve (hnz::entity entity) const -> hnz::Component* {
            return components[sparse_to_dense.at (entity)].get ();
        }

        [[nodiscard]] auto contains (hnz::entity entity) const -> bool {
            return sparse_to_dense.contains (entity);
        }

        // add an entity to the pool, make its component and update the sparse to dense mapping
        template<typename T, typename... Args>
        auto add (hnz::entity entity, Args&& ... args) -> void {
            entities.emplace_back (entity);
            components.emplace_back (std::make_unique<T> (std::forward<Args> (args)...));
            sparse_to_dense[entity] = entities.size () - 1;
        }

        // remove an entity from the pool, and update the sparse to dense mapping
        auto remove (hnz::entity entity) -> void {
            auto index = sparse_to_dense[entity];

            entities[index]   = entities.back ();
            components[index] = std::move (components.back ());

            entities.pop_back ();
            components.pop_back ();

            sparse_to_dense.erase (entity);

            sparse_to_dense[entities[index]] = index;
        }
    };
}