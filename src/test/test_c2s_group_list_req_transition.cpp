/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_c2s_group_list_req_transition.h"

#include <iostream>
#include <string_view>

#include "map/packets/c2s/0x076_group_list_req.h"

namespace
{

auto expect(const bool condition, const std::string_view label) -> bool
{
    if (!condition)
    {
        std::cerr << "c2s GROUP_LIST_REQ transition self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto testDispatchSelection() -> bool
{
    return expect(grouplistreqhelpers::SelectDispatch(true) == grouplistreqhelpers::Dispatch::ReloadPartyMembers, "party reloads members") &&
           expect(grouplistreqhelpers::SelectDispatch(false) == grouplistreqhelpers::Dispatch::SendEmptyGroupTable, "no party sends empty group table");
}

} // namespace

auto runC2SGroupListReqTransitionSelfTests() -> bool
{
    return testDispatchSelection();
}
