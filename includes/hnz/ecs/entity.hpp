#pragma once

#include <hnz/types.hpp>

#include <future>

namespace hnz {
    using entity = hnz::u64;

    using fentity = std::shared_future<hnz::entity>;
    using pentity = std::promise<hnz::entity>;

    static constexpr entity INVALID_ENTITY = 0;
}