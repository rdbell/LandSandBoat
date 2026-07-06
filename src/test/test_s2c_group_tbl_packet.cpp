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

#include "test_s2c_group_tbl_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/packets/s2c/0x0c8_group_tbl.h"
#include "enums/party_kind.h"

namespace
{

using GroupTblPacket = GP_SERV_COMMAND_GROUP_TBL;

constexpr auto groupTblKindOffset        = sizeof(GP_SERV_HEADER) + offsetof(GroupTblPacket::PacketData, Kind);
constexpr auto groupTblPadding05Offset   = sizeof(GP_SERV_HEADER) + offsetof(GroupTblPacket::PacketData, padding05);
constexpr auto groupTblGroupTblOffset    = sizeof(GP_SERV_HEADER) + offsetof(GroupTblPacket::PacketData, GroupTbl);
constexpr auto groupTblEntrySize         = sizeof(GROUP_TBL);
constexpr auto groupTblEntryCount        = sizeof(GroupTblPacket::PacketData::GroupTbl) / sizeof(GROUP_TBL);
constexpr auto groupTblPacketDataSize     = sizeof(GroupTblPacket::PacketData);
constexpr auto groupTblPacketSize        = sizeof(GP_SERV_HEADER) + groupTblPacketDataSize;

// Per-entry field offsets, relative to the start of a GROUP_TBL entry.
constexpr auto groupTblUniqueNoOffset   = offsetof(GROUP_TBL, UniqueNo);
constexpr auto groupTblActIndexOffset   = offsetof(GROUP_TBL, ActIndex);
// The party flags are a packed bit-field (PartyNo:2, PartyLeaderFlg:1, ...)
// occupying the byte immediately after ActIndex; offsetof cannot address a
// bit-field, so derive the byte offset from the preceding field.
constexpr auto groupTblFlagsOffset     = groupTblActIndexOffset + sizeof(uint16_t);
constexpr auto groupTblPadding07Offset  = offsetof(GROUP_TBL, padding07);
constexpr auto groupTblZoneNoOffset    = offsetof(GROUP_TBL, ZoneNo);
constexpr auto groupTblPadding0AOffset  = offsetof(GROUP_TBL, padding0A);

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c GROUP_TBL packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c GROUP_TBL packet self-test failed: " << label << " got";
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
            std::cerr << "s2c GROUP_TBL packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto testLayoutAndEnums() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(static_cast<uint16>(PacketS2C::GP_SERV_COMMAND_GROUP_TBL), 0x0C8, "GROUP_TBL packet id") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(groupTblEntrySize, 12, "sizeof(GROUP_TBL)") && ok;
    ok      = expectEqualUInt(groupTblEntryCount, 20, "GroupTbl entry count") && ok;
    ok      = expectEqualUInt(groupTblPacketDataSize, 244, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(groupTblPacketSize, 248, "packet size") && ok;

    ok = expectEqualUInt(groupTblKindOffset, 4, "Kind offset") && ok;
    ok = expectEqualUInt(groupTblPadding05Offset, 5, "padding05 offset") && ok;
    ok = expectEqualUInt(groupTblGroupTblOffset, 8, "GroupTbl array offset") && ok;

    // Per-entry field offsets within a GROUP_TBL.
    ok = expectEqualUInt(groupTblUniqueNoOffset, 0, "entry UniqueNo offset") && ok;
    ok = expectEqualUInt(groupTblActIndexOffset, 4, "entry ActIndex offset") && ok;
    ok = expectEqualUInt(groupTblFlagsOffset, 6, "entry flags byte offset") && ok;
    ok = expectEqualUInt(groupTblPadding07Offset, 7, "entry padding07 offset") && ok;
    ok = expectEqualUInt(groupTblZoneNoOffset, 8, "entry ZoneNo offset") && ok;
    ok = expectEqualUInt(groupTblPadding0AOffset, 10, "entry padding0A offset") && ok;

    // The 8-bit flag field packs PartyNo:2, PartyLeaderFlg:1, AllianceLeaderFlg:1,
    // PartyRFlg:1, AllianceRFlg:1, unknown06:1, unknown07:1 (bits 0-7). The Go
    // port pins bit-position parity for this field via dedicated helper tests.
    ok = expectEqualUInt(static_cast<uint8>(PartyKind::Party), 0, "PartyKind::Party") && ok;
    ok = expectEqualUInt(static_cast<uint8>(PartyKind::Alliance), 5, "PartyKind::Alliance") && ok;
    return ok;
}

// The GROUP_TBL constructor is state-coupled (queries accounts_parties and
// iterates party members), so this slice pins only the state-light path: a null
// party yields a fully zeroed packet with Kind = Party (0). The populated
// constructor path and per-entry flag bit packing are covered by the Go port's
// isolated helper tests, which is where the partyflag -> bit mapping is pinned.
auto testConstructorWithoutParty() -> bool
{
    auto packet = GroupTblPacket(nullptr);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x0C8, "type") && ok;
    ok      = expectEqualUInt(packet.getSize(), groupTblPacketSize, "size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0xC8, 0x7C, 0xEF, 0xBE }, "header") && ok;
    ok      = expectZeroRange(packet, groupTblKindOffset, PACKET_SIZE, "null party payload") && ok;
    return ok;
}

} // namespace

auto runS2CGroupTblPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayoutAndEnums() && ok;
    ok      = testConstructorWithoutParty() && ok;
    return ok;
}
