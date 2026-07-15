#pragma once

#include <cstdint>

// Pure CBattleEntity::SetMLevel / SetSLevel policy without DB writes.
// Parity: internal/subjoblevel (slice 1657).
//
// Reference: src/map/entities/battle_entity.cpp
//   CBattleEntity::SetMLevel (~1746–1756)
//   CBattleEntity::SetSLevel (~1758–1793)
//
// Host retains entity storage (m_mlvl / m_slvl), settings lookup
// (map.SUBJOB_RATIO, map.INCLUDE_MOB_SJ), TYPE_MOB projection into isMob,
// and char_stats DB updates for TYPE_PC.
//
// Default retail settings (settings/default/map.lua):
//   map.SUBJOB_RATIO   = 1   // 1/2 (75/37, 99/49)
//   map.INCLUDE_MOB_SJ = false

namespace subjoblevelhelpers
{

// SUBJOB_RATIO pins from settings/default/map.lua.
constexpr std::uint8_t RatioNone      = 0;
constexpr std::uint8_t RatioHalf      = 1;
constexpr std::uint8_t RatioTwoThirds = 2;
constexpr std::uint8_t RatioEqual     = 3;

// ResolveMainLevel mirrors CBattleEntity::SetMLevel pure clamp.
// LSB: m_mlvl = (mlvl == 0 ? 1 : mlvl);
constexpr auto ResolveMainLevel(const std::uint8_t requested) -> std::uint8_t
{
    return requested == 0 ? static_cast<std::uint8_t>(1) : requested;
}

// CapHalf mirrors ratio 1: m_mlvl >> 1, with mlvl == 1 → 1.
constexpr auto CapHalf(const std::uint8_t mainLevel) -> std::uint8_t
{
    if (mainLevel == 1)
    {
        return 1;
    }
    return static_cast<std::uint8_t>(mainLevel >> 1);
}

// CapTwoThirds mirrors ratio 2: (m_mlvl * 2) / 3, with mlvl == 1 → 1.
// Widens through int so high main levels do not overflow uint8 (matches C++
// usual arithmetic conversions on the original expression).
constexpr auto CapTwoThirds(const std::uint8_t mainLevel) -> std::uint8_t
{
    if (mainLevel == 1)
    {
        return 1;
    }
    return static_cast<std::uint8_t>((static_cast<int>(mainLevel) * 2) / 3);
}

// CapEqual mirrors ratio 3: m_mlvl, with mlvl == 1 → 1.
constexpr auto CapEqual(const std::uint8_t mainLevel) -> std::uint8_t
{
    if (mainLevel == 1)
    {
        return 1;
    }
    return mainLevel;
}

// ClampToCap returns min(requestedSub, cap).
constexpr auto ClampToCap(const std::uint8_t requestedSub, const std::uint8_t cap) -> std::uint8_t
{
    return requestedSub > cap ? cap : requestedSub;
}

// ResolveSubLevel mirrors CBattleEntity::SetSLevel pure policy.
//
// if (!includeMobSJ && isMob) return mainLevel; // 1:1 when mob SJ disabled
// switch (ratio):
//   0: return 0
//   1: return ClampToCap(requestedSub, CapHalf(mainLevel))
//   2: return ClampToCap(requestedSub, CapTwoThirds(mainLevel))
//   3: return ClampToCap(requestedSub, CapEqual(mainLevel))
//   default: return requestedSub  // LSB error path leaves m_slvl; pure default
//
// isMob is host-projected objtype == TYPE_MOB (pets are TYPE_PET → false).
// includeMobSJ / ratio are injected map settings. DB update is host-owned.
constexpr auto ResolveSubLevel(const std::uint8_t mainLevel,
                               const std::uint8_t requestedSub,
                               const bool         isMob,
                               const bool         includeMobSJ,
                               const std::uint8_t ratio) -> std::uint8_t
{
    if (!includeMobSJ && isMob)
    {
        return mainLevel;
    }

    switch (ratio)
    {
        case RatioNone:
            return 0;
        case RatioHalf:
            return ClampToCap(requestedSub, CapHalf(mainLevel));
        case RatioTwoThirds:
            return ClampToCap(requestedSub, CapTwoThirds(mainLevel));
        case RatioEqual:
            return ClampToCap(requestedSub, CapEqual(mainLevel));
        default:
            // LSB logs ShowError and does not assign m_slvl. Without prior
            // state the pure helper returns the requested sub level.
            return requestedSub;
    }
}

} // namespace subjoblevelhelpers
