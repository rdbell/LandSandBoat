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

#include "test_c2s_item_lookup_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "common/utils.h"
#include "map/packets/c2s/0x02b_translate.h"
#include "map/packets/c2s/0x02c_itemsearch.h"

namespace
{

constexpr auto nameOffset  = 8U;
constexpr auto nameMaxSize = 64U;

using TranslateShortBytes   = std::array<std::uint8_t, 16>;
using ItemSearchShortBytes  = std::array<std::uint8_t, 12>;
using TranslatePacketBytes  = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_TRANSLATE)>;
using ItemSearchPacketBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_ITEMSEARCH)>;

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s TRANSLATE/ITEMSEARCH packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s TRANSLATE/ITEMSEARCH packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectFalse(bool actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "c2s TRANSLATE/ITEMSEARCH packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s TRANSLATE/ITEMSEARCH packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(const std::array<std::uint8_t, N>& actual, const std::array<std::uint8_t, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s TRANSLATE/ITEMSEARCH packet self-test failed: " << label << " got";
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

auto validateTranslateIndices(std::uint8_t fromIndex, std::uint8_t toIndex) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator
        .oneOf<GP_CLI_COMMAND_TRANSLATE_INDEX>(fromIndex)
        .oneOf<GP_CLI_COMMAND_TRANSLATE_INDEX>(toIndex);
    return static_cast<PacketValidationResult>(validator);
}

auto validateItemSearchLanguage(std::uint8_t language) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.oneOf<GP_CLI_COMMAND_ITEMSEARCH_LANGUAGE>(language);
    return static_cast<PacketValidationResult>(validator);
}

auto encodedTranslateShortPacket() -> TranslateShortBytes
{
    static constexpr auto name = std::string_view("Potion");

    auto packet         = GP_CLI_COMMAND_TRANSLATE{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_TRANSLATE);
    packet.header.size = roundUpToNearestFour(static_cast<uint32>(nameOffset + name.size())) / 4U;
    packet.header.sync = 0xBEEF;
    packet.FromIndex   = GP_CLI_COMMAND_TRANSLATE_INDEX::English;
    packet.ToIndex     = GP_CLI_COMMAND_TRANSLATE_INDEX::Japanese;
    packet.padding00   = 0x3344;
    std::memcpy(packet.Name, name.data(), name.size());

    auto bytes = TranslateShortBytes{};
    std::memcpy(bytes.data(), &packet, bytes.size());
    return bytes;
}

auto encodedTranslateFullPacket() -> TranslatePacketBytes
{
    auto packet         = GP_CLI_COMMAND_TRANSLATE{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_TRANSLATE);
    packet.header.size = roundUpToNearestFour(static_cast<uint32>(nameOffset + nameMaxSize)) / 4U;
    packet.header.sync = 0x1234;
    packet.FromIndex   = GP_CLI_COMMAND_TRANSLATE_INDEX::Japanese;
    packet.ToIndex     = GP_CLI_COMMAND_TRANSLATE_INDEX::English;
    packet.padding00   = 0x55AA;
    for (std::size_t i = 0; i < nameMaxSize; ++i)
    {
        packet.Name[i] = static_cast<std::uint8_t>('A' + (i % 26));
    }

    auto bytes = TranslatePacketBytes{};
    std::memcpy(bytes.data(), &packet, bytes.size());
    return bytes;
}

auto expectedTranslateFullPacket() -> TranslatePacketBytes
{
    auto expected = TranslatePacketBytes{};
    expected[0]   = 0x2B;
    expected[1]   = 0x24;
    expected[2]   = 0x34;
    expected[3]   = 0x12;
    expected[4]   = static_cast<std::uint8_t>(GP_CLI_COMMAND_TRANSLATE_INDEX::Japanese);
    expected[5]   = static_cast<std::uint8_t>(GP_CLI_COMMAND_TRANSLATE_INDEX::English);
    expected[6]   = 0xAA;
    expected[7]   = 0x55;
    for (std::size_t i = 0; i < nameMaxSize; ++i)
    {
        expected[nameOffset + i] = static_cast<std::uint8_t>('A' + (i % 26));
    }
    return expected;
}

auto encodedItemSearchShortPacket() -> ItemSearchShortBytes
{
    static constexpr auto name = std::string_view("Map");

    auto packet          = GP_CLI_COMMAND_ITEMSEARCH{};
    packet.header.id    = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_ITEMSEARCH);
    packet.header.size  = roundUpToNearestFour(static_cast<uint32>(nameOffset + name.size())) / 4U;
    packet.header.sync  = 0xBEEF;
    packet.Language     = static_cast<std::uint8_t>(GP_CLI_COMMAND_ITEMSEARCH_LANGUAGE::English);
    packet.padding00[0] = 0xAA;
    packet.padding00[1] = 0xBB;
    packet.padding00[2] = 0xCC;
    std::memcpy(packet.Name, name.data(), name.size());

    auto bytes = ItemSearchShortBytes{};
    std::memcpy(bytes.data(), &packet, bytes.size());
    return bytes;
}

auto encodedItemSearchFullPacket() -> ItemSearchPacketBytes
{
    auto packet          = GP_CLI_COMMAND_ITEMSEARCH{};
    packet.header.id    = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_ITEMSEARCH);
    packet.header.size  = roundUpToNearestFour(static_cast<uint32>(nameOffset + nameMaxSize)) / 4U;
    packet.header.sync  = 0x1234;
    packet.Language     = static_cast<std::uint8_t>(GP_CLI_COMMAND_ITEMSEARCH_LANGUAGE::Japanese);
    packet.padding00[0] = 0x01;
    packet.padding00[1] = 0x02;
    packet.padding00[2] = 0x03;
    for (std::size_t i = 0; i < nameMaxSize; ++i)
    {
        packet.Name[i] = static_cast<std::uint8_t>('0' + (i % 26));
    }

    auto bytes = ItemSearchPacketBytes{};
    std::memcpy(bytes.data(), &packet, bytes.size());
    return bytes;
}

auto expectedItemSearchFullPacket() -> ItemSearchPacketBytes
{
    auto expected = ItemSearchPacketBytes{};
    expected[0]   = 0x2C;
    expected[1]   = 0x24;
    expected[2]   = 0x34;
    expected[3]   = 0x12;
    expected[4]   = static_cast<std::uint8_t>(GP_CLI_COMMAND_ITEMSEARCH_LANGUAGE::Japanese);
    expected[5]   = 0x01;
    expected[6]   = 0x02;
    expected[7]   = 0x03;
    for (std::size_t i = 0; i < nameMaxSize; ++i)
    {
        expected[nameOffset + i] = static_cast<std::uint8_t>('0' + (i % 26));
    }
    return expected;
}

auto testTranslateLayoutAndMetadata() -> bool
{
    bool ok = true;

    ok = expectEqualString(GP_CLI_COMMAND_TRANSLATE::name, "GP_CLI_COMMAND_TRANSLATE", "TRANSLATE static name") && ok;
    ok = expectTrue(GP_CLI_COMMAND_TRANSLATE::packetId == PacketC2S::GP_CLI_COMMAND_TRANSLATE, "TRANSLATE static packetId") && ok;
    ok = expectEqualInt(GP_CLI_COMMAND_TRANSLATE::getMinSize(), nameOffset, "TRANSLATE getMinSize") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_TRANSLATE), 72, "sizeof(GP_CLI_COMMAND_TRANSLATE)") && ok;
    ok = expectEqualInt(roundUpToNearestFour(static_cast<uint32>(sizeof(GP_CLI_COMMAND_TRANSLATE))), 72, "TRANSLATE rounded max size") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_TRANSLATE, header), 0, "TRANSLATE header offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_TRANSLATE, FromIndex), 4, "TRANSLATE FromIndex offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_TRANSLATE, ToIndex), 5, "TRANSLATE ToIndex offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_TRANSLATE, padding00), 6, "TRANSLATE padding00 offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_TRANSLATE, Name), nameOffset, "TRANSLATE Name offset") && ok;

    return ok;
}

auto testTranslateEnumValuesAndDomain() -> bool
{
    bool ok = true;

    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_TRANSLATE_INDEX::Japanese), 0, "TRANSLATE Japanese enum") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_TRANSLATE_INDEX::English), 1, "TRANSLATE English enum") && ok;
    ok = expectValid(validateTranslateIndices(0, 1), "TRANSLATE Japanese to English validation") && ok;
    ok = expectValid(validateTranslateIndices(1, 0), "TRANSLATE English to Japanese validation") && ok;
    ok = expectInvalidError(validateTranslateIndices(2, 0), "2 not a valid GP_CLI_COMMAND_TRANSLATE_INDEX value.", "TRANSLATE invalid FromIndex validation") && ok;
    ok = expectInvalidError(validateTranslateIndices(0, 2), "2 not a valid GP_CLI_COMMAND_TRANSLATE_INDEX value.", "TRANSLATE invalid ToIndex validation") && ok;

    return ok;
}

auto testTranslateEncodedPayloads() -> bool
{
    bool ok = true;

    ok = expectBytes(encodedTranslateShortPacket(),
                     TranslateShortBytes{ 0x2B, 0x08, 0xEF, 0xBE,
                                          0x01, 0x00, 0x44, 0x33,
                                          0x50, 0x6F, 0x74, 0x69,
                                          0x6F, 0x6E, 0x00, 0x00 },
                     "TRANSLATE short encoded packet") &&
         ok;
    ok = expectBytes(encodedTranslateFullPacket(), expectedTranslateFullPacket(), "TRANSLATE full encoded packet") && ok;

    return ok;
}

auto testItemSearchLayoutAndMetadata() -> bool
{
    bool ok = true;

    ok = expectEqualString(GP_CLI_COMMAND_ITEMSEARCH::name, "GP_CLI_COMMAND_ITEMSEARCH", "ITEMSEARCH static name") && ok;
    ok = expectTrue(GP_CLI_COMMAND_ITEMSEARCH::packetId == PacketC2S::GP_CLI_COMMAND_ITEMSEARCH, "ITEMSEARCH static packetId") && ok;
    ok = expectEqualInt(GP_CLI_COMMAND_ITEMSEARCH::getMinSize(), nameOffset, "ITEMSEARCH getMinSize") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_ITEMSEARCH), 72, "sizeof(GP_CLI_COMMAND_ITEMSEARCH)") && ok;
    ok = expectEqualInt(roundUpToNearestFour(static_cast<uint32>(sizeof(GP_CLI_COMMAND_ITEMSEARCH))), 72, "ITEMSEARCH rounded max size") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ITEMSEARCH, header), 0, "ITEMSEARCH header offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ITEMSEARCH, Language), 4, "ITEMSEARCH Language offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ITEMSEARCH, padding00), 5, "ITEMSEARCH padding00 offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ITEMSEARCH, Name), nameOffset, "ITEMSEARCH Name offset") && ok;

    return ok;
}

auto testItemSearchEnumValuesAndDomain() -> bool
{
    bool ok = true;

    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_ITEMSEARCH_LANGUAGE::Japanese), 0, "ITEMSEARCH Japanese enum") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_ITEMSEARCH_LANGUAGE::English), 1, "ITEMSEARCH English enum") && ok;
    ok = expectValid(validateItemSearchLanguage(0), "ITEMSEARCH Japanese validation") && ok;
    ok = expectValid(validateItemSearchLanguage(1), "ITEMSEARCH English validation") && ok;
    ok = expectInvalidError(validateItemSearchLanguage(2), "2 not a valid GP_CLI_COMMAND_ITEMSEARCH_LANGUAGE value.", "ITEMSEARCH invalid Language validation") && ok;

    return ok;
}

auto testItemSearchEncodedPayloads() -> bool
{
    bool ok = true;

    ok = expectBytes(encodedItemSearchShortPacket(),
                     ItemSearchShortBytes{ 0x2C, 0x06, 0xEF, 0xBE,
                                           0x01, 0xAA, 0xBB, 0xCC,
                                           0x4D, 0x61, 0x70, 0x00 },
                     "ITEMSEARCH short encoded packet") &&
         ok;
    ok = expectBytes(encodedItemSearchFullPacket(), expectedItemSearchFullPacket(), "ITEMSEARCH full encoded packet") && ok;

    return ok;
}

} // namespace

auto runC2SItemLookupPacketSelfTests() -> bool
{
    bool ok = true;

    ok = testTranslateLayoutAndMetadata() && ok;
    ok = testTranslateEnumValuesAndDomain() && ok;
    ok = testTranslateEncodedPayloads() && ok;
    ok = testItemSearchLayoutAndMetadata() && ok;
    ok = testItemSearchEnumValuesAndDomain() && ok;
    ok = testItemSearchEncodedPayloads() && ok;

    return ok;
}
