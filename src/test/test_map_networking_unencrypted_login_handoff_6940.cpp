/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_map_networking_unencrypted_login_handoff_6940.h"

#include <array>
#include <cstddef>
#include <cstring>
#include <iostream>

#include "common/md52.h"
#include "common/mmo.h"
#include "common/scheduler.h"
#include "map/map_config.h"
#include "map/map_networking.h"
#include "map/map_statistics.h"

namespace
{

auto expect(const bool actual, const char* label) -> bool
{
    if (!actual)
    {
        std::cerr << "map networking unencrypted-login handoff self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto makeLoginFrame(const uint32 charID) -> std::array<uint8, FFXI_HEADER_SIZE + sizeof(GP_CLI_LOGIN) + 16>
{
    auto frame = std::array<uint8, FFXI_HEADER_SIZE + sizeof(GP_CLI_LOGIN) + 16>{};
    auto login = GP_CLI_LOGIN{};
    login.id   = 0x00A;
    login.UniqueNo = charID;

    const auto* loginBytes = reinterpret_cast<const uint8*>(&login);
    for (std::size_t index = offsetof(GP_CLI_LOGIN, unknown01); index < sizeof(login); ++index)
    {
        login.LoginPacketCheck = static_cast<uint8>(login.LoginPacketCheck + loginBytes[index]);
    }
    std::memcpy(frame.data() + FFXI_HEADER_SIZE, &login, sizeof(login));
    md5(frame.data() + FFXI_HEADER_SIZE, frame.data() + FFXI_HEADER_SIZE + sizeof(login), static_cast<int32>(sizeof(login)));
    return frame;
}

} // namespace

auto runMapNetworkingUnencryptedLoginHandoff6940SelfTests() -> bool
{
    auto config         = MapConfig{};
    config.isTestServer = true;

    Scheduler     scheduler(1);
    MapStatistics statistics;
    MapNetworking networking(scheduler, statistics, config);
    auto          frame = makeLoginFrame(42);
    auto          size  = frame.size();

    return expect(networking.recv_parse(frame.data(), &size, nullptr, IPP{}) == -1,
                  "valid unencrypted login rejects when no pending session exists");
}
