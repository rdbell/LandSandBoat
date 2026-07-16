#pragma once

#include "0x05e_conquest.h"

#include <array>
#include <cstddef>

#include "conquest_system.h"

namespace conquesthelpers
{

constexpr auto ActiveRegionCount = static_cast<std::size_t>(REGION_TYPE::TAVNAZIA) + 1;

struct NationCounts
{
    uint8 sandoria{};
    uint8 bastok{};
    uint8 windurst{};
};

struct RegionInfluence
{
    uint8 owner{};
    int32 sandoria{};
    int32 bastok{};
    int32 windurst{};
    int32 beastmen{};
};

struct OverviewState
{
    uint8 astralCandescence{};
    uint8 alZahbiOrders{};
    uint8 mamookLevel{};
    uint8 halvungLevel{};
    uint8 arrapagoLevel{};
    uint8 mamookOrders{};
    uint8 halvungOrders{};
    uint8 arrapagoOrders{};
};

struct StrongholdState
{
    uint8 orders{};
    uint8 forces{};
    uint8 level{};
    bool  mirrorDestroyed{};
    uint8 mirrors{};
    uint8 prisoners{};
};

struct BesiegedState
{
    OverviewState   overview{};
    StrongholdState mamook{};
    StrongholdState halvung{};
    StrongholdState arrapago{};
    uint32          imperialStanding{};
};

struct Facts
{
    std::array<RegionInfluence, ActiveRegionCount> regions{};
    REGION_TYPE                                    currentRegion{ REGION_TYPE::UNKNOWN };
    NationCounts                                   regionControls{};
    NationCounts                                   prevRegionControls{};
    uint8                                          nextTally{};
    uint32                                         conquestPoints{};
    BesiegedState                                  besieged{};
};

[[nodiscard]] inline auto Percentage(const int32 value, const int64 total) -> uint8
{
    return static_cast<uint8>((static_cast<int64>(value) * 100) / (total == 0 ? 1 : total));
}

[[nodiscard]] inline auto PlanFor(const Facts& facts) -> GP_SERV_COMMAND_CONQUEST::PacketData
{
    auto packet = GP_SERV_COMMAND_CONQUEST::PacketData{};

    for (std::size_t regionId = 0; regionId < facts.regions.size(); ++regionId)
    {
        const auto& region = facts.regions[regionId];
        auto&       output = packet.Conquest.Regions[regionId];

        output.InfluenceRankingWithBeastmen = conquest::GetInfluenceRanking(region.sandoria, region.bastok, region.windurst, region.beastmen);
        output.InfluenceRankingNoBeastmen   = conquest::GetInfluenceRanking(region.sandoria, region.bastok, region.windurst);
        output.InfluenceGraphics            = conquest::GetInfluenceGraphics(region.sandoria, region.bastok, region.windurst, region.beastmen);
        output.Owner                        = region.owner + 1;

        if (facts.currentRegion == static_cast<REGION_TYPE>(regionId))
        {
            const int64 total         = region.sandoria + region.bastok + region.windurst;
            const int64 totalBeastmen = total + region.beastmen;

            packet.Conquest.CurrentRegionSandoria    = Percentage(region.sandoria, totalBeastmen);
            packet.Conquest.CurrentRegionBastok      = Percentage(region.bastok, totalBeastmen);
            packet.Conquest.CurrentRegionWindurst    = Percentage(region.windurst, totalBeastmen);
            packet.Conquest.CurrentRegionSandoriaPct = Percentage(region.sandoria, total);
            packet.Conquest.CurrentRegionBastokPct   = Percentage(region.bastok, total);
            packet.Conquest.CurrentRegionWindurstPct = Percentage(region.windurst, total);
            packet.Conquest.CurrentRegionBeastmen    = Percentage(region.beastmen, totalBeastmen);
        }
    }

    packet.Conquest.Balance        = conquest::GetBalance(facts.regionControls.sandoria, facts.regionControls.bastok, facts.regionControls.windurst, facts.prevRegionControls.sandoria, facts.prevRegionControls.bastok, facts.prevRegionControls.windurst);
    packet.Conquest.Alliance       = conquest::GetAlliance(facts.regionControls.sandoria, facts.regionControls.bastok, facts.regionControls.windurst, facts.prevRegionControls.sandoria, facts.prevRegionControls.bastok, facts.prevRegionControls.windurst);
    packet.Conquest.NextTally      = facts.nextTally;
    packet.Conquest.ConquestPoints = facts.conquestPoints;
    packet.Conquest.Unknown9C      = 1;

    const auto copyStronghold = [](const StrongholdState& input, besiegedstronghold_t& output)
    {
        output.Orders          = input.orders;
        output.Forces          = input.forces;
        output.Level           = input.level;
        output.MirrorDestroyed = input.mirrorDestroyed;
        output.Mirrors         = input.mirrors / 2;
        output.Prisoners       = input.prisoners;
    };

    packet.Besieged.Overview.AstralCandescence = facts.besieged.overview.astralCandescence;
    packet.Besieged.Overview.AlZahbiOrders     = facts.besieged.overview.alZahbiOrders;
    packet.Besieged.Overview.MamookLevel       = facts.besieged.overview.mamookLevel;
    packet.Besieged.Overview.HalvungLevel      = facts.besieged.overview.halvungLevel;
    packet.Besieged.Overview.ArrapagoLevel     = facts.besieged.overview.arrapagoLevel;
    packet.Besieged.Overview.MamookOrders      = facts.besieged.overview.mamookOrders;
    packet.Besieged.Overview.HalvungOrders     = facts.besieged.overview.halvungOrders;
    packet.Besieged.Overview.ArrapagoOrders    = facts.besieged.overview.arrapagoOrders;
    packet.Besieged.Overview.Unknown           = 1;
    copyStronghold(facts.besieged.mamook, packet.Besieged.MamookStronghold);
    copyStronghold(facts.besieged.halvung, packet.Besieged.HalvungStronghold);
    copyStronghold(facts.besieged.arrapago, packet.Besieged.ArrapagoStronghold);
    packet.Besieged.ImperialStanding = facts.besieged.imperialStanding;

    return packet;
}

} // namespace conquesthelpers
