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

#include "test_login_packet_helpers.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "login/login_packets.h"

namespace
{

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "login packet helper self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "login packet helper self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto packetHeaderFieldsUnchanged(const packet_t& packet) -> bool
{
    bool ok = true;
    ok      = expectEqualInt(packet.packet_size, 0x01020304, "packet_size unchanged") && ok;
    ok      = expectEqualInt(packet.terminator, 0x11223344, "terminator unchanged") && ok;
    ok      = expectEqualInt(packet.command, 0x55667788, "command unchanged") && ok;
    return ok;
}

auto makePacketWithHeader() -> packet_t
{
    auto packet        = packet_t{};
    packet.packet_size = 0x01020304;
    packet.terminator  = 0x11223344;
    packet.command     = 0x55667788;
    std::memset(packet.identifer, 0xAA, sizeof(packet.identifer));
    return packet;
}

auto testPacketHeaderLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualInt(sizeof(packet_t), 28, "packet_t size") && ok;
    ok      = expectEqualInt(offsetof(packet_t, identifer), 12, "packet_t identifier offset") && ok;
    ok      = expectEqualInt(sizeof(packet_t{}.identifer), 16, "packet_t identifier length") && ok;
    return ok;
}

auto testTerminator() -> bool
{
    const auto terminator = loginPackets::getTerminator();

    bool ok = true;
    ok      = expectEqualInt(terminator, 0x46465849, "terminator value") && ok;
    ok      = expectEqualInt((terminator >> 0) & 0xFF, 'I', "terminator byte 0") && ok;
    ok      = expectEqualInt((terminator >> 8) & 0xFF, 'X', "terminator byte 1") && ok;
    ok      = expectEqualInt((terminator >> 16) & 0xFF, 'F', "terminator byte 2") && ok;
    ok      = expectEqualInt((terminator >> 24) & 0xFF, 'F', "terminator byte 3") && ok;
    return ok;
}

auto testCopyHashIntoPacket() -> bool
{
    auto packet = makePacketWithHeader();
    auto hash   = std::array<uint8_t, 16>{
        0x00, 0x11, 0x22, 0x33,
        0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xAA, 0xBB,
        0xCC, 0xDD, 0xEE, 0xFF,
    };

    loginPackets::copyHashIntoPacket(packet, hash.data());

    bool ok = true;
    ok      = expectTrue(std::memcmp(packet.identifer, hash.data(), hash.size()) == 0, "identifier contains copied hash") && ok;
    ok      = packetHeaderFieldsUnchanged(packet) && ok;
    return ok;
}

auto testClearIdentifier() -> bool
{
    auto packet = makePacketWithHeader();
    loginPackets::clearIdentifier(packet);

    bool ok = true;
    for (const auto value : packet.identifer)
    {
        ok = expectEqualInt(value, 0, "identifier byte cleared") && ok;
    }
    ok = packetHeaderFieldsUnchanged(packet) && ok;
    return ok;
}

} // namespace

auto runLoginPacketHelperSelfTests() -> bool
{
    return testPacketHeaderLayout() &&
           testTerminator() &&
           testCopyHashIntoPacket() &&
           testClearIdentifier();
}
