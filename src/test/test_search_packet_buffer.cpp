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

#include "test_search_packet_buffer.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "common/logging.h"
#include "common/types/maybe.h"
#include "search/search.h"

namespace
{

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "search packet buffer self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "search packet buffer self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto testAcceptedPacketCopiesBytesAndSize() -> bool
{
    const auto expected = std::array<std::uint8_t, 5>{ 0x10, 0x20, 0x30, 0x40, 0x50 };
    auto input          = expected;
    auto packet         = searchPacket(input.data(), static_cast<std::uint16_t>(input.size()));

    std::fill(input.begin(), input.end(), 0xEE);

    bool ok = true;
    ok      = expectEqualInt(packet.getSize(), expected.size(), "accepted packet size") && ok;
    ok      = expectTrue(std::memcmp(packet.getData(), expected.data(), expected.size()) == 0, "accepted packet bytes copied") && ok;
    return ok;
}

auto testMaxSizePacketIsAccepted() -> bool
{
    auto input = std::array<std::uint8_t, searchPacket::max_size>{};
    for (std::size_t i = 0; i < input.size(); ++i)
    {
        input[i] = static_cast<std::uint8_t>((i * 31U) + 7U);
    }

    auto packet = searchPacket(input.data(), static_cast<std::uint16_t>(input.size()));

    bool ok = true;
    ok      = expectEqualInt(packet.getSize(), searchPacket::max_size, "max-size packet size") && ok;
    ok      = expectTrue(std::memcmp(packet.getData(), input.data(), input.size()) == 0, "max-size packet bytes copied") && ok;
    return ok;
}

auto testShortPacketCopiesPrefixAndSize() -> bool
{
    const auto input = std::array<std::uint8_t, 3>{ 0xAA, 0xBB, 0xCC };
    auto mutableInput = input;
    auto packet       = searchPacket(mutableInput.data(), static_cast<std::uint16_t>(mutableInput.size()));

    bool ok = true;
    ok      = expectEqualInt(packet.getSize(), input.size(), "short packet size") && ok;
    ok      = expectTrue(std::memcmp(packet.getData(), input.data(), input.size()) == 0, "short packet bytes copied") && ok;
    return ok;
}

auto testOversizedPacketIsRejected() -> bool
{
    auto input = std::array<std::uint8_t, searchPacket::max_size + 1>{};
    auto packet = searchPacket(input.data(), static_cast<std::uint16_t>(input.size()));

    return expectEqualInt(packet.getSize(), 0, "oversized packet size");
}

} // namespace

auto runSearchPacketBufferSelfTests() -> bool
{
    return testAcceptedPacketCopiesBytesAndSize() &&
           testMaxSizePacketIsAccepted() &&
           testShortPacketCopiesPrefixAndSize() &&
           testOversizedPacketIsRejected();
}
