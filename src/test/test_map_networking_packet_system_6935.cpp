/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_map_networking_packet_system_6935.h"

#include <cstring>
#include <iostream>
#include <memory>

#include "common/scheduler.h"
#include "map/map_config.h"
#include "map/entities/char_entity.h"
#include "map/map_networking.h"
#include "map/map_statistics.h"
#include "map/packets/c2s/0x00a_login.h"

namespace
{

auto expect(bool actual, const char* label) -> bool
{
    if (!actual)
    {
        std::cerr << "map networking packet-system self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runMapNetworkingPacketSystem6935SelfTests() -> bool
{
    auto config         = MapConfig{};
    config.isTestServer = true;

    Scheduler     scheduler(1);
    MapStatistics statistics;
    MapNetworking networking(scheduler, statistics, config);

    bool ok = true;
    ok      = expect(networking.packetSystem().registeredHandlerCount() == 129, "owned handler count") && ok;
    ok      = expect(networking.packetSystem().isHandlerRegistered(0x00A), "owned LOGIN handler") && ok;
    ok      = expect(!networking.packetSystem().isHandlerRegistered(0x066), "owned FISHING alias hole") && ok;

    MapSession session{};
    session.PChar          = std::make_unique<CCharEntity>();
    session.PChar->PSession = &session;
    session.client_packet_id = 0;
    session.blowfish.status  = BLOWFISH_ACCEPTED;

    CBasicPacket login{};
    login.setType(0x00A);
    login.setSize(sizeof(GP_CLI_COMMAND_LOGIN));
    login.setSequence(1);

    NetworkBuffer buffer{};
    buffer[0] = 1; // enclosing datagram sequence
    std::memcpy(buffer.data() + FFXI_HEADER_SIZE, static_cast<uint8*>(login), login.getSize());
    auto size = static_cast<size_t>(FFXI_HEADER_SIZE) + login.getSize();
    networking.parse(buffer.data(), &size, &session);
    ok = expect(session.PChar->m_LastPacketType == 0x00A, "parse dispatches eligible login through owned packet system") && ok;
    return ok;
}
