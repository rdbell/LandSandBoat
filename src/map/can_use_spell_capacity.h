#pragma once

#include <cstdint>

// Pure xi.spells.canUseSpellOverride gift catalog + inject form.
// Parity: internal/canusespell (slice 0883); production wire slice 1582.

namespace canusespellhelpers
{

constexpr std::int16_t UnknownCost = -1;

constexpr std::uint8_t JobWHM = 3;
constexpr std::uint8_t JobBLM = 4;
constexpr std::uint8_t JobRDM = 5;
constexpr std::uint8_t JobPLD = 7;
constexpr std::uint8_t JobDRK = 8;
constexpr std::uint8_t JobBRD = 10;
constexpr std::uint8_t JobNIN = 13;
constexpr std::uint8_t JobSCH = 20;
constexpr std::uint8_t JobGEO = 21;
constexpr std::uint8_t JobRUN = 22;

constexpr int GiftJP100  = 100;
constexpr int GiftJP550  = 550;
constexpr int GiftJP1200 = 1200;

// Minimum main level for spent-JP host inject (Lua getSpentJobPoints Level Sync gate).
constexpr std::uint8_t MinLevelForSpentJP = 99;

struct GiftRow
{
    std::uint8_t  job{};
    std::uint16_t spell{};
    int           cost{};
};

// jobPointSpellGiftMap flattened (60 rows).
// clang-format off
inline constexpr GiftRow GiftCatalog[] = {
    { JobWHM, 848, GiftJP100 },  // Reraise IV
    { JobWHM, 893, GiftJP1200 }, // Full Cure
    { JobBLM, 849, GiftJP100 },  // Fire VI
    { JobBLM, 850, GiftJP100 },  // Blizzard VI
    { JobBLM, 851, GiftJP100 },  // Aero VI
    { JobBLM, 852, GiftJP100 },  // Stone VI
    { JobBLM, 853, GiftJP100 },  // Thunder VI
    { JobBLM, 854, GiftJP100 },  // Water VI
    { JobBLM, 881, GiftJP550 },  // Aspir III
    { JobBLM, 367, GiftJP1200 }, // Death
    { JobRDM, 148, GiftJP100 },  // Fire V
    { JobRDM, 153, GiftJP100 },  // Blizzard V
    { JobRDM, 158, GiftJP100 },  // Aero V
    { JobRDM, 163, GiftJP100 },  // Stone V
    { JobRDM, 168, GiftJP100 },  // Thunder V
    { JobRDM, 173, GiftJP100 },  // Water V
    { JobRDM, 884, GiftJP550 },  // Addle II
    { JobRDM, 882, GiftJP550 },  // Distract III
    { JobRDM, 883, GiftJP550 },  // Frazzle III
    { JobRDM, 894, GiftJP1200 }, // Refresh III
    { JobRDM, 895, GiftJP1200 }, // Temper II
    { JobPLD, 855, GiftJP100 },  // Enlight II
    { JobDRK, 856, GiftJP100 },  // Endark II
    { JobDRK, 880, GiftJP100 },  // Drain III
    { JobBRD, 871, GiftJP100 },  // Fire Threnody II
    { JobBRD, 872, GiftJP100 },  // Ice Threnody II
    { JobBRD, 873, GiftJP100 },  // Wind Threnody II
    { JobBRD, 874, GiftJP100 },  // Earth Threnody II
    { JobBRD, 875, GiftJP100 },  // Lightning Threnody II
    { JobBRD, 876, GiftJP100 },  // Water Threnody II
    { JobBRD, 877, GiftJP100 },  // Light Threnody II
    { JobBRD, 878, GiftJP100 },  // Dark Threnody II
    { JobNIN, 340, GiftJP100 },  // Utsusemi: San
    { JobSCH, 860, GiftJP100 },  // Firestorm II
    { JobSCH, 861, GiftJP100 },  // Hailstorm II
    { JobSCH, 859, GiftJP100 },  // Windstorm II
    { JobSCH, 857, GiftJP100 },  // Sandstorm II
    { JobSCH, 862, GiftJP100 },  // Thunderstorm II
    { JobSCH, 858, GiftJP100 },  // Rainstorm II
    { JobSCH, 864, GiftJP100 },  // Aurorastorm II
    { JobSCH, 863, GiftJP100 },  // Voidstorm II
    { JobSCH, 888, GiftJP1200 }, // Pyrohelix II
    { JobSCH, 889, GiftJP1200 }, // Cryohelix II
    { JobSCH, 887, GiftJP1200 }, // Anemohelix II
    { JobSCH, 885, GiftJP1200 }, // Geohelix II
    { JobSCH, 892, GiftJP1200 }, // Luminohelix II
    { JobSCH, 891, GiftJP1200 }, // Noctohelix II
    { JobGEO, 148, GiftJP100 },  // Fire V
    { JobGEO, 153, GiftJP100 },  // Blizzard V
    { JobGEO, 158, GiftJP100 },  // Aero V
    { JobGEO, 163, GiftJP100 },  // Stone V
    { JobGEO, 168, GiftJP100 },  // Thunder V
    { JobGEO, 173, GiftJP100 },  // Water V
    { JobGEO, 865, GiftJP1200 }, // Fira III
    { JobGEO, 866, GiftJP1200 }, // Blizzara III
    { JobGEO, 867, GiftJP1200 }, // Aera III
    { JobGEO, 868, GiftJP1200 }, // Stonera III
    { JobGEO, 869, GiftJP1200 }, // Thundara III
    { JobGEO, 870, GiftJP1200 }, // Watera III
    { JobRUN, 493, GiftJP550 },  // Temper
};
// clang-format on

constexpr auto GiftCatalogSize() -> std::size_t
{
    return sizeof(GiftCatalog) / sizeof(GiftCatalog[0]);
}

// getSpellJobPointCostForJob pure. Returns UnknownCost when not a gift for job.
constexpr auto GiftCost(const std::uint8_t job, const std::uint16_t spellID) -> int
{
    for (const auto& row : GiftCatalog)
    {
        if (row.job == job && row.spell == spellID)
        {
            return row.cost;
        }
    }
    return UnknownCost;
}

// Spent JP host inject after Level Sync gate (main level < 99 → 0).
constexpr auto SpentJobPointsForOverride(const bool isPC, const std::uint8_t mainLevel, const int rawSpentJP) -> int
{
    if (!isPC || mainLevel < MinLevelForSpentJP)
    {
        return 0;
    }
    return rawSpentJP;
}

// xi.spells.canUseSpellOverride after injects.
constexpr auto CanUseOverride(const std::uint8_t job, const std::uint16_t spellID, const int spentJobPoints) -> bool
{
    const int cost = GiftCost(job, spellID);
    if (cost == UnknownCost)
    {
        return false;
    }
    return spentJobPoints >= cost;
}

} // namespace canusespellhelpers
