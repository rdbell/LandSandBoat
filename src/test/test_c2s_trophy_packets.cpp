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

#include "test_c2s_trophy_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "common/utils.h"
#include "map/packets/c2s/0x041_trophy_entry.h"
#include "map/packets/c2s/0x042_trophy_absence.h"
#include "map/treasure_pool.h"

namespace
{

constexpr auto EntryRoundedPacketSize   = roundUpToNearestFour(static_cast<uint32>(sizeof(GP_CLI_COMMAND_TROPHY_ENTRY)));
constexpr auto AbsenceRoundedPacketSize = roundUpToNearestFour(static_cast<uint32>(sizeof(GP_CLI_COMMAND_TROPHY_ABSENCE)));

using EntryPacketBytes   = std::array<std::uint8_t, EntryRoundedPacketSize>;
using AbsencePacketBytes = std::array<std::uint8_t, AbsenceRoundedPacketSize>;

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s trophy packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s trophy packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectFalse(bool actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "c2s trophy packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s trophy packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(const std::array<std::uint8_t, N>& actual, const std::array<std::uint8_t, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s trophy packet self-test failed: " << label << " got";
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

auto encodedEntryPacket() -> EntryPacketBytes
{
    auto packet               = GP_CLI_COMMAND_TROPHY_ENTRY{};
    packet.header.id         = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_TROPHY_ENTRY);
    packet.header.size       = EntryRoundedPacketSize / 4U;
    packet.header.sync       = 0xBEEF;
    packet.TrophyItemIndex   = 9;
    packet.PropertyItemIndex = 0x33;
    packet.padding00[0]      = 0xAA;
    packet.padding00[1]      = 0xBB;

    auto bytes = EntryPacketBytes{};
    std::memcpy(bytes.data(), &packet, sizeof(packet));
    return bytes;
}

auto encodedAbsencePacket() -> AbsencePacketBytes
{
    auto packet             = GP_CLI_COMMAND_TROPHY_ABSENCE{};
    packet.header.id       = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_TROPHY_ABSENCE);
    packet.header.size     = AbsenceRoundedPacketSize / 4U;
    packet.header.sync     = 0xBEEF;
    packet.TrophyItemIndex = 9;
    packet.padding00       = 0xCC;

    auto bytes = AbsencePacketBytes{};
    std::memcpy(bytes.data(), &packet, sizeof(packet));
    return bytes;
}

auto trophyIndexPureValidation(std::uint8_t trophyItemIndex) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.range("TrophyItemIndex", trophyItemIndex, 0, TREASUREPOOL_SIZE - 1);
    return static_cast<PacketValidationResult>(validator);
}

auto testEntryLayoutAndMetadata() -> bool
{
    bool ok = true;
    auto packet = GP_CLI_COMMAND_TROPHY_ENTRY{};

    ok = expectEqualString(GP_CLI_COMMAND_TROPHY_ENTRY::name, "GP_CLI_COMMAND_TROPHY_ENTRY", "TROPHY_ENTRY static name") && ok;
    ok = expectTrue(GP_CLI_COMMAND_TROPHY_ENTRY::packetId == PacketC2S::GP_CLI_COMMAND_TROPHY_ENTRY, "TROPHY_ENTRY static packetId") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_HEADER), 4, "sizeof(GP_CLI_HEADER)") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_TROPHY_ENTRY), 8, "sizeof(GP_CLI_COMMAND_TROPHY_ENTRY)") && ok;
    ok = expectEqualInt(EntryRoundedPacketSize, 8, "TROPHY_ENTRY rounded fixed size") && ok;
    ok = expectEqualInt(sizeof(packet.padding00), 2, "sizeof(TROPHY_ENTRY padding00)") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_TROPHY_ENTRY, header), 0, "TROPHY_ENTRY header offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_TROPHY_ENTRY, TrophyItemIndex), 4, "TROPHY_ENTRY TrophyItemIndex offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_TROPHY_ENTRY, PropertyItemIndex), 5, "TROPHY_ENTRY PropertyItemIndex offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_TROPHY_ENTRY, padding00), 6, "TROPHY_ENTRY padding00 offset") && ok;
    ok = expectBytes(encodedEntryPacket(),
                     EntryPacketBytes{ 0x41, 0x04, 0xEF, 0xBE,
                                       0x09, 0x33, 0xAA, 0xBB },
                     "TROPHY_ENTRY encoded packet") &&
         ok;

    return ok;
}

auto testEntryPayloadStorage() -> bool
{
    auto packet               = GP_CLI_COMMAND_TROPHY_ENTRY{};
    packet.TrophyItemIndex   = 0xFE;
    packet.PropertyItemIndex = 0x88;
    packet.padding00[0]      = 0xAA;
    packet.padding00[1]      = 0xBB;

    bool ok = true;
    ok      = expectEqualInt(packet.TrophyItemIndex, 0xFE, "TROPHY_ENTRY TrophyItemIndex") && ok;
    ok      = expectEqualInt(packet.PropertyItemIndex, 0x88, "TROPHY_ENTRY PropertyItemIndex") && ok;
    ok      = expectEqualInt(packet.padding00[0], 0xAA, "TROPHY_ENTRY padding00[0]") && ok;
    ok      = expectEqualInt(packet.padding00[1], 0xBB, "TROPHY_ENTRY padding00[1]") && ok;
    return ok;
}

auto testAbsenceLayoutAndMetadata() -> bool
{
    bool ok = true;

    ok = expectEqualString(GP_CLI_COMMAND_TROPHY_ABSENCE::name, "GP_CLI_COMMAND_TROPHY_ABSENCE", "TROPHY_ABSENCE static name") && ok;
    ok = expectTrue(GP_CLI_COMMAND_TROPHY_ABSENCE::packetId == PacketC2S::GP_CLI_COMMAND_TROPHY_ABSENCE, "TROPHY_ABSENCE static packetId") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_TROPHY_ABSENCE), 6, "sizeof(GP_CLI_COMMAND_TROPHY_ABSENCE)") && ok;
    ok = expectEqualInt(AbsenceRoundedPacketSize, 8, "TROPHY_ABSENCE rounded fixed size") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_TROPHY_ABSENCE, header), 0, "TROPHY_ABSENCE header offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_TROPHY_ABSENCE, TrophyItemIndex), 4, "TROPHY_ABSENCE TrophyItemIndex offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_TROPHY_ABSENCE, padding00), 5, "TROPHY_ABSENCE padding00 offset") && ok;
    ok = expectBytes(encodedAbsencePacket(),
                     AbsencePacketBytes{ 0x42, 0x04, 0xEF, 0xBE,
                                         0x09, 0xCC, 0x00, 0x00 },
                     "TROPHY_ABSENCE encoded packet") &&
         ok;

    return ok;
}

auto testAbsencePayloadStorage() -> bool
{
    auto packet             = GP_CLI_COMMAND_TROPHY_ABSENCE{};
    packet.TrophyItemIndex = 0xFD;
    packet.padding00       = 0xCC;

    bool ok = true;
    ok      = expectEqualInt(packet.TrophyItemIndex, 0xFD, "TROPHY_ABSENCE TrophyItemIndex") && ok;
    ok      = expectEqualInt(packet.padding00, 0xCC, "TROPHY_ABSENCE padding00") && ok;
    return ok;
}

auto testPureValidationFacts() -> bool
{
    bool ok = true;
    ok      = expectEqualInt(TREASUREPOOL_SIZE, 10, "TREASUREPOOL_SIZE") && ok;
    ok      = expectValid(trophyIndexPureValidation(0), "minimum TrophyItemIndex validation") && ok;
    ok      = expectValid(trophyIndexPureValidation(9), "maximum TrophyItemIndex validation") && ok;
    ok      = expectInvalidError(trophyIndexPureValidation(10), "TrophyItemIndex out of range: 10 not in [0, 9]", "high TrophyItemIndex validation") && ok;
    ok      = expectInvalidError(trophyIndexPureValidation(255), "TrophyItemIndex out of range: 255 not in [0, 9]", "large TrophyItemIndex validation") && ok;
    return ok;
}

} // namespace

auto runC2STrophyPacketSelfTests() -> bool
{
    bool ok = true;

    ok = testEntryLayoutAndMetadata() && ok;
    ok = testEntryPayloadStorage() && ok;
    ok = testAbsenceLayoutAndMetadata() && ok;
    ok = testAbsencePayloadStorage() && ok;
    ok = testPureValidationFacts() && ok;

    return ok;
}
