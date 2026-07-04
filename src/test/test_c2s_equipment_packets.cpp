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

#include "test_c2s_equipment_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "map/entities/battle_entity.h"
#include "map/packets/c2s/0x050_equip_set.h"
#include "map/packets/c2s/0x051_equipset_set.h"
#include "map/packets/c2s/0x052_equipset_check.h"

namespace
{

constexpr auto equipSetPropertyItemIndexOffset = 4U;
constexpr auto equipSetEquipKindOffset         = 5U;
constexpr auto equipSetCategoryOffset          = 6U;

constexpr auto equipSetSetCountOffset       = 4U;
constexpr auto equipSetSetPadding00Offset   = 5U;
constexpr auto equipSetSetEquipmentOffset   = 8U;
constexpr auto equipSetSetEquipmentCount    = 16U;
constexpr auto equipSetSetItemSize          = 4U;
constexpr auto equipSetSetItemIndexOffset   = 0U;
constexpr auto equipSetSetEquipKindOffset   = 1U;
constexpr auto equipSetSetCategoryOffset    = 2U;
constexpr auto equipSetSetItemPaddingOffset = 3U;

constexpr auto equipSetCheckEquipKindOffset          = 4U;
constexpr auto equipSetCheckPadding00Offset          = 5U;
constexpr auto equipSetCheckItemChangeOffset         = 8U;
constexpr auto equipSetCheckEquipmentOffset          = 12U;
constexpr auto equipSetCheckEquipmentCount           = 16U;
constexpr auto equipSetCheckRequestItemSize          = 4U;
constexpr auto equipSetCheckRequestItemFlagsOffset   = 0U;
constexpr auto equipSetCheckRequestItemIndexOffset   = 1U;
constexpr auto equipSetCheckRequestItemItemNoOffset  = 2U;

using EquipSetBytes      = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_EQUIP_SET)>;
using EquipSetSetBytes   = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_EQUIPSET_SET)>;
using EquipSetCheckBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_EQUIPSET_CHECK)>;

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s equipment packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s equipment packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectFalse(bool actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "c2s equipment packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s equipment packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(const std::array<std::uint8_t, N>& actual, const std::array<std::uint8_t, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s equipment packet self-test failed: " << label << " got";
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

auto equipSetSetItemOffset(std::size_t index) -> std::size_t
{
    return equipSetSetEquipmentOffset + index * equipSetSetItemSize;
}

auto equipSetCheckEquipmentItemOffset(std::size_t index) -> std::size_t
{
    return equipSetCheckEquipmentOffset + index * equipSetCheckRequestItemSize;
}

auto requestItemFlags(const equipsetrequestitem_t& item) -> std::uint8_t
{
    auto bytes = std::array<std::uint8_t, sizeof(equipsetrequestitem_t)>{};
    std::memcpy(bytes.data(), &item, bytes.size());
    return bytes[0];
}

auto makeEquipSetPacket() -> GP_CLI_COMMAND_EQUIP_SET
{
    auto packet               = GP_CLI_COMMAND_EQUIP_SET{};
    packet.header.id         = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_EQUIP_SET);
    packet.header.size       = sizeof(packet) / 4U;
    packet.header.sync       = 0xBEEF;
    packet.PropertyItemIndex = 0x12;
    packet.EquipKind         = SLOT_HANDS;
    packet.Category          = 0x0B;
    return packet;
}

auto makeEquipSetSetPacket() -> GP_CLI_COMMAND_EQUIPSET_SET
{
    auto packet         = GP_CLI_COMMAND_EQUIPSET_SET{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_EQUIPSET_SET);
    packet.header.size = sizeof(packet) / 4U;
    packet.header.sync = 0xBEEF;
    packet.Count       = 3;
    packet.padding00[0] = 0xA0;
    packet.padding00[1] = 0xA1;
    packet.padding00[2] = 0xA2;

    for (std::size_t i = 0; i < equipSetSetEquipmentCount; ++i)
    {
        auto& item     = packet.Equipment[i];
        item.ItemIndex = static_cast<std::uint8_t>(i + 1U);
        item.EquipKind = static_cast<std::uint8_t>(i % MAX_SLOTTYPE);
        item.Category  = static_cast<std::uint8_t>(0x20U + i);
        item.padding00 = static_cast<std::uint8_t>(0x40U + i);
    }

    return packet;
}

auto makeEquipSetCheckPacket() -> GP_CLI_COMMAND_EQUIPSET_CHECK
{
    auto packet         = GP_CLI_COMMAND_EQUIPSET_CHECK{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_EQUIPSET_CHECK);
    packet.header.size = sizeof(packet) / 4U;
    packet.header.sync = 0xBEEF;
    packet.EquipKind   = 0x05;
    packet.padding00[0] = 0xB0;
    packet.padding00[1] = 0xB1;
    packet.padding00[2] = 0xB2;

    packet.ItemChange.HasItemFlg    = 1;
    packet.ItemChange.RemoveItemFlg = 0;
    packet.ItemChange.Category      = 0x03;
    packet.ItemChange.ItemIndex     = 0x44;
    packet.ItemChange.ItemNo        = 0x1234;

    for (std::size_t i = 0; i < equipSetCheckEquipmentCount; ++i)
    {
        auto& item           = packet.Equipment[i];
        item.HasItemFlg      = static_cast<std::uint8_t>(i % 2U);
        item.RemoveItemFlg   = static_cast<std::uint8_t>((i + 1U) % 2U);
        item.Category        = static_cast<std::uint8_t>(i & 0x3FU);
        item.ItemIndex       = static_cast<std::uint8_t>(0x50U + i);
        item.ItemNo          = static_cast<std::uint16_t>(0x2000U + i);
    }

    return packet;
}

auto makeExpectedEquipSetSetBytes() -> EquipSetSetBytes
{
    auto bytes = EquipSetSetBytes{};
    bytes[0] = 0x51;
    bytes[1] = 0x24;
    bytes[2] = 0xEF;
    bytes[3] = 0xBE;
    bytes[equipSetSetCountOffset] = 3;
    bytes[equipSetSetPadding00Offset] = 0xA0;
    bytes[equipSetSetPadding00Offset + 1U] = 0xA1;
    bytes[equipSetSetPadding00Offset + 2U] = 0xA2;

    for (std::size_t i = 0; i < equipSetSetEquipmentCount; ++i)
    {
        const auto offset = equipSetSetItemOffset(i);
        bytes[offset + equipSetSetItemIndexOffset] = static_cast<std::uint8_t>(i + 1U);
        bytes[offset + equipSetSetEquipKindOffset] = static_cast<std::uint8_t>(i % MAX_SLOTTYPE);
        bytes[offset + equipSetSetCategoryOffset] = static_cast<std::uint8_t>(0x20U + i);
        bytes[offset + equipSetSetItemPaddingOffset] = static_cast<std::uint8_t>(0x40U + i);
    }

    return bytes;
}

auto makeExpectedEquipSetCheckBytes() -> EquipSetCheckBytes
{
    auto bytes = EquipSetCheckBytes{};
    bytes[0] = 0x52;
    bytes[1] = 0x26;
    bytes[2] = 0xEF;
    bytes[3] = 0xBE;
    bytes[equipSetCheckEquipKindOffset] = 0x05;
    bytes[equipSetCheckPadding00Offset] = 0xB0;
    bytes[equipSetCheckPadding00Offset + 1U] = 0xB1;
    bytes[equipSetCheckPadding00Offset + 2U] = 0xB2;

    bytes[equipSetCheckItemChangeOffset] = 0x0D;
    bytes[equipSetCheckItemChangeOffset + equipSetCheckRequestItemIndexOffset] = 0x44;
    bytes[equipSetCheckItemChangeOffset + equipSetCheckRequestItemItemNoOffset] = 0x34;
    bytes[equipSetCheckItemChangeOffset + equipSetCheckRequestItemItemNoOffset + 1U] = 0x12;

    for (std::size_t i = 0; i < equipSetCheckEquipmentCount; ++i)
    {
        const auto offset = equipSetCheckEquipmentItemOffset(i);
        const auto flags  = static_cast<std::uint8_t>((i % 2U) | (((i + 1U) % 2U) << 1U) | ((i & 0x3FU) << 2U));
        bytes[offset + equipSetCheckRequestItemFlagsOffset] = flags;
        bytes[offset + equipSetCheckRequestItemIndexOffset] = static_cast<std::uint8_t>(0x50U + i);
        const auto itemNo = static_cast<std::uint16_t>(0x2000U + i);
        bytes[offset + equipSetCheckRequestItemItemNoOffset] = static_cast<std::uint8_t>(itemNo & 0xFFU);
        bytes[offset + equipSetCheckRequestItemItemNoOffset + 1U] = static_cast<std::uint8_t>(itemNo >> 8U);
    }

    return bytes;
}

auto validateEquipSetPure(std::uint8_t equipKind) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.oneOf<SLOTTYPE>(equipKind);
    return validator;
}

auto validateEquipSetSetPure(std::uint8_t count, const std::array<std::uint8_t, equipSetSetEquipmentCount>& equipKinds) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.range("Count", count, 1, 16);
    if (count <= 16)
    {
        for (std::uint8_t i = 0; i < count; ++i)
        {
            validator.oneOf<SLOTTYPE>(equipKinds[i]);
        }
    }
    return validator;
}

auto validateEquipSetCheckPure() -> PacketValidationResult
{
    return PacketValidator(nullptr);
}

auto testEquipmentLayoutsAndMetadata() -> bool
{
    bool ok = true;

    const auto equipSetSetPacket   = GP_CLI_COMMAND_EQUIPSET_SET{};
    const auto equipSetCheckPacket = GP_CLI_COMMAND_EQUIPSET_CHECK{};
    const auto equipSetSetItem     = equipsetitem_t{};
    const auto requestItem         = equipsetrequestitem_t{};

    ok = expectEqualString(GP_CLI_COMMAND_EQUIP_SET::name, "GP_CLI_COMMAND_EQUIP_SET", "EQUIP_SET name") && ok;
    ok = expectEqualString(GP_CLI_COMMAND_EQUIPSET_SET::name, "GP_CLI_COMMAND_EQUIPSET_SET", "EQUIPSET_SET name") && ok;
    ok = expectEqualString(GP_CLI_COMMAND_EQUIPSET_CHECK::name, "GP_CLI_COMMAND_EQUIPSET_CHECK", "EQUIPSET_CHECK name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_EQUIP_SET::packetId), 0x050, "EQUIP_SET packet id") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_EQUIPSET_SET::packetId), 0x051, "EQUIPSET_SET packet id") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_EQUIPSET_CHECK::packetId), 0x052, "EQUIPSET_CHECK packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_HEADER), 4, "client header size") && ok;

    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_EQUIP_SET), 8, "EQUIP_SET sizeof") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_EQUIP_SET, PropertyItemIndex), equipSetPropertyItemIndexOffset, "PropertyItemIndex offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_EQUIP_SET, EquipKind), equipSetEquipKindOffset, "EquipKind offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_EQUIP_SET, Category), equipSetCategoryOffset, "Category offset") && ok;

    ok = expectEqualInt(sizeof(equipsetitem_t), equipSetSetItemSize, "equipsetitem_t sizeof") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_EQUIPSET_SET), 72, "EQUIPSET_SET sizeof") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_EQUIPSET_SET, Count), equipSetSetCountOffset, "Count offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_EQUIPSET_SET, padding00), equipSetSetPadding00Offset, "EQUIPSET_SET padding00 offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_EQUIPSET_SET, Equipment), equipSetSetEquipmentOffset, "Equipment offset") && ok;
    ok = expectEqualInt(sizeof(equipSetSetPacket.Equipment) / sizeof(equipSetSetPacket.Equipment[0]), equipSetSetEquipmentCount, "Equipment count") && ok;
    ok = expectEqualInt(offsetof(equipsetitem_t, ItemIndex), equipSetSetItemIndexOffset, "equipsetitem_t ItemIndex offset") && ok;
    ok = expectEqualInt(offsetof(equipsetitem_t, EquipKind), equipSetSetEquipKindOffset, "equipsetitem_t EquipKind offset") && ok;
    ok = expectEqualInt(offsetof(equipsetitem_t, Category), equipSetSetCategoryOffset, "equipsetitem_t Category offset") && ok;
    ok = expectEqualInt(offsetof(equipsetitem_t, padding00), equipSetSetItemPaddingOffset, "equipsetitem_t padding00 offset") && ok;
    ok = expectEqualInt(sizeof(equipSetSetItem.ItemIndex), 1, "equipsetitem_t ItemIndex size") && ok;
    ok = expectEqualInt(equipSetSetItemOffset(15), 68, "EQUIPSET_SET last item offset") && ok;

    ok = expectEqualInt(sizeof(equipsetrequestitem_t), equipSetCheckRequestItemSize, "equipsetrequestitem_t sizeof") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_EQUIPSET_CHECK), 76, "EQUIPSET_CHECK sizeof") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_EQUIPSET_CHECK, EquipKind), equipSetCheckEquipKindOffset, "EQUIPSET_CHECK EquipKind offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_EQUIPSET_CHECK, padding00), equipSetCheckPadding00Offset, "EQUIPSET_CHECK padding00 offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_EQUIPSET_CHECK, ItemChange), equipSetCheckItemChangeOffset, "ItemChange offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_EQUIPSET_CHECK, Equipment), equipSetCheckEquipmentOffset, "EQUIPSET_CHECK Equipment offset") && ok;
    ok = expectEqualInt(sizeof(equipSetCheckPacket.Equipment) / sizeof(equipSetCheckPacket.Equipment[0]), equipSetCheckEquipmentCount, "EQUIPSET_CHECK Equipment count") && ok;
    ok = expectEqualInt(offsetof(equipsetrequestitem_t, ItemIndex), equipSetCheckRequestItemIndexOffset, "equipsetrequestitem_t ItemIndex offset") && ok;
    ok = expectEqualInt(offsetof(equipsetrequestitem_t, ItemNo), equipSetCheckRequestItemItemNoOffset, "equipsetrequestitem_t ItemNo offset") && ok;
    ok = expectEqualInt(sizeof(requestItem.ItemIndex), 1, "equipsetrequestitem_t ItemIndex size") && ok;
    ok = expectEqualInt(sizeof(requestItem.ItemNo), 2, "equipsetrequestitem_t ItemNo size") && ok;
    ok = expectEqualInt(equipSetCheckEquipmentItemOffset(15), 72, "EQUIPSET_CHECK last item offset") && ok;

    ok = expectEqualInt(MAX_SLOTTYPE, 18, "MAX_SLOTTYPE") && ok;
    ok = expectEqualInt(SLOT_MAIN, 0, "SLOT_MAIN") && ok;
    ok = expectEqualInt(SLOT_FEET, 8, "SLOT_FEET") && ok;
    ok = expectEqualInt(SLOT_LINK2, 17, "SLOT_LINK2") && ok;

    return ok;
}

auto testEquipmentEncodedBytes() -> bool
{
    bool ok = true;

    ok = expectBytes(encodedPacketBytes(makeEquipSetPacket()),
                     EquipSetBytes{ 0x50, 0x04, 0xEF, 0xBE, 0x12, 0x06, 0x0B, 0x00 },
                     "EQUIP_SET bytes") &&
         ok;
    ok = expectBytes(encodedPacketBytes(makeEquipSetSetPacket()), makeExpectedEquipSetSetBytes(), "EQUIPSET_SET bytes") && ok;
    ok = expectBytes(encodedPacketBytes(makeEquipSetCheckPacket()), makeExpectedEquipSetCheckBytes(), "EQUIPSET_CHECK bytes") && ok;

    auto item = equipsetrequestitem_t{};
    item.HasItemFlg = 1;
    item.RemoveItemFlg = 1;
    item.Category = 0x3F;
    ok = expectEqualInt(requestItemFlags(item), 0xFF, "equipsetrequestitem_t all bitfield flags") && ok;
    item = equipsetrequestitem_t{};
    item.Category = 0x15;
    ok = expectEqualInt(requestItemFlags(item), 0x54, "equipsetrequestitem_t category bitfield flags") && ok;

    return ok;
}

auto testEquipmentValidation() -> bool
{
    bool ok = true;

    auto equipKinds = std::array<std::uint8_t, equipSetSetEquipmentCount>{};
    equipKinds[0] = SLOT_MAIN;
    equipKinds[1] = SLOT_FEET;
    equipKinds[2] = SLOT_LINK2;

    ok = expectValid(validateEquipSetPure(SLOT_MAIN), "EQUIP_SET main slot") && ok;
    ok = expectValid(validateEquipSetPure(SLOT_LINK2), "EQUIP_SET link2 slot") && ok;
    ok = expectInvalidError(validateEquipSetPure(MAX_SLOTTYPE), "18 not a valid SLOTTYPE value.", "EQUIP_SET invalid slot") && ok;
    ok = expectValid(validateEquipSetSetPure(3, equipKinds), "EQUIPSET_SET valid slots") && ok;
    ok = expectInvalidError(validateEquipSetSetPure(0, equipKinds), "Count out of range: 0 not in [1, 16]", "EQUIPSET_SET count zero") && ok;
    ok = expectInvalidError(validateEquipSetSetPure(17, equipKinds), "Count out of range: 17 not in [1, 16]", "EQUIPSET_SET count above max") && ok;
    equipKinds[1] = MAX_SLOTTYPE;
    ok = expectInvalidError(validateEquipSetSetPure(3, equipKinds), "18 not a valid SLOTTYPE value.", "EQUIPSET_SET invalid slot") && ok;
    ok = expectValid(validateEquipSetCheckPure(), "EQUIPSET_CHECK live-only validation") && ok;

    return ok;
}

} // namespace

auto runC2SEquipmentPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testEquipmentLayoutsAndMetadata() && ok;
    ok      = testEquipmentEncodedBytes() && ok;
    ok      = testEquipmentValidation() && ok;
    return ok;
}
