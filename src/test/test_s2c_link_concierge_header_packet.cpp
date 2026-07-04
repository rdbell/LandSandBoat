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

#include "test_s2c_link_concierge_header_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <optional>
#include <string>

#include "map/packets/s2c/0x048_link_concierge_header.h"

namespace
{

using Header = GP_SERV_COMMAND_LINK_CONCIERGE::HEADER;

constexpr auto linkConciergeSentinelOffset    = sizeof(GP_SERV_HEADER) + offsetof(Header::PacketData, Sentinel);
constexpr auto linkConciergeSlotIndexOffset   = sizeof(GP_SERV_HEADER) + offsetof(Header::PacketData, SlotIndex);
constexpr auto linkConciergeListingFlagOffset = sizeof(GP_SERV_HEADER) + offsetof(Header::PacketData, ListingFlag);
constexpr auto linkConciergePadding00Offset   = sizeof(GP_SERV_HEADER) + offsetof(Header::PacketData, padding00);
constexpr auto linkConciergePostedDaysOffset  = sizeof(GP_SERV_HEADER) + offsetof(Header::PacketData, PostedDays);
constexpr auto linkConciergePadding01Offset   = sizeof(GP_SERV_HEADER) + offsetof(Header::PacketData, padding01);
constexpr auto linkConciergeRegisteredOffset  = sizeof(GP_SERV_HEADER) + offsetof(Header::PacketData, Registered);
constexpr auto linkConciergePadding02Offset   = sizeof(GP_SERV_HEADER) + offsetof(Header::PacketData, padding02);
constexpr auto linkConciergePacketSize        = sizeof(GP_SERV_HEADER) + sizeof(Header::PacketData);

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c LINK_CONCIERGE HEADER packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

template <std::size_t Size>
auto expectBytes(CBasicPacket& packet, std::size_t offset, const std::array<uint8, Size>& expected, const std::string& label) -> bool
{
    const auto* data = packetData(packet) + offset;
    if (std::memcmp(data, expected.data(), expected.size()) != 0)
    {
        std::cerr << "s2c LINK_CONCIERGE HEADER packet self-test failed: " << label << " got";
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

auto expectZeroRange(CBasicPacket& packet, std::size_t offset, std::size_t end, const std::string& label) -> bool
{
    const auto* data = packetData(packet);
    for (std::size_t i = offset; i < end; ++i)
    {
        if (data[i] != 0)
        {
            std::cerr << "s2c LINK_CONCIERGE HEADER packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(sizeof(Header::PacketData), 124, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(linkConciergePacketSize, 128, "packet size") && ok;
    ok      = expectEqualUInt(linkConciergeSentinelOffset, 4, "Sentinel offset") && ok;
    ok      = expectEqualUInt(linkConciergeSlotIndexOffset, 8, "SlotIndex offset") && ok;
    ok      = expectEqualUInt(linkConciergeListingFlagOffset, 10, "ListingFlag offset") && ok;
    ok      = expectEqualUInt(linkConciergePadding00Offset, 12, "padding00 offset") && ok;
    ok      = expectEqualUInt(linkConciergePostedDaysOffset, 28, "PostedDays offset") && ok;
    ok      = expectEqualUInt(linkConciergePadding01Offset, 31, "padding01 offset") && ok;
    ok      = expectEqualUInt(linkConciergeRegisteredOffset, 48, "Registered offset") && ok;
    ok      = expectEqualUInt(linkConciergePadding02Offset, 49, "padding02 offset") && ok;
    return ok;
}

auto testNoSlotNoPostedDaysConstructor() -> bool
{
    auto packet = Header(std::nullopt, 0);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x048, "empty type") && ok;
    ok      = expectEqualUInt(packet.getSize(), linkConciergePacketSize, "empty size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x48, 0x40, 0xEF, 0xBE }, "empty header") && ok;
    ok      = expectBytes(packet, linkConciergeSentinelOffset, std::array<uint8, 4>{ 0xFE, 0xFE, 0xFE, 0xFE }, "empty sentinel") && ok;
    ok      = expectZeroRange(packet, linkConciergeSlotIndexOffset, linkConciergePacketSize, "empty payload after sentinel") && ok;
    ok      = expectZeroRange(packet, linkConciergePacketSize, PACKET_SIZE, "empty tail") && ok;
    return ok;
}

auto testRegisteredSlotConstructor() -> bool
{
    auto packet = Header(std::optional<uint8>{ 7 }, 22);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x48, 0x40, 0xEF, 0xBE }, "registered header") && ok;
    ok      = expectBytes(packet, linkConciergeSentinelOffset, std::array<uint8, 4>{ 0xFE, 0xFE, 0xFE, 0xFE }, "registered sentinel") && ok;
    ok      = expectBytes(packet, linkConciergeSlotIndexOffset, std::array<uint8, 2>{ 0x07, 0x00 }, "SlotIndex") && ok;
    ok      = expectBytes(packet, linkConciergeListingFlagOffset, std::array<uint8, 2>{ 0xFF, 0xFF }, "ListingFlag") && ok;
    ok      = expectBytes(packet, linkConciergePostedDaysOffset, std::array<uint8, 3>{ '2', '2', 'd' }, "PostedDays") && ok;
    ok      = expectBytes(packet, linkConciergeRegisteredOffset, std::array<uint8, 1>{ 0x01 }, "Registered") && ok;
    ok      = expectZeroRange(packet, linkConciergePadding00Offset, linkConciergePostedDaysOffset, "padding00") && ok;
    ok      = expectZeroRange(packet, linkConciergePadding01Offset, linkConciergeRegisteredOffset, "padding01") && ok;
    ok      = expectZeroRange(packet, linkConciergePadding02Offset, linkConciergePacketSize, "padding02") && ok;
    return ok;
}

auto testPostedDaysTruncatesToThreeBytes() -> bool
{
    auto packet = Header(std::nullopt, 1234);

    bool ok = true;
    ok      = expectBytes(packet, linkConciergePostedDaysOffset, std::array<uint8, 3>{ '1', '2', '3' }, "truncated PostedDays") && ok;
    ok      = expectZeroRange(packet, linkConciergeSlotIndexOffset, linkConciergePostedDaysOffset, "truncated prefix") && ok;
    ok      = expectZeroRange(packet, linkConciergePadding01Offset, linkConciergePacketSize, "truncated suffix") && ok;
    return ok;
}

} // namespace

auto runS2CLinkConciergeHeaderPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testNoSlotNoPostedDaysConstructor() && ok;
    ok      = testRegisteredSlotConstructor() && ok;
    ok      = testPostedDaysTruncatesToThreeBytes() && ok;
    return ok;
}
