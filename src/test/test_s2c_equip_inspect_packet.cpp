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

#include "test_s2c_equip_inspect_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>

#include "entities/char_entity.h"
#include "items/exdata/augment_standard.h"
#include "items/item_equipment.h"
#include "map/packets/s2c/0x0c9_equip_inspect_equipment.h"
#include "map/packets/s2c/0x0c9_equip_inspect_general.h"
#include "omega_self_test_registry.h"

namespace
{

using GeneralPacket   = GP_SERV_COMMAND_EQUIP_INSPECT::GENERAL;
using EquipmentPacket = GP_SERV_COMMAND_EQUIP_INSPECT::EQUIPMENT;

constexpr auto generalUniqNoOffset            = sizeof(GP_SERV_HEADER) + offsetof(GeneralPacket::PacketData, UniqNo);
constexpr auto generalActIndexOffset           = sizeof(GP_SERV_HEADER) + offsetof(GeneralPacket::PacketData, ActIndex);
constexpr auto generalOptionFlagOffset          = sizeof(GP_SERV_HEADER) + offsetof(GeneralPacket::PacketData, OptionFlag);
constexpr auto generalPadding0BOffset          = sizeof(GP_SERV_HEADER) + offsetof(GeneralPacket::PacketData, padding0B);
constexpr auto generalItemNoOffset             = sizeof(GP_SERV_HEADER) + offsetof(GeneralPacket::PacketData, ItemNo);
constexpr auto generalSComLinkNameOffset       = sizeof(GP_SERV_HEADER) + offsetof(GeneralPacket::PacketData, sComLinkName);
constexpr auto generalSComColorOffset          = sizeof(GP_SERV_HEADER) + offsetof(GeneralPacket::PacketData, sComColor);
constexpr auto generalJobOffset               = sizeof(GP_SERV_HEADER) + offsetof(GeneralPacket::PacketData, job);
constexpr auto generalLvlOffset               = sizeof(GP_SERV_HEADER) + offsetof(GeneralPacket::PacketData, lvl);
constexpr auto generalMJobOffset              = sizeof(GP_SERV_HEADER) + offsetof(GeneralPacket::PacketData, mjob);
constexpr auto generalMLvlOffset              = sizeof(GP_SERV_HEADER) + offsetof(GeneralPacket::PacketData, mlvl);
constexpr auto generalMFlagsOffset            = sizeof(GP_SERV_HEADER) + offsetof(GeneralPacket::PacketData, mflags);
constexpr auto generalPadding29Offset         = sizeof(GP_SERV_HEADER) + offsetof(GeneralPacket::PacketData, padding29);
constexpr auto generalBallistaChevronCountOffset = sizeof(GP_SERV_HEADER) + offsetof(GeneralPacket::PacketData, BallistaChevronCount);
constexpr auto generalBallistaChevronFlagsOffset = sizeof(GP_SERV_HEADER) + offsetof(GeneralPacket::PacketData, BallistaChevronFlags);
constexpr auto generalPadding31Offset          = sizeof(GP_SERV_HEADER) + offsetof(GeneralPacket::PacketData, padding31);
constexpr auto generalBallistaFlagsOffset      = sizeof(GP_SERV_HEADER) + offsetof(GeneralPacket::PacketData, BallistaFlags);
constexpr auto generalMesNoOffset              = sizeof(GP_SERV_HEADER) + offsetof(GeneralPacket::PacketData, MesNo);
constexpr auto generalParamsOffset            = sizeof(GP_SERV_HEADER) + offsetof(GeneralPacket::PacketData, Params);
constexpr auto generalPadding4COffset          = sizeof(GP_SERV_HEADER) + offsetof(GeneralPacket::PacketData, padding4C);
constexpr auto generalPacketDataSize          = sizeof(GeneralPacket::PacketData);
constexpr auto generalPacketSize              = sizeof(GP_SERV_HEADER) + generalPacketDataSize;
constexpr auto generalSComLinkNameSize        = sizeof(GeneralPacket::PacketData::sComLinkName);
constexpr auto generalParamsCount             = sizeof(GeneralPacket::PacketData::Params) / sizeof(int32_t);
constexpr auto generalPadding4CSize           = sizeof(GeneralPacket::PacketData::padding4C);

constexpr auto equipUniqNoOffset   = sizeof(GP_SERV_HEADER) + offsetof(EquipmentPacket::PacketData, UniqNo);
constexpr auto equipActIndexOffset  = sizeof(GP_SERV_HEADER) + offsetof(EquipmentPacket::PacketData, ActIndex);
constexpr auto equipOptionFlagOffset = sizeof(GP_SERV_HEADER) + offsetof(EquipmentPacket::PacketData, OptionFlag);
constexpr auto equipCountOffset    = sizeof(GP_SERV_HEADER) + offsetof(EquipmentPacket::PacketData, EquipCount);
constexpr auto equipArrayOffset    = sizeof(GP_SERV_HEADER) + offsetof(EquipmentPacket::PacketData, Equip);
constexpr auto equipMaxItems       = sizeof(EquipmentPacket::PacketData::Equip) / sizeof(checkitem_t);
constexpr auto checkItemSize       = sizeof(checkitem_t);
constexpr auto checkItemDataSize   = sizeof(checkitem_t::Data);
constexpr auto equipHeaderSize     = offsetof(EquipmentPacket::PacketData, Equip);
constexpr auto equipMaxPacketSize  = sizeof(GP_SERV_HEADER) + sizeof(EquipmentPacket::PacketData);

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c EQUIP_INSPECT packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c EQUIP_INSPECT packet self-test failed: " << label << " got";
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
            std::cerr << "s2c EQUIP_INSPECT packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto expectCheckItem(CBasicPacket& packet, std::size_t index, std::uint16_t itemNo, std::uint8_t equipKind, std::uint8_t signatureSeed, const std::string& label) -> bool
{
    const auto itemOffset = equipArrayOffset + index * checkItemSize;

    bool ok = true;
    ok      = expectBytes(packet, itemOffset, std::array<uint8, 2>{ static_cast<uint8>(itemNo), static_cast<uint8>(itemNo >> 8) }, label + " ItemNo") && ok;
    ok      = expectBytes(packet, itemOffset + 2, std::array<uint8, 2>{ equipKind, 0x00 }, label + " EquipKind and padding") && ok;
    ok      = expectZeroRange(packet, itemOffset + 4, itemOffset + 16, label + " pre-signature data") && ok;

    std::array<uint8, 12> signature{};
    for (std::size_t i = 0; i < signature.size(); ++i)
    {
        signature[i] = static_cast<uint8>(signatureSeed + i);
    }
    ok = expectBytes(packet, itemOffset + 16, signature, label + " signature") && ok;
    return ok;
}

void populateEntity(CCharEntity& entity, std::uint32_t id, std::uint16_t targid)
{
    entity.id      = id;
    entity.targid  = targid;
}

auto testLayoutAndEnums() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(static_cast<uint16>(PacketS2C::GP_SERV_COMMAND_EQUIP_INSPECT), 0x0C9, "EQUIP_INSPECT packet id") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;

    ok = expectEqualUInt(generalPacketDataSize, 80, "GENERAL PacketData size") && ok;
    ok = expectEqualUInt(generalPacketSize, 84, "GENERAL packet size") && ok;
    ok = expectEqualUInt(generalSComLinkNameSize, 16, "GENERAL sComLinkName size") && ok;
    ok = expectEqualUInt(generalParamsCount, 5, "GENERAL Params count") && ok;
    ok = expectEqualUInt(generalPadding4CSize, 8, "GENERAL padding4C size") && ok;

    ok = expectEqualUInt(generalUniqNoOffset, 4, "GENERAL UniqNo offset") && ok;
    ok = expectEqualUInt(generalActIndexOffset, 8, "GENERAL ActIndex offset") && ok;
    ok = expectEqualUInt(generalOptionFlagOffset, 10, "GENERAL OptionFlag offset") && ok;
    ok = expectEqualUInt(generalPadding0BOffset, 11, "GENERAL padding0B offset") && ok;
    ok = expectEqualUInt(generalItemNoOffset, 14, "GENERAL ItemNo offset") && ok;
    ok = expectEqualUInt(generalSComLinkNameOffset, 16, "GENERAL sComLinkName offset") && ok;
    ok = expectEqualUInt(generalSComColorOffset, 32, "GENERAL sComColor offset") && ok;
    ok = expectEqualUInt(generalJobOffset, 34, "GENERAL job offset") && ok;
    ok = expectEqualUInt(generalLvlOffset, 36, "GENERAL lvl offset") && ok;
    ok = expectEqualUInt(generalMJobOffset, 38, "GENERAL mjob offset") && ok;
    ok = expectEqualUInt(generalMLvlOffset, 39, "GENERAL mlvl offset") && ok;
    ok = expectEqualUInt(generalMFlagsOffset, 40, "GENERAL mflags offset") && ok;
    ok = expectEqualUInt(generalPadding29Offset, 41, "GENERAL padding29 offset") && ok;
    ok = expectEqualUInt(generalBallistaChevronCountOffset, 44, "GENERAL BallistaChevronCount offset") && ok;
    ok = expectEqualUInt(generalBallistaChevronFlagsOffset, 48, "GENERAL BallistaChevronFlags offset") && ok;
    ok = expectEqualUInt(generalPadding31Offset, 49, "GENERAL padding31 offset") && ok;
    ok = expectEqualUInt(generalBallistaFlagsOffset, 50, "GENERAL BallistaFlags offset") && ok;
    ok = expectEqualUInt(generalMesNoOffset, 52, "GENERAL MesNo offset") && ok;
    ok = expectEqualUInt(generalParamsOffset, 56, "GENERAL Params offset") && ok;
    ok = expectEqualUInt(generalPadding4COffset, 76, "GENERAL padding4C offset") && ok;

    ok = expectEqualUInt(checkItemSize, 28, "checkitem_t size") && ok;
    ok = expectEqualUInt(checkItemDataSize, 24, "checkitem_t Data size") && ok;
    ok = expectEqualUInt(equipMaxItems, 8, "EQUIPMENT max items") && ok;
    ok = expectEqualUInt(equipHeaderSize, 8, "EQUIPMENT header size") && ok;
    ok = expectEqualUInt(equipMaxPacketSize, 236, "EQUIPMENT max packet size") && ok;
    ok = expectEqualUInt(equipUniqNoOffset, 4, "EQUIPMENT UniqNo offset") && ok;
    ok = expectEqualUInt(equipActIndexOffset, 8, "EQUIPMENT ActIndex offset") && ok;
    ok = expectEqualUInt(equipOptionFlagOffset, 10, "EQUIPMENT OptionFlag offset") && ok;
    ok = expectEqualUInt(equipCountOffset, 11, "EQUIPMENT EquipCount offset") && ok;
    ok = expectEqualUInt(equipArrayOffset, 12, "EQUIPMENT Equip array offset") && ok;

    ok = expectEqualUInt(static_cast<uint8>(SAVE_EQUIP_KIND::SAVE_EQUIP_KIND_RIGHTHAND), 0, "SAVE_EQUIP_KIND::RIGHTHAND") && ok;
    ok = expectEqualUInt(static_cast<uint8>(SAVE_EQUIP_KIND::SAVE_EQUIP_KIND_BACKPACK), 15, "SAVE_EQUIP_KIND::BACKPACK") && ok;
    return ok;
}

auto testGeneralConstructorBytes() -> bool
{
    auto target = CCharEntity{};
    populateEntity(target, 0x11223344, 0x5566);
    auto checker = CCharEntity{};

    auto packet = GeneralPacket(&checker, &target);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x0C9, "GENERAL type") && ok;
    ok      = expectEqualUInt(packet.getSize(), generalPacketSize, "GENERAL size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0xC9, 0x2A, 0xEF, 0xBE }, "GENERAL header") && ok;
    ok      = expectBytes(packet, generalUniqNoOffset, std::array<uint8, 4>{ 0x44, 0x33, 0x22, 0x11 }, "GENERAL UniqNo") && ok;
    ok      = expectBytes(packet, generalActIndexOffset, std::array<uint8, 2>{ 0x66, 0x55 }, "GENERAL ActIndex") && ok;
    ok      = expectBytes(packet, generalOptionFlagOffset, std::array<uint8, 1>{ 0x01 }, "GENERAL OptionFlag") && ok;
    ok      = expectZeroRange(packet, generalPadding0BOffset, generalItemNoOffset, "GENERAL padding0B") && ok;
    ok      = expectZeroRange(packet, generalItemNoOffset, generalJobOffset, "GENERAL linkshell fields") && ok;
    ok      = expectBytes(packet, generalJobOffset, std::array<uint8, 2>{ 0x01, 0x01 }, "GENERAL jobs") && ok;
    ok      = expectBytes(packet, generalLvlOffset, std::array<uint8, 2>{ 0x00, 0x00 }, "GENERAL levels") && ok;
    ok      = expectBytes(packet, generalMJobOffset, std::array<uint8, 2>{ 0x01, 0x00 }, "GENERAL master job and level") && ok;
    ok      = expectZeroRange(packet, generalMFlagsOffset, generalPacketSize, "GENERAL master flags, ballista, and padding") && ok;
    return ok;
}

auto testGeneralConstructorAnonVisibility() -> bool
{
    auto target = CCharEntity{};
    populateEntity(target, 0x01020304, 0x0506);
    target.playerConfig.AnonymityFlg = true;
    target.SetMJob(8);
    target.SetSJob(9);

    auto normalChecker = CCharEntity{};
    auto hiddenPacket  = GeneralPacket(&normalChecker, &target);

    auto gmChecker           = CCharEntity{};
    gmChecker.visibleGmLevel = 3;
    auto visiblePacket       = GeneralPacket(&gmChecker, &target);

    bool ok = true;
    ok      = expectZeroRange(hiddenPacket, generalJobOffset, generalMFlagsOffset, "GENERAL anon hidden jobs") && ok;
    ok      = expectBytes(visiblePacket, generalJobOffset, std::array<uint8, 2>{ 0x08, 0x09 }, "GENERAL GM-visible anon jobs") && ok;
    ok      = expectBytes(visiblePacket, generalLvlOffset, std::array<uint8, 2>{ 0x00, 0x00 }, "GENERAL GM-visible anon levels") && ok;
    ok      = expectBytes(visiblePacket, generalMJobOffset, std::array<uint8, 2>{ 0x08, 0x00 }, "GENERAL GM-visible anon master job and level") && ok;
    return ok;
}

auto testEquipmentConstructorBytes() -> bool
{
    auto target = CCharEntity{};
    populateEntity(target, 0x01020304, 0x0708);
    auto checker = CCharEntity{};

    auto packet = EquipmentPacket(&checker, &target);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x0C9, "EQUIPMENT type") && ok;
    ok      = expectEqualUInt(packet.getSize(), sizeof(GP_SERV_HEADER) + equipHeaderSize + checkItemSize, "EQUIPMENT empty size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0xC9, 0x14, 0xEF, 0xBE }, "EQUIPMENT header") && ok;
    ok      = expectBytes(packet, equipUniqNoOffset, std::array<uint8, 4>{ 0x04, 0x03, 0x02, 0x01 }, "EQUIPMENT UniqNo") && ok;
    ok      = expectBytes(packet, equipActIndexOffset, std::array<uint8, 2>{ 0x08, 0x07 }, "EQUIPMENT ActIndex") && ok;
    ok      = expectBytes(packet, equipOptionFlagOffset, std::array<uint8, 1>{ 0x03 }, "EQUIPMENT OptionFlag") && ok;
    ok      = expectBytes(packet, equipCountOffset, std::array<uint8, 1>{ 0x00 }, "EQUIPMENT EquipCount") && ok;
    // No equipped items: the item region is zeroed.
    ok      = expectZeroRange(packet, equipArrayOffset, packet.getSize(), "EQUIPMENT zeroed item region") && ok;
    return ok;
}

auto testEquipmentConstructorChunksEquippedItems() -> bool
{
    std::array<std::unique_ptr<CItemEquipment>, 8> items{};
    auto                                          target = CCharEntity{};
    populateEntity(target, 0x11223344, 0x5566);

    for (std::size_t slot = 0; slot < items.size(); ++slot)
    {
        items[slot] = std::make_unique<CItemEquipment>(static_cast<uint16>(0x2000 + slot));

        auto& signature = items[slot]->exdata<Exdata::AugmentStandard>().Signature;
        for (std::size_t i = 0; i < sizeof(signature); ++i)
        {
            signature[i] = static_cast<uint8>(0xA0 + slot + i);
        }

        if (!target.bindEquip(static_cast<uint8>(slot), items[slot].get()))
        {
            std::cerr << "s2c EQUIP_INSPECT packet self-test failed: bindEquip slot " << slot << '\n';
            return false;
        }
    }

    auto checker = CCharEntity{};
    auto packet  = EquipmentPacket(&checker, &target);
    packet.setSequence(0xBEEF);

    const auto& pushedPackets = checker.getPacketList();
    bool        ok            = true;
    ok                        = expectEqualUInt(pushedPackets.size(), 1, "EQUIPMENT pushed full packet count") && ok;
    if (!pushedPackets.empty())
    {
        auto& firstPacket = *pushedPackets.front();
        ok               = expectEqualUInt(firstPacket.getType(), 0x0C9, "EQUIPMENT full type") && ok;
        ok               = expectEqualUInt(firstPacket.getSize(), equipMaxPacketSize, "EQUIPMENT full size") && ok;
        ok               = expectBytes(firstPacket, equipUniqNoOffset, std::array<uint8, 4>{ 0x44, 0x33, 0x22, 0x11 }, "EQUIPMENT full UniqNo") && ok;
        ok               = expectBytes(firstPacket, equipActIndexOffset, std::array<uint8, 2>{ 0x66, 0x55 }, "EQUIPMENT full ActIndex") && ok;
        ok               = expectBytes(firstPacket, equipOptionFlagOffset, std::array<uint8, 1>{ 0x03 }, "EQUIPMENT full OptionFlag") && ok;
        ok               = expectBytes(firstPacket, equipCountOffset, std::array<uint8, 1>{ 0x08 }, "EQUIPMENT full EquipCount") && ok;
        ok               = expectCheckItem(firstPacket, 0, 0x2000, 0, 0xA0, "EQUIPMENT full item 0") && ok;
        ok               = expectCheckItem(firstPacket, 7, 0x2007, 7, 0xA7, "EQUIPMENT full item 7") && ok;
    }

    ok = expectEqualUInt(packet.getType(), 0x0C9, "EQUIPMENT final type") && ok;
    ok = expectEqualUInt(packet.getSize(), sizeof(GP_SERV_HEADER) + equipHeaderSize + checkItemSize, "EQUIPMENT final empty size") && ok;
    ok = expectBytes(packet, 0, std::array<uint8, 4>{ 0xC9, 0x14, 0xEF, 0xBE }, "EQUIPMENT final header") && ok;
    ok = expectBytes(packet, equipCountOffset, std::array<uint8, 1>{ 0x00 }, "EQUIPMENT final EquipCount") && ok;
    ok = expectZeroRange(packet, equipArrayOffset, packet.getSize(), "EQUIPMENT final zeroed item region") && ok;
    return ok;
}

auto testEquipmentConstructorSparseSlots() -> bool
{
    std::array<std::unique_ptr<CItemEquipment>, 2> items{};
    auto                                           target = CCharEntity{};
    populateEntity(target, 0x22334455, 0x6677);

    const std::array<uint8, 2> slots{ 2, 15 };
    for (std::size_t index = 0; index < slots.size(); ++index)
    {
        const auto slot = slots[index];
        items[index]    = std::make_unique<CItemEquipment>(static_cast<uint16>(0x3000 + slot));
        auto& signature = items[index]->exdata<Exdata::AugmentStandard>().Signature;
        for (std::size_t signatureIndex = 0; signatureIndex < sizeof(signature); ++signatureIndex)
        {
            signature[signatureIndex] = static_cast<uint8>(0xB0 + slot + signatureIndex);
        }
        if (!target.bindEquip(slot, items[index].get()))
        {
            std::cerr << "s2c EQUIP_INSPECT packet self-test failed: sparse bindEquip slot " << static_cast<unsigned>(slot) << '\n';
            return false;
        }
    }

    auto checker = CCharEntity{};
    auto packet  = EquipmentPacket(&checker, &target);

    bool ok = true;
    ok      = expectEqualUInt(packet.getSize(), sizeof(GP_SERV_HEADER) + equipHeaderSize + checkItemSize * 2, "EQUIPMENT sparse size") && ok;
    ok      = expectEqualUInt(packetData(packet)[equipCountOffset], 2, "EQUIPMENT sparse count") && ok;
    ok      = expectCheckItem(packet, 0, 0x3002, 2, 0xB2, "EQUIPMENT sparse slot 2") && ok;
    ok      = expectCheckItem(packet, 1, 0x300F, 15, 0xBF, "EQUIPMENT sparse slot 15") && ok;
    return ok;
}

} // namespace

auto runS2CEquipInspectPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayoutAndEnums() && ok;
    ok      = testGeneralConstructorBytes() && ok;
    ok      = testGeneralConstructorAnonVisibility() && ok;
    ok      = testEquipmentConstructorBytes() && ok;
    ok      = testEquipmentConstructorChunksEquippedItems() && ok;
    ok      = testEquipmentConstructorSparseSlots() && ok;
    return ok;
}

OMEGA_REGISTER_SELF_TEST("s2c-equip-inspect-packet-8702", runS2CEquipInspectPacketSelfTests);
