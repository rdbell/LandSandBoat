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

#include "test_login_key_packet.h"

#include "common/md52.h"
#include "login/login_packets.h"

#include <array>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string_view>

namespace
{

using KeyPacketBytes = std::array<std::uint8_t, sizeof(lpkt_key)>;

auto packetBytes(const lpkt_key& packet) -> KeyPacketBytes
{
    auto bytes = KeyPacketBytes{};
    std::memcpy(bytes.data(), &packet, bytes.size());
    return bytes;
}

auto expectEqual(std::uint64_t actual, std::uint64_t expected, std::string_view label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "login key packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectBytes(const KeyPacketBytes& actual, const KeyPacketBytes& expected, std::string_view label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "login key packet self-test failed: " << label << " bytes differ\n";
        return false;
    }
    return true;
}

auto expectTrue(bool actual, std::string_view label) -> bool
{
    if (!actual)
    {
        std::cerr << "login key packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto identifierMatchesZeroedPacket(const lpkt_key& packet) -> bool
{
    auto input = packet;
    loginPackets::clearIdentifier(input);

    auto expected = std::array<uint8, 16>{};
    md5(reinterpret_cast<uint8*>(&input), expected.data(), static_cast<int32>(sizeof(input)));
    return std::memcmp(packet.identifer, expected.data(), expected.size()) == 0;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqual(sizeof(packet_t), 28, "packet_t size") && ok;
    ok      = expectEqual(sizeof(lpkt_key), 40, "lpkt_key size") && ok;
    return ok;
}

auto testZeroMasksGoldenPacket() -> bool
{
    const auto packet   = loginPackets::generateKeyPacket(0, 0);
    const auto expected = KeyPacketBytes{
        0x28,
        0x00,
        0x00,
        0x00,
        0x49,
        0x58,
        0x46,
        0x46,
        0x05,
        0x00,
        0x00,
        0x00,
        0x19,
        0x54,
        0xEE,
        0x27,
        0x85,
        0x05,
        0x72,
        0x78,
        0xD3,
        0x95,
        0x95,
        0x8C,
        0x2B,
        0x24,
        0x41,
        0x3A,
        0x4F,
        0xE0,
        0x5D,
        0xAD,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
    };

    bool ok = true;
    ok      = expectBytes(packetBytes(packet), expected, "zero-mask golden packet") && ok;
    ok      = expectTrue(identifierMatchesZeroedPacket(packet), "zero-mask identifier hash") && ok;
    return ok;
}

auto testRepresentativeMasksGoldenPacket() -> bool
{
    const auto packet   = loginPackets::generateKeyPacket(0x0A55, 0x81C3);
    const auto expected = KeyPacketBytes{
        0x28,
        0x00,
        0x00,
        0x00,
        0x49,
        0x58,
        0x46,
        0x46,
        0x05,
        0x00,
        0x00,
        0x00,
        0x2C,
        0x7A,
        0x57,
        0x5A,
        0x84,
        0x1A,
        0xCF,
        0xA4,
        0x3A,
        0x80,
        0x82,
        0xEC,
        0x30,
        0x66,
        0xD4,
        0x26,
        0x4F,
        0xE0,
        0x5D,
        0xAD,
        0x55,
        0x0A,
        0x00,
        0x00,
        0xC3,
        0x81,
        0x00,
        0x00,
    };

    bool ok = true;
    ok      = expectBytes(packetBytes(packet), expected, "representative-mask golden packet") && ok;
    ok      = expectTrue(identifierMatchesZeroedPacket(packet), "representative-mask identifier hash") && ok;
    return ok;
}

auto testMaskWidening() -> bool
{
    const auto packet = loginPackets::generateKeyPacket(
        static_cast<std::uint16_t>(0x1234FFFFU),
        static_cast<std::uint16_t>(0xABCDFFFFU));

    bool ok = true;
    ok      = expectEqual(packet.packet_size, 0x28, "packet size") && ok;
    ok      = expectEqual(packet.terminator, 0x46465849, "terminator") && ok;
    ok      = expectEqual(packet.command, 0x05, "command") && ok;
    ok      = expectEqual(packet.key, 0xAD5DE04F, "magic key") && ok;
    ok      = expectEqual(packet.excode_server, 0x0000FFFF, "widened expansion mask") && ok;
    ok      = expectEqual(packet.excode_server2, 0x0000FFFF, "widened feature mask") && ok;
    ok      = expectTrue(identifierMatchesZeroedPacket(packet), "max-mask identifier hash") && ok;
    return ok;
}

} // namespace

auto runLoginKeyPacketSelfTests() -> bool
{
    return testLayout() &&
           testZeroMasksGoldenPacket() &&
           testRepresentativeMasksGoldenPacket() &&
           testMaskWidening();
}
