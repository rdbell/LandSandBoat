#pragma once

#include <cstdint>

// Pure takeDamage absorb-to-MP and Nightmare wake-up gates with fully injected
// inputs. Parity: internal/takedamagegates (slice 1697).
//
// Reference:
//   src/map/entities/battle_entity.cpp
//     CBattleEntity::takeDamage (~1068–1126) — ABSORB_DMG_TO_MP → addMP
//   src/map/lua/lua_base_entity.cpp
//     CLuaBaseEntity::takeDamage (~10295–10391) — wakeUp / Nightmare gates
//
// Host retains entity mutation (addMP, addHP, WakeUp / DelStatusEffect).
// Helpers take injected amounts, mod percent, flag, and SleepI tier only.
//
// Distinct from ConvertDmgToMP / COVER_TO_MP + ABSORB_PHYSDMG_TO_MP (float
// intermediate). This path uses integer amount * percent / 100 cast to int16.

namespace takedamagegateshelpers
{

// PlayerAvatarNightmareTierMin is SleepI tier for player-avatar Nightmare.
// LSB: lua_base_entity.cpp takeDamage — "Tier 4 = Player Avatar Nightmare".
constexpr std::uint16_t PlayerAvatarNightmareTierMin = 4;

// DiabolosNightmareTierMin is SleepI tier for Diabolos NM Nightmare.
// LSB: lua_base_entity.cpp takeDamage — "Tier 11 = Diabolos NM Nightmare".
//
// Note: status-effect DelStatusEffectsByFlag Damage-skip uses a different
// Diabolos threshold (tier >= 5); takeDamage wake-up uses 11.
constexpr std::uint16_t DiabolosNightmareTierMin = 11;

// AbsorbDamageToMP is the pure form of CBattleEntity::takeDamage's
// ABSORB_DMG_TO_MP conversion. Returns the MP amount the host should restore
// via addMP; 0 means no restore.
//
// LSB (battle_entity.cpp):
//   if (getMod(Mod::ABSORB_DMG_TO_MP) > 0) {
//     int16 absorbedMP = (int16)(amount * getMod(Mod::ABSORB_DMG_TO_MP) / 100);
//     if (absorbedMP > 0) { addMP(absorbedMP); }
//   }
//
// Integer product/division (not float). Narrowing to int16 matches LSB before
// the positive check. Negative or zero absorbed amounts return 0.
inline auto AbsorbDamageToMP(const std::int32_t amount, const std::int16_t absorbPercent) -> std::int32_t
{
    if (absorbPercent <= 0)
    {
        return 0;
    }
    const auto absorbedMP = static_cast<std::int16_t>(amount * static_cast<std::int32_t>(absorbPercent) / 100);
    if (absorbedMP <= 0)
    {
        return 0;
    }
    return static_cast<std::int32_t>(absorbedMP);
}

// ResolveWakeUp is the pure form of CLuaBaseEntity::takeDamage's Nightmare
// wake-up gate. Returns whether the host should strip sleep-family effects
// after positive damage (caller still gates amount > 0 and applies WakeUp).
//
// LSB (lua_base_entity.cpp):
//   bool wakeUp = flags.wakeUp (default true);
//   if (SleepI present && tier >= 4) {            // player avatar Nightmare
//     if (!breakBind) wakeUp = false;             // DoT / non-bind-break dmg
//     if (wakeUp && tier >= 11) wakeUp = false;   // Diabolos NM Nightmare
//   }
//
// Equivalent compact form:
//   if tier >= 4 && (!breakBind || tier >= 11) { wakeUp = false }
//
// nightmareTier is the SleepI effect tier when present, or 0 when absent
// (host injects 0 when GetStatusEffect(SleepI) is null).
inline auto ResolveWakeUp(const bool wakeUpFlag, const std::uint16_t nightmareTier, const bool breakBind) -> bool
{
    if (nightmareTier >= PlayerAvatarNightmareTierMin &&
        (!breakBind || nightmareTier >= DiabolosNightmareTierMin))
    {
        return false;
    }
    return wakeUpFlag;
}

} // namespace takedamagegateshelpers
