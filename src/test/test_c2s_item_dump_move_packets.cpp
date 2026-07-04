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

#include "test_c2s_item_dump_move_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <set>
#include <string>
#include <string_view>

#include "map/item_container.h"
#include "map/packets/c2s/0x028_item_dump.h"
#include "map/packets/c2s/0x029_item_move.h"

namespace
{

using DumpPacketBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_ITEM_DUMP)>;
using MovePacketBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_ITEM_MOVE)>;

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s ITEM_DUMP/MOVE packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s ITEM_DUMP/MOVE packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectFalse(bool actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "c2s ITEM_DUMP/MOVE packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s ITEM_DUMP/MOVE packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(const std::array<std::uint8_t, N>& actual, const std::array<std::uint8_t, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s ITEM_DUMP/MOVE packet self-test failed: " << label << " got";
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

auto encodedDumpPacket(std::uint32_t itemNum, std::uint8_t category, std::uint8_t itemIndex) -> DumpPacketBytes
{
    auto packet         = GP_CLI_COMMAND_ITEM_DUMP{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_ITEM_DUMP);
    packet.header.size = sizeof(GP_CLI_COMMAND_ITEM_DUMP) / 4;
    packet.header.sync = 0xBEEF;
    packet.ItemNum     = itemNum;
    packet.Category    = category;
    packet.ItemIndex   = itemIndex;

    auto bytes = DumpPacketBytes{};
    std::memcpy(bytes.data(), &packet, bytes.size());
    return bytes;
}

auto encodedMovePacket(std::uint32_t itemNum, std::uint8_t category1, std::uint8_t category2, std::uint8_t itemIndex1, std::uint8_t itemIndex2) -> MovePacketBytes
{
    auto packet         = GP_CLI_COMMAND_ITEM_MOVE{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_ITEM_MOVE);
    packet.header.size = sizeof(GP_CLI_COMMAND_ITEM_MOVE) / 4;
    packet.header.sync = 0xBEEF;
    packet.ItemNum     = itemNum;
    packet.Category1   = category1;
    packet.Category2   = category2;
    packet.ItemIndex1  = itemIndex1;
    packet.ItemIndex2  = itemIndex2;

    auto bytes = MovePacketBytes{};
    std::memcpy(bytes.data(), &packet, bytes.size());
    return bytes;
}

auto dumpValidContainers() -> std::set<CONTAINER_ID>
{
    return {
        LOC_INVENTORY,
        LOC_MOGSAFE,
        LOC_MOGSAFE2,
        LOC_STORAGE,
        LOC_TEMPITEMS,
        LOC_MOGLOCKER,
        LOC_MOGSATCHEL,
        LOC_MOGSACK,
        LOC_MOGCASE,
        LOC_WARDROBE,
        LOC_WARDROBE2,
        LOC_WARDROBE3,
        LOC_WARDROBE4,
        LOC_WARDROBE5,
        LOC_WARDROBE6,
        LOC_WARDROBE7,
        LOC_WARDROBE8,
    };
}

auto dumpPureValidation(std::uint8_t category, std::uint32_t itemNum) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator
        .oneOf("Category", static_cast<CONTAINER_ID>(category), dumpValidContainers())
        .range("ItemNum", itemNum, 0, 99);
    return static_cast<PacketValidationResult>(validator);
}

auto movePotentialContainers() -> std::set<CONTAINER_ID>
{
    // validContainers(PChar) is stateful; this is the union of values that can
    // appear in that set for any character/configuration.
    return {
        LOC_INVENTORY,
        LOC_MOGSAFE,
        LOC_STORAGE,
        LOC_MOGLOCKER,
        LOC_MOGSATCHEL,
        LOC_MOGSACK,
        LOC_MOGCASE,
        LOC_WARDROBE,
        LOC_MOGSAFE2,
        LOC_WARDROBE2,
        LOC_WARDROBE3,
        LOC_WARDROBE4,
        LOC_WARDROBE5,
        LOC_WARDROBE6,
        LOC_WARDROBE7,
        LOC_WARDROBE8,
        LOC_RECYCLEBIN,
    };
}

auto movePureValidation(std::uint8_t category1, std::uint8_t category2) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator
        .oneOf("Category1", static_cast<CONTAINER_ID>(category1), movePotentialContainers())
        .oneOf("Category2", static_cast<CONTAINER_ID>(category2), movePotentialContainers());
    return static_cast<PacketValidationResult>(validator);
}

auto testDumpLayoutAndMetadata() -> bool
{
    bool ok = true;

    ok = expectEqualString(GP_CLI_COMMAND_ITEM_DUMP::name, "GP_CLI_COMMAND_ITEM_DUMP", "ITEM_DUMP static name") && ok;
    ok = expectTrue(GP_CLI_COMMAND_ITEM_DUMP::packetId == PacketC2S::GP_CLI_COMMAND_ITEM_DUMP, "ITEM_DUMP static packetId") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_ITEM_DUMP), 12, "sizeof(GP_CLI_COMMAND_ITEM_DUMP)") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ITEM_DUMP, header), 0, "ITEM_DUMP header offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ITEM_DUMP, ItemNum), 4, "ITEM_DUMP ItemNum offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ITEM_DUMP, Category), 8, "ITEM_DUMP Category offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ITEM_DUMP, ItemIndex), 9, "ITEM_DUMP ItemIndex offset") && ok;
    ok = expectBytes(encodedDumpPacket(0x11223344, LOC_MOGLOCKER, 0x55),
                     DumpPacketBytes{ 0x28, 0x06, 0xEF, 0xBE,
                                      0x44, 0x33, 0x22, 0x11,
                                      0x04, 0x55, 0x00, 0x00 },
                     "ITEM_DUMP encoded packet") &&
         ok;

    return ok;
}

auto testDumpPayloadStorage() -> bool
{
    auto packet       = GP_CLI_COMMAND_ITEM_DUMP{};
    packet.ItemNum   = 0xFFFFFFFF;
    packet.Category  = LOC_WARDROBE8;
    packet.ItemIndex = 0xFF;

    bool ok = true;
    ok      = expectEqualInt(packet.ItemNum, 0xFFFFFFFF, "ITEM_DUMP ItemNum") && ok;
    ok      = expectEqualInt(packet.Category, LOC_WARDROBE8, "ITEM_DUMP Category") && ok;
    ok      = expectEqualInt(packet.ItemIndex, 0xFF, "ITEM_DUMP ItemIndex") && ok;
    return ok;
}

auto testDumpPureValidationFacts() -> bool
{
    bool ok = true;
    ok      = expectValid(dumpPureValidation(LOC_INVENTORY, 0), "ITEM_DUMP zero ItemNum validation") && ok;
    ok      = expectValid(dumpPureValidation(LOC_WARDROBE8, 99), "ITEM_DUMP max ItemNum validation") && ok;
    ok      = expectValid(dumpPureValidation(LOC_TEMPITEMS, 1), "ITEM_DUMP temp items validation") && ok;
    ok      = expectInvalidError(dumpPureValidation(LOC_INVENTORY, 100), "ItemNum out of range: 100 not in [0, 99]", "ITEM_DUMP ItemNum high validation") && ok;
    ok      = expectInvalidError(dumpPureValidation(LOC_RECYCLEBIN, 1), "Category value 17 is not allowed.", "ITEM_DUMP recycle bin validation") && ok;
    ok      = expectInvalidError(dumpPureValidation(MAX_CONTAINER_ID, 1), "Category value 18 is not allowed.", "ITEM_DUMP unknown container validation") && ok;
    return ok;
}

auto testMoveLayoutAndMetadata() -> bool
{
    bool ok = true;

    ok = expectEqualString(GP_CLI_COMMAND_ITEM_MOVE::name, "GP_CLI_COMMAND_ITEM_MOVE", "ITEM_MOVE static name") && ok;
    ok = expectTrue(GP_CLI_COMMAND_ITEM_MOVE::packetId == PacketC2S::GP_CLI_COMMAND_ITEM_MOVE, "ITEM_MOVE static packetId") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_ITEM_MOVE), 12, "sizeof(GP_CLI_COMMAND_ITEM_MOVE)") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ITEM_MOVE, header), 0, "ITEM_MOVE header offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ITEM_MOVE, ItemNum), 4, "ITEM_MOVE ItemNum offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ITEM_MOVE, Category1), 8, "ITEM_MOVE Category1 offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ITEM_MOVE, Category2), 9, "ITEM_MOVE Category2 offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ITEM_MOVE, ItemIndex1), 10, "ITEM_MOVE ItemIndex1 offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ITEM_MOVE, ItemIndex2), 11, "ITEM_MOVE ItemIndex2 offset") && ok;
    ok = expectBytes(encodedMovePacket(0x11223344, LOC_MOGLOCKER, LOC_RECYCLEBIN, 0x55, 0x66),
                     MovePacketBytes{ 0x29, 0x06, 0xEF, 0xBE,
                                      0x44, 0x33, 0x22, 0x11,
                                      0x04, 0x11, 0x55, 0x66 },
                     "ITEM_MOVE encoded packet") &&
         ok;

    return ok;
}

auto testMovePayloadStorage() -> bool
{
    auto packet        = GP_CLI_COMMAND_ITEM_MOVE{};
    packet.ItemNum    = 0xFFFFFFFF;
    packet.Category1  = LOC_MOGLOCKER;
    packet.Category2  = LOC_RECYCLEBIN;
    packet.ItemIndex1 = 0xFE;
    packet.ItemIndex2 = 0xFF;

    bool ok = true;
    ok      = expectEqualInt(packet.ItemNum, 0xFFFFFFFF, "ITEM_MOVE ItemNum") && ok;
    ok      = expectEqualInt(packet.Category1, LOC_MOGLOCKER, "ITEM_MOVE Category1") && ok;
    ok      = expectEqualInt(packet.Category2, LOC_RECYCLEBIN, "ITEM_MOVE Category2") && ok;
    ok      = expectEqualInt(packet.ItemIndex1, 0xFE, "ITEM_MOVE ItemIndex1") && ok;
    ok      = expectEqualInt(packet.ItemIndex2, 0xFF, "ITEM_MOVE ItemIndex2") && ok;
    return ok;
}

auto testMovePureValidationFacts() -> bool
{
    bool ok = true;
    ok      = expectValid(movePureValidation(LOC_MOGLOCKER, LOC_RECYCLEBIN), "ITEM_MOVE possible container validation") && ok;
    ok      = expectInvalidError(movePureValidation(LOC_TEMPITEMS, LOC_INVENTORY), "Category1 value 3 is not allowed.", "ITEM_MOVE temp items validation") && ok;
    ok      = expectInvalidError(movePureValidation(LOC_INVENTORY, MAX_CONTAINER_ID), "Category2 value 18 is not allowed.", "ITEM_MOVE unknown container validation") && ok;
    return ok;
}

} // namespace

auto runC2SItemDumpMovePacketSelfTests() -> bool
{
    bool ok = true;

    ok = testDumpLayoutAndMetadata() && ok;
    ok = testDumpPayloadStorage() && ok;
    ok = testDumpPureValidationFacts() && ok;
    ok = testMoveLayoutAndMetadata() && ok;
    ok = testMovePayloadStorage() && ok;
    ok = testMovePureValidationFacts() && ok;

    return ok;
}
