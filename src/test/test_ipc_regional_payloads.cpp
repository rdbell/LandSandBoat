/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

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

#include "test_ipc_regional_payloads.h"

#include "common/ipc_structs.h"
#include "common/regional_event.h"

#include <iostream>
#include <string>
#include <vector>

namespace
{

auto expectEqualInt(int actual, int expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "IPC regional payload self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectEqualBool(bool actual, bool expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "IPC regional payload self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto testConquestMessageValues() -> bool
{
    bool ok = true;
    ok      = expectEqualInt(W2M_WeeklyUpdateStart, 0, "W2M_WeeklyUpdateStart") && ok;
    ok      = expectEqualInt(W2M_WeeklyUpdateEnd, 1, "W2M_WeeklyUpdateEnd") && ok;
    ok      = expectEqualInt(W2M_BroadcastInfluencePoints, 2, "W2M_BroadcastInfluencePoints") && ok;
    ok      = expectEqualInt(W2M_BroadcastRegionControls, 3, "W2M_BroadcastRegionControls") && ok;
    ok      = expectEqualInt(M2W_GM_WeeklyUpdate, 4, "M2W_GM_WeeklyUpdate") && ok;
    ok      = expectEqualInt(M2W_GM_ConquestUpdate, 5, "M2W_GM_ConquestUpdate") && ok;
    ok      = expectEqualInt(M2W_AddInfluencePoints, 6, "M2W_AddInfluencePoints") && ok;
    return ok;
}

auto testDefaultPayloads() -> bool
{
    bool ok = true;

    const region_control_t control{};
    ok = expectEqualInt(control.current, 0, "region current default") && ok;
    ok = expectEqualInt(control.prev, 0, "region prev default") && ok;

    const influence_t influence{};
    ok = expectEqualInt(influence.sandoria_influence, 0, "sandoria influence default") && ok;
    ok = expectEqualInt(influence.bastok_influence, 0, "bastok influence default") && ok;
    ok = expectEqualInt(influence.windurst_influence, 0, "windurst influence default") && ok;
    ok = expectEqualInt(influence.beastmen_influence, 0, "beastmen influence default") && ok;

    const ConquestInfluenceUpdate influenceUpdate{};
    ok = expectEqualBool(influenceUpdate.shouldUpdateZones, false, "influence update flag default") && ok;
    ok = expectEqualInt(static_cast<int>(influenceUpdate.influences.size()), 0, "influence update vector default") && ok;

    const ConquestRegionControlUpdate controlUpdate{};
    ok = expectEqualInt(static_cast<int>(controlUpdate.regionControls.size()), 0, "region control update vector default") && ok;

    const ConquestAddInfluencePoints addInfluence{};
    ok = expectEqualInt(addInfluence.points, 0, "add influence points default") && ok;
    ok = expectEqualInt(addInfluence.nation, 0, "add influence nation default") && ok;
    ok = expectEqualInt(addInfluence.region, 0, "add influence region default") && ok;

    const ipc::ConquestEvent conquest{};
    ok = expectEqualInt(conquest.type, W2M_WeeklyUpdateStart, "conquest event type default") && ok;
    ok = expectEqualInt(static_cast<int>(conquest.payload.size()), 0, "conquest event payload default") && ok;

    const ipc::BesiegedEvent besieged{};
    ok = expectEqualInt(static_cast<int>(besieged.type), 0, "besieged event type default") && ok;
    ok = expectEqualInt(static_cast<int>(besieged.payload.size()), 0, "besieged event payload default") && ok;

    const ipc::CampaignEvent campaign{};
    ok = expectEqualInt(static_cast<int>(campaign.type), 0, "campaign event type default") && ok;
    ok = expectEqualInt(static_cast<int>(campaign.payload.size()), 0, "campaign event payload default") && ok;

    const ipc::ColonizationEvent colonization{};
    ok = expectEqualInt(static_cast<int>(colonization.type), 0, "colonization event type default") && ok;
    ok = expectEqualInt(static_cast<int>(colonization.payload.size()), 0, "colonization event payload default") && ok;

    return ok;
}

auto testAssignedPayloads() -> bool
{
    bool ok = true;

    const region_control_t control{ .current = 1, .prev = 2 };
    ok = expectEqualInt(control.current, 1, "assigned region current") && ok;
    ok = expectEqualInt(control.prev, 2, "assigned region prev") && ok;

    const influence_t influence{ .sandoria_influence = 10, .bastok_influence = 20, .windurst_influence = 30, .beastmen_influence = 40 };
    ok = expectEqualInt(influence.sandoria_influence, 10, "assigned sandoria influence") && ok;
    ok = expectEqualInt(influence.bastok_influence, 20, "assigned bastok influence") && ok;
    ok = expectEqualInt(influence.windurst_influence, 30, "assigned windurst influence") && ok;
    ok = expectEqualInt(influence.beastmen_influence, 40, "assigned beastmen influence") && ok;

    const ConquestAddInfluencePoints addInfluence{ .points = -50, .nation = 2, .region = 7 };
    ok = expectEqualInt(addInfluence.points, -50, "assigned add influence points") && ok;
    ok = expectEqualInt(addInfluence.nation, 2, "assigned add influence nation") && ok;
    ok = expectEqualInt(addInfluence.region, 7, "assigned add influence region") && ok;

    const ipc::ConquestEvent conquest{ .type = M2W_AddInfluencePoints, .payload = { 1, 2, 3 } };
    ok = expectEqualInt(conquest.type, M2W_AddInfluencePoints, "assigned conquest event type") && ok;
    ok = expectEqualInt(conquest.payload.at(0), 1, "assigned conquest payload first") && ok;
    ok = expectEqualInt(static_cast<int>(conquest.payload.size()), 3, "assigned conquest payload count") && ok;

    return ok;
}

auto testVectorCopyBehavior() -> bool
{
    bool ok = true;

    ConquestInfluenceUpdate influenceUpdate{
        .shouldUpdateZones = true,
        .influences        = { influence_t{ .sandoria_influence = 10 } },
    };
    auto copiedInfluenceUpdate = influenceUpdate;
    copiedInfluenceUpdate.influences[0].sandoria_influence = 99;
    ok = expectEqualBool(influenceUpdate.shouldUpdateZones, true, "copied influence update flag") && ok;
    ok = expectEqualInt(influenceUpdate.influences[0].sandoria_influence, 10, "copied influence vector owns values") && ok;

    ConquestRegionControlUpdate regionUpdate{
        .regionControls = { region_control_t{ .current = 1, .prev = 2 } },
    };
    auto copiedRegionUpdate = regionUpdate;
    copiedRegionUpdate.regionControls[0].current = 9;
    ok = expectEqualInt(regionUpdate.regionControls[0].current, 1, "copied region vector owns values") && ok;

    ipc::ConquestEvent event{ .type = W2M_BroadcastRegionControls, .payload = { 1, 2, 3 } };
    auto copiedEvent = event;
    copiedEvent.payload[0] = 9;
    ok = expectEqualInt(event.payload[0], 1, "copied event payload owns values") && ok;

    return ok;
}

} // namespace

auto runIPCRegionalPayloadSelfTests() -> bool
{
    return testConquestMessageValues() && testDefaultPayloads() && testAssignedPayloads() && testVectorCopyBehavior();
}
