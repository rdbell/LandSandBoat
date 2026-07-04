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

#include "test_map_packet_basic_header.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <type_traits>
#include <vector>

#include "map/packets/basic.h"
#include "map/packets/s2c/base.h"

namespace
{

class HeaderOnlyPacket final : public GP_SERV_PACKET<PacketS2C::GP_SERV_COMMAND_NARAKU, HeaderOnlyPacket>
{
public:
    struct PacketData
    {
    };

    HeaderOnlyPacket() = default;
};

class PayloadPacket final : public GP_SERV_PACKET<PacketS2C::GP_SERV_COMMAND_REQSUBMAPNUM, PayloadPacket>
{
public:
    struct PacketData
    {
        uint8_t payload[8];
    };

    PayloadPacket() = default;
};

class RoundedPayloadPacket final : public GP_SERV_PACKET<PacketS2C::GP_SERV_COMMAND_WEATHER, RoundedPayloadPacket>
{
public:
    struct PacketData
    {
        uint8_t payload[5];
    };

    RoundedPayloadPacket() = default;
};

struct SizeCase
{
    std::string label;
    std::size_t size;
    std::size_t expectedSize;
    uint8_t expectedByte1;
};

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto makePatternedBuffer() -> std::array<uint8, PACKET_SIZE>
{
    auto buffer = std::array<uint8, PACKET_SIZE>{};
    for (std::size_t i = 0; i < buffer.size(); ++i)
    {
        buffer[i] = static_cast<uint8>((i * 37U) + 11U);
    }
    return buffer;
}

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "map packet basic header self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "map packet basic header self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectHeader(CBasicPacket& packet, const std::array<uint8, 4>& expected, const std::string& label) -> bool
{
    const auto* data = packetData(packet);
    if (std::memcmp(data, expected.data(), expected.size()) != 0)
    {
        std::cerr << "map packet basic header self-test failed: " << label << " got";
        for (std::size_t i = 0; i < expected.size(); ++i)
        {
            std::cerr << ' ' << static_cast<unsigned>(data[i]);
        }
        std::cerr << " expected";
        for (const auto value : expected)
        {
            std::cerr << ' ' << static_cast<unsigned>(value);
        }
        std::cerr << '\n';
        return false;
    }
    return true;
}

auto testZeroedFixedBuffer() -> bool
{
    auto packet = CBasicPacket();
    auto* data  = packetData(packet);

    bool ok = expectEqualInt(PACKET_SIZE, 0x1FF, "PACKET_SIZE") && expectEqualInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)");
    for (std::size_t i = 0; i < PACKET_SIZE; ++i)
    {
        if (data[i] != 0)
        {
            std::cerr << "map packet basic header self-test failed: zeroed byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            ok = false;
            break;
        }
    }
    return ok;
}

auto testTypeAndSizeShareHeaderBytes() -> bool
{
    auto packet = CBasicPacket();

    packet.setSize(7);
    packet.setType(0x10E);

    bool ok = expectHeader(packet, { 0x0E, 0x05, 0x00, 0x00 }, "size 7 and type 0x10e header");
    ok      = expectEqualInt(packet.getType(), 0x10E, "type 0x10e") && ok;
    ok      = expectEqualInt(packet.getSize(), 8, "rounded size 8") && ok;

    packet.setSize(13);
    ok = expectHeader(packet, { 0x0E, 0x09, 0x00, 0x00 }, "size 13 preserves high type bit") && ok;
    ok = expectEqualInt(packet.getType(), 0x10E, "type preserved by setSize") && ok;
    ok = expectEqualInt(packet.getSize(), 16, "rounded size 16") && ok;

    packet.setType(0x012);
    ok = expectHeader(packet, { 0x12, 0x08, 0x00, 0x00 }, "low type clears byte 1 low bit") && ok;
    ok = expectEqualInt(packet.getType(), 0x012, "low type") && ok;
    ok = expectEqualInt(packet.getSize(), 16, "size preserved by setType") && ok;

    packet.setType(0x3AB);
    ok = expectHeader(packet, { 0xAB, 0x09, 0x00, 0x00 }, "masked type") && ok;
    ok = expectEqualInt(packet.getType(), 0x1AB, "masked type value") && ok;

    return ok;
}

auto testSizeRoundingAndTypeBitPreservation() -> bool
{
    const auto cases = std::vector<SizeCase>{
        { "zero", 0, 0, 0x01 },
        { "one", 1, 4, 0x03 },
        { "two", 2, 4, 0x03 },
        { "three", 3, 4, 0x03 },
        { "four", 4, 4, 0x03 },
        { "five", 5, 8, 0x05 },
        { "eight", 8, 8, 0x05 },
    };

    bool ok = true;
    for (const auto& test : cases)
    {
        auto packet = CBasicPacket();
        packet.setType(0x100);
        packet.setSize(test.size);

        ok = expectEqualInt(packetData(packet)[1], test.expectedByte1, test.label + " byte 1") && ok;
        ok = expectEqualInt(packet.getType(), 0x100, test.label + " type preserved") && ok;
        ok = expectEqualInt(packet.getSize(), test.expectedSize, test.label + " size") && ok;
    }
    return ok;
}

auto testSequenceLittleEndian() -> bool
{
    auto packet = CBasicPacket();
    packet.setSequence(0xBEEF);

    bool ok = expectHeader(packet, { 0x00, 0x00, 0xEF, 0xBE }, "sequence header");
    ok      = expectEqualInt(packet.getSequence(), 0xBEEF, "sequence value") && ok;

    auto input = std::array<uint8, PACKET_SIZE>{};
    input[2]   = 0x34;
    input[3]   = 0x12;
    auto from  = CBasicPacket::createFromBuffer(input.data());
    ok         = expectEqualInt(from->getSequence(), 0x1234, "sequence from buffer") && ok;
    return ok;
}

auto testCopyIsolationAndFullBufferCopy() -> bool
{
    const auto input = makePatternedBuffer();
    auto packet      = CBasicPacket::createFromBuffer(input.data());
    auto copied      = packet->copy();

    bool ok = expectTrue(std::memcmp(packetData(*copied), input.data(), PACKET_SIZE) == 0, "copy matches full buffer");

    packet->setType(0x10E);
    packet->setSequence(0xBEEF);
    packetData(*packet)[PACKET_SIZE - 1] = 0xEE;

    ok = expectTrue(std::memcmp(packetData(*copied), input.data(), PACKET_SIZE) == 0, "copy isolated from original mutation") && ok;

    packetData(*copied)[4] = 0x55;
    ok = expectEqualInt(packetData(*packet)[4], input[4], "original isolated from copy mutation") && ok;
    return ok;
}

auto testCreateFromBufferCopiesFullBuffer() -> bool
{
    auto input          = makePatternedBuffer();
    const auto expected = input;
    auto packet         = CBasicPacket::createFromBuffer(input.data());

    input[0]               = 0xFF;
    input[PACKET_SIZE - 1] = 0xEE;

    return expectTrue(std::memcmp(packetData(*packet), expected.data(), PACKET_SIZE) == 0, "createFromBuffer full copy");
}

auto testServerPacketDefaults() -> bool
{
    auto headerOnly = HeaderOnlyPacket();
    bool ok         = expectHeader(headerOnly, { 0x06, 0x02, 0x00, 0x00 }, "header-only server packet");
    ok              = expectEqualInt(headerOnly.getType(), 0x006, "header-only type") && ok;
    ok              = expectEqualInt(headerOnly.getSize(), sizeof(GP_SERV_HEADER), "header-only size") && ok;

    auto payload = PayloadPacket();
    ok           = expectHeader(payload, { 0x0E, 0x07, 0x00, 0x00 }, "payload server packet") && ok;
    ok           = expectEqualInt(payload.getType(), 0x10E, "payload type") && ok;
    ok           = expectEqualInt(payload.getSize(), 12, "payload size") && ok;

    auto roundedPayload = RoundedPayloadPacket();
    ok                  = expectHeader(roundedPayload, { 0x57, 0x06, 0x00, 0x00 }, "rounded payload server packet") && ok;
    ok                  = expectEqualInt(roundedPayload.getType(), 0x057, "rounded payload type") && ok;
    ok                  = expectEqualInt(roundedPayload.getSize(), 12, "rounded payload size") && ok;
    return ok;
}

} // namespace

auto runMapPacketBasicHeaderSelfTests() -> bool
{
    return testZeroedFixedBuffer() &&
           testTypeAndSizeShareHeaderBytes() &&
           testSizeRoundingAndTypeBitPreservation() &&
           testSequenceLittleEndian() &&
           testCopyIsolationAndFullBufferCopy() &&
           testCreateFromBufferCopiesFullBuffer() &&
           testServerPacketDefaults();
}
