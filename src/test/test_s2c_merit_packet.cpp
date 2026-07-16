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

#include "test_s2c_merit_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/packets/s2c/0x08c_merit.h"

namespace
{

using MeritPacket = GP_SERV_COMMAND_MERIT;

constexpr auto meritCountOffset            = sizeof(GP_SERV_HEADER) + offsetof(MeritPacket::PacketData, merit_count);
constexpr auto meritPadding00Offset        = sizeof(GP_SERV_HEADER) + offsetof(MeritPacket::PacketData, padding00);
constexpr auto meritEntriesOffset          = sizeof(GP_SERV_HEADER) + offsetof(MeritPacket::PacketData, merits);
constexpr auto meritPadding01Offset        = sizeof(GP_SERV_HEADER) + offsetof(MeritPacket::PacketData, padding01);
constexpr auto meritPacketDataSize         = sizeof(MeritPacket::PacketData);
constexpr auto meritPacketSize             = sizeof(GP_SERV_HEADER) + meritPacketDataSize;
constexpr auto singleMeritUpdatePacketSize = 0x08 + (2 * sizeof(merit_t));

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c MERIT packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

template <typename T, std::size_t Size>
auto expectStructBytes(const T& value, const std::array<uint8, Size>& expected, const std::string& label) -> bool
{
    const auto* data = reinterpret_cast<const uint8*>(&value);
    if (std::memcmp(data, expected.data(), expected.size()) != 0)
    {
        std::cerr << "s2c MERIT packet self-test failed: " << label << " got";
        for (std::size_t i = 0; i < expected.size(); ++i)
        {
            std::cerr << ' ' << static_cast<unsigned>(data[i]);
        }
        std::cerr << " expected";
        for (const auto valueByte : expected)
        {
            std::cerr << ' ' << static_cast<unsigned>(valueByte);
        }
        std::cerr << '\n';
        return false;
    }
    return true;
}

template <std::size_t Size>
void putLE16(std::array<uint8, Size>& buffer, std::size_t offset, std::uint16_t value)
{
    buffer[offset]     = static_cast<uint8>(value & 0xFF);
    buffer[offset + 1] = static_cast<uint8>((value >> 8) & 0xFF);
}

template <std::size_t Size>
void putLE32(std::array<uint8, Size>& buffer, std::size_t offset, std::uint32_t value)
{
    buffer[offset]     = static_cast<uint8>(value & 0xFF);
    buffer[offset + 1] = static_cast<uint8>((value >> 8) & 0xFF);
    buffer[offset + 2] = static_cast<uint8>((value >> 16) & 0xFF);
    buffer[offset + 3] = static_cast<uint8>((value >> 24) & 0xFF);
}

template <std::size_t Size>
void putMeritEntry(std::array<uint8, Size>& buffer, std::size_t offset, std::uint16_t index, uint8 next, uint8 count)
{
    putLE16(buffer, offset, index);
    buffer[offset + 2] = next;
    buffer[offset + 3] = count;
}

constexpr auto packetDataMeritEntryOffset(std::size_t index) -> std::size_t
{
    return offsetof(MeritPacket::PacketData, merits) + index * sizeof(merit_t);
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(static_cast<std::uint16_t>(PacketS2C::GP_SERV_COMMAND_MERIT), 0x08C, "MERIT packet id") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(MAX_MERITS_IN_PACKET, 61, "MAX_MERITS_IN_PACKET") && ok;
    ok      = expectEqualUInt(sizeof(merit_t), 4, "sizeof(merit_t)") && ok;
    ok      = expectEqualUInt(offsetof(merit_t, index), 0, "merit_t index offset") && ok;
    ok      = expectEqualUInt(offsetof(merit_t, next), 2, "merit_t next offset") && ok;
    ok      = expectEqualUInt(offsetof(merit_t, count), 3, "merit_t count offset") && ok;
    ok      = expectEqualUInt(meritPacketDataSize, 252, "PacketData size") && ok;
    ok      = expectEqualUInt(meritPacketSize, 256, "packet size") && ok;
    ok      = expectEqualUInt(singleMeritUpdatePacketSize, 16, "single update packet size") && ok;
    ok      = expectEqualUInt(meritCountOffset, 4, "merit_count offset") && ok;
    ok      = expectEqualUInt(meritPadding00Offset, 6, "padding00 offset") && ok;
    ok      = expectEqualUInt(meritEntriesOffset, 8, "merits offset") && ok;
    ok      = expectEqualUInt(meritPadding01Offset, 252, "padding01 offset") && ok;
    return ok;
}

auto testMeritEntryBytes() -> bool
{
    auto entry = merit_t{
        .index = 0x1234,
        .next  = 0x56,
        .count = 0x78,
    };

    return expectStructBytes(entry, std::array<uint8, sizeof(merit_t)>{ 0x34, 0x12, 0x56, 0x78 }, "merit_t bytes");
}

auto testPacketDataBytes() -> bool
{
    auto data          = MeritPacket::PacketData{};
    data.merit_count  = 3;
    data.padding00    = 0x9988;
    data.merits[0]    = { .index = 0x0040, .next = 0x01, .count = 0x02 };
    data.merits[1]    = { .index = 0x0184, .next = 0x03, .count = 0x04 };
    data.merits[60]   = { .index = 0x0D86, .next = 0x05, .count = 0x06 };
    data.padding01    = 0xAABBCCDD;

    auto expected = std::array<uint8, meritPacketDataSize>{};
    putLE16(expected, offsetof(MeritPacket::PacketData, merit_count), 3);
    putLE16(expected, offsetof(MeritPacket::PacketData, padding00), 0x9988);
    putMeritEntry(expected, packetDataMeritEntryOffset(0), 0x0040, 0x01, 0x02);
    putMeritEntry(expected, packetDataMeritEntryOffset(1), 0x0184, 0x03, 0x04);
    putMeritEntry(expected, packetDataMeritEntryOffset(60), 0x0D86, 0x05, 0x06);
    putLE32(expected, offsetof(MeritPacket::PacketData, padding01), 0xAABBCCDD);

    return expectStructBytes(data, expected, "MERIT PacketData bytes");
}

auto testRuntimePlans() -> bool
{
    auto merits      = meritpackethelpers::Page{};
    merits[0]        = { .index = 0x0040, .next = 0x01, .count = 0x02 };
    merits[60]       = { .index = 0x0D86, .next = 0x05, .count = 0x06 };
    const auto inMog = meritpackethelpers::PlanFullPage(merits, true);
    const auto away  = meritpackethelpers::PlanFullPage(merits, false);
    const auto one   = meritpackethelpers::PlanSingleUpdate(merits[60]);

    bool ok = true;
    ok      = expectEqualUInt(inMog.merit_count, MAX_MERITS_IN_PACKET, "full plan merit count") && ok;
    ok      = expectEqualUInt(inMog.merits[0].index, 0x0040, "full plan first index") && ok;
    ok      = expectEqualUInt(inMog.merits[0].next, 0x01, "Mog House preserves next") && ok;
    ok      = expectEqualUInt(inMog.merits[60].count, 0x06, "full plan last count") && ok;
    ok      = expectEqualUInt(away.merits[0].next, 0, "non-Mog-House resets first next") && ok;
    ok      = expectEqualUInt(away.merits[60].next, 0, "non-Mog-House resets last next") && ok;
    ok      = expectEqualUInt(one.merit_count, 1, "single plan merit count") && ok;
    ok      = expectEqualUInt(one.merits[0].index, 0x0D86, "single plan index") && ok;
    ok      = expectEqualUInt(one.merits[0].next, 0x05, "single plan next") && ok;
    ok      = expectEqualUInt(one.merits[1].index, 0, "single plan trailing entry") && ok;
    return ok;
}

} // namespace

auto runS2CMeritPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testMeritEntryBytes() && ok;
    ok      = testPacketDataBytes() && ok;
    ok      = testRuntimePlans() && ok;
    return ok;
}
