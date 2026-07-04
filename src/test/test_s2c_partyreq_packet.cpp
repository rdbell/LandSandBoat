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

#include "test_s2c_partyreq_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/packets/s2c/0x11d_partyreq.h"

namespace
{

constexpr auto partyReqUniqueNoOffset     = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_PARTYREQ::PacketData, UniqueNo);
constexpr auto partyReqActIndexOffset     = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_PARTYREQ::PacketData, ActIndex);
constexpr auto partyReqFlagsOffset        = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_PARTYREQ::PacketData, Flags);
constexpr auto partyReqStatusOffset       = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_PARTYREQ::PacketData, Status);
constexpr auto partyReqNameOffset         = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_PARTYREQ::PacketData, sName);
constexpr auto partyReqRaceOffset         = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_PARTYREQ::PacketData, Race);
constexpr auto partyReqPadding1EOffset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_PARTYREQ::PacketData, padding1E);
constexpr auto partyReqPacketDefaultSize  = sizeof(GP_SERV_HEADER) + sizeof(GP_SERV_COMMAND_PARTYREQ::PacketData);

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c PARTYREQ packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectBytes(CBasicPacket& packet, const std::array<uint8, 32>& expected, const std::string& label) -> bool
{
    const auto* data = packetData(packet);
    if (std::memcmp(data, expected.data(), expected.size()) != 0)
    {
        std::cerr << "s2c PARTYREQ packet self-test failed: " << label << " got";
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

auto expectZeroTail(CBasicPacket& packet, std::size_t offset, const std::string& label) -> bool
{
    const auto* data = packetData(packet);
    for (std::size_t i = offset; i < PACKET_SIZE; ++i)
    {
        if (data[i] != 0)
        {
            std::cerr << "s2c PARTYREQ packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_PARTYREQ::PacketData), 28, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(partyReqPacketDefaultSize, 32, "packet default size") && ok;
    ok      = expectEqualUInt(partyReqUniqueNoOffset, 4, "UniqueNo offset") && ok;
    ok      = expectEqualUInt(partyReqActIndexOffset, 8, "ActIndex offset") && ok;
    ok      = expectEqualUInt(partyReqFlagsOffset, 10, "Flags offset") && ok;
    ok      = expectEqualUInt(partyReqStatusOffset, 11, "Status offset") && ok;
    ok      = expectEqualUInt(partyReqNameOffset, 12, "sName offset") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_PARTYREQ::PacketData::sName), 16, "sizeof(sName)") && ok;
    ok      = expectEqualUInt(partyReqRaceOffset, 28, "Race offset") && ok;
    ok      = expectEqualUInt(partyReqPadding1EOffset, 30, "padding1E offset") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_PARTYREQ::PacketData::padding1E), 2, "sizeof(padding1E)") && ok;
    return ok;
}

auto testConstructor() -> bool
{
    auto packet = GP_SERV_COMMAND_PARTYREQ();
    packet.setSequence(0xBEEF);

    const auto expected = std::array<uint8, 32>{
        0x1D, 0x11, 0xEF, 0xBE,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
    };

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x11D, "PARTYREQ type") && ok;
    ok      = expectEqualUInt(packet.getSize(), expected.size(), "PARTYREQ size") && ok;
    ok      = expectBytes(packet, expected, "encoded PARTYREQ prefix") && ok;
    ok      = expectZeroTail(packet, expected.size(), "PARTYREQ tail") && ok;
    return ok;
}

} // namespace

auto runS2CPartyReqPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testConstructor() && ok;
    return ok;
}
