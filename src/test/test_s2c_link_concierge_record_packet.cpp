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

#include "test_s2c_link_concierge_record_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/packets/s2c/0x048_link_concierge_record.h"

namespace
{

using Record    = GP_SERV_COMMAND_LINK_CONCIERGE::RECORD;
using SlotInput = GP_SERV_COMMAND_LINK_CONCIERGE::SlotInput;

constexpr auto linkConciergeRecordIndicesOffset   = sizeof(GP_SERV_HEADER) + offsetof(Record::PacketData, Indices);
constexpr auto linkConciergeRecordUnknown00Offset = sizeof(GP_SERV_HEADER) + offsetof(Record::PacketData, unknown00);
constexpr auto linkConciergeRecordBodiesOffset    = sizeof(GP_SERV_HEADER) + offsetof(Record::PacketData, Bodies);
constexpr auto linkConciergeRecordAttrsOffset     = sizeof(GP_SERV_HEADER) + offsetof(Record::PacketData, Attrs);
constexpr auto linkConciergeRecordPacketDataSize  = sizeof(Record::PacketData);
constexpr auto linkConciergeRecordPacketSize      = sizeof(GP_SERV_HEADER) + linkConciergeRecordPacketDataSize;

constexpr auto linkshellBodyGroupIdOffset  = offsetof(Exdata::Linkshell, GroupId);
constexpr auto linkshellBodyGroupKeyOffset = offsetof(Exdata::Linkshell, GroupKey);
constexpr auto linkshellBodyColorOffset    = offsetof(Exdata::Linkshell, Color);
constexpr auto linkshellBodyFlagOffset     = offsetof(Exdata::Linkshell, Flag);
constexpr auto linkshellBodyNameOffset     = offsetof(Exdata::Linkshell, Name);

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c LINK_CONCIERGE RECORD packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c LINK_CONCIERGE RECORD packet self-test failed: " << label << " got";
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
            std::cerr << "s2c LINK_CONCIERGE RECORD packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto bodyOffset(std::size_t index, std::size_t fieldOffset = 0) -> std::size_t
{
    return linkConciergeRecordBodiesOffset + (index * sizeof(Exdata::Linkshell)) + fieldOffset;
}

auto attrOffset(std::size_t index) -> std::size_t
{
    return linkConciergeRecordAttrsOffset + (index * sizeof(GP_SERV_COMMAND_LINK_CONCIERGE::SlotAttrs));
}

auto attrWord(std::uint8_t lang, std::uint8_t membersGoal, std::uint8_t activeTier, std::uint16_t characteristics) -> std::uint32_t
{
    auto word = std::uint32_t{ 1 };
    if (lang == 1)
    {
        word |= 1u << 1;
    }
    if (lang == 2)
    {
        word |= 1u << 2;
    }
    if (lang == 3)
    {
        word |= 1u << 5;
    }
    word |= (std::uint32_t{ membersGoal } & 0x0Fu) << 6;
    word |= (std::uint32_t{ activeTier } & 0x03u) << 14;
    word |= std::uint32_t{ characteristics } << 16;
    return word;
}

auto attrBytes(std::uint32_t word) -> std::array<uint8, 4>
{
    return {
        static_cast<uint8>(word & 0xFFu),
        static_cast<uint8>((word >> 8) & 0xFFu),
        static_cast<uint8>((word >> 16) & 0xFFu),
        static_cast<uint8>((word >> 24) & 0xFFu),
    };
}

auto filledSlot(std::uint8_t slotIndex, std::uint32_t groupId, std::uint16_t groupKey, std::uint16_t color, std::uint8_t flag, const std::string& name, std::uint8_t lang, std::uint8_t membersGoal, std::uint8_t activeTier, std::uint16_t characteristics) -> SlotInput
{
    auto slot             = SlotInput{};
    slot.filled          = true;
    slot.slotIndex       = slotIndex;
    slot.groupId         = groupId;
    slot.groupKey        = groupKey;
    slot.color           = color;
    slot.flag            = flag;
    slot.name            = name;
    slot.lang            = lang;
    slot.membersGoal     = membersGoal;
    slot.activeTier      = activeTier;
    slot.characteristics = characteristics;
    return slot;
}

auto testLayoutAndEnums() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(static_cast<std::uint16_t>(PacketS2C::GP_SERV_COMMAND_LINK_CONCIERGE), 0x048, "packet id") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(sizeof(Record::PacketData{}.Indices), 4, "Indices size") && ok;
    ok      = expectEqualUInt(sizeof(Record::PacketData{}.unknown00), 8, "unknown00 size") && ok;
    ok      = expectEqualUInt(sizeof(Record::PacketData{}.Bodies) / sizeof(Exdata::Linkshell), 4, "Bodies count") && ok;
    ok      = expectEqualUInt(sizeof(Exdata::Linkshell), 24, "Exdata::Linkshell size") && ok;
    ok      = expectEqualUInt(sizeof(Record::PacketData{}.Attrs) / sizeof(GP_SERV_COMMAND_LINK_CONCIERGE::SlotAttrs), 4, "Attrs count") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_LINK_CONCIERGE::SlotAttrs), 4, "SlotAttrs size") && ok;
    ok      = expectEqualUInt(linkConciergeRecordPacketDataSize, 124, "PacketData size") && ok;
    ok      = expectEqualUInt(linkConciergeRecordPacketSize, 128, "packet size") && ok;
    ok      = expectEqualUInt(linkConciergeRecordIndicesOffset, 4, "Indices offset") && ok;
    ok      = expectEqualUInt(linkConciergeRecordUnknown00Offset, 8, "unknown00 offset") && ok;
    ok      = expectEqualUInt(linkConciergeRecordBodiesOffset, 16, "Bodies offset") && ok;
    ok      = expectEqualUInt(linkConciergeRecordAttrsOffset, 112, "Attrs offset") && ok;
    ok      = expectEqualUInt(linkshellBodyGroupIdOffset, 0, "Linkshell GroupId offset") && ok;
    ok      = expectEqualUInt(linkshellBodyGroupKeyOffset, 4, "Linkshell GroupKey offset") && ok;
    ok      = expectEqualUInt(linkshellBodyColorOffset, 6, "Linkshell Color offset") && ok;
    ok      = expectEqualUInt(linkshellBodyFlagOffset, 8, "Linkshell Flag offset") && ok;
    ok      = expectEqualUInt(linkshellBodyNameOffset, 9, "Linkshell Name offset") && ok;
    return ok;
}

auto testConstructorBytes() -> bool
{
    auto slots = std::array<SlotInput, 4>{};
    slots[0]   = filledSlot(3, 0x11223344, 0x5566, 0x789A, 0xBC, "OmegaXI2026", 2, 7, 1, 0x2468);
    slots[2]   = filledSlot(12, 0x01020304, 0x0A0B, 0x0C0D, 0x0E, "abc", 3, 10, 2, 0x1357);

    auto packet = Record(slots);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x048, "type") && ok;
    ok      = expectEqualUInt(packet.getSize(), linkConciergeRecordPacketSize, "size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x48, 0x40, 0xEF, 0xBE }, "header") && ok;
    ok      = expectBytes(packet, linkConciergeRecordIndicesOffset, std::array<uint8, 4>{ 0x03, 0xFF, 0x0C, 0xFF }, "Indices") && ok;
    ok      = expectBytes(packet, linkConciergeRecordUnknown00Offset, std::array<uint8, 8>{ 0x03, 0x02, 0x03, 0x02, 0x03, 0x02, 0x03, 0x02 }, "unknown00") && ok;

    ok = expectBytes(packet, bodyOffset(0, linkshellBodyGroupIdOffset), std::array<uint8, 4>{ 0x44, 0x33, 0x22, 0x11 }, "slot 0 GroupId") && ok;
    ok = expectBytes(packet, bodyOffset(0, linkshellBodyGroupKeyOffset), std::array<uint8, 2>{ 0x66, 0x55 }, "slot 0 GroupKey") && ok;
    ok = expectBytes(packet, bodyOffset(0, linkshellBodyColorOffset), std::array<uint8, 2>{ 0x9A, 0x78 }, "slot 0 Color") && ok;
    ok = expectBytes(packet, bodyOffset(0, linkshellBodyFlagOffset), std::array<uint8, 1>{ 0xBC }, "slot 0 Flag") && ok;
    ok = expectBytes(packet, bodyOffset(0, linkshellBodyNameOffset), std::array<uint8, 15>{ 0xA4, 0xD1, 0x47, 0x07, 0x28, 0xF7, 0xD7, 0x7E, 0xFF }, "slot 0 Name") && ok;
    ok = expectBytes(packet, attrOffset(0), attrBytes(attrWord(2, 7, 1, 0x2468)), "slot 0 Attrs") && ok;

    ok = expectZeroRange(packet, bodyOffset(1), bodyOffset(2), "slot 1 body") && ok;
    ok = expectBytes(packet, attrOffset(1), std::array<uint8, 4>{}, "slot 1 Attrs") && ok;

    ok = expectBytes(packet, bodyOffset(2, linkshellBodyGroupIdOffset), std::array<uint8, 4>{ 0x04, 0x03, 0x02, 0x01 }, "slot 2 GroupId") && ok;
    ok = expectBytes(packet, bodyOffset(2, linkshellBodyGroupKeyOffset), std::array<uint8, 2>{ 0x0B, 0x0A }, "slot 2 GroupKey") && ok;
    ok = expectBytes(packet, bodyOffset(2, linkshellBodyColorOffset), std::array<uint8, 2>{ 0x0D, 0x0C }, "slot 2 Color") && ok;
    ok = expectBytes(packet, bodyOffset(2, linkshellBodyFlagOffset), std::array<uint8, 1>{ 0x0E }, "slot 2 Flag") && ok;
    ok = expectBytes(packet, bodyOffset(2, linkshellBodyNameOffset), std::array<uint8, 15>{ 0x04, 0x20, 0xFF }, "slot 2 Name") && ok;
    ok = expectBytes(packet, attrOffset(2), attrBytes(attrWord(3, 10, 2, 0x1357)), "slot 2 Attrs") && ok;

    ok = expectZeroRange(packet, bodyOffset(3), bodyOffset(3) + sizeof(Exdata::Linkshell), "slot 3 body") && ok;
    ok = expectBytes(packet, attrOffset(3), std::array<uint8, 4>{}, "slot 3 Attrs") && ok;
    ok = expectZeroRange(packet, linkConciergeRecordPacketSize, PACKET_SIZE, "tail") && ok;
    return ok;
}

} // namespace

auto runS2CLinkConciergeRecordPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayoutAndEnums() && ok;
    ok      = testConstructorBytes() && ok;
    return ok;
}
