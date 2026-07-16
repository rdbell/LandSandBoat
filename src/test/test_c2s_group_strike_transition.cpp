/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_c2s_group_strike_transition.h"

#include <iostream>
#include <string_view>

#include "map/packets/c2s/0x071_group_strike.h"

namespace
{

auto expect(const bool condition, const std::string_view label) -> bool
{
    if (!condition)
    {
        std::cerr << "c2s GROUP_STRIKE transition self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto testLocalPartyPlans() -> bool
{
    const auto other           = groupstrikehelpers::MakeLocalPartyPlan(true, false, true, true, true);
    const auto selfRemoveParty = groupstrikehelpers::MakeLocalPartyPlan(true, true, true, true, false);
    const auto selfDissolve    = groupstrikehelpers::MakeLocalPartyPlan(true, true, true, true, true);
    const auto noLocalVictim   = groupstrikehelpers::MakeLocalPartyPlan(false, false, false, false, false);

    return expect(other.allianceAction == groupstrikehelpers::AllianceAction::None && other.removeVictim, "other member removes victim") &&
           expect(selfRemoveParty.allianceAction == groupstrikehelpers::AllianceAction::RemoveParty && selfRemoveParty.removeVictim, "self removes alliance party before member") &&
           expect(selfDissolve.allianceAction == groupstrikehelpers::AllianceAction::Dissolve && selfDissolve.removeVictim, "self dissolves singleton before member") &&
           expect(noLocalVictim.allianceAction == groupstrikehelpers::AllianceAction::None && !noLocalVictim.removeVictim, "remote victim has no local plan");
}

auto testLocalAlliancePlans() -> bool
{
    const auto self       = groupstrikehelpers::MakeLocalAlliancePlan(true, true, true, false, false, false);
    const auto leader     = groupstrikehelpers::MakeLocalAlliancePlan(true, true, false, true, true, false);
    const auto singleton  = groupstrikehelpers::MakeLocalAlliancePlan(true, true, false, true, true, true);
    const auto nonLeader  = groupstrikehelpers::MakeLocalAlliancePlan(true, true, false, true, false, false);
    const auto nonMain    = groupstrikehelpers::MakeLocalAlliancePlan(true, true, false, false, true, false);
    const auto noAlliance = groupstrikehelpers::MakeLocalAlliancePlan(true, false, true, true, true, false);

    return expect(self.allianceAction == groupstrikehelpers::AllianceAction::RemoveParty, "self removes party") &&
           expect(leader.allianceAction == groupstrikehelpers::AllianceAction::RemoveParty, "main party targets party leader") &&
           expect(singleton.allianceAction == groupstrikehelpers::AllianceAction::Dissolve, "singleton alliance dissolves") &&
           expect(nonLeader.allianceAction == groupstrikehelpers::AllianceAction::None, "nonleader target no op") &&
           expect(nonMain.allianceAction == groupstrikehelpers::AllianceAction::None, "nonmain requester no op") &&
           expect(noAlliance.allianceAction == groupstrikehelpers::AllianceAction::None, "victim outside alliance no op");
}

} // namespace

auto runC2SGroupStrikeTransitionSelfTests() -> bool
{
    return testLocalPartyPlans() && testLocalAlliancePlans();
}
