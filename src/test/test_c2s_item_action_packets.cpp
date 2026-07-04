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

#include "test_c2s_item_action_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <set>
#include <string>
#include <string_view>

#include "map/item_container.h"
#include "map/packets/c2s/0x036_item_transfer.h"
#include "map/packets/c2s/0x037_item_use.h"

namespace
{

using TransferPacketBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_ITEM_TRANSFER)>;
using UsePacketBytes      = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_ITEM_USE)>;

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s ITEM_TRANSFER/USE packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s ITEM_TRANSFER/USE packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectFalse(bool actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "c2s ITEM_TRANSFER/USE packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s ITEM_TRANSFER/USE packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(const std::array<std::uint8_t, N>& actual, const std::array<std::uint8_t, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s ITEM_TRANSFER/USE packet self-test failed: " << label << " got";
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

auto transferItemNums() -> std::array<std::uint32_t, 10>
{
    return {
        0x01020304,
        0x11121314,
        0x21222324,
        0x31323334,
        0x41424344,
        0x51525354,
        0x61626364,
        0x71727374,
        0x81828384,
        0x91929394,
    };
}

auto transferItemIndexes() -> std::array<std::uint8_t, 10>
{
    return {
        0xA0,
        0xA1,
        0xA2,
        0xA3,
        0xA4,
        0xA5,
        0xA6,
        0xA7,
        0xA8,
        0xA9,
    };
}

auto encodedTransferPacket() -> TransferPacketBytes
{
    auto packet         = GP_CLI_COMMAND_ITEM_TRANSFER{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_ITEM_TRANSFER);
    packet.header.size = sizeof(GP_CLI_COMMAND_ITEM_TRANSFER) / 4;
    packet.header.sync = 0xBEEF;
    packet.UniqueNo    = 0x11223344;

    const auto itemNums = transferItemNums();
    for (std::size_t i = 0; i < itemNums.size(); ++i)
    {
        packet.ItemNumTbl[i] = itemNums[i];
    }

    const auto itemIndexes = transferItemIndexes();
    for (std::size_t i = 0; i < itemIndexes.size(); ++i)
    {
        packet.PropertyItemIndexTbl[i] = itemIndexes[i];
    }

    packet.ActIndex     = 0x5566;
    packet.ItemNum      = 9;
    packet.padding00[0] = 0xC0;
    packet.padding00[1] = 0xC1;
    packet.padding00[2] = 0xC2;

    auto bytes = TransferPacketBytes{};
    std::memcpy(bytes.data(), &packet, bytes.size());
    return bytes;
}

auto encodedUsePacket() -> UsePacketBytes
{
    auto packet                = GP_CLI_COMMAND_ITEM_USE{};
    packet.header.id          = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_ITEM_USE);
    packet.header.size        = sizeof(GP_CLI_COMMAND_ITEM_USE) / 4;
    packet.header.sync        = 0xBEEF;
    packet.UniqueNo           = 0x11223344;
    packet.ItemNum            = 0x01020304;
    packet.ActIndex           = 0x5566;
    packet.PropertyItemIndex  = 0x77;
    packet.padding00          = 0x88;
    packet.Category           = 0xAABBCCDD;

    auto bytes = UsePacketBytes{};
    std::memcpy(bytes.data(), &packet, bytes.size());
    return bytes;
}

auto transferPureValidation(std::uint8_t itemNum) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.range("ItemNum", itemNum, 1, 9);
    return static_cast<PacketValidationResult>(validator);
}

auto itemUseValidContainers() -> std::set<CONTAINER_ID>
{
    return {
        LOC_INVENTORY,
        LOC_TEMPITEMS,
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

auto itemUsePureValidation(std::uint32_t itemNum, std::uint32_t category) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator
        .mustEqual(itemNum, 0, "ItemNum not 0")
        .oneOf("Category", static_cast<CONTAINER_ID>(category), itemUseValidContainers());
    return static_cast<PacketValidationResult>(validator);
}

auto testTransferLayoutAndMetadata() -> bool
{
    bool ok = true;
    auto packet = GP_CLI_COMMAND_ITEM_TRANSFER{};

    ok = expectEqualString(GP_CLI_COMMAND_ITEM_TRANSFER::name, "GP_CLI_COMMAND_ITEM_TRANSFER", "ITEM_TRANSFER static name") && ok;
    ok = expectTrue(GP_CLI_COMMAND_ITEM_TRANSFER::packetId == PacketC2S::GP_CLI_COMMAND_ITEM_TRANSFER, "ITEM_TRANSFER static packetId") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_HEADER), 4, "sizeof(GP_CLI_HEADER)") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_ITEM_TRANSFER), 64, "sizeof(GP_CLI_COMMAND_ITEM_TRANSFER)") && ok;
    ok = expectEqualInt(sizeof(packet.ItemNumTbl), 40, "sizeof(ITEM_TRANSFER ItemNumTbl)") && ok;
    ok = expectEqualInt(sizeof(packet.PropertyItemIndexTbl), 10, "sizeof(ITEM_TRANSFER PropertyItemIndexTbl)") && ok;
    ok = expectEqualInt(sizeof(packet.padding00), 3, "sizeof(ITEM_TRANSFER padding00)") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ITEM_TRANSFER, header), 0, "ITEM_TRANSFER header offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ITEM_TRANSFER, UniqueNo), 4, "ITEM_TRANSFER UniqueNo offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ITEM_TRANSFER, ItemNumTbl), 8, "ITEM_TRANSFER ItemNumTbl offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ITEM_TRANSFER, PropertyItemIndexTbl), 48, "ITEM_TRANSFER PropertyItemIndexTbl offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ITEM_TRANSFER, ActIndex), 58, "ITEM_TRANSFER ActIndex offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ITEM_TRANSFER, ItemNum), 60, "ITEM_TRANSFER ItemNum offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ITEM_TRANSFER, padding00), 61, "ITEM_TRANSFER padding00 offset") && ok;
    ok = expectBytes(encodedTransferPacket(),
                     TransferPacketBytes{ 0x36, 0x20, 0xEF, 0xBE,
                                          0x44, 0x33, 0x22, 0x11,
                                          0x04, 0x03, 0x02, 0x01,
                                          0x14, 0x13, 0x12, 0x11,
                                          0x24, 0x23, 0x22, 0x21,
                                          0x34, 0x33, 0x32, 0x31,
                                          0x44, 0x43, 0x42, 0x41,
                                          0x54, 0x53, 0x52, 0x51,
                                          0x64, 0x63, 0x62, 0x61,
                                          0x74, 0x73, 0x72, 0x71,
                                          0x84, 0x83, 0x82, 0x81,
                                          0x94, 0x93, 0x92, 0x91,
                                          0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9,
                                          0x66, 0x55,
                                          0x09,
                                          0xC0, 0xC1, 0xC2 },
                     "ITEM_TRANSFER encoded packet") &&
         ok;

    return ok;
}

auto testTransferPayloadStorage() -> bool
{
    auto packet     = GP_CLI_COMMAND_ITEM_TRANSFER{};
    packet.UniqueNo = 0xFFFFFFFF;

    const auto itemNums = transferItemNums();
    for (std::size_t i = 0; i < itemNums.size(); ++i)
    {
        packet.ItemNumTbl[i] = itemNums[i];
    }

    const auto itemIndexes = transferItemIndexes();
    for (std::size_t i = 0; i < itemIndexes.size(); ++i)
    {
        packet.PropertyItemIndexTbl[i] = itemIndexes[i];
    }

    packet.ActIndex     = 0xFFFF;
    packet.ItemNum      = 9;
    packet.padding00[0] = 0xAA;
    packet.padding00[1] = 0xBB;
    packet.padding00[2] = 0xCC;

    bool ok = true;
    ok      = expectEqualInt(packet.UniqueNo, 0xFFFFFFFF, "ITEM_TRANSFER UniqueNo") && ok;
    ok      = expectEqualInt(packet.ItemNumTbl[0], 0x01020304, "ITEM_TRANSFER ItemNumTbl[0]") && ok;
    ok      = expectEqualInt(packet.ItemNumTbl[9], 0x91929394, "ITEM_TRANSFER ItemNumTbl[9]") && ok;
    ok      = expectEqualInt(packet.PropertyItemIndexTbl[0], 0xA0, "ITEM_TRANSFER PropertyItemIndexTbl[0]") && ok;
    ok      = expectEqualInt(packet.PropertyItemIndexTbl[9], 0xA9, "ITEM_TRANSFER PropertyItemIndexTbl[9]") && ok;
    ok      = expectEqualInt(packet.ActIndex, 0xFFFF, "ITEM_TRANSFER ActIndex") && ok;
    ok      = expectEqualInt(packet.ItemNum, 9, "ITEM_TRANSFER ItemNum") && ok;
    ok      = expectEqualInt(packet.padding00[0], 0xAA, "ITEM_TRANSFER padding00[0]") && ok;
    ok      = expectEqualInt(packet.padding00[2], 0xCC, "ITEM_TRANSFER padding00[2]") && ok;
    return ok;
}

auto testTransferPureValidationFacts() -> bool
{
    bool ok = true;
    ok      = expectValid(transferPureValidation(1), "ITEM_TRANSFER minimum ItemNum validation") && ok;
    ok      = expectValid(transferPureValidation(9), "ITEM_TRANSFER maximum ItemNum validation") && ok;
    ok      = expectInvalidError(transferPureValidation(0), "ItemNum out of range: 0 not in [1, 9]", "ITEM_TRANSFER zero ItemNum validation") && ok;
    ok      = expectInvalidError(transferPureValidation(10), "ItemNum out of range: 10 not in [1, 9]", "ITEM_TRANSFER high ItemNum validation") && ok;
    return ok;
}

auto testUseLayoutAndMetadata() -> bool
{
    bool ok = true;

    ok = expectEqualString(GP_CLI_COMMAND_ITEM_USE::name, "GP_CLI_COMMAND_ITEM_USE", "ITEM_USE static name") && ok;
    ok = expectTrue(GP_CLI_COMMAND_ITEM_USE::packetId == PacketC2S::GP_CLI_COMMAND_ITEM_USE, "ITEM_USE static packetId") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_ITEM_USE), 20, "sizeof(GP_CLI_COMMAND_ITEM_USE)") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ITEM_USE, header), 0, "ITEM_USE header offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ITEM_USE, UniqueNo), 4, "ITEM_USE UniqueNo offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ITEM_USE, ItemNum), 8, "ITEM_USE ItemNum offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ITEM_USE, ActIndex), 12, "ITEM_USE ActIndex offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ITEM_USE, PropertyItemIndex), 14, "ITEM_USE PropertyItemIndex offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ITEM_USE, padding00), 15, "ITEM_USE padding00 offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ITEM_USE, Category), 16, "ITEM_USE Category offset") && ok;
    ok = expectBytes(encodedUsePacket(),
                     UsePacketBytes{ 0x37, 0x0A, 0xEF, 0xBE,
                                     0x44, 0x33, 0x22, 0x11,
                                     0x04, 0x03, 0x02, 0x01,
                                     0x66, 0x55,
                                     0x77,
                                     0x88,
                                     0xDD, 0xCC, 0xBB, 0xAA },
                     "ITEM_USE encoded packet") &&
         ok;

    return ok;
}

auto testUsePayloadStorage() -> bool
{
    auto packet               = GP_CLI_COMMAND_ITEM_USE{};
    packet.UniqueNo          = 0xFFFFFFFF;
    packet.ItemNum           = 0x01020304;
    packet.ActIndex          = 0x5566;
    packet.PropertyItemIndex = 0x77;
    packet.padding00         = 0x88;
    packet.Category          = 0xAABBCCDD;

    bool ok = true;
    ok      = expectEqualInt(packet.UniqueNo, 0xFFFFFFFF, "ITEM_USE UniqueNo") && ok;
    ok      = expectEqualInt(packet.ItemNum, 0x01020304, "ITEM_USE ItemNum") && ok;
    ok      = expectEqualInt(packet.ActIndex, 0x5566, "ITEM_USE ActIndex") && ok;
    ok      = expectEqualInt(packet.PropertyItemIndex, 0x77, "ITEM_USE PropertyItemIndex") && ok;
    ok      = expectEqualInt(packet.padding00, 0x88, "ITEM_USE padding00") && ok;
    ok      = expectEqualInt(packet.Category, 0xAABBCCDD, "ITEM_USE Category") && ok;
    return ok;
}

auto testUsePureValidationFacts() -> bool
{
    bool ok = true;
    ok      = expectValid(itemUsePureValidation(0, LOC_INVENTORY), "ITEM_USE inventory validation") && ok;
    ok      = expectValid(itemUsePureValidation(0, LOC_TEMPITEMS), "ITEM_USE temp items validation") && ok;
    ok      = expectValid(itemUsePureValidation(0, LOC_WARDROBE8), "ITEM_USE wardrobe8 validation") && ok;
    ok      = expectValid(itemUsePureValidation(0, 0x00000110), "ITEM_USE category CONTAINER_ID cast validation") && ok;
    ok      = expectInvalidError(itemUsePureValidation(1, LOC_INVENTORY), "ItemNum not 0", "ITEM_USE ItemNum validation") && ok;
    ok      = expectInvalidError(itemUsePureValidation(0, LOC_RECYCLEBIN), "Category value 17 is not allowed.", "ITEM_USE recycle bin validation") && ok;
    ok      = expectInvalidError(itemUsePureValidation(0, MAX_CONTAINER_ID), "Category value 18 is not allowed.", "ITEM_USE unknown container validation") && ok;
    return ok;
}

} // namespace

auto runC2SItemActionPacketSelfTests() -> bool
{
    bool ok = true;

    ok = testTransferLayoutAndMetadata() && ok;
    ok = testTransferPayloadStorage() && ok;
    ok = testTransferPureValidationFacts() && ok;
    ok = testUseLayoutAndMetadata() && ok;
    ok = testUsePayloadStorage() && ok;
    ok = testUsePureValidationFacts() && ok;

    return ok;
}
