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

#include "test_c2s_gm_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "common/utils.h"
#include "map/packets/c2s/0x01e_gm.h"
#include "map/packets/c2s/0x01f_gmcommand.h"
#include "map/packets/c2s/gm_runtime.h"

namespace
{

constexpr auto gmCommandOffset             = 4U;
constexpr auto gmCommandMaxSize            = 115U;
constexpr auto gmCommandUniqueNoOffset     = 4U;
constexpr auto gmCommandCommandOffset      = 8U;
constexpr auto gmCommandCommandMaxSize     = 128U;

using GMShortBytes        = std::array<std::uint8_t, 8>;
using GMFullBytes         = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_GM)>;
using GMCommandShortBytes = std::array<std::uint8_t, 12>;
using GMCommandFullBytes  = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_GMCOMMAND)>;

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s gm packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s gm packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(const std::array<std::uint8_t, N>& actual, const std::array<std::uint8_t, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s gm packet self-test failed: " << label << " got";
        for (const auto value : actual)
        {
            std::cerr << ' ' << static_cast<unsigned>(value);
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

template <typename Packet, std::size_t N>
auto encodedPacketBytes(const Packet& packet) -> std::array<std::uint8_t, N>
{
    auto bytes = std::array<std::uint8_t, N>{};
    std::memcpy(bytes.data(), &packet, bytes.size());
    return bytes;
}

template <std::size_t N>
auto makePattern(std::uint8_t first) -> std::array<std::uint8_t, N>
{
    auto value = std::array<std::uint8_t, N>{};
    for (std::size_t i = 0; i < value.size(); ++i)
    {
        value[i] = static_cast<std::uint8_t>(first + (i % 26U));
    }
    return value;
}

auto makeGMShortPacket() -> GP_CLI_COMMAND_GM
{
    static constexpr auto text = std::string_view("Vol");
    auto                  packet = GP_CLI_COMMAND_GM{};
    packet.header.id     = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_GM);
    packet.header.size   = roundUpToNearestFour(static_cast<uint32>(gmCommandOffset + text.size())) / 4U;
    packet.header.sync   = 0xBEEF;
    std::memcpy(packet.Command, text.data(), text.size());
    return packet;
}

auto makeGMFullPacket() -> GP_CLI_COMMAND_GM
{
    const auto command = makePattern<gmCommandMaxSize>('A');
    auto       packet = GP_CLI_COMMAND_GM{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_GM);
    packet.header.size = roundUpToNearestFour(static_cast<uint32>(gmCommandOffset + command.size())) / 4U;
    packet.header.sync = 0x1234;
    std::memcpy(packet.Command, command.data(), command.size());
    return packet;
}

auto makeGMCommandShortPacket() -> GP_CLI_COMMAND_GMCOMMAND
{
    static constexpr auto text = std::string_view("Run");
    auto                  packet = GP_CLI_COMMAND_GMCOMMAND{};
    packet.header.id     = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_GMCOMMAND);
    packet.header.size   = roundUpToNearestFour(static_cast<uint32>(gmCommandCommandOffset + text.size())) / 4U;
    packet.header.sync   = 0xBEEF;
    packet.GMUniqueNo    = 0x01020304;
    std::memcpy(packet.Command, text.data(), text.size());
    return packet;
}

auto makeGMCommandFullPacket() -> GP_CLI_COMMAND_GMCOMMAND
{
    const auto command = makePattern<gmCommandCommandMaxSize>('0');
    auto       packet = GP_CLI_COMMAND_GMCOMMAND{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_GMCOMMAND);
    packet.header.size = roundUpToNearestFour(static_cast<uint32>(gmCommandCommandOffset + command.size())) / 4U;
    packet.header.sync = 0x5678;
    packet.GMUniqueNo  = 0xAABBCCDD;
    std::memcpy(packet.Command, command.data(), command.size());
    return packet;
}

auto testGMPacketLayoutsAndMetadata() -> bool
{
    bool ok = true;

    const auto gmPacket        = GP_CLI_COMMAND_GM{};
    const auto gmCommandPacket = GP_CLI_COMMAND_GMCOMMAND{};

    ok = expectEqualString(GP_CLI_COMMAND_GM::name, "GP_CLI_COMMAND_GM", "GM name") && ok;
    ok = expectEqualString(GP_CLI_COMMAND_GMCOMMAND::name, "GP_CLI_COMMAND_GMCOMMAND", "GMCOMMAND name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_GM::packetId), 0x01E, "GM packet id") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_GMCOMMAND::packetId), 0x01F, "GMCOMMAND packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_HEADER), 4, "client header size") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_GM), 120, "GM sizeof") && ok;
    ok = expectEqualInt(GP_CLI_COMMAND_GM::getMinSize(), gmCommandOffset, "GM min size") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_GM, Command), gmCommandOffset, "GM Command offset") && ok;
    ok = expectEqualInt(sizeof(gmPacket.Command), gmCommandMaxSize, "GM Command array size") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_GMCOMMAND), 136, "GMCOMMAND sizeof") && ok;
    ok = expectEqualInt(GP_CLI_COMMAND_GMCOMMAND::getMinSize(), gmCommandCommandOffset, "GMCOMMAND min size") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_GMCOMMAND, GMUniqueNo), gmCommandUniqueNoOffset, "GMCOMMAND GMUniqueNo offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_GMCOMMAND, Command), gmCommandCommandOffset, "GMCOMMAND Command offset") && ok;
    ok = expectEqualInt(sizeof(gmCommandPacket.GMUniqueNo), 4, "GMCOMMAND GMUniqueNo size") && ok;
    ok = expectEqualInt(sizeof(gmCommandPacket.Command), gmCommandCommandMaxSize, "GMCOMMAND Command array size") && ok;

    return ok;
}

auto testGMPacketEncodedBytes() -> bool
{
    bool ok = true;

    ok = expectBytes(encodedPacketBytes<GP_CLI_COMMAND_GM, GMShortBytes{}.size()>(makeGMShortPacket()),
                     GMShortBytes{ 0x1E, 0x04, 0xEF, 0xBE, 0x56, 0x6F, 0x6C, 0x00 },
                     "GM short bytes") &&
         ok;

    const auto gmCommand = makePattern<gmCommandMaxSize>('A');
    auto       gmFullExpected = GMFullBytes{};
    gmFullExpected[0] = 0x1E;
    gmFullExpected[1] = 0x3C;
    gmFullExpected[2] = 0x34;
    gmFullExpected[3] = 0x12;
    std::memcpy(gmFullExpected.data() + gmCommandOffset, gmCommand.data(), gmCommand.size());
    ok = expectBytes(encodedPacketBytes<GP_CLI_COMMAND_GM, GMFullBytes{}.size()>(makeGMFullPacket()), gmFullExpected, "GM full bytes") && ok;

    ok = expectBytes(encodedPacketBytes<GP_CLI_COMMAND_GMCOMMAND, GMCommandShortBytes{}.size()>(makeGMCommandShortPacket()),
                     GMCommandShortBytes{ 0x1F, 0x06, 0xEF, 0xBE, 0x04, 0x03, 0x02, 0x01, 0x52, 0x75, 0x6E, 0x00 },
                     "GMCOMMAND short bytes") &&
         ok;

    const auto gmCommandCommand = makePattern<gmCommandCommandMaxSize>('0');
    auto       gmCommandFullExpected = GMCommandFullBytes{};
    gmCommandFullExpected[0] = 0x1F;
    gmCommandFullExpected[1] = 0x44;
    gmCommandFullExpected[2] = 0x78;
    gmCommandFullExpected[3] = 0x56;
    gmCommandFullExpected[4] = 0xDD;
    gmCommandFullExpected[5] = 0xCC;
    gmCommandFullExpected[6] = 0xBB;
    gmCommandFullExpected[7] = 0xAA;
    std::memcpy(gmCommandFullExpected.data() + gmCommandCommandOffset, gmCommandCommand.data(), gmCommandCommand.size());
    ok = expectBytes(encodedPacketBytes<GP_CLI_COMMAND_GMCOMMAND, GMCommandFullBytes{}.size()>(makeGMCommandFullPacket()), gmCommandFullExpected, "GMCOMMAND full bytes") && ok;

    return ok;
}

auto testGMCommandExtraction() -> bool
{
    uint8_t command[gmCommandMaxSize]{};
    std::memcpy(command, "Vol\0ignored", 12);

    bool ok = true;
    ok      = expectEqualString(gmhelpers::CommandFrom(command, 1), "", "GM zero-byte command") && ok;
    ok      = expectEqualString(gmhelpers::CommandFrom(command, 2), "Vol", "GM short command") && ok;

    uint8_t fullCommand[gmCommandMaxSize]{};
    const auto pattern = makePattern<gmCommandMaxSize>('A');
    std::memcpy(fullCommand, pattern.data(), pattern.size());
    ok = expectEqualString(gmhelpers::CommandFrom(fullCommand, 255), std::string(pattern.begin(), pattern.end()), "GM capped command") && ok;
    return ok;
}

} // namespace

auto runC2SGMPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testGMPacketLayoutsAndMetadata() && ok;
    ok      = testGMPacketEncodedBytes() && ok;
    ok      = testGMCommandExtraction() && ok;
    return ok;
}
