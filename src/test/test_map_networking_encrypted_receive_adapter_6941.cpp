/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_map_networking_encrypted_receive_adapter_6941.h"

#include <array>
#include <cstring>
#include <iostream>

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
        std::cerr << "map networking encrypted receive adapter self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runMapNetworkingEncryptedReceiveAdapter6941SelfTests() -> bool
{
    auto config         = MapConfig{};
    config.isTestServer = true;

    Scheduler     scheduler(1);
    MapStatistics statistics;
    MapNetworking networking(scheduler, statistics, config);
    MapSession    session{};
    const auto key = std::array<uint32, 5>{ 1, 2, 3, 4, 5 };
    std::memcpy(session.blowfish.key, key.data(), sizeof(session.blowfish.key));
    session.initBlowfish();

    // A fixed all-zero payload keeps this envelope test independent of the
    // legacy codec's unstable arbitrary-byte fixture behavior.
    const auto payload = std::array<uint8, 12>{};
    auto       frame   = NetworkBuffer{};
    std::memcpy(frame.data() + FFXI_HEADER_SIZE, payload.data(), payload.size());
    auto maybePacketSize = networking.compressPacket(frame.data(), FFXI_HEADER_SIZE + payload.size());
    if (!maybePacketSize)
    {
        return expect(false, "compression succeeded");
    }

    auto size = FFXI_HEADER_SIZE + payload.size();
    networking.finalizePacket(frame.data(), &size, *maybePacketSize, &session, MapNetworking::UsePreviousKey::No);
    const auto decryptCount = networking.recv_parse(frame.data(), &size, &session, IPP{});

    return expect(decryptCount == 0, "current key decrypts") &&
           expect(session.hasDecryptedPacket, "current key marks session") &&
           expect(size == FFXI_HEADER_SIZE + payload.size(), "decompression restores payload size") &&
           expect(std::memcmp(frame.data() + FFXI_HEADER_SIZE, payload.data(), payload.size()) == 0, "decompression restores payload");
}
