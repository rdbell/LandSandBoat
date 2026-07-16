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

#include "test_map_network_packet_building.h"

#include "common/blowfish.h"
#include "common/md52.h"
#include "common/scheduler.h"
#include "common/zlib.h"
#include "map/map_config.h"
#include "map/map_networking.h"
#include "map/map_session.h"
#include "map/map_statistics.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string_view>

namespace
{

constexpr auto HeaderSize = std::size_t{ FFXI_HEADER_SIZE };

auto expectEqual(std::uint64_t actual, std::uint64_t expected, std::string_view label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "map network packet-building self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, std::string_view label) -> bool
{
    if (!actual)
    {
        std::cerr << "map network packet-building self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

template <std::size_t Size>
auto expectBytes(const std::uint8_t* actual, const std::array<std::uint8_t, Size>& expected, std::string_view label) -> bool
{
    if (std::memcmp(actual, expected.data(), expected.size()) != 0)
    {
        std::cerr << "map network packet-building self-test failed: " << label << " bytes differ\n";
        return false;
    }
    return true;
}

auto testConfig() -> MapConfig
{
    auto config         = MapConfig{};
    config.isTestServer = true;
    return config;
}

struct NetworkingFixture
{
    NetworkingFixture()
    : scheduler(1)
    , statistics()
    , networking(scheduler, statistics, testConfig())
    {
    }

    Scheduler     scheduler;
    MapStatistics statistics;
    MapNetworking networking;
};

auto initTestCipher(blowfish_t& cipher) -> void
{
    auto key = std::array<int8, 16>{};
    for (std::size_t index = 0; index < key.size(); ++index)
    {
        key[index] = static_cast<int8>(index);
    }
    blowfish_init(key.data(), static_cast<int16>(key.size()), cipher.P, cipher.S[0]);
}

auto decryptCompleteBlocks(NetworkBuffer& packet, std::size_t dataSize, blowfish_t& cipher) -> void
{
    const auto encryptedBytes = dataSize / 8 * 8;
    blowfish_decipher_blocks(
        reinterpret_cast<uint32*>(packet.data() + HeaderSize),
        encryptedBytes / 8,
        cipher.P,
        cipher.S[0]);
}

auto testPrepareHeader() -> bool
{
    auto packet = NetworkBuffer{};
    packet.fill(0xA5);

    MapNetworking::preparePacketHeader(packet.data(), 0x1234, 0xBEEF, 0x66778899);

    const auto expectedPrefix = std::array<std::uint8_t, 12>{
        0x34,
        0x12,
        0xEF,
        0xBE,
        0xA5,
        0xA5,
        0xA5,
        0xA5,
        0x99,
        0x88,
        0x77,
        0x66,
    };

    bool ok        = true;
    ok             = expectBytes(packet.data(), expectedPrefix, "prepared header prefix") && ok;
    auto preserved = true;
    for (std::size_t index = 12; index < packet.size(); ++index)
    {
        if (packet[index] != 0xA5)
        {
            preserved = false;
            break;
        }
    }
    ok = expectTrue(preserved, "prepared header preserves unrelated bytes") && ok;
    return ok;
}

auto testCompressedAndFinalizedGoldenPacket(NetworkingFixture& fixture) -> bool
{
    constexpr auto payload = std::array<std::uint8_t, 7>{ 'O', 'm', 'e', 'g', 'a', 'X', 'I' };

    auto packet = NetworkBuffer{};
    MapNetworking::preparePacketHeader(packet.data(), 0x1234, 0xBEEF, 0x66778899);
    std::memcpy(packet.data() + HeaderSize, payload.data(), payload.size());

    const auto maybeCompressedSize = fixture.networking.compressPacket(packet.data(), HeaderSize + payload.size());
    if (!maybeCompressedSize)
    {
        std::cerr << "map network packet-building self-test failed: compression returned no size\n";
        return false;
    }

    auto session = MapSession{};
    initTestCipher(session.blowfish);

    auto packetSize = HeaderSize + payload.size();
    fixture.networking.finalizePacket(
        packet.data(),
        &packetSize,
        *maybeCompressedSize,
        &session,
        MapNetworking::UsePreviousKey::No);

    const auto expected = std::array<std::uint8_t, 59>{
        0x34,
        0x12,
        0xEF,
        0xBE,
        0x00,
        0x00,
        0x00,
        0x00,
        0x99,
        0x88,
        0x77,
        0x66,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x0F,
        0x97,
        0xA8,
        0x4B,
        0x5F,
        0xB3,
        0x9D,
        0x9D,
        0x41,
        0xD6,
        0xD5,
        0xAA,
        0xA5,
        0xB7,
        0xF7,
        0x4D,
        0x84,
        0xF0,
        0x1B,
        0xE9,
        0x73,
        0x76,
        0x3D,
        0xF9,
        0x3C,
        0xE6,
        0x79,
        0xB3,
        0x6E,
        0x81,
        0x50,
    };

    bool ok = true;
    ok      = expectEqual(*maybeCompressedSize, 15, "compressed block size") && ok;
    ok      = expectEqual(packetSize, expected.size(), "final packet size") && ok;
    ok      = expectBytes(packet.data(), expected, "finalized golden packet") && ok;

    const auto encryptedPacket = packet;
    const auto dataSize        = packetSize - HeaderSize;
    decryptCompleteBlocks(packet, dataSize, session.blowfish);

    const auto expectedPlainData = std::array<std::uint8_t, 31>{
        0x01,
        0xC6,
        0x73,
        0x18,
        0x32,
        0x44,
        0x20,
        0x40,
        0x7F,
        0x0E,
        0x07,
        0x53,
        0x00,
        0x00,
        0x00,
        0xCD,
        0xE5,
        0x41,
        0xE8,
        0xC7,
        0x2E,
        0x2F,
        0x0D,
        0x8C,
        0x3C,
        0xE6,
        0x79,
        0xB3,
        0x6E,
        0x81,
        0x50,
    };
    ok = expectBytes(packet.data() + HeaderSize, expectedPlainData, "decrypted compressed data and digest") && ok;

    const auto compressedSize = *maybeCompressedSize;
    ok                        = expectEqual(ref<uint32>(packet.data() + HeaderSize, compressedSize - sizeof(uint32)), 83, "compressed bit-count trailer") && ok;

    auto expectedDigest = std::array<uint8, 16>{};
    md5(packet.data() + HeaderSize, expectedDigest.data(), static_cast<int32>(compressedSize));
    ok = expectBytes(packet.data() + HeaderSize + compressedSize, expectedDigest, "MD5 covers compressed block and bit count") && ok;

    const auto encryptedBytes = dataSize / 8 * 8;
    ok                        = expectBytes(
             encryptedPacket.data() + HeaderSize + encryptedBytes,
             std::array<std::uint8_t, 7>{ 0x3C, 0xE6, 0x79, 0xB3, 0x6E, 0x81, 0x50 },
             "non-block tail remains clear") &&
         ok;
    return ok;
}

auto testEmptyPayloadCompression(NetworkingFixture& fixture) -> bool
{
    auto       packet              = NetworkBuffer{};
    const auto maybeCompressedSize = fixture.networking.compressPacket(packet.data(), HeaderSize);
    if (!maybeCompressedSize)
    {
        std::cerr << "map network packet-building self-test failed: empty compression returned no size\n";
        return false;
    }

    auto session = MapSession{};
    initTestCipher(session.blowfish);

    auto packetSize = HeaderSize;
    fixture.networking.finalizePacket(
        packet.data(),
        &packetSize,
        *maybeCompressedSize,
        &session,
        MapNetworking::UsePreviousKey::No);

    const auto dataSize = packetSize - HeaderSize;
    decryptCompleteBlocks(packet, dataSize, session.blowfish);

    bool ok = true;
    ok      = expectEqual(*maybeCompressedSize, 5, "empty compressed block size") && ok;
    ok      = expectEqual(packetSize, HeaderSize + 5 + 16, "empty finalized packet size") && ok;
    ok      = expectBytes(packet.data() + HeaderSize, std::array<std::uint8_t, 5>{ 0x01, 0x08, 0x00, 0x00, 0x00 }, "empty compressed block") && ok;
    return ok;
}

auto testIncomingDecoder(NetworkingFixture& fixture) -> bool
{
    auto packet = NetworkBuffer{};
    MapNetworking::preparePacketHeader(packet.data(), 1, 2, 3);
    constexpr auto payload = std::array<std::uint8_t, 8>{ 'i', 'n', 'b', 'o', 'u', 'n', 'd', '!' };
    std::memcpy(packet.data() + HeaderSize, payload.data(), payload.size());
    const auto compressedSize = fixture.networking.compressPacket(packet.data(), HeaderSize + payload.size());
    if (!compressedSize)
    {
        return false;
    }
    auto session = MapSession{};
    initTestCipher(session.blowfish);
    auto wireSize = HeaderSize + payload.size();
    fixture.networking.finalizePacket(packet.data(), &wireSize, *compressedSize, &session, MapNetworking::UsePreviousKey::No);

    auto decoded = NetworkBuffer{};
    size_t decodedSize{};
    bool ok = MapNetworking::decodeIncomingPacket({ packet.data(), wireSize }, decoded, decodedSize, &session.blowfish);
    ok = expectEqual(decodedSize, HeaderSize + payload.size(), "incoming decoded size") && ok;
    ok = expectBytes(decoded.data() + HeaderSize, payload, "incoming decoded payload") && ok;

    auto malformed = packet;
    malformed[wireSize - 1] ^= 1;
    ok = !MapNetworking::decodeIncomingPacket({ malformed.data(), wireSize }, decoded, decodedSize, &session.blowfish) && ok;
    ok = !MapNetworking::decodeIncomingPacket({ packet.data(), HeaderSize + 20 }, decoded, decodedSize, &session.blowfish) && ok;
    return ok;
}

} // namespace

auto runMapNetworkPacketBuildingSelfTests() -> bool
{
    if (zlib_init() != 0)
    {
        std::cerr << "map network packet-building self-test failed: zlib_init failed\n";
        return false;
    }

    auto fixture = NetworkingFixture{};
    return testPrepareHeader() &&
           testCompressedAndFinalizedGoldenPacket(fixture) &&
           testEmptyPayloadCompression(fixture) &&
           testIncomingDecoder(fixture);
}
