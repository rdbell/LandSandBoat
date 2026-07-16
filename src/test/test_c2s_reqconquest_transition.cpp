/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_c2s_reqconquest_transition.h"

#include <iostream>
#include <string_view>

#include "map/packets/c2s/0x05a_reqconquest.h"

namespace
{

auto expect(const bool condition, const std::string_view label) -> bool
{
    if (!condition)
    {
        std::cerr << "c2s REQCONQUEST transition self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto testResponseOrderAndCampaignOmission() -> bool
{
    const auto plan = reqconquesthelpers::MakeResponsePlan();
    return expect(plan.size() == 2, "response count") &&
           expect(plan[0] == reqconquesthelpers::Response::Conquest, "conquest is first") &&
           expect(plan[1] == reqconquesthelpers::Response::Colonization, "colonization is second") &&
           expect(plan[0] != reqconquesthelpers::Response::Campaign &&
                      plan[1] != reqconquesthelpers::Response::Campaign,
                  "campaign is intentionally omitted");
}

} // namespace

auto runC2SReqConquestTransitionSelfTests() -> bool
{
    return testResponseOrderAndCampaignOmission();
}
