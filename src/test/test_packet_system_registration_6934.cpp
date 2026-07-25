/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_packet_system_registration_6934.h"

#include <iostream>

#include "map/packet_system.h"

namespace
{

auto expect(bool actual, const char* label) -> bool
{
    if (!actual)
    {
        std::cerr << "packet system registration self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runPacketSystemRegistration6934SelfTests() -> bool
{
    bool ok = true;
    ok      = expect(PacketSystem::registeredHandlerCount() == 129, "registered handler count") && ok;
    ok      = expect(PacketSystem::isHandlerRegistered(0x00A), "LOGIN registered") && ok;
    ok      = expect(PacketSystem::isHandlerRegistered(0x085), "SHOP_SELL_SET registered") && ok;
    ok      = expect(PacketSystem::isHandlerRegistered(0x11D), "JUMP registered") && ok;
    ok      = expect(!PacketSystem::isHandlerRegistered(0x000), "leading sparse hole") && ok;
    ok      = expect(!PacketSystem::isHandlerRegistered(0x001), "interior sparse hole") && ok;
    ok      = expect(!PacketSystem::isHandlerRegistered(0x066), "FISHING alias slot") && ok;
    ok      = expect(!PacketSystem::isHandlerRegistered(0x11E), "adjacent sparse hole") && ok;
    ok      = expect(!PacketSystem::isHandlerRegistered(512), "out-of-range id") && ok;
    return ok;
}
