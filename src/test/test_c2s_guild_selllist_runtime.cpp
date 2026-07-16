/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_c2s_guild_selllist_runtime.h"

#include <iostream>
#include <optional>
#include <vector>

#include "map/packets/c2s/0x0ad_guild_selllist.h"

auto runC2SGuildSellListRuntimeSelfTests() -> bool
{
    using guildselllisthelpers::BuildResponsePlan;
    using guildselllisthelpers::SourceEntry;

    const std::vector<std::optional<SourceEntry>> entries{
        SourceEntry{ .ItemNo = 0x1234, .Count = 5, .Max = 9, .Price = -7 },
        std::nullopt,
        SourceEntry{},
    };

    const auto absentNpc = BuildResponsePlan(false, entries);
    if (absentNpc.sendResponse || !absentNpc.items.empty())
    {
        std::cerr << "c2s GUILD_SELLLIST runtime self-test failed: absent NPC must not send a response\n";
        return false;
    }

    const auto presentNpc = BuildResponsePlan(true, entries);
    if (!presentNpc.sendResponse || presentNpc.items.size() != 2)
    {
        std::cerr << "c2s GUILD_SELLLIST runtime self-test failed: table rows were not converted\n";
        return false;
    }

    const auto& item = presentNpc.items[0];
    if (item.ItemNo != 0x1234 || item.Count != 5 || item.Max != 9 || item.Price != -7)
    {
        std::cerr << "c2s GUILD_SELLLIST runtime self-test failed: populated row changed during conversion\n";
        return false;
    }

    const auto& defaults = presentNpc.items[1];
    if (defaults.ItemNo != 0 || defaults.Count != 0 || defaults.Max != 0 || defaults.Price != 0)
    {
        std::cerr << "c2s GUILD_SELLLIST runtime self-test failed: missing Lua fields must default to zero\n";
        return false;
    }

    return true;
}
