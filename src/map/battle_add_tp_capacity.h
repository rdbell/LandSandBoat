#pragma once

#include "common/cbasetypes.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace battleaddtphelpers
{

// ENTITYTYPE pins from base_entity.h.
constexpr uint8 TypePC     = 0x01;
constexpr uint8 TypeMob    = 0x04;
constexpr uint8 TypePet    = 0x08;
constexpr uint8 TypeTrust  = 0x20;
constexpr uint8 TypeFellow = 0x40;

// TP clamp bounds from addTP.
constexpr int16 TPMin = 0;
constexpr int16 TPMax = 3000;

// UPDATE_HP pin used when tp delta is non-zero after scaling.
constexpr uint8 UpdateHP = 0x04;

// ResolveTPMultiplier mirrors the objtype branch for positive TP gains.
// isCharmedPet is (objtype == TYPE_MOB && PMaster != nullptr).
// Multipliers are injected from settings map.*_TP_MULTIPLIER.
inline auto ResolveTPMultiplier(
    const uint8 objtype,
    const bool isCharmedPet,
    const float playerTPMulti,
    const float petTPMulti,
    const float mobTPMulti,
    const float trustTPMulti,
    const float fellowTPMulti) -> float
{
    if (objtype == TypePC)
    {
        return playerTPMulti;
    }
    if (objtype == TypePet || isCharmedPet)
    {
        return petTPMulti;
    }
    if (objtype == TypeMob)
    {
        return mobTPMulti;
    }
    if (objtype == TypeTrust)
    {
        return trustTPMulti;
    }
    if (objtype == TypeFellow)
    {
        return fellowTPMulti;
    }
    return 1.0f;
}

// ApplyInhibitTP mirrors tp - (tp * INHIBIT_TP/100) for positive gains.
// inhibitPercent is getMod(Mod::INHIBIT_TP) as a percent (0–100+).
inline auto ApplyInhibitTP(const int16 tpGain, const int16 inhibitPercent) -> int16
{
    if (tpGain <= 0)
    {
        return tpGain;
    }
    const float reduce = static_cast<float>(inhibitPercent) / 100.0f;
    return static_cast<int16>(static_cast<float>(tpGain) - (static_cast<float>(tpGain) * reduce));
}

// ScaleTPGain mirrors positive-tp inhibit then entity multiplier.
// Non-positive tp is returned unchanged (no inhibit/multi).
inline auto ScaleTPGain(
    const int16 tp,
    const int16 inhibitPercent,
    const float tpMultiplier) -> int16
{
    if (tp <= 0)
    {
        return tp;
    }
    const auto inhibited = ApplyInhibitTP(tp, inhibitPercent);
    return static_cast<int16>(static_cast<float>(inhibited) * tpMultiplier);
}

// AddTPResult is the pure outcome of addTP after scaling.
struct AddTPResult
{
    int16 newTP{ 0 };          // clamped health.tp
    int16 returnedAbsDelta{ 0 }; // abs(health.tp - cap) before write — LSB returns abs(tp)
    bool  setUpdateHP{ false };  // tp != 0 after scaling
};

// ResolveAddTP applies clamp and computes return value.
// scaledTP is the post-inhibit/multiplier delta (or raw negative/zero).
// currentTP is health.tp before the change.
//
// LSB:
//   cap = clamp(health.tp + tp, 0, 3000)
//   tp  = health.tp - cap   // signed applied delta from old to new, inverted
//   health.tp = cap
//   return abs(tp)
inline auto ResolveAddTP(const int16 currentTP, const int16 scaledTP) -> AddTPResult
{
    AddTPResult out{};
    out.setUpdateHP = scaledTP != 0;
    const int16 cap = std::clamp(static_cast<int16>(currentTP + scaledTP), TPMin, TPMax);
    const int16 deltaFromOld = static_cast<int16>(currentTP - cap);
    out.newTP            = cap;
    out.returnedAbsDelta = static_cast<int16>(std::abs(deltaFromOld));
    return out;
}

// ResolveFullAddTP is the complete pure addTP pipeline.
inline auto ResolveFullAddTP(
    const int16 currentTP,
    const int16 requestedTP,
    const int16 inhibitPercent,
    const uint8 objtype,
    const bool isCharmedPet,
    const float playerTPMulti,
    const float petTPMulti,
    const float mobTPMulti,
    const float trustTPMulti,
    const float fellowTPMulti) -> AddTPResult
{
    const float multi = ResolveTPMultiplier(
        objtype, isCharmedPet, playerTPMulti, petTPMulti, mobTPMulti, trustTPMulti, fellowTPMulti);
    const int16 scaled = ScaleTPGain(requestedTP, inhibitPercent, multi);
    return ResolveAddTP(currentTP, scaled);
}

} // namespace battleaddtphelpers
