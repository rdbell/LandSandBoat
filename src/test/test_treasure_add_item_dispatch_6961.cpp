/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_treasure_add_item_dispatch_6961.h"

#include "map/enums/packet_s2c.h"
#include "map/entities/char_entity.h"
#include "map/packets/s2c/0x0d2_trophy_list.h"
#include "map/treasure_pool.h"

#include <cstddef>
#include <iostream>

namespace
{

constexpr auto trophyItemNoOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TROPHY_LIST::PacketData, TrophyItemNo);
constexpr auto entryOffset        = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TROPHY_LIST::PacketData, Entry);

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "treasure addItem dispatch 6961 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectNewItemPacket(CCharEntity& member) -> bool
{
    bool ok = expect(member.getPacketCount() == 1, "each member receives one packet");
    if (!ok)
    {
        return false;
    }

    const auto& packet = member.getPacketList().front();
    ok = expect(packet->getType() == static_cast<uint16>(PacketS2C::GP_SERV_COMMAND_TROPHY_LIST), "packet is TROPHY_LIST") && ok;
    ok = expect(packet->ref<uint16>(trophyItemNoOffset) == 1, "packet carries inserted item") && ok;
    ok = expect(packet->ref<uint8>(entryOffset) == 0, "new item is not historical entry") && ok;
    return ok;
}

} // namespace

// Direct CTreasurePool::addItem characterization (slice 6961). A party pool
// avoids solo auto-resolution, so the test isolates the live per-member
// TROPHY_LIST broadcast after insertion. Item ID 1 is part of the fixture data
// loaded by xi_test.
auto runTreasureAddItemDispatch6961SelfTests() -> bool
{
    bool ok = true;

    CTreasurePool pool(TreasurePoolType::Party);
    CCharEntity   first;
    CCharEntity   second;
    first.PTreasurePool  = &pool;
    second.PTreasurePool = &pool;
    pool.addMember(&first);
    pool.addMember(&second);
    first.clearPacketList();
    second.clearPacketList();

    ok = expect(pool.addItem(1, nullptr) == 1, "insert increments pool count") && ok;
    ok = expectNewItemPacket(first) && ok;
    ok = expectNewItemPacket(second) && ok;

    first.PTreasurePool  = nullptr;
    second.PTreasurePool = nullptr;
    return ok;
}
