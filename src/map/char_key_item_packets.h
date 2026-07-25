#pragma once

#include "common/mmo.h"

#include <array>
#include <tuple>

namespace keyitempackethelpers
{

constexpr auto BuildPlan() -> std::array<uint8, std::tuple_size_v<decltype(keyitems_t::tables)>>
{
    auto plan = std::array<uint8, std::tuple_size_v<decltype(keyitems_t::tables)>>{};
    for (uint8 table = 0; table < plan.size(); ++table)
    {
        plan[table] = table;
    }
    return plan;
}

} // namespace keyitempackethelpers
