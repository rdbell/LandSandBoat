#pragma once

#include <algorithm>
#include <cstdint>

// Pure xi.chocoboDig checkDiggingCooldowns / skill-up / fatigue injects.
// Parity: internal/chocobodig cooldown.go (slice 1595).

namespace chocobodighelpers
{

constexpr auto ClampInt(const int v, const int minV, const int maxV) -> int
{
    return std::max(minV, std::min(maxV, v));
}

// Active diggingZoneList (logic.lua; commented digInfo zones excluded).
constexpr auto IsDiggingZone(const std::uint16_t zoneId) -> bool
{
    switch (zoneId)
    {
        case 2:   // CARPENTERS_LANDING
        case 4:   // BIBIKI_BAY
        case 51:  // WAJAOM_WOODLANDS
        case 52:  // BHAFLAU_THICKETS
        case 100: // WEST_RONFAURE
        case 101: // EAST_RONFAURE
        case 102: // LA_THEINE_PLATEAU
        case 103: // VALKURM_DUNES
        case 104: // JUGNER_FOREST
        case 105: // BATALLIA_DOWNS
        case 106: // NORTH_GUSTABERG
        case 107: // SOUTH_GUSTABERG
        case 108: // KONSCHTAT_HIGHLANDS
        case 109: // PASHHOW_MARSHLANDS
        case 110: // ROLANBERRY_FIELDS
        case 114: // EASTERN_ALTEPA_DESERT
        case 115: // WEST_SARUTABARUTA
        case 116: // EAST_SARUTABARUTA
        case 117: // TAHRONGI_CANYON
        case 118: // BUBURIMU_PENINSULA
        case 119: // MERIPHATAUD_MOUNTAINS
        case 120: // SAUROMUGUE_CHAMPAIGN
        case 121: // THE_SANCTUARY_OF_ZITAH
        case 123: // YUHTUNGA_JUNGLE
        case 124: // YHOATOR_JUNGLE
        case 125: // WESTERN_ALTEPA_DESERT
            return true;
        default:
            return false;
    }
}

constexpr auto ZoneCooldownSeconds(const std::uint8_t skillRank) -> int
{
    return ClampInt(60 - static_cast<int>(skillRank) * 5, 10, 60);
}

constexpr auto DigCooldownSeconds(const std::uint8_t skillRank) -> int
{
    return ClampInt(15 - static_cast<int>(skillRank) * 5, 3, 16);
}

constexpr auto CooldownsReady(const std::int64_t currentTime,
                              const std::int64_t zoneInTime,
                              const std::int64_t lastDigTime,
                              const std::uint8_t skillRank) -> bool
{
    const auto zoneReadyAt = zoneInTime + static_cast<std::int64_t>(ZoneCooldownSeconds(skillRank));
    const auto digReadyAt  = lastDigTime + static_cast<std::int64_t>(DigCooldownSeconds(skillRank));
    return currentTime >= zoneReadyAt && currentTime >= digReadyAt;
}

constexpr auto DigCooldownAllowed(const bool         zoneAllowed,
                                 const std::int64_t currentTime,
                                 const std::int64_t zoneInTime,
                                 const std::int64_t lastDigTime,
                                 const std::uint8_t skillRank) -> bool
{
    if (!zoneAllowed)
    {
        return false;
    }
    return CooldownsReady(currentTime, zoneInTime, lastDigTime, skillRank);
}

constexpr auto DigSkillCap(const std::uint8_t skillRank) -> int
{
    return ClampInt(static_cast<int>(skillRank + 1) * 100, 0, 1000);
}

constexpr auto DigSkillUpRollSucceeds(const int roll) -> bool
{
    return roll >= 1 && roll <= 15;
}

constexpr auto DigSkillUpIncrement(const int realSkill, const int maxSkill, const int increment) -> int
{
    if (realSkill >= maxSkill || increment <= 0)
    {
        return 0;
    }
    if (realSkill + increment > maxSkill)
    {
        return maxSkill - realSkill;
    }
    return increment;
}

constexpr auto DigRankIncreases(const int realSkill, const int increment, const std::uint8_t skillRank) -> bool
{
    return realSkill + increment >= static_cast<int>(skillRank) * 100 + 100;
}

constexpr auto FatigueBlocksDig(const int digFatigueSetting, const int todayDigCount) -> bool
{
    return digFatigueSetting > 0 && digFatigueSetting <= todayDigCount;
}

constexpr auto PositionTooClose(const float distance) -> bool
{
    return distance < 5.0f;
}

constexpr auto DigRareRateAdjust(const int digRate, const bool hasRareAbility) -> int
{
    if (!hasRareAbility)
    {
        return digRate;
    }
    if (digRate >= 100)
    {
        return digRate / 2;
    }
    return digRate * 2;
}

inline auto MoonAdjustedRoll(const int rawRoll, const double moonMultiplier) -> int
{
    int v = static_cast<int>(static_cast<double>(rawRoll) * moonMultiplier);
    if (v < 1)
    {
        return 1;
    }
    if (v > 1000)
    {
        return 1000;
    }
    return v;
}

// 1.5 - abs(moonPhase-50)/50
constexpr auto MoonRollMultiplier(const int moonPhase) -> double
{
    const int absDelta = moonPhase >= 50 ? moonPhase - 50 : 50 - moonPhase;
    return 1.5 - static_cast<double>(absDelta) / 50.0;
}

} // namespace chocobodighelpers
