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

#include "test_login_world_list_packet.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "common/md52.h"
#include "login/login_packets.h"

namespace
{

constexpr std::size_t WorldListPacketSize = sizeof(packet_t) + sizeof(std::uint32_t) + sizeof(lpkt_world_name);

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "login world list packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "login world list packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto makeWorldList(const std::string& serverName) -> lpkt_world_list
{
    auto worldList        = lpkt_world_list{};
    worldList.terminator  = loginPackets::getTerminator();
    worldList.command     = 0x23;
    worldList.sumofworld  = 0x01;
    worldList.packet_size = static_cast<std::uint32_t>(WorldListPacketSize);
    loginPackets::clearIdentifier(worldList);

    worldList.world_name[0].no = 0x20;
    std::memcpy(
        worldList.world_name[0].name,
        serverName.c_str(),
        std::clamp<std::size_t>(serverName.length(), std::size_t{ 0 }, std::size_t{ 15 }));

    auto hash = std::array<uint8, 16>{};
    md5(reinterpret_cast<uint8*>(&worldList), hash.data(), static_cast<int32>(worldList.packet_size));
    loginPackets::copyHashIntoPacket(worldList, hash.data());

    return worldList;
}

auto packetHashIsValid(const lpkt_world_list& packet) -> bool
{
    auto input = packet;
    loginPackets::clearIdentifier(input);

    auto expected = std::array<uint8, 16>{};
    md5(reinterpret_cast<uint8*>(&input), expected.data(), static_cast<int32>(packet.packet_size));

    return std::memcmp(packet.identifer, expected.data(), expected.size()) == 0;
}

auto testWorldListLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualInt(sizeof(packet_t), 28, "packet_t size") && ok;
    ok      = expectEqualInt(sizeof(lpkt_world_name), 20, "lpkt_world_name size") && ok;
    ok      = expectEqualInt(sizeof(lpkt_world_list), 52, "lpkt_world_list size") && ok;
    ok      = expectEqualInt(WorldListPacketSize, 52, "computed packet size") && ok;
    ok      = expectEqualInt(offsetof(lpkt_world_name, name), 4, "world_name name offset") && ok;
    return ok;
}

auto testWorldListPacketFields() -> bool
{
    const auto packet = makeWorldList("OmegaXI");

    bool ok = true;
    ok      = expectEqualInt(packet.packet_size, 52, "packet size") && ok;
    ok      = expectEqualInt(packet.terminator, 0x46465849, "terminator") && ok;
    ok      = expectEqualInt(packet.command, 0x23, "command") && ok;
    ok      = expectEqualInt(packet.sumofworld, 1, "world count") && ok;
    ok      = expectEqualInt(packet.world_name[0].no, 0x20, "world number") && ok;
    ok      = expectTrue(packetHashIsValid(packet), "hash field") && ok;
    return ok;
}

auto testWorldNameCopy() -> bool
{
    const auto packet = makeWorldList("OmegaXI");
    const auto want   = std::array<char, 16>{ 'O', 'm', 'e', 'g', 'a', 'X', 'I' };

    return expectTrue(std::memcmp(packet.world_name[0].name, want.data(), want.size()) == 0, "world name copy");
}

auto testWorldNameTruncation() -> bool
{
    const auto packet = makeWorldList("1234567890abcdef");
    const auto want   = std::array<char, 16>{ '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 'a', 'b', 'c', 'd', 'e' };

    bool ok = true;
    ok      = expectTrue(std::memcmp(packet.world_name[0].name, want.data(), want.size()) == 0, "truncated world name") && ok;
    ok      = expectTrue(packetHashIsValid(packet), "truncated hash field") && ok;
    return ok;
}

} // namespace

auto runLoginWorldListPacketSelfTests() -> bool
{
    return testWorldListLayout() &&
           testWorldListPacketFields() &&
           testWorldNameCopy() &&
           testWorldNameTruncation();
}
