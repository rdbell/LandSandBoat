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

#include "test_login_error_packet.h"

#include "common/md52.h"
#include "login/login_helpers.h"

#include <array>
#include <cstring>
#include <iostream>
#include <string>

namespace
{

constexpr std::size_t PacketSize = 0x24;

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "login error packet self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

auto packetHashIsValid(const std::array<uint8, PacketSize>& packet) -> bool
{
    auto input = packet;
    std::memset(input.data() + 12, 0, 16);

    auto expected = std::array<uint8, 16>{};
    md5(input.data(), expected.data(), static_cast<int32>(input.size()));

    return std::memcmp(packet.data() + 12, expected.data(), expected.size()) == 0;
}

auto testErrorPacketLayout() -> bool
{
    auto packet = std::array<uint8, PacketSize>{};
    packet.fill(0xAA);
    loginHelpers::generateErrorMessage(packet.data(), 0x1234);

    bool ok = true;

    ok = expectTrue(packet[0] == 0x24, "packet size field") && ok;
    ok = expectTrue(packet[4] == 'I' && packet[5] == 'X' && packet[6] == 'F' && packet[7] == 'F', "IXFF marker") && ok;
    ok = expectTrue(packet[8] == 0x04, "result field") && ok;
    ok = expectTrue(packet[28] == 0x10, "legacy field") && ok;
    ok = expectTrue(packet[32] == 0x34 && packet[33] == 0x12, "little endian error code") && ok;
    ok = expectTrue(packetHashIsValid(packet), "md5 hash field") && ok;

    for (const auto index : { 1, 2, 3, 9, 10, 11, 29, 30, 31, 34, 35 })
    {
        ok = expectTrue(packet[index] == 0, "zeroed field") && ok;
    }

    return ok;
}

auto testDistinctErrorCodesUpdateHash() -> bool
{
    auto first = std::array<uint8, PacketSize>{};
    auto second = std::array<uint8, PacketSize>{};

    loginHelpers::generateErrorMessage(first.data(), 0x0000);
    loginHelpers::generateErrorMessage(second.data(), 0x1234);

    bool ok = true;
    ok = expectTrue(first[32] == 0 && first[33] == 0, "zero error code") && ok;
    ok = expectTrue(std::memcmp(first.data() + 12, second.data() + 12, 16) != 0, "distinct hashes") && ok;
    ok = expectTrue(packetHashIsValid(first), "first hash field") && ok;
    ok = expectTrue(packetHashIsValid(second), "second hash field") && ok;

    return ok;
}

} // namespace

auto runLoginErrorPacketSelfTests() -> bool
{
    bool ok = true;

    ok = testErrorPacketLayout() && ok;
    ok = testDistinctErrorCodesUpdateHash() && ok;

    return ok;
}
