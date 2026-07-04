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

#include "test_c2s_item_container_ops_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <set>
#include <string>
#include <string_view>
#include <type_traits>

#include "map/item_container.h"
#include "map/packets/c2s/0x03a_item_stack.h"
#include "map/packets/c2s/0x03b_subcontainer.h"

namespace
{

using StackPacketBytes        = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_ITEM_STACK)>;
using SubcontainerPacketBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_SUBCONTAINER)>;

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s ITEM_STACK/SUBCONTAINER packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s ITEM_STACK/SUBCONTAINER packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectFalse(bool actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "c2s ITEM_STACK/SUBCONTAINER packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s ITEM_STACK/SUBCONTAINER packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(const std::array<std::uint8_t, N>& actual, const std::array<std::uint8_t, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s ITEM_STACK/SUBCONTAINER packet self-test failed: " << label << " got";
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

template <typename E>
auto enumValue(E value) -> std::uint64_t
{
    return static_cast<std::uint64_t>(static_cast<std::underlying_type_t<E>>(value));
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

auto encodedStackPacket(std::uint32_t category) -> StackPacketBytes
{
    auto packet         = GP_CLI_COMMAND_ITEM_STACK{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_ITEM_STACK);
    packet.header.size = sizeof(GP_CLI_COMMAND_ITEM_STACK) / 4;
    packet.header.sync = 0xBEEF;
    packet.Category    = category;

    auto bytes = StackPacketBytes{};
    std::memcpy(bytes.data(), &packet, bytes.size());
    return bytes;
}

auto encodedSubcontainerPacket() -> SubcontainerPacketBytes
{
    auto packet            = GP_CLI_COMMAND_SUBCONTAINER{};
    packet.header.id      = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_SUBCONTAINER);
    packet.header.size    = sizeof(GP_CLI_COMMAND_SUBCONTAINER) / 4;
    packet.header.sync    = 0xBEEF;
    packet.Kind           = static_cast<std::uint32_t>(enumValue(GP_CLI_COMMAND_SUBCONTAINER_KIND::Equip));
    packet.Category1      = LOC_MOGSAFE2;
    packet.ItemIndex1     = 0x22;
    packet.ContainerIndex = static_cast<std::uint8_t>(enumValue(GP_CLI_COMMAND_SUBCONTAINER_CONTAINERINDEX::Hands));
    packet.padding00      = 0xBBAA;
    packet.Category2      = LOC_STORAGE;
    packet.ItemIndex2     = 0x33;
    packet.padding01[0]   = 0x44;
    packet.padding01[1]   = 0x55;
    packet.padding01[2]   = 0x66;
    packet.unknown00      = 0x11223344;
    packet.unknown01      = 0x77;
    packet.padding02[0]   = 0x88;
    packet.padding02[1]   = 0x99;
    packet.padding02[2]   = 0xAA;

    auto bytes = SubcontainerPacketBytes{};
    std::memcpy(bytes.data(), &packet, bytes.size());
    return bytes;
}

auto stackPureValidation(std::uint32_t category) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.oneOf<CONTAINER_ID>(category);
    return static_cast<PacketValidationResult>(validator);
}

auto subcontainerCategory1PureContainers() -> std::set<CONTAINER_ID>
{
    return {
        LOC_MOGSAFE,
        LOC_MOGSAFE2,
    };
}

auto subcontainerPureValidation(std::uint32_t kind, std::uint32_t containerIndex, std::uint32_t category1) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator
        .oneOf<GP_CLI_COMMAND_SUBCONTAINER_KIND>(kind)
        .oneOf<GP_CLI_COMMAND_SUBCONTAINER_CONTAINERINDEX>(containerIndex)
        .oneOf("Category1", static_cast<CONTAINER_ID>(category1), subcontainerCategory1PureContainers());
    return static_cast<PacketValidationResult>(validator);
}

auto testStackLayoutAndMetadata() -> bool
{
    bool ok = true;

    ok = expectEqualString(GP_CLI_COMMAND_ITEM_STACK::name, "GP_CLI_COMMAND_ITEM_STACK", "ITEM_STACK static name") && ok;
    ok = expectTrue(GP_CLI_COMMAND_ITEM_STACK::packetId == PacketC2S::GP_CLI_COMMAND_ITEM_STACK, "ITEM_STACK static packetId") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_HEADER), 4, "sizeof(GP_CLI_HEADER)") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_ITEM_STACK), 8, "sizeof(GP_CLI_COMMAND_ITEM_STACK)") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ITEM_STACK, header), 0, "ITEM_STACK header offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ITEM_STACK, Category), 4, "ITEM_STACK Category offset") && ok;
    ok = expectBytes(encodedStackPacket(0x11223344),
                     StackPacketBytes{ 0x3A, 0x04, 0xEF, 0xBE,
                                       0x44, 0x33, 0x22, 0x11 },
                     "ITEM_STACK encoded packet") &&
         ok;

    return ok;
}

auto testStackPayloadStorage() -> bool
{
    auto packet     = GP_CLI_COMMAND_ITEM_STACK{};
    packet.Category = 0xFFFFFFFF;

    bool ok = true;
    ok      = expectEqualInt(packet.Category, 0xFFFFFFFF, "ITEM_STACK Category") && ok;
    return ok;
}

auto testStackPureValidationFacts() -> bool
{
    bool ok = true;
    for (std::uint32_t value = LOC_INVENTORY; value <= MAX_CONTAINER_ID; ++value)
    {
        ok = expectValid(stackPureValidation(value), "ITEM_STACK container " + std::to_string(value) + " validation") && ok;
    }

    ok = expectValid(stackPureValidation(0x100), "ITEM_STACK high-byte truncated inventory validation") && ok;
    ok = expectInvalidError(stackPureValidation(0xFF), "255 not a valid CONTAINER_ID value.", "ITEM_STACK invalid low-byte validation") && ok;
    return ok;
}

auto testSubcontainerLayoutAndMetadata() -> bool
{
    bool ok = true;

    ok = expectEqualString(GP_CLI_COMMAND_SUBCONTAINER::name, "GP_CLI_COMMAND_SUBCONTAINER", "SUBCONTAINER static name") && ok;
    ok = expectTrue(GP_CLI_COMMAND_SUBCONTAINER::packetId == PacketC2S::GP_CLI_COMMAND_SUBCONTAINER, "SUBCONTAINER static packetId") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_SUBCONTAINER), 32, "sizeof(GP_CLI_COMMAND_SUBCONTAINER)") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_SUBCONTAINER, header), 0, "SUBCONTAINER header offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_SUBCONTAINER, Kind), 4, "SUBCONTAINER Kind offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_SUBCONTAINER, Category1), 8, "SUBCONTAINER Category1 offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_SUBCONTAINER, ItemIndex1), 12, "SUBCONTAINER ItemIndex1 offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_SUBCONTAINER, ContainerIndex), 13, "SUBCONTAINER ContainerIndex offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_SUBCONTAINER, padding00), 14, "SUBCONTAINER padding00 offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_SUBCONTAINER, Category2), 16, "SUBCONTAINER Category2 offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_SUBCONTAINER, ItemIndex2), 20, "SUBCONTAINER ItemIndex2 offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_SUBCONTAINER, padding01), 21, "SUBCONTAINER padding01 offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_SUBCONTAINER, unknown00), 24, "SUBCONTAINER unknown00 offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_SUBCONTAINER, unknown01), 28, "SUBCONTAINER unknown01 offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_SUBCONTAINER, padding02), 29, "SUBCONTAINER padding02 offset") && ok;
    ok = expectBytes(encodedSubcontainerPacket(),
                     SubcontainerPacketBytes{ 0x3B, 0x10, 0xEF, 0xBE,
                                              0x01, 0x00, 0x00, 0x00,
                                              0x09, 0x00, 0x00, 0x00,
                                              0x22, 0x05, 0xAA, 0xBB,
                                              0x02, 0x00, 0x00, 0x00,
                                              0x33, 0x44, 0x55, 0x66,
                                              0x44, 0x33, 0x22, 0x11,
                                              0x77, 0x88, 0x99, 0xAA },
                     "SUBCONTAINER encoded packet") &&
         ok;

    return ok;
}

auto testSubcontainerEnumValuesAndDomains() -> bool
{
    bool ok = true;

    ok = expectEqualInt(enumValue(GP_CLI_COMMAND_SUBCONTAINER_KIND::Equip), 1, "Kind::Equip") && ok;
    ok = expectEqualInt(enumValue(GP_CLI_COMMAND_SUBCONTAINER_KIND::Unequip), 2, "Kind::Unequip") && ok;
    ok = expectEqualInt(enumValue(GP_CLI_COMMAND_SUBCONTAINER_KIND::UnequipAll), 5, "Kind::UnequipAll") && ok;
    ok = expectTrue(magic_enum::enum_contains<GP_CLI_COMMAND_SUBCONTAINER_KIND>(1), "Kind domain contains 1") && ok;
    ok = expectTrue(magic_enum::enum_contains<GP_CLI_COMMAND_SUBCONTAINER_KIND>(2), "Kind domain contains 2") && ok;
    ok = expectTrue(magic_enum::enum_contains<GP_CLI_COMMAND_SUBCONTAINER_KIND>(5), "Kind domain contains 5") && ok;
    ok = expectFalse(magic_enum::enum_contains<GP_CLI_COMMAND_SUBCONTAINER_KIND>(0), "Kind domain rejects 0") && ok;
    ok = expectFalse(magic_enum::enum_contains<GP_CLI_COMMAND_SUBCONTAINER_KIND>(3), "Kind domain rejects 3") && ok;
    ok = expectFalse(magic_enum::enum_contains<GP_CLI_COMMAND_SUBCONTAINER_KIND>(6), "Kind domain rejects 6") && ok;

    ok = expectEqualInt(enumValue(GP_CLI_COMMAND_SUBCONTAINER_CONTAINERINDEX::MainWeapon), 0, "ContainerIndex::MainWeapon") && ok;
    ok = expectEqualInt(enumValue(GP_CLI_COMMAND_SUBCONTAINER_CONTAINERINDEX::SubWeapon), 1, "ContainerIndex::SubWeapon") && ok;
    ok = expectEqualInt(enumValue(GP_CLI_COMMAND_SUBCONTAINER_CONTAINERINDEX::RangedWeapon), 2, "ContainerIndex::RangedWeapon") && ok;
    ok = expectEqualInt(enumValue(GP_CLI_COMMAND_SUBCONTAINER_CONTAINERINDEX::Head), 3, "ContainerIndex::Head") && ok;
    ok = expectEqualInt(enumValue(GP_CLI_COMMAND_SUBCONTAINER_CONTAINERINDEX::Body), 4, "ContainerIndex::Body") && ok;
    ok = expectEqualInt(enumValue(GP_CLI_COMMAND_SUBCONTAINER_CONTAINERINDEX::Hands), 5, "ContainerIndex::Hands") && ok;
    ok = expectEqualInt(enumValue(GP_CLI_COMMAND_SUBCONTAINER_CONTAINERINDEX::Legs), 6, "ContainerIndex::Legs") && ok;
    ok = expectEqualInt(enumValue(GP_CLI_COMMAND_SUBCONTAINER_CONTAINERINDEX::Feet), 7, "ContainerIndex::Feet") && ok;
    for (std::uint32_t value = 0; value <= 7; ++value)
    {
        ok = expectTrue(magic_enum::enum_contains<GP_CLI_COMMAND_SUBCONTAINER_CONTAINERINDEX>(value), "ContainerIndex domain contains " + std::to_string(value)) && ok;
    }
    ok = expectFalse(magic_enum::enum_contains<GP_CLI_COMMAND_SUBCONTAINER_CONTAINERINDEX>(8), "ContainerIndex domain rejects 8") && ok;

    return ok;
}

auto testSubcontainerPayloadStorage() -> bool
{
    auto packet            = GP_CLI_COMMAND_SUBCONTAINER{};
    packet.Kind           = static_cast<std::uint32_t>(enumValue(GP_CLI_COMMAND_SUBCONTAINER_KIND::UnequipAll));
    packet.Category1      = LOC_MOGSAFE;
    packet.ItemIndex1     = 0x12;
    packet.ContainerIndex = static_cast<std::uint8_t>(enumValue(GP_CLI_COMMAND_SUBCONTAINER_CONTAINERINDEX::Feet));
    packet.padding00      = 0x0201;
    packet.Category2      = 0xAABBCCDD;
    packet.ItemIndex2     = 0x34;
    packet.padding01[0]   = 0x56;
    packet.padding01[1]   = 0x78;
    packet.padding01[2]   = 0x9A;
    packet.unknown00      = 0x11223344;
    packet.unknown01      = 0xBC;
    packet.padding02[0]   = 0xDE;
    packet.padding02[1]   = 0xF0;
    packet.padding02[2]   = 0x13;

    bool ok = true;
    ok      = expectEqualInt(packet.Kind, 5, "SUBCONTAINER Kind") && ok;
    ok      = expectEqualInt(packet.Category1, LOC_MOGSAFE, "SUBCONTAINER Category1") && ok;
    ok      = expectEqualInt(packet.ItemIndex1, 0x12, "SUBCONTAINER ItemIndex1") && ok;
    ok      = expectEqualInt(packet.ContainerIndex, 7, "SUBCONTAINER ContainerIndex") && ok;
    ok      = expectEqualInt(packet.padding00, 0x0201, "SUBCONTAINER padding00") && ok;
    ok      = expectEqualInt(packet.Category2, 0xAABBCCDD, "SUBCONTAINER Category2") && ok;
    ok      = expectEqualInt(packet.ItemIndex2, 0x34, "SUBCONTAINER ItemIndex2") && ok;
    ok      = expectEqualInt(packet.padding01[0], 0x56, "SUBCONTAINER padding01[0]") && ok;
    ok      = expectEqualInt(packet.padding01[1], 0x78, "SUBCONTAINER padding01[1]") && ok;
    ok      = expectEqualInt(packet.padding01[2], 0x9A, "SUBCONTAINER padding01[2]") && ok;
    ok      = expectEqualInt(packet.unknown00, 0x11223344, "SUBCONTAINER unknown00") && ok;
    ok      = expectEqualInt(packet.unknown01, 0xBC, "SUBCONTAINER unknown01") && ok;
    ok      = expectEqualInt(packet.padding02[0], 0xDE, "SUBCONTAINER padding02[0]") && ok;
    ok      = expectEqualInt(packet.padding02[1], 0xF0, "SUBCONTAINER padding02[1]") && ok;
    ok      = expectEqualInt(packet.padding02[2], 0x13, "SUBCONTAINER padding02[2]") && ok;
    return ok;
}

auto testSubcontainerPureValidationFacts() -> bool
{
    bool ok = true;
    ok      = expectValid(subcontainerPureValidation(1, 0, LOC_MOGSAFE), "SUBCONTAINER equip Mog Safe validation") && ok;
    ok      = expectValid(subcontainerPureValidation(5, 7, LOC_MOGSAFE2), "SUBCONTAINER unequip all Mog Safe 2 validation") && ok;
    ok      = expectValid(subcontainerPureValidation(1, 1, 0x101), "SUBCONTAINER high-byte truncated Mog Safe validation") && ok;
    ok      = expectInvalidError(subcontainerPureValidation(3, 0, LOC_MOGSAFE), "3 not a valid GP_CLI_COMMAND_SUBCONTAINER_KIND value.", "SUBCONTAINER invalid Kind validation") && ok;
    ok      = expectInvalidError(subcontainerPureValidation(1, 8, LOC_MOGSAFE), "8 not a valid GP_CLI_COMMAND_SUBCONTAINER_CONTAINERINDEX value.", "SUBCONTAINER invalid ContainerIndex validation") && ok;
    ok      = expectInvalidError(subcontainerPureValidation(2, 5, LOC_STORAGE), "Category1 value 2 is not allowed.", "SUBCONTAINER invalid Category1 validation") && ok;
    return ok;
}

} // namespace

auto runC2SItemContainerOpsPacketSelfTests() -> bool
{
    bool ok = true;

    ok = testStackLayoutAndMetadata() && ok;
    ok = testStackPayloadStorage() && ok;
    ok = testStackPureValidationFacts() && ok;
    ok = testSubcontainerLayoutAndMetadata() && ok;
    ok = testSubcontainerEnumValuesAndDomains() && ok;
    ok = testSubcontainerPayloadStorage() && ok;
    ok = testSubcontainerPureValidationFacts() && ok;

    return ok;
}
