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

#include "test_c2s_player_points_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "map/enums/alter_ego_points.h"
#include "map/packets/c2s/0x0be_merits.h"
#include "map/packets/c2s/0x0bf_job_points_spend.h"
#include "map/packets/c2s/0x0c0_job_points_req.h"
#include "map/packets/c2s/0x0c1_alter_ego_points.h"

namespace
{

using MeritsBytes         = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_MERITS)>;
using JobPointsSpendBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_JOB_POINTS_SPEND)>;
using JobPointsReqBytes   = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_JOB_POINTS_REQ)>;
using AlterEgoPointsBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_ALTER_EGO_POINTS)>;

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s player points packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s player points packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectFalse(bool actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "c2s player points packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s player points packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(const std::array<std::uint8_t, N>& actual, const std::array<std::uint8_t, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s player points packet self-test failed: " << label << " got";
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

auto validateMeritsPure(std::uint8_t kind, std::uint8_t param1) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.oneOf<GP_CLI_COMMAND_MERITS_KIND>(kind)
        .oneOf<GP_CLI_COMMAND_MERITS_PARAM1>(param1);
    return validator;
}

auto validateJobPointsSpendPure(std::uint16_t index) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    (void)index;
    return validator;
}

auto validateJobPointsReqPure() -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    return validator;
}

auto validateAlterEgoCategoryPure(std::uint16_t categoryIndex) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.oneOf<AlterEgoCategory>(categoryIndex);
    return validator;
}

auto makeMeritsPacket() -> GP_CLI_COMMAND_MERITS
{
    auto packet         = GP_CLI_COMMAND_MERITS{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_MERITS);
    packet.header.size = sizeof(GP_CLI_COMMAND_MERITS) / 4U;
    packet.header.sync = 0xBEEF;
    packet.Kind        = static_cast<std::uint8_t>(GP_CLI_COMMAND_MERITS_KIND::EditMode);
    packet.Param1      = static_cast<std::uint8_t>(GP_CLI_COMMAND_MERITS_PARAM1::Raise);
    packet.Param2      = 0x1234;
    packet.Param3      = 0x89ABCDEF;
    return packet;
}

auto makeJobPointsSpendPacket() -> GP_CLI_COMMAND_JOB_POINTS_SPEND
{
    auto packet         = GP_CLI_COMMAND_JOB_POINTS_SPEND{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_JOB_POINTS_SPEND);
    packet.header.size = sizeof(GP_CLI_COMMAND_JOB_POINTS_SPEND) / 4U;
    packet.header.sync = 0xBEEF;
    packet.Index       = 0x1234;
    packet.padding00   = 0x55AA;
    return packet;
}

auto makeJobPointsReqPacket() -> GP_CLI_COMMAND_JOB_POINTS_REQ
{
    auto packet         = GP_CLI_COMMAND_JOB_POINTS_REQ{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_JOB_POINTS_REQ);
    packet.header.size = sizeof(GP_CLI_COMMAND_JOB_POINTS_REQ) / 4U;
    packet.header.sync = 0xBEEF;
    return packet;
}

auto makeAlterEgoPointsPacket() -> GP_CLI_COMMAND_ALTER_EGO_POINTS
{
    auto packet           = GP_CLI_COMMAND_ALTER_EGO_POINTS{};
    packet.header.id     = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_ALTER_EGO_POINTS);
    packet.header.size   = sizeof(GP_CLI_COMMAND_ALTER_EGO_POINTS) / 4U;
    packet.header.sync   = 0xBEEF;
    packet.CategoryIndex = static_cast<std::uint16_t>(AlterEgoCategory::CombatSkills);
    packet.padding00     = 0x4433;
    return packet;
}

auto testMeritsLayoutMetadataAndPayload() -> bool
{
    const auto packet = makeMeritsPacket();
    bool       ok     = true;

    ok = expectEqualString(GP_CLI_COMMAND_MERITS::name, "GP_CLI_COMMAND_MERITS", "MERITS name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_MERITS::packetId), 0x0BE, "MERITS packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_MERITS), 12, "MERITS sizeof") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_MERITS, Kind), 4, "MERITS Kind offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_MERITS, Param1), 5, "MERITS Param1 offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_MERITS, Param2), 6, "MERITS Param2 offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_MERITS, Param3), 8, "MERITS Param3 offset") && ok;
    ok = expectBytes(encodedPacketBytes(packet),
                     MeritsBytes{ 0xBE, 0x06, 0xEF, 0xBE, 0x03, 0x01, 0x34, 0x12, 0xEF, 0xCD, 0xAB, 0x89 },
                     "MERITS encoded packet") &&
         ok;
    return ok;
}

auto testJobPointsLayoutMetadataAndPayload() -> bool
{
    const auto spendPacket = makeJobPointsSpendPacket();
    const auto reqPacket   = makeJobPointsReqPacket();
    bool       ok          = true;

    ok = expectEqualString(GP_CLI_COMMAND_JOB_POINTS_SPEND::name, "GP_CLI_COMMAND_JOB_POINTS_SPEND", "JOB_POINTS_SPEND name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_JOB_POINTS_SPEND::packetId), 0x0BF, "JOB_POINTS_SPEND packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_JOB_POINTS_SPEND), 8, "JOB_POINTS_SPEND sizeof") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_JOB_POINTS_SPEND, Index), 4, "JOB_POINTS_SPEND Index offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_JOB_POINTS_SPEND, padding00), 6, "JOB_POINTS_SPEND padding00 offset") && ok;
    ok = expectBytes(encodedPacketBytes(spendPacket),
                     JobPointsSpendBytes{ 0xBF, 0x04, 0xEF, 0xBE, 0x34, 0x12, 0xAA, 0x55 },
                     "JOB_POINTS_SPEND encoded packet") &&
         ok;

    ok = expectEqualString(GP_CLI_COMMAND_JOB_POINTS_REQ::name, "GP_CLI_COMMAND_JOB_POINTS_REQ", "JOB_POINTS_REQ name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_JOB_POINTS_REQ::packetId), 0x0C0, "JOB_POINTS_REQ packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_JOB_POINTS_REQ), 4, "JOB_POINTS_REQ sizeof") && ok;
    ok = expectBytes(encodedPacketBytes(reqPacket),
                     JobPointsReqBytes{ 0xC0, 0x02, 0xEF, 0xBE },
                     "JOB_POINTS_REQ encoded packet") &&
         ok;
    return ok;
}

auto testAlterEgoPointsLayoutMetadataAndPayload() -> bool
{
    const auto packet = makeAlterEgoPointsPacket();
    bool       ok     = true;

    ok = expectEqualString(GP_CLI_COMMAND_ALTER_EGO_POINTS::name, "GP_CLI_COMMAND_ALTER_EGO_POINTS", "ALTER_EGO_POINTS name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_ALTER_EGO_POINTS::packetId), 0x0C1, "ALTER_EGO_POINTS packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_ALTER_EGO_POINTS), 8, "ALTER_EGO_POINTS sizeof") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ALTER_EGO_POINTS, CategoryIndex), 4, "ALTER_EGO_POINTS CategoryIndex offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ALTER_EGO_POINTS, padding00), 6, "ALTER_EGO_POINTS padding00 offset") && ok;
    ok = expectBytes(encodedPacketBytes(packet),
                     AlterEgoPointsBytes{ 0xC1, 0x04, 0xEF, 0xBE, 0x11, 0x00, 0x33, 0x44 },
                     "ALTER_EGO_POINTS encoded packet") &&
         ok;
    return ok;
}

auto testPlayerPointsConstantsAndValidation() -> bool
{
    bool ok = true;
    ok      = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_MERITS_KIND::ChangeMode), 2, "MERITS Kind::ChangeMode") && ok;
    ok      = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_MERITS_KIND::EditMode), 3, "MERITS Kind::EditMode") && ok;
    ok      = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_MERITS_PARAM1::Lower), 0, "MERITS Param1::Lower") && ok;
    ok      = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_MERITS_PARAM1::Raise), 1, "MERITS Param1::Raise") && ok;
    ok      = expectValid(validateMeritsPure(2, 0), "MERITS change/lower validation") && ok;
    ok      = expectValid(validateMeritsPure(3, 1), "MERITS edit/raise validation") && ok;
    ok      = expectInvalidError(validateMeritsPure(1, 0), "1 not a valid GP_CLI_COMMAND_MERITS_KIND value.", "MERITS invalid kind validation") && ok;
    ok      = expectInvalidError(validateMeritsPure(2, 2), "2 not a valid GP_CLI_COMMAND_MERITS_PARAM1 value.", "MERITS invalid Param1 validation") && ok;

    ok = expectValid(validateJobPointsSpendPure(0xFFFF), "JOB_POINTS_SPEND pure validation") && ok;
    ok = expectValid(validateJobPointsReqPure(), "JOB_POINTS_REQ pure validation") && ok;

    ok = expectEqualInt(static_cast<std::uint8_t>(AlterEgoCategory::HP), 8, "AlterEgoCategory::HP") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(AlterEgoCategory::MagicSkills), 18, "AlterEgoCategory::MagicSkills") && ok;
    ok = expectValid(validateAlterEgoCategoryPure(8), "ALTER_EGO_POINTS HP validation") && ok;
    ok = expectValid(validateAlterEgoCategoryPure(18), "ALTER_EGO_POINTS MagicSkills validation") && ok;
    ok = expectInvalidError(validateAlterEgoCategoryPure(7), "7 not a valid AlterEgoCategory value.", "ALTER_EGO_POINTS below enum validation") && ok;
    ok = expectInvalidError(validateAlterEgoCategoryPure(19), "19 not a valid AlterEgoCategory value.", "ALTER_EGO_POINTS above enum validation") && ok;
    ok = expectValid(validateAlterEgoCategoryPure(0x0108), "ALTER_EGO_POINTS narrowed HP validation") && ok;
    ok = expectInvalidError(validateAlterEgoCategoryPure(0x0113), "19 not a valid AlterEgoCategory value.", "ALTER_EGO_POINTS narrowed invalid validation") && ok;
    return ok;
}

} // namespace

auto runC2SPlayerPointsPacketSelfTests() -> bool
{
    return testMeritsLayoutMetadataAndPayload() &&
           testJobPointsLayoutMetadataAndPayload() &&
           testAlterEgoPointsLayoutMetadataAndPayload() &&
           testPlayerPointsConstantsAndValidation();
}
