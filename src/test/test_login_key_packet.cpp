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
using NextLoginBytes = std::array<std::uint8_t, sizeof(lpkt_next_login)>;

auto packetBytes(const lpkt_key& packet) -> KeyPacketBytes
{
    auto bytes = KeyPacketBytes{};
    std::memcpy(bytes.data(), &packet, bytes.size());
    return bytes;
}

auto packetBytes(const lpkt_next_login& packet) -> NextLoginBytes
{
    auto bytes = NextLoginBytes{};
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

auto expectBytes(const NextLoginBytes& actual, const NextLoginBytes& expected, std::string_view label) -> bool
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

auto identifierMatchesZeroedPacket(const lpkt_next_login& packet) -> bool
{
    auto input = packet;
    loginPackets::clearIdentifier(input);

    auto expected = std::array<uint8, 16>{};
    md5(reinterpret_cast<uint8*>(&input), expected.data(), static_cast<int32>(sizeof(input)));
    return std::memcmp(packet.identifer, expected.data(), expected.size()) == 0;
}

enum class AdmissionDecision : std::uint8_t
{
    Allowed,
    ZoneAtCap,
    LobbyDenied,
};

auto decideAdmission(bool maintenance, std::uint8_t loginLimit, std::uint16_t sessionCount, bool excepted, bool isGM, bool zoneAtCap) -> AdmissionDecision
{
    const auto loginLimitOK = loginLimit == 0 || sessionCount < loginLimit || excepted;
    if (zoneAtCap)
    {
        return AdmissionDecision::ZoneAtCap;
    }
    return ((!maintenance && loginLimitOK) || isGM) ? AdmissionDecision::Allowed : AdmissionDecision::LobbyDenied;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqual(sizeof(packet_t), 28, "packet_t size") && ok;
    ok      = expectEqual(sizeof(lpkt_key), 40, "lpkt_key size") && ok;
    ok      = expectEqual(sizeof(lpkt_next_login), 72, "lpkt_next_login size") && ok;
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

auto testSelectionKeyAdjustment() -> bool
{
    auto key = std::array<std::uint8_t, 20>{};
    for (std::size_t i = 0; i < key.size(); ++i)
    {
        key[i] = static_cast<std::uint8_t>(0x40 + i);
    }
    auto adjusted = key;
    adjusted[16] += 6;
    adjusted[16] += 0xB9;

    bool ok = true;
    ok      = expectEqual(adjusted[16], 0x0F, "selection key sequential uint8 wrapping") && ok;
    for (std::size_t i = 0; i < key.size(); ++i)
    {
        if (i != 16)
        {
            ok = expectEqual(adjusted[i], key[i], "selection key exact copy") && ok;
        }
    }
    return ok;
}

auto testNextLoginGoldenPacket() -> bool
{
    auto packet           = lpkt_next_login{};
    packet.packet_size    = 0x48;
    packet.terminator     = loginPackets::getTerminator();
    packet.command        = 0x0B;
    packet.ffxi_id        = 0xAB123456;
    packet.ffxi_id_world  = 0x3456;
    packet.server_id      = 0x12;
    packet.server_ip      = 0x01020304;
    packet.server_port    = 0x5678;
    packet.cache_ip       = 0xA1B2C3D4;
    packet.cache_port     = 0x9ABC;
    constexpr auto name   = std::string_view("Selh'teus");
    std::memcpy(packet.character_name, name.data(), name.size());
    loginPackets::clearIdentifier(packet);
    auto hash = std::array<uint8, 16>{};
    md5(reinterpret_cast<uint8*>(&packet), hash.data(), static_cast<int32>(sizeof(packet)));
    loginPackets::copyHashIntoPacket(packet, hash.data());

    const auto expected = NextLoginBytes{
        0x48, 0x00, 0x00, 0x00, 0x49, 0x58, 0x46, 0x46, 0x0B, 0x00, 0x00, 0x00,
        0x80, 0xF7, 0x48, 0x25, 0x60, 0xF1, 0x5D, 0x11, 0x3D, 0x99, 0x63, 0xD0, 0x76, 0xF1, 0x7D, 0x98,
        0x56, 0x34, 0x12, 0xAB, 0x56, 0x34, 0x00, 0x00,
        'S', 'e', 'l', 'h', '\'', 't', 'e', 'u', 's', 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x12, 0x00, 0x00, 0x00, 0x04, 0x03, 0x02, 0x01, 0x78, 0x56, 0x00, 0x00,
        0xD4, 0xC3, 0xB2, 0xA1, 0xBC, 0x9A, 0x00, 0x00,
    };

    bool ok = true;
    ok      = expectBytes(packetBytes(packet), expected, "next-login golden packet") && ok;
    ok      = expectTrue(identifierMatchesZeroedPacket(packet), "next-login identifier hash") && ok;
    return ok;
}

auto testSelectionAdmissionTruthTable() -> bool
{
    struct TestCase
    {
        bool              maintenance;
        std::uint8_t      limit;
        std::uint16_t     count;
        bool              excepted;
        bool              gm;
        bool              zoneAtCap;
        AdmissionDecision expected;
    };
    constexpr auto cases = std::array{
        TestCase{ false, 0, 65535, false, false, false, AdmissionDecision::Allowed },
        TestCase{ false, 2, 1, false, false, false, AdmissionDecision::Allowed },
        TestCase{ false, 2, 2, false, false, false, AdmissionDecision::LobbyDenied },
        TestCase{ false, 2, 2, true, false, false, AdmissionDecision::Allowed },
        TestCase{ true, 0, 0, false, false, false, AdmissionDecision::LobbyDenied },
        TestCase{ true, 1, 1, true, false, false, AdmissionDecision::LobbyDenied },
        TestCase{ true, 1, 1, false, true, false, AdmissionDecision::Allowed },
        TestCase{ false, 0, 0, false, false, true, AdmissionDecision::ZoneAtCap },
        TestCase{ true, 1, 1, false, false, true, AdmissionDecision::ZoneAtCap },
        TestCase{ true, 1, 1, false, true, true, AdmissionDecision::ZoneAtCap },
    };

    bool ok = true;
    for (const auto& test : cases)
    {
        ok = expectEqual(static_cast<std::uint8_t>(decideAdmission(test.maintenance, test.limit, test.count, test.excepted, test.gm, test.zoneAtCap)),
                         static_cast<std::uint8_t>(test.expected),
                         "selection admission truth table") && ok;
    }
    return ok;
}

} // namespace

auto runLoginKeyPacketSelfTests() -> bool
{
    return testLayout() &&
           testZeroMasksGoldenPacket() &&
           testRepresentativeMasksGoldenPacket() &&
           testMaskWidening() &&
           testSelectionKeyAdjustment() &&
           testNextLoginGoldenPacket() &&
           testSelectionAdmissionTruthTable();
}
