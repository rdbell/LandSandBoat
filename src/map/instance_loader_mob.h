#pragma once

#include "common/cbasetypes.h"

namespace instanceloader
{

// MOBTYPE bits from mob_entity.h; kept local so this pure loader helper has no
// entity dependency.
constexpr uint8 MobTypeNotorious   = 0x02;
constexpr uint8 MobTypeFished      = 0x04;
constexpr uint8 MobTypeBattlefield = 0x10;
constexpr uint8 MobTypeEvent       = 0x20;

struct MobLoadPlan
{
    bool  load{};
    bool  setAlwaysAggro{};
    int16 charmable{};

    auto operator==(const MobLoadPlan&) const -> bool = default;
};

// PlanMobLoad mirrors LoadInstance's zero-position filter and instance-only
// special-mob modifier overrides.
inline auto PlanMobLoad(const float x, const float y, const float z, const uint8 mobType, const int16 baseCharmable) -> MobLoadPlan
{
    if (x == 0.0f && y == 0.0f && z == 0.0f)
    {
        return {};
    }

    const bool special = (mobType & (MobTypeEvent | MobTypeFished | MobTypeBattlefield | MobTypeNotorious)) != 0;
    return {
        true,
        (mobType & MobTypeEvent) != 0,
        special ? static_cast<int16>(0) : baseCharmable,
    };
}

} // namespace instanceloader
