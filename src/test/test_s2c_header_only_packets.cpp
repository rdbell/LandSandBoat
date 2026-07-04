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

#include "test_s2c_header_only_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/packets/s2c/0x006_naraku.h"
#include "map/packets/s2c/0x02e_openmogmenu.h"

namespace
{

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c header-only packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectHeader(CBasicPacket& packet, const std::array<uint8, 4>& expected, const std::string& label) -> bool
{
    const auto* data = packetData(packet);
    if (std::memcmp(data, expected.data(), expected.size()) != 0)
    {
        std::cerr << "s2c header-only packet self-test failed: " << label << " got";
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
            std::cerr << "s2c header-only packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto testNarakuPacket() -> bool
{
    auto packet = GP_SERV_COMMAND_NARAKU();
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualInt(packet.getType(), 0x006, "NARAKU type") && ok;
    ok      = expectEqualInt(packet.getSize(), sizeof(GP_SERV_HEADER), "NARAKU size") && ok;
    ok      = expectHeader(packet, { 0x06, 0x02, 0xEF, 0xBE }, "NARAKU header") && ok;
    ok      = expectZeroTail(packet, sizeof(GP_SERV_HEADER), "NARAKU tail") && ok;

    return ok;
}

auto testOpenMogMenuPacket() -> bool
{
    auto packet = GP_SERV_COMMAND_OPENMOGMENU();
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualInt(packet.getType(), 0x02E, "OPENMOGMENU type") && ok;
    ok      = expectEqualInt(packet.getSize(), sizeof(GP_SERV_HEADER), "OPENMOGMENU size") && ok;
    ok      = expectHeader(packet, { 0x2E, 0x02, 0xEF, 0xBE }, "OPENMOGMENU header") && ok;
    ok      = expectZeroTail(packet, sizeof(GP_SERV_HEADER), "OPENMOGMENU tail") && ok;

    return ok;
}

template <typename Packet>
auto testPacketCopy(Packet& source, const std::array<uint8, 4>& expectedHeader, const std::string& label) -> bool
{
    auto copy = source.copy();
    bool ok   = true;

    ok = expectEqualInt(copy->getType(), source.getType(), label + " copy type") && ok;
    ok = expectEqualInt(copy->getSize(), source.getSize(), label + " copy size") && ok;
    ok = expectHeader(*copy, expectedHeader, label + " copy header") && ok;

    return ok;
}

auto testCopyPreservesHeader() -> bool
{
    auto naraku = GP_SERV_COMMAND_NARAKU();
    naraku.setSequence(0xBEEF);

    auto openMogMenu = GP_SERV_COMMAND_OPENMOGMENU();
    openMogMenu.setSequence(0xBEEF);

    bool ok = true;
    ok      = testPacketCopy(naraku, { 0x06, 0x02, 0xEF, 0xBE }, "NARAKU") && ok;
    ok      = testPacketCopy(openMogMenu, { 0x2E, 0x02, 0xEF, 0xBE }, "OPENMOGMENU") && ok;

    return ok;
}

} // namespace

auto runS2CHeaderOnlyPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testNarakuPacket() && ok;
    ok      = testOpenMogMenuPacket() && ok;
    ok      = testCopyPreservesHeader() && ok;
    return ok;
}
