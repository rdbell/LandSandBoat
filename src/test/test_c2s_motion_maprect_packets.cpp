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

#include "test_c2s_motion_maprect_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "map/enums/emote.h"
#include "map/packets/c2s/0x05d_motion.h"
#include "map/packets/c2s/0x05e_maprect.h"

namespace
{

using MotionPacketBytes  = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_MOTION)>;
using MapRectPacketBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_MAPRECT)>;

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s MOTION/MAPRECT packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s MOTION/MAPRECT packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s MOTION/MAPRECT packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(const std::array<std::uint8_t, N>& actual, const std::array<std::uint8_t, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s MOTION/MAPRECT packet self-test failed: " << label << " got";
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

auto makeMotionPacket() -> GP_CLI_COMMAND_MOTION
{
    auto packet         = GP_CLI_COMMAND_MOTION{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_MOTION);
    packet.header.size = sizeof(GP_CLI_COMMAND_MOTION) / 4;
    packet.header.sync = 0xBEEF;
    packet.UniqueNo    = 0x11223344;
    packet.ActIndex    = 0x5566;
    packet.Number      = static_cast<std::uint8_t>(Emote::Bell);
    packet.Mode        = static_cast<std::uint8_t>(EmoteMode::Motion);
    packet.Param       = 0x7788;
    packet.padding00   = 0x99AA;
    return packet;
}

auto encodedMotionPacket() -> MotionPacketBytes
{
    const auto packet = makeMotionPacket();
    auto       bytes  = MotionPacketBytes{};
    std::memcpy(bytes.data(), &packet, bytes.size());
    return bytes;
}

auto makeMapRectPacket() -> GP_CLI_COMMAND_MAPRECT
{
    auto packet            = GP_CLI_COMMAND_MAPRECT{};
    packet.header.id      = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_MAPRECT);
    packet.header.size    = sizeof(GP_CLI_COMMAND_MAPRECT) / 4;
    packet.header.sync    = 0xBEEF;
    packet.RectID         = 0x71726D7A; // "zmrq" as little-endian bytes.
    packet.x              = 1.25F;
    packet.y              = -2.5F;
    packet.z              = 3.75F;
    packet.ActIndex       = 0x99AA;
    packet.MyRoomExitBit  = static_cast<std::uint8_t>(GP_CLI_COMMAND_MAPRECT_MYROOMEXITBIT::Adoulin);
    packet.MyRoomExitMode = static_cast<std::uint8_t>(GP_CLI_COMMAND_MAPRECT_MYROOMEXITMODE::MogGarden);
    return packet;
}

auto encodedMapRectPacket() -> MapRectPacketBytes
{
    const auto packet = makeMapRectPacket();
    auto       bytes  = MapRectPacketBytes{};
    std::memcpy(bytes.data(), &packet, bytes.size());
    return bytes;
}

auto testMotionLayoutAndMetadata() -> bool
{
    bool ok = true;
    ok      = expectEqualString(GP_CLI_COMMAND_MOTION::name, "GP_CLI_COMMAND_MOTION", "MOTION name") && ok;
    ok      = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_MOTION::packetId), 0x05D, "MOTION packet id") && ok;
    ok      = expectEqualInt(sizeof(GP_CLI_COMMAND_MOTION), 16, "MOTION sizeof") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_MOTION, UniqueNo), 4, "MOTION UniqueNo offset") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_MOTION, ActIndex), 8, "MOTION ActIndex offset") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_MOTION, Number), 10, "MOTION Number offset") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_MOTION, Mode), 11, "MOTION Mode offset") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_MOTION, Param), 12, "MOTION Param offset") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_MOTION, padding00), 14, "MOTION padding00 offset") && ok;
    return ok;
}

auto testMotionEnumsAndScalarDomain() -> bool
{
    bool ok = true;
    ok      = expectEqualInt(static_cast<std::uint8_t>(Emote::Point), 0, "Emote::Point") && ok;
    ok      = expectEqualInt(static_cast<std::uint8_t>(Emote::Bell), 73, "Emote::Bell") && ok;
    ok      = expectEqualInt(static_cast<std::uint8_t>(Emote::Aim), 96, "Emote::Aim") && ok;
    ok      = expectEqualInt(static_cast<std::uint8_t>(EmoteMode::All), 0, "EmoteMode::All") && ok;
    ok      = expectEqualInt(static_cast<std::uint8_t>(EmoteMode::Text), 1, "EmoteMode::Text") && ok;
    ok      = expectEqualInt(static_cast<std::uint8_t>(EmoteMode::Motion), 2, "EmoteMode::Motion") && ok;
    ok      = expectTrue(static_cast<std::uint8_t>(Emote::Point) <= 95 && 95 <= static_cast<std::uint8_t>(Emote::Aim),
                         "MOTION Number range accepts sparse emote hole 95") &&
         ok;
    return ok;
}

auto testMotionEncodedBytesAndPayload() -> bool
{
    bool ok = true;

    constexpr auto expected = MotionPacketBytes{
        0x5D, 0x08, 0xEF, 0xBE,
        0x44, 0x33, 0x22, 0x11,
        0x66, 0x55,
        0x49, 0x02,
        0x88, 0x77,
        0xAA, 0x99,
    };

    const auto packet = makeMotionPacket();
    ok                = expectBytes(encodedMotionPacket(), expected, "MOTION encoded bytes") && ok;
    ok                = expectEqualInt(packet.UniqueNo, 0x11223344, "MOTION UniqueNo payload") && ok;
    ok                = expectEqualInt(packet.ActIndex, 0x5566, "MOTION ActIndex payload") && ok;
    ok                = expectEqualInt(packet.Number, static_cast<std::uint8_t>(Emote::Bell), "MOTION Number payload") && ok;
    ok                = expectEqualInt(packet.Mode, static_cast<std::uint8_t>(EmoteMode::Motion), "MOTION Mode payload") && ok;
    ok                = expectEqualInt(packet.Param, 0x7788, "MOTION Param payload") && ok;
    ok                = expectEqualInt(packet.padding00, 0x99AA, "MOTION padding00 payload") && ok;
    return ok;
}

auto testMapRectLayoutAndMetadata() -> bool
{
    bool ok = true;
    ok      = expectEqualString(GP_CLI_COMMAND_MAPRECT::name, "GP_CLI_COMMAND_MAPRECT", "MAPRECT name") && ok;
    ok      = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_MAPRECT::packetId), 0x05E, "MAPRECT packet id") && ok;
    ok      = expectEqualInt(sizeof(GP_CLI_COMMAND_MAPRECT), 24, "MAPRECT sizeof") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_MAPRECT, RectID), 4, "MAPRECT RectID offset") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_MAPRECT, x), 8, "MAPRECT x offset") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_MAPRECT, y), 12, "MAPRECT y offset") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_MAPRECT, z), 16, "MAPRECT z offset") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_MAPRECT, ActIndex), 20, "MAPRECT ActIndex offset") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_MAPRECT, MyRoomExitBit), 22, "MAPRECT MyRoomExitBit offset") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_MAPRECT, MyRoomExitMode), 23, "MAPRECT MyRoomExitMode offset") && ok;
    return ok;
}

auto testMapRectEnums() -> bool
{
    bool ok = true;
    ok      = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_MAPRECT_MYROOMEXITBIT::Default), 0, "MyRoomExitBit::Default") && ok;
    ok      = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_MAPRECT_MYROOMEXITBIT::SandOria), 1, "MyRoomExitBit::SandOria") && ok;
    ok      = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_MAPRECT_MYROOMEXITBIT::Bastok), 2, "MyRoomExitBit::Bastok") && ok;
    ok      = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_MAPRECT_MYROOMEXITBIT::Windurst), 3, "MyRoomExitBit::Windurst") && ok;
    ok      = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_MAPRECT_MYROOMEXITBIT::Jeuno), 4, "MyRoomExitBit::Jeuno") && ok;
    ok      = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_MAPRECT_MYROOMEXITBIT::Whitegate), 5, "MyRoomExitBit::Whitegate") && ok;
    ok      = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_MAPRECT_MYROOMEXITBIT::RonfaureFront), 6, "MyRoomExitBit::RonfaureFront") && ok;
    ok      = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_MAPRECT_MYROOMEXITBIT::GustabergFront), 7, "MyRoomExitBit::GustabergFront") && ok;
    ok      = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_MAPRECT_MYROOMEXITBIT::SarutaFront), 8, "MyRoomExitBit::SarutaFront") && ok;
    ok      = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_MAPRECT_MYROOMEXITBIT::Adoulin), 9, "MyRoomExitBit::Adoulin") && ok;

    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_MAPRECT_MYROOMEXITMODE::AreaEnteredFrom), 0, "MyRoomExitMode::AreaEnteredFrom") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_MAPRECT_MYROOMEXITMODE::Option1), 1, "MyRoomExitMode::Option1") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_MAPRECT_MYROOMEXITMODE::Option2), 2, "MyRoomExitMode::Option2") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_MAPRECT_MYROOMEXITMODE::Option3), 3, "MyRoomExitMode::Option3") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_MAPRECT_MYROOMEXITMODE::Option4), 4, "MyRoomExitMode::Option4") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_MAPRECT_MYROOMEXITMODE::Mog2F), 125, "MyRoomExitMode::Mog2F") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_MAPRECT_MYROOMEXITMODE::Mog1F), 126, "MyRoomExitMode::Mog1F") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_MAPRECT_MYROOMEXITMODE::MogGarden), 127, "MyRoomExitMode::MogGarden") && ok;
    return ok;
}

auto testMapRectEncodedBytesAndPayload() -> bool
{
    bool ok = true;

    constexpr auto expected = MapRectPacketBytes{
        0x5E, 0x0C, 0xEF, 0xBE,
        0x7A, 0x6D, 0x72, 0x71,
        0x00, 0x00, 0xA0, 0x3F,
        0x00, 0x00, 0x20, 0xC0,
        0x00, 0x00, 0x70, 0x40,
        0xAA, 0x99,
        0x09, 0x7F,
    };

    const auto packet = makeMapRectPacket();
    ok                = expectBytes(encodedMapRectPacket(), expected, "MAPRECT encoded bytes") && ok;
    ok                = expectEqualInt(packet.RectID, 0x71726D7A, "MAPRECT RectID payload") && ok;
    ok                = expectEqualInt(packet.ActIndex, 0x99AA, "MAPRECT ActIndex payload") && ok;
    ok                = expectEqualInt(packet.MyRoomExitBit, static_cast<std::uint8_t>(GP_CLI_COMMAND_MAPRECT_MYROOMEXITBIT::Adoulin), "MAPRECT MyRoomExitBit payload") && ok;
    ok                = expectEqualInt(packet.MyRoomExitMode, static_cast<std::uint8_t>(GP_CLI_COMMAND_MAPRECT_MYROOMEXITMODE::MogGarden), "MAPRECT MyRoomExitMode payload") && ok;
    return ok;
}

} // namespace

auto runC2SMotionMapRectPacketSelfTests() -> bool
{
    return testMotionLayoutAndMetadata() &&
           testMotionEnumsAndScalarDomain() &&
           testMotionEncodedBytesAndPayload() &&
           testMapRectLayoutAndMetadata() &&
           testMapRectEnums() &&
           testMapRectEncodedBytesAndPayload();
}
