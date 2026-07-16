/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#pragma once

#include "map/linkshell.h"

// GET_LSMSG's validated slot selects the matching equipped linkshell, which
// receives PushLinkshellMessage. Character linkshell ownership stays in the
// packet handler.
namespace getlsmsgdispatch
{

struct Plan
{
    LinkshellSlot slot;
    bool          pushLinkshellMessage;
};

[[nodiscard]] constexpr auto PlanFor(const LinkshellSlot slot) -> Plan
{
    switch (slot)
    {
        case LinkshellSlot::LS1:
        case LinkshellSlot::LS2:
            return { slot, true };
    }

    // validate() rejects every other raw value before process(). This mirrors
    // process()'s switch if it is ever reached with one nevertheless.
    return { slot, false };
}

} // namespace getlsmsgdispatch
