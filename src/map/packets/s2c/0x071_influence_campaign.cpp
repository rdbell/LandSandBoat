/*
===========================================================================

  Copyright (c) 2025 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses/

===========================================================================
*/

#include "0x071_influence_campaign.h"

#include "0x071_influence.h"
#include "campaign_system.h"
#include "entities/char_entity.h"

auto influencecampaignhelpers::PlanFor(const Facts& facts) -> GP_SERV_COMMAND_INFLUENCE::CAMPAIGN::PacketData
{
    auto packet = GP_SERV_COMMAND_INFLUENCE::CAMPAIGN::PacketData{};

    packet.Mode        = GP_SERV_COMMAND_INFLUENCE_MODE::Campaign;
    packet.Length      = 0xC4; // This is the size of the rest of the packet after Length but without the padding.
    packet.AlliedNotes = facts.alliedNotes;

    packet.ControlledAreas.Sandoria        = facts.controlSandoria;
    packet.Nations.Sandoria.Reconnaissance = facts.nations[0].reconnaissance;
    packet.Nations.Sandoria.Morale         = facts.nations[0].morale;
    packet.Nations.Sandoria.Prosperity     = facts.nations[0].prosperity;

    packet.ControlledAreas.Bastok        = facts.controlBastok;
    packet.Nations.Bastok.Reconnaissance = facts.nations[1].reconnaissance;
    packet.Nations.Bastok.Morale         = facts.nations[1].morale;
    packet.Nations.Bastok.Prosperity     = facts.nations[1].prosperity;

    packet.ControlledAreas.Windurst        = facts.controlWindurst;
    packet.Nations.Windurst.Reconnaissance = facts.nations[2].reconnaissance;
    packet.Nations.Windurst.Morale         = facts.nations[2].morale;
    packet.Nations.Windurst.Prosperity     = facts.nations[2].prosperity;

    packet.ControlledAreas.Beastman                   = facts.controlBeastman;
    packet.Nations.BeastmanOrc.Reconnaissance         = facts.nations[3].reconnaissance;
    packet.Nations.BeastmanOrc.Morale                 = facts.nations[3].morale;
    packet.Nations.BeastmanOrc.Prosperity             = facts.nations[3].prosperity;
    packet.Nations.BeastmanQuadav.Reconnaissance      = facts.nations[4].reconnaissance;
    packet.Nations.BeastmanQuadav.Morale              = facts.nations[4].morale;
    packet.Nations.BeastmanQuadav.Prosperity          = facts.nations[4].prosperity;
    packet.Nations.BeastmanYagudo.Reconnaissance      = facts.nations[5].reconnaissance;
    packet.Nations.BeastmanYagudo.Morale              = facts.nations[5].morale;
    packet.Nations.BeastmanYagudo.Prosperity          = facts.nations[5].prosperity;
    packet.Nations.BeastmanDarkKindred.Reconnaissance = facts.nations[6].reconnaissance;
    packet.Nations.BeastmanDarkKindred.Morale         = facts.nations[6].morale;
    packet.Nations.BeastmanDarkKindred.Prosperity     = facts.nations[6].prosperity;

    const int start   = facts.number == 0 ? 0 : 13;
    packet.ZoneOffset = start;

    for (int i = start; i < start + 13; i++)
    {
        const auto& region = facts.regions[i];
        const int   idx    = i - start;

        packet.Zones[idx].Owner                 = region.owner;
        packet.Zones[idx].CurrentFortifications = region.currentFortifications;
        packet.Zones[idx].CurrentResources      = region.currentResources;
        packet.Zones[idx].Heroism               = region.heroism;
        packet.Zones[idx].InfluenceSandoria     = region.influenceSandoria;
        packet.Zones[idx].InfluenceBastok       = region.influenceBastok;
        packet.Zones[idx].InfluenceWindurst     = region.influenceWindurst;
        packet.Zones[idx].InfluenceBeastman     = region.influenceBeastman;
        packet.Zones[idx].MaxFortifications     = region.maxFortifications;
        packet.Zones[idx].MaxResources          = region.maxResources;
    }
    return packet;
}

GP_SERV_COMMAND_INFLUENCE::CAMPAIGN::CAMPAIGN(CCharEntity* PChar, const CampaignState& state, const uint8 number)
{
    auto facts             = influencecampaignhelpers::Facts{};
    facts.alliedNotes      = campaign::GetAlliedNotes(PChar);
    facts.controlSandoria  = state.controlSandoria;
    facts.controlBastok    = state.controlBastok;
    facts.controlWindurst  = state.controlWindurst;
    facts.controlBeastman  = state.controlBeastman;
    facts.number           = number;

    for (std::size_t i = 0; i < facts.nations.size(); ++i)
    {
        facts.nations[i] = {
            .reconnaissance = state.nations[i].reconnaissance,
            .morale         = state.nations[i].morale,
            .prosperity     = state.nations[i].prosperity,
        };
    }

    const std::size_t start = number == 0 ? 0 : 13;
    for (std::size_t i = 0; i < 13; ++i)
    {
        const auto& region       = state.regions[start + i];
        facts.regions[start + i] = {
            .owner                 = region.nationControl,
            .currentFortifications = region.currentFortifications,
            .currentResources      = region.currentResources,
            .heroism               = region.heroism,
            .influenceSandoria     = region.influenceSandoria,
            .influenceBastok       = region.influenceBastok,
            .influenceWindurst     = region.influenceWindurst,
            .influenceBeastman     = region.influenceBeastman,
            .maxFortifications     = region.maxFortifications,
            .maxResources          = region.maxResources,
        };
    }

    this->data() = influencecampaignhelpers::PlanFor(facts);
}
