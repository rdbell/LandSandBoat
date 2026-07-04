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

#include "test_c2s_lockstyle_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "map/packets/c2s/0x053_lockstyle.h"

namespace
{

constexpr auto lockstyleCountOffset      = 4U;
constexpr auto lockstyleModeOffset       = 5U;
constexpr auto lockstyleFlagsOffset      = 6U;
constexpr auto lockstylePadding00Offset  = 7U;
constexpr auto lockstyleItemsOffset      = 8U;
constexpr auto lockstyleItemCount        = 16U;
constexpr auto lockstyleItemSize         = 8U;
constexpr auto itemItemIndexOffset       = 0U;
constexpr auto itemEquipKindOffset       = 1U;
constexpr auto itemCategoryOffset        = 2U;
constexpr auto itemPadding00Offset       = 3U;
constexpr auto itemItemNoOffset          = 4U;
constexpr auto itemPadding01Offset       = 6U;

using LockstyleBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_LOCKSTYLE)>;

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s lockstyle packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s lockstyle packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectFalse(bool actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "c2s lockstyle packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s lockstyle packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(const std::array<std::uint8_t, N>& actual, const std::array<std::uint8_t, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s lockstyle packet self-test failed: " << label << " got";
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

auto lockstyleItemOffset(std::size_t index) -> std::size_t
{
    return lockstyleItemsOffset + index * lockstyleItemSize;
}

auto makeLockstylePacket() -> GP_CLI_COMMAND_LOCKSTYLE
{
    auto packet         = GP_CLI_COMMAND_LOCKSTYLE{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_LOCKSTYLE);
    packet.header.size = sizeof(packet) / 4U;
    packet.header.sync = 0xBEEF;
    packet.Count       = 3;
    packet.Mode        = static_cast<std::uint8_t>(GP_CLI_COMMAND_LOCKSTYLE_MODE::Set);
    packet.Flags       = 0xAA;
    packet.padding00   = 0xBB;

    for (std::size_t i = 0; i < lockstyleItemCount; ++i)
    {
        auto& item      = packet.Items[i];
        item.ItemIndex  = static_cast<std::uint8_t>(i + 1U);
        item.EquipKind  = static_cast<std::uint8_t>(i % 10U);
        item.Category   = static_cast<std::uint8_t>(0x20U + i);
        item.padding00  = static_cast<std::uint8_t>(0x40U + i);
        item.ItemNo     = static_cast<std::uint16_t>(0x1000U + i);
        item.padding01  = static_cast<std::uint16_t>(0x2000U + i);
    }

    return packet;
}

auto makeExpectedLockstyleBytes() -> LockstyleBytes
{
    auto bytes = LockstyleBytes{};
    bytes[0] = 0x53;
    bytes[1] = 0x44;
    bytes[2] = 0xEF;
    bytes[3] = 0xBE;
    bytes[lockstyleCountOffset] = 3;
    bytes[lockstyleModeOffset] = static_cast<std::uint8_t>(GP_CLI_COMMAND_LOCKSTYLE_MODE::Set);
    bytes[lockstyleFlagsOffset] = 0xAA;
    bytes[lockstylePadding00Offset] = 0xBB;

    for (std::size_t i = 0; i < lockstyleItemCount; ++i)
    {
        const auto offset = lockstyleItemOffset(i);
        bytes[offset + itemItemIndexOffset] = static_cast<std::uint8_t>(i + 1U);
        bytes[offset + itemEquipKindOffset] = static_cast<std::uint8_t>(i % 10U);
        bytes[offset + itemCategoryOffset] = static_cast<std::uint8_t>(0x20U + i);
        bytes[offset + itemPadding00Offset] = static_cast<std::uint8_t>(0x40U + i);
        const auto itemNo = static_cast<std::uint16_t>(0x1000U + i);
        bytes[offset + itemItemNoOffset] = static_cast<std::uint8_t>(itemNo & 0xFFU);
        bytes[offset + itemItemNoOffset + 1U] = static_cast<std::uint8_t>(itemNo >> 8U);
        const auto padding01 = static_cast<std::uint16_t>(0x2000U + i);
        bytes[offset + itemPadding01Offset] = static_cast<std::uint8_t>(padding01 & 0xFFU);
        bytes[offset + itemPadding01Offset + 1U] = static_cast<std::uint8_t>(padding01 >> 8U);
    }

    return bytes;
}

auto validateLockstylePure(std::uint8_t mode, std::uint8_t count) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.oneOf<GP_CLI_COMMAND_LOCKSTYLE_MODE>(mode)
        .range("Count", count, 0, 16);
    return validator;
}

auto testLockstyleLayoutAndMetadata() -> bool
{
    bool ok = true;

    const auto packet = GP_CLI_COMMAND_LOCKSTYLE{};
    const auto item   = lockstyleitem_t{};

    ok = expectEqualString(GP_CLI_COMMAND_LOCKSTYLE::name, "GP_CLI_COMMAND_LOCKSTYLE", "LOCKSTYLE name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_LOCKSTYLE::packetId), 0x053, "LOCKSTYLE packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_HEADER), 4, "client header size") && ok;
    ok = expectEqualInt(sizeof(lockstyleitem_t), lockstyleItemSize, "lockstyleitem_t sizeof") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_LOCKSTYLE), 136, "LOCKSTYLE sizeof") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_LOCKSTYLE, Count), lockstyleCountOffset, "Count offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_LOCKSTYLE, Mode), lockstyleModeOffset, "Mode offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_LOCKSTYLE, Flags), lockstyleFlagsOffset, "Flags offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_LOCKSTYLE, padding00), lockstylePadding00Offset, "padding00 offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_LOCKSTYLE, Items), lockstyleItemsOffset, "Items offset") && ok;
    ok = expectEqualInt(sizeof(packet.Items) / sizeof(packet.Items[0]), lockstyleItemCount, "Items count") && ok;
    ok = expectEqualInt(offsetof(lockstyleitem_t, ItemIndex), itemItemIndexOffset, "ItemIndex offset") && ok;
    ok = expectEqualInt(offsetof(lockstyleitem_t, EquipKind), itemEquipKindOffset, "EquipKind offset") && ok;
    ok = expectEqualInt(offsetof(lockstyleitem_t, Category), itemCategoryOffset, "Category offset") && ok;
    ok = expectEqualInt(offsetof(lockstyleitem_t, padding00), itemPadding00Offset, "item padding00 offset") && ok;
    ok = expectEqualInt(offsetof(lockstyleitem_t, ItemNo), itemItemNoOffset, "ItemNo offset") && ok;
    ok = expectEqualInt(offsetof(lockstyleitem_t, padding01), itemPadding01Offset, "padding01 offset") && ok;
    ok = expectEqualInt(sizeof(item.ItemIndex), 1, "ItemIndex size") && ok;
    ok = expectEqualInt(sizeof(item.EquipKind), 1, "EquipKind size") && ok;
    ok = expectEqualInt(sizeof(item.Category), 1, "Category size") && ok;
    ok = expectEqualInt(sizeof(item.ItemNo), 2, "ItemNo size") && ok;
    ok = expectEqualInt(lockstyleItemOffset(15), 128, "last item offset") && ok;

    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_LOCKSTYLE_MODE::Disable), 0, "Disable mode") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_LOCKSTYLE_MODE::Continue), 1, "Continue mode") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_LOCKSTYLE_MODE::Query), 2, "Query mode") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_LOCKSTYLE_MODE::Set), 3, "Set mode") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_LOCKSTYLE_MODE::Enable), 4, "Enable mode") && ok;

    return ok;
}

auto testLockstyleEncodedBytes() -> bool
{
    return expectBytes(encodedPacketBytes(makeLockstylePacket()), makeExpectedLockstyleBytes(), "LOCKSTYLE bytes");
}

auto testLockstyleValidation() -> bool
{
    bool ok = true;

    ok = expectValid(validateLockstylePure(static_cast<std::uint8_t>(GP_CLI_COMMAND_LOCKSTYLE_MODE::Disable), 0), "disable count min") && ok;
    ok = expectValid(validateLockstylePure(static_cast<std::uint8_t>(GP_CLI_COMMAND_LOCKSTYLE_MODE::Continue), 1), "continue") && ok;
    ok = expectValid(validateLockstylePure(static_cast<std::uint8_t>(GP_CLI_COMMAND_LOCKSTYLE_MODE::Query), 2), "query") && ok;
    ok = expectValid(validateLockstylePure(static_cast<std::uint8_t>(GP_CLI_COMMAND_LOCKSTYLE_MODE::Set), 16), "set count max") && ok;
    ok = expectValid(validateLockstylePure(static_cast<std::uint8_t>(GP_CLI_COMMAND_LOCKSTYLE_MODE::Enable), 0), "enable") && ok;
    ok = expectInvalidError(validateLockstylePure(5, 0), "5 not a valid GP_CLI_COMMAND_LOCKSTYLE_MODE value.", "invalid mode") && ok;
    ok = expectInvalidError(validateLockstylePure(static_cast<std::uint8_t>(GP_CLI_COMMAND_LOCKSTYLE_MODE::Set), 17), "Count out of range: 17 not in [0, 16]", "invalid count") && ok;
    ok = expectInvalidError(validateLockstylePure(5, 17), "5 not a valid GP_CLI_COMMAND_LOCKSTYLE_MODE value.", "validation order") && ok;

    return ok;
}

} // namespace

auto runC2SLockstylePacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLockstyleLayoutAndMetadata() && ok;
    ok      = testLockstyleEncodedBytes() && ok;
    ok      = testLockstyleValidation() && ok;
    return ok;
}
