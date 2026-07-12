#pragma once

#include <algorithm>
#include <cstdint>

// Pure DistributeGil amount policy from charutils.

namespace distributegilhelpers
{

// Gil distribution range pin (TODO verify in LSB).
constexpr float GilShareDistance = 100.0f;

// ShouldApplyMobGilMultiplier mirrors gil != 0 && multiplier >= 0.
constexpr auto ShouldApplyMobGilMultiplier(const std::uint32_t gil, const float mobGilMultiplier) -> bool
{
    return gil != 0 && mobGilMultiplier >= 0.0f;
}

// ApplyMobGilMultiplier mirrors static_cast<uint32>(gil * multiplier).
inline auto ApplyMobGilMultiplier(const std::uint32_t gil, const float mobGilMultiplier) -> std::uint32_t
{
    return static_cast<std::uint32_t>(static_cast<float>(gil) * mobGilMultiplier);
}

// ShouldApplyAllMobsGilBonus mirrors ALL_MOBS_GIL_BONUS non-zero.
constexpr auto ShouldApplyAllMobsGilBonus(const std::uint8_t allMobsGilBonus) -> bool
{
    return allMobsGilBonus != 0;
}

// AllMobsGilBonusAmount mirrors bonusSetting * mobLevel.
constexpr auto AllMobsGilBonusAmount(const std::uint8_t allMobsGilBonus, const std::uint8_t mobLevel) -> std::uint32_t
{
    return static_cast<std::uint32_t>(allMobsGilBonus) * static_cast<std::uint32_t>(mobLevel);
}

// ClampAllMobsGilBonus mirrors clamp(gBonus, 1, maxGilBonus).
constexpr auto ClampAllMobsGilBonus(const std::uint32_t gBonus, const std::uint32_t maxGilBonus) -> std::uint32_t
{
    return std::clamp(gBonus, std::uint32_t{ 1 }, maxGilBonus);
}

// ApplyAllMobsGilBonus adds clamped bonus to gil.
constexpr auto ApplyAllMobsGilBonus(const std::uint32_t gil, const std::uint32_t clampedBonus) -> std::uint32_t
{
    return gil + clampedBonus;
}

// ShouldApplyGilfinder mirrors gilfinderLevel > 0.
constexpr auto ShouldApplyGilfinder(const std::uint8_t gilfinderLevel) -> bool
{
    return gilfinderLevel > 0;
}

// GilfinderRollMax is gilfinderLevel * 16 (exclusive upper for xirand 0..max).
constexpr auto GilfinderRollMax(const std::uint8_t gilfinderLevel) -> std::uint16_t
{
    return static_cast<std::uint16_t>(gilfinderLevel * 16);
}

// GilfinderMultiplier mirrors 1 + (128 + roll) / 256.0 with roll in [0, level*16).
inline auto GilfinderMultiplier(const std::uint16_t roll) -> double
{
    return 1.0 + ((128.0 + static_cast<double>(roll)) / 256.0);
}

// ApplyGilfinder multiplies gil by gilfinder multiplier (truncating via double→uint32 assign in host).
inline auto ApplyGilfinder(const std::uint32_t gil, const double multiplier) -> std::uint32_t
{
    return static_cast<std::uint32_t>(static_cast<double>(gil) * multiplier);
}

// ShouldApplyKillshotGilBonus mirrors killshotBonus > 0.
constexpr auto ShouldApplyKillshotGilBonus(const std::int16_t killshotBonus) -> bool
{
    return killshotBonus > 0;
}

// KillshotGilMultiplier mirrors (100.0 + bonus) / 100.0.
inline auto KillshotGilMultiplier(const std::int16_t killshotBonus) -> double
{
    return (100.0 + static_cast<double>(killshotBonus)) / 100.0;
}

// ApplyKillshotGil multiplies gil by killshot multiplier.
inline auto ApplyKillshotGil(const std::uint32_t gil, const double multiplier) -> std::uint32_t
{
    return static_cast<std::uint32_t>(static_cast<double>(gil) * multiplier);
}

// IsGilShareMemberEligible mirrors same zone && within distance (distance precomputed by host).
constexpr auto IsGilShareMemberEligible(const bool sameZone, const bool withinDistance) -> bool
{
    return sameZone && withinDistance;
}

// GilPerPerson mirrors gil / memberCount (integer).
constexpr auto GilPerPerson(const std::uint32_t gil, const std::size_t memberCount) -> std::uint32_t
{
    if (memberCount == 0)
    {
        return 0;
    }
    return static_cast<std::uint32_t>(gil / memberCount);
}

// ShouldAwardSoloGil mirrors solo path within distance.
constexpr auto ShouldAwardSoloGil(const bool hasParty, const bool withinDistance) -> bool
{
    return !hasParty && withinDistance;
}

} // namespace distributegilhelpers
