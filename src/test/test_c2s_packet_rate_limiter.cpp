/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses/

===========================================================================
*/

#include "test_c2s_packet_rate_limiter.h"

#include <chrono>
#include <cstddef>
#include <iostream>
#include <string>
#include <unordered_map>

#include "common/cbasetypes.h"
#include "common/timer.h"
#include "map/packets/c2s/rate_limiter.h"

using namespace std::chrono_literals;

namespace
{

using TimestampMap = std::unordered_map<uint16, timer::time_point>;

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s packet rate limiter self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectFalse(bool actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "c2s packet rate limiter self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectSize(std::size_t actual, std::size_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s packet rate limiter self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTimestamp(const TimestampMap& timestamps, uint16 packetId, timer::time_point expected, const std::string& label) -> bool
{
    const auto it = timestamps.find(packetId);
    if (it == timestamps.end())
    {
        std::cerr << "c2s packet rate limiter self-test failed: " << label << " missing timestamp for packet " << packetId << '\n';
        return false;
    }
    if (it->second != expected)
    {
        std::cerr << "c2s packet rate limiter self-test failed: " << label << " timestamp mismatch\n";
        return false;
    }
    return true;
}

auto expectPacketLimit(PacketRateLimiter& limiter, uint16 packetId, timer::duration limit, const std::string& label) -> bool
{
    const auto base = timer::time_point{} + 24h;
    auto       timestamps = TimestampMap{};

    bool ok = true;
    ok      = expectFalse(limiter.isLimited(timestamps, packetId, base), label + " first packet allowed") && ok;
    ok      = expectTimestamp(timestamps, packetId, base, label + " first packet timestamp") && ok;

    timestamps[packetId] = base;
    ok = expectTrue(limiter.isLimited(timestamps, packetId, base + limit - 1ns), label + " packet before boundary limited") && ok;
    ok = expectTimestamp(timestamps, packetId, base, label + " before boundary timestamp unchanged") && ok;

    timestamps[packetId] = base;
    ok = expectFalse(limiter.isLimited(timestamps, packetId, base + limit), label + " packet at boundary allowed") && ok;
    ok = expectTimestamp(timestamps, packetId, base + limit, label + " boundary timestamp updated") && ok;

    timestamps[packetId] = base;
    ok = expectFalse(limiter.isLimited(timestamps, packetId, base + limit + 1ns), label + " packet after boundary allowed") && ok;
    ok = expectTimestamp(timestamps, packetId, base + limit + 1ns, label + " after boundary timestamp updated") && ok;

    return ok;
}

auto isExpectedLimitedPacket(uint16 packetId) -> bool
{
    switch (packetId)
    {
        case 0x017:
        case 0x03B:
        case 0x05D:
        case 0x083:
        case 0x0AA:
        case 0x0B7:
        case 0x0F4:
        case 0x0F5:
        case 0x11B:
        case 0x11D:
            return true;
        default:
            return false;
    }
}

auto testDefaultLimits() -> bool
{
    auto limiter = PacketRateLimiter();

    bool ok = true;
    ok      = expectPacketLimit(limiter, 0x017, 1s, "0x017") && ok;
    ok      = expectPacketLimit(limiter, 0x03B, 1s, "0x03b") && ok;
    ok      = expectPacketLimit(limiter, 0x05D, 1s, "0x05d") && ok;
    ok      = expectPacketLimit(limiter, 0x083, 250ms, "0x083") && ok;
    ok      = expectPacketLimit(limiter, 0x0AA, 250ms, "0x0aa") && ok;
    ok      = expectPacketLimit(limiter, 0x0B7, 1s, "0x0b7") && ok;
    ok      = expectPacketLimit(limiter, 0x0F4, 1s, "0x0f4") && ok;
    ok      = expectPacketLimit(limiter, 0x0F5, 1s, "0x0f5") && ok;
    ok      = expectPacketLimit(limiter, 0x11B, 2s, "0x11b") && ok;
    ok      = expectPacketLimit(limiter, 0x11D, 2s, "0x11d") && ok;
    return ok;
}

auto testUnlimitedPacketDoesNotTouchTimestamps() -> bool
{
    auto limiter      = PacketRateLimiter();
    const auto base   = timer::time_point{} + 48h;
    const auto old    = base - 1h;
    auto       stamps = TimestampMap{ { 0x999, old } };

    bool ok = true;
    ok      = expectFalse(limiter.isLimited(stamps, 0x999, base), "unlimited packet allowed") && ok;
    ok      = expectSize(stamps.size(), 1, "unlimited packet timestamp count") && ok;
    ok      = expectTimestamp(stamps, 0x999, old, "unlimited packet timestamp unchanged") && ok;

    for (uint16 packetId = 0; packetId <= 300; ++packetId)
    {
        if (isExpectedLimitedPacket(packetId))
        {
            continue;
        }

        ok = expectFalse(limiter.isLimited(stamps, packetId, base), "unlimited packet " + std::to_string(packetId) + " allowed") && ok;
        ok = expectSize(stamps.size(), 1, "unlimited packet " + std::to_string(packetId) + " timestamp count") && ok;
    }

    return ok;
}

auto testRepeatedPacketBeforeLimitDoesNotUpdateTimestamp() -> bool
{
    auto       limiter = PacketRateLimiter();
    const auto base    = timer::time_point{} + 72h;
    auto       stamps  = TimestampMap{};

    bool ok = true;
    ok      = expectFalse(limiter.isLimited(stamps, 0x017, base), "first limited packet allowed") && ok;
    ok      = expectTrue(limiter.isLimited(stamps, 0x017, base + 999ms), "repeated packet before limit blocked") && ok;
    ok      = expectTimestamp(stamps, 0x017, base, "repeated packet timestamp unchanged") && ok;
    return ok;
}

auto testDifferentPacketIdsTrackIndependently() -> bool
{
    auto       limiter = PacketRateLimiter();
    const auto base    = timer::time_point{} + 96h;
    auto       stamps  = TimestampMap{};

    bool ok = true;
    ok      = expectFalse(limiter.isLimited(stamps, 0x017, base), "first 0x017 packet allowed") && ok;
    ok      = expectTimestamp(stamps, 0x017, base, "first 0x017 timestamp") && ok;

    ok = expectFalse(limiter.isLimited(stamps, 0x083, base + 100ms), "first 0x083 packet allowed") && ok;
    ok = expectTimestamp(stamps, 0x083, base + 100ms, "first 0x083 timestamp") && ok;

    ok = expectTrue(limiter.isLimited(stamps, 0x017, base + 100ms), "0x017 still limited") && ok;
    ok = expectTimestamp(stamps, 0x017, base, "0x017 timestamp unchanged") && ok;

    ok = expectFalse(limiter.isLimited(stamps, 0x083, base + 350ms), "0x083 at its boundary allowed") && ok;
    ok = expectTimestamp(stamps, 0x083, base + 350ms, "0x083 boundary timestamp updated") && ok;

    ok = expectTrue(limiter.isLimited(stamps, 0x017, base + 350ms), "0x017 remains independently limited") && ok;
    ok = expectTimestamp(stamps, 0x017, base, "0x017 timestamp still unchanged") && ok;
    return ok;
}

auto testSeparateTimestampMapsBehaveLikeSeparateCharacters() -> bool
{
    auto       limiter         = PacketRateLimiter();
    const auto base            = timer::time_point{} + 120h;
    auto       firstCharacter  = TimestampMap{};
    auto       secondCharacter = TimestampMap{};

    bool ok = true;
    ok      = expectFalse(limiter.isLimited(firstCharacter, 0x11B, base), "first character first packet allowed") && ok;
    ok      = expectTrue(limiter.isLimited(firstCharacter, 0x11B, base + 1s), "first character repeated packet limited") && ok;
    ok      = expectTimestamp(firstCharacter, 0x11B, base, "first character timestamp unchanged") && ok;

    ok = expectFalse(limiter.isLimited(secondCharacter, 0x11B, base + 1s), "second character first packet allowed") && ok;
    ok = expectTimestamp(secondCharacter, 0x11B, base + 1s, "second character timestamp inserted") && ok;
    return ok;
}

} // namespace

auto runC2SPacketRateLimiterSelfTests() -> bool
{
    bool ok = true;
    ok      = testDefaultLimits() && ok;
    ok      = testUnlimitedPacketDoesNotTouchTimestamps() && ok;
    ok      = testRepeatedPacketBeforeLimitDoesNotUpdateTimestamp() && ok;
    ok      = testDifferentPacketIdsTrackIndependently() && ok;
    ok      = testSeparateTimestampMapsBehaveLikeSeparateCharacters() && ok;
    return ok;
}
