#pragma once

#include <algorithm>
#include <cstdint>
#include <cstdlib>

// Pure CBattleEntity HP/MP resource helpers with fully injected inputs.
// Parity: internal/battleresource (slice 1644).
//
// Reference: src/map/entities/battle_entity.cpp
//   CBattleEntity::isFullyHealed (~166–176)
//   CBattleEntity::addHP (~1027–1054)
//   CBattleEntity::addMP (~1056–1066)
//
// Host side-effects (MakeEntityStandUp, updatemask |= UPDATE_HP) are expressed
// as result flags. GetMaxHP/GetMaxMP are injected as maxHP/maxMP (typically
// health.modhp/modmp). isFullyHealed uses raw health.maxhp/maxmp.

namespace battleresourcehelpers
{

struct AddResourceResult
{
    std::int32_t NewValue{};
    std::int32_t ReturnedAbsDelta{};
    std::int32_t AppliedDelta{}; // residual = current - cap (LSB request reassignment)
    bool         SetUpdateMask{};
    bool         StandUp{};
    bool         ForceUnkillableOne{};
};

// IsFullyHealed mirrors CBattleEntity::isFullyHealed.
// isAlive is injected; maxHP/maxMP are raw maxima (not modhp/modmp).
inline auto IsFullyHealed(const bool        isAlive,
                          const std::int32_t hp,
                          const std::int32_t maxHP,
                          const std::int32_t mp,
                          const std::int32_t maxMP) -> bool
{
    if (!isAlive)
    {
        return false;
    }
    return hp >= maxHP && mp >= maxMP;
}

// ResolveAddHP mirrors CBattleEntity::addHP.
// residual = current - cap; StandUp when residual > 0 (damage taken).
// Unkillable rewrites NewValue to 1 after residual/return are computed.
inline auto ResolveAddHP(const std::int32_t current,
                         const std::int32_t delta,
                         const std::int32_t maxHP,
                         const bool         unkillable) -> AddResourceResult
{
    if (current == 0 && delta < 0)
    {
        return {};
    }

    const std::int32_t cap      = std::clamp(current + delta, static_cast<std::int32_t>(0), maxHP);
    const std::int32_t residual = current - cap;

    AddResourceResult out{};
    out.NewValue         = cap;
    out.AppliedDelta     = residual;
    out.ReturnedAbsDelta = std::abs(residual);
    out.SetUpdateMask    = residual != 0;
    out.StandUp          = residual > 0;

    if (cap == 0 && unkillable)
    {
        out.NewValue           = 1;
        out.ForceUnkillableOne = true;
    }
    return out;
}

// ResolveAddMP mirrors CBattleEntity::addMP (no dead/stand/unkillable paths).
inline auto ResolveAddMP(const std::int32_t current, const std::int32_t delta, const std::int32_t maxMP)
    -> AddResourceResult
{
    const std::int32_t cap      = std::clamp(current + delta, static_cast<std::int32_t>(0), maxMP);
    const std::int32_t residual = current - cap;

    AddResourceResult out{};
    out.NewValue         = cap;
    out.AppliedDelta     = residual;
    out.ReturnedAbsDelta = std::abs(residual);
    out.SetUpdateMask    = residual != 0;
    return out;
}

} // namespace battleresourcehelpers
