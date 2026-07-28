/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_treasure_update_pool_dispatch_6960.h"

#include "map/entities/char_entity.h"
#include "map/enums/packet_s2c.h"
#include "map/treasure_pool.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "treasure updatePool dispatch 6960 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Direct CTreasurePool::updatePool characterization (slice 6960). The
// packet constructor's byte layout is pinned by the existing TROPHY_LIST
// packet tests; this pins updatePool's ten-slot recipient dispatch.
auto runTreasureUpdatePoolDispatch6960SelfTests() -> bool
{
    bool ok = true;

    CTreasurePool pool(TreasurePoolType::Solo);
    CCharEntity   visible;
    visible.status        = STATUS_TYPE::NORMAL;
    visible.PTreasurePool = &pool;

    pool.updatePool(&visible);
    ok = expect(visible.getPacketCount() == TREASUREPOOL_SIZE, "visible member receives every pool slot") && ok;
    for (const auto& packet : visible.getPacketList())
    {
        ok = expect(packet->getType() == static_cast<uint16>(PacketS2C::GP_SERV_COMMAND_TROPHY_LIST), "each slot queues TROPHY_LIST") && ok;
    }
    visible.clearPacketList();

    visible.status = STATUS_TYPE::DISAPPEAR;
    pool.updatePool(&visible);
    ok = expect(visible.getPacketCount() == 0, "disappeared member receives no packet") && ok;

    visible.status        = STATUS_TYPE::NORMAL;
    visible.PTreasurePool = nullptr;
    pool.updatePool(&visible);
    ok = expect(visible.getPacketCount() == 0, "mismatched member receives no packet") && ok;

    return ok;
}
