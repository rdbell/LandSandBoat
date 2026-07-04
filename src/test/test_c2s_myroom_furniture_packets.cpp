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

#include "test_c2s_myroom_furniture_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <set>
#include <string>
#include <string_view>

#include "map/item_container.h"
#include "map/packets/c2s/0x0fa_myroom_layout.h"
#include "map/packets/c2s/0x0fb_myroom_bankin.h"

namespace
{

using MyRoomLayoutBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_MYROOM_LAYOUT)>;
using MyRoomBankInBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_MYROOM_BANKIN)>;

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s myroom furniture packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s myroom furniture packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectFalse(bool actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "c2s myroom furniture packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s myroom furniture packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(const std::array<std::uint8_t, N>& actual, const std::array<std::uint8_t, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s myroom furniture packet self-test failed: " << label << " got";
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

auto expectValid(const PacketValidationResult& result, const std::string& label) -> bool
{
    bool ok = true;
    ok      = expectTrue(result.valid(), label + " valid") && ok;
    ok      = expectEqualString(result.errorString(), "", label + " error string") && ok;
    return ok;
}

auto expectInvalidError(const PacketValidationResult& result, const std::string& expected, const std::string& label) -> bool
{
    bool ok = true;
    ok      = expectFalse(result.valid(), label + " valid") && ok;
    ok      = expectEqualString(result.errorString(), expected, label + " error string") && ok;
    return ok;
}

template <typename Packet>
auto encodedPacketBytes(const Packet& packet) -> std::array<std::uint8_t, sizeof(Packet)>
{
    auto bytes = std::array<std::uint8_t, sizeof(Packet)>{};
    std::memcpy(bytes.data(), &packet, bytes.size());
    return bytes;
}

auto myroomValidCategories() -> std::set<std::uint8_t>
{
    return { LOC_MOGSAFE, LOC_MOGSAFE2 };
}

auto validateLayoutPure(std::uint8_t floorFlg, std::uint8_t v, std::uint8_t y) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.range("MyroomFloorFlg", floorFlg, 0, 1)
        .range("v", v, 0, 3)
        .range("y", y, 0, 25);
    return validator;
}

auto validateBankInPure(std::uint16_t itemNo, std::uint8_t category) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.mustNotEqual(itemNo, 0, "MyroomItemNo must not equal 0")
        .oneOf("MyroomCategory", category, myroomValidCategories());
    return validator;
}

auto makeMyRoomLayoutPacket() -> GP_CLI_COMMAND_MYROOM_LAYOUT
{
    auto packet              = GP_CLI_COMMAND_MYROOM_LAYOUT{};
    packet.header.id        = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_MYROOM_LAYOUT);
    packet.header.size      = sizeof(GP_CLI_COMMAND_MYROOM_LAYOUT) / 4U;
    packet.header.sync      = 0xBEEF;
    packet.MyroomItemNo     = 0x1234;
    packet.MyroomItemIndex  = 0x56;
    packet.MyroomCategory   = LOC_MOGSAFE2;
    packet.MyroomFloorFlg   = 1;
    packet.x                = 0x0A;
    packet.y                = 0x19;
    packet.z                = 0x0B;
    packet.v                = 3;
    packet.padding00[0]     = 0xAA;
    packet.padding00[1]     = 0xBB;
    packet.padding00[2]     = 0xCC;
    return packet;
}

auto makeMyRoomBankInPacket() -> GP_CLI_COMMAND_MYROOM_BANKIN
{
    auto packet             = GP_CLI_COMMAND_MYROOM_BANKIN{};
    packet.header.id       = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_MYROOM_BANKIN);
    packet.header.size     = sizeof(GP_CLI_COMMAND_MYROOM_BANKIN) / 4U;
    packet.header.sync     = 0xBEEF;
    packet.MyroomItemNo    = 0x3456;
    packet.MyroomItemIndex = 0x78;
    packet.MyroomCategory  = LOC_MOGSAFE;
    return packet;
}

auto testMyRoomFurnitureLayoutsAndMetadata() -> bool
{
    bool ok = true;

    ok = expectEqualString(GP_CLI_COMMAND_MYROOM_LAYOUT::name, "GP_CLI_COMMAND_MYROOM_LAYOUT", "MYROOM_LAYOUT name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_MYROOM_LAYOUT::packetId), 0x0FA, "MYROOM_LAYOUT packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_MYROOM_LAYOUT), 16, "MYROOM_LAYOUT sizeof") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_MYROOM_LAYOUT, MyroomItemNo), 4, "MYROOM_LAYOUT MyroomItemNo offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_MYROOM_LAYOUT, MyroomItemIndex), 6, "MYROOM_LAYOUT MyroomItemIndex offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_MYROOM_LAYOUT, MyroomCategory), 7, "MYROOM_LAYOUT MyroomCategory offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_MYROOM_LAYOUT, MyroomFloorFlg), 8, "MYROOM_LAYOUT MyroomFloorFlg offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_MYROOM_LAYOUT, x), 9, "MYROOM_LAYOUT x offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_MYROOM_LAYOUT, y), 10, "MYROOM_LAYOUT y offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_MYROOM_LAYOUT, z), 11, "MYROOM_LAYOUT z offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_MYROOM_LAYOUT, v), 12, "MYROOM_LAYOUT v offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_MYROOM_LAYOUT, padding00), 13, "MYROOM_LAYOUT padding00 offset") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_MYROOM_LAYOUT::padding00), 3, "MYROOM_LAYOUT padding00 sizeof") && ok;

    ok = expectEqualString(GP_CLI_COMMAND_MYROOM_BANKIN::name, "GP_CLI_COMMAND_MYROOM_BANKIN", "MYROOM_BANKIN name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_MYROOM_BANKIN::packetId), 0x0FB, "MYROOM_BANKIN packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_MYROOM_BANKIN), 8, "MYROOM_BANKIN sizeof") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_MYROOM_BANKIN, MyroomItemNo), 4, "MYROOM_BANKIN MyroomItemNo offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_MYROOM_BANKIN, MyroomItemIndex), 6, "MYROOM_BANKIN MyroomItemIndex offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_MYROOM_BANKIN, MyroomCategory), 7, "MYROOM_BANKIN MyroomCategory offset") && ok;
    ok = expectEqualInt(LOC_MOGSAFE, 1, "LOC_MOGSAFE") && ok;
    ok = expectEqualInt(LOC_MOGSAFE2, 9, "LOC_MOGSAFE2") && ok;
    return ok;
}

auto testMyRoomFurnitureEncodedBytesAndPayloads() -> bool
{
    bool ok = true;
    ok      = expectBytes(encodedPacketBytes(makeMyRoomLayoutPacket()), MyRoomLayoutBytes{ 0xFA, 0x08, 0xEF, 0xBE, 0x34, 0x12, 0x56, 0x09, 0x01, 0x0A, 0x19, 0x0B, 0x03, 0xAA, 0xBB, 0xCC }, "MYROOM_LAYOUT encoded packet") && ok;
    ok      = expectBytes(encodedPacketBytes(makeMyRoomBankInPacket()), MyRoomBankInBytes{ 0xFB, 0x04, 0xEF, 0xBE, 0x56, 0x34, 0x78, 0x01 }, "MYROOM_BANKIN encoded packet") && ok;
    return ok;
}

auto testMyRoomFurnitureValidation() -> bool
{
    bool ok = true;
    ok      = expectValid(validateLayoutPure(0, 0, 0), "MYROOM_LAYOUT zero bounds validation") && ok;
    ok      = expectValid(validateLayoutPure(1, 3, 25), "MYROOM_LAYOUT max bounds validation") && ok;
    ok      = expectInvalidError(validateLayoutPure(2, 0, 0), "MyroomFloorFlg out of range: 2 not in [0, 1]", "MYROOM_LAYOUT invalid floor validation") && ok;
    ok      = expectInvalidError(validateLayoutPure(1, 4, 0), "v out of range: 4 not in [0, 3]", "MYROOM_LAYOUT invalid rotation validation") && ok;
    ok      = expectInvalidError(validateLayoutPure(1, 3, 26), "y out of range: 26 not in [0, 25]", "MYROOM_LAYOUT invalid y validation") && ok;
    ok      = expectInvalidError(validateLayoutPure(2, 4, 26), "MyroomFloorFlg out of range: 2 not in [0, 1]", "MYROOM_LAYOUT validation order") && ok;
    ok      = expectValid(validateBankInPure(1, LOC_MOGSAFE), "MYROOM_BANKIN mogsafe validation") && ok;
    ok      = expectValid(validateBankInPure(1, LOC_MOGSAFE2), "MYROOM_BANKIN mogsafe2 validation") && ok;
    ok      = expectInvalidError(validateBankInPure(0, LOC_MOGSAFE), "MyroomItemNo must not equal 0", "MYROOM_BANKIN item zero validation") && ok;
    ok      = expectInvalidError(validateBankInPure(1, LOC_INVENTORY), "MyroomCategory value 0 is not allowed.", "MYROOM_BANKIN invalid category validation") && ok;
    ok      = expectInvalidError(validateBankInPure(0, LOC_INVENTORY), "MyroomItemNo must not equal 0", "MYROOM_BANKIN validation order") && ok;
    return ok;
}

} // namespace

auto runC2SMyRoomFurniturePacketSelfTests() -> bool
{
    return testMyRoomFurnitureLayoutsAndMetadata() &&
           testMyRoomFurnitureEncodedBytesAndPayloads() &&
           testMyRoomFurnitureValidation();
}
