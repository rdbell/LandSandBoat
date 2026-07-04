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

#include "test_c2s_recipe_misc_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>
#include <type_traits>

#include "map/packets/c2s/0x058_recipe.h"
#include "map/packets/c2s/0x059_effectend.h"
#include "map/packets/c2s/0x063_dig.h"

namespace
{

using RecipePacketBytes    = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_RECIPE)>;
using EffectEndPacketBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_EFFECTEND)>;
using DigPacketBytes       = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_DIG)>;

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s RECIPE/EFFECTEND/DIG packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s RECIPE/EFFECTEND/DIG packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectFalse(bool actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "c2s RECIPE/EFFECTEND/DIG packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s RECIPE/EFFECTEND/DIG packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(const std::array<std::uint8_t, N>& actual, const std::array<std::uint8_t, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s RECIPE/EFFECTEND/DIG packet self-test failed: " << label << " got";
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

auto recipePacket() -> GP_CLI_COMMAND_RECIPE
{
    auto packet         = GP_CLI_COMMAND_RECIPE{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_RECIPE);
    packet.header.size = sizeof(GP_CLI_COMMAND_RECIPE) / 4;
    packet.header.sync = 0xBEEF;
    packet.skill       = 0x0102;
    packet.level       = 0x0304;
    packet.Param0      = 0x0506;
    packet.Mode        = static_cast<std::uint16_t>(GP_CLI_COMMAND_RECIPE_MODE::RequestAvailableRecipeList);
    packet.Param1      = 0x0708;
    packet.Param2      = 0x090A;
    packet.Param3      = 0x0B0C;
    packet.Param4      = 0x0D0E;
    return packet;
}

auto encodedRecipePacket() -> RecipePacketBytes
{
    const auto packet = recipePacket();
    auto       bytes  = RecipePacketBytes{};
    std::memcpy(bytes.data(), &packet, bytes.size());
    return bytes;
}

auto effectEndPacket() -> GP_CLI_COMMAND_EFFECTEND
{
    auto packet          = GP_CLI_COMMAND_EFFECTEND{};
    packet.header.id    = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_EFFECTEND);
    packet.header.size  = sizeof(GP_CLI_COMMAND_EFFECTEND) / 4;
    packet.header.sync  = 0xBEEF;
    packet.effectpara   = 0x11223344;
    packet.padding00[0] = 0xA0;
    packet.padding00[1] = 0xA1;
    packet.padding00[2] = 0xA2;
    packet.padding00[3] = 0xA3;
    packet.padding00[4] = 0xA4;
    packet.padding00[5] = 0xA5;
    packet.padding00[6] = 0xA6;
    packet.padding00[7] = 0xA7;
    return packet;
}

auto encodedEffectEndPacket() -> EffectEndPacketBytes
{
    const auto packet = effectEndPacket();
    auto       bytes  = EffectEndPacketBytes{};
    std::memcpy(bytes.data(), &packet, bytes.size());
    return bytes;
}

auto digPacket() -> GP_CLI_COMMAND_DIG
{
    auto packet         = GP_CLI_COMMAND_DIG{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_DIG);
    packet.header.size = sizeof(GP_CLI_COMMAND_DIG) / 4;
    packet.header.sync = 0xBEEF;
    packet.UniqueNo    = 0x11223344;
    packet.para        = 0x55667788;
    packet.ActIndex    = 0x99AA;
    packet.mode        = 0xBB;
    packet.padding00   = 0xCC;
    return packet;
}

auto encodedDigPacket() -> DigPacketBytes
{
    const auto packet = digPacket();
    auto       bytes  = DigPacketBytes{};
    std::memcpy(bytes.data(), &packet, bytes.size());
    return bytes;
}

auto recipePureValidation(std::uint16_t skill, std::uint16_t level, std::uint16_t mode, std::uint16_t param1, std::uint16_t param4) -> PacketValidationResult
{
    auto packet   = GP_CLI_COMMAND_RECIPE{};
    packet.skill  = skill;
    packet.level  = level;
    packet.Mode   = mode;
    packet.Param1 = param1;
    packet.Param4 = param4;
    return packet.validate(nullptr, nullptr);
}

auto testRecipeLayoutAndMetadata() -> bool
{
    bool ok = true;

    ok = expectEqualString(GP_CLI_COMMAND_RECIPE::name, "GP_CLI_COMMAND_RECIPE", "RECIPE static name") && ok;
    ok = expectTrue(GP_CLI_COMMAND_RECIPE::packetId == PacketC2S::GP_CLI_COMMAND_RECIPE, "RECIPE static packetId") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_HEADER), 4, "sizeof(GP_CLI_HEADER)") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_RECIPE), 20, "sizeof(GP_CLI_COMMAND_RECIPE)") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_RECIPE, header), 0, "RECIPE header offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_RECIPE, skill), 4, "skill offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_RECIPE, level), 6, "level offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_RECIPE, Param0), 8, "Param0 offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_RECIPE, Mode), 10, "Mode offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_RECIPE, Param1), 12, "Param1 offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_RECIPE, Param2), 14, "Param2 offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_RECIPE, Param3), 16, "Param3 offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_RECIPE, Param4), 18, "Param4 offset") && ok;
    ok = expectBytes(encodedRecipePacket(),
                     RecipePacketBytes{ 0x58, 0x0A, 0xEF, 0xBE,
                                        0x02, 0x01, 0x04, 0x03,
                                        0x06, 0x05, 0x02, 0x00,
                                        0x08, 0x07, 0x0A, 0x09,
                                        0x0C, 0x0B, 0x0E, 0x0D },
                     "RECIPE encoded packet") &&
         ok;

    const auto packet = recipePacket();
    ok                = expectEqualInt(packet.skill, 0x0102, "RECIPE skill storage") && ok;
    ok                = expectEqualInt(packet.level, 0x0304, "RECIPE level storage") && ok;
    ok                = expectEqualInt(packet.Param0, 0x0506, "RECIPE Param0 storage") && ok;
    ok                = expectEqualInt(packet.Mode, 2, "RECIPE Mode storage") && ok;
    ok                = expectEqualInt(packet.Param1, 0x0708, "RECIPE Param1 storage") && ok;
    ok                = expectEqualInt(packet.Param2, 0x090A, "RECIPE Param2 storage") && ok;
    ok                = expectEqualInt(packet.Param3, 0x0B0C, "RECIPE Param3 storage") && ok;
    ok                = expectEqualInt(packet.Param4, 0x0D0E, "RECIPE Param4 storage") && ok;

    return ok;
}

auto testRecipeModeEnumValuesAndValidationDomain() -> bool
{
    bool ok = true;

    ok = expectTrue((std::is_same_v<std::underlying_type_t<GP_CLI_COMMAND_RECIPE_MODE>, std::uint8_t>), "RECIPE mode enum underlying type") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_RECIPE_MODE::RequestAvailableRankList), 1, "RequestAvailableRankList enum") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_RECIPE_MODE::RequestAvailableRecipeList), 2, "RequestAvailableRecipeList enum") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_RECIPE_MODE::RequestRecipeMaterials), 3, "RequestRecipeMaterials enum") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_RECIPE_MODE::RequestCampaignOpsRecipe), 4, "RequestCampaignOpsRecipe enum") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_RECIPE_MODE::SubmitCampaignOpsMaterialList), 5, "SubmitCampaignOpsMaterialList enum") && ok;
    ok = expectValid(recipePureValidation(1, 0, 1, 17, 12), "rank list validation") && ok;
    ok = expectValid(recipePureValidation(8, 110, 2, 16, 11), "recipe list validation") && ok;
    ok = expectValid(recipePureValidation(4, 50, 3, 17, 0), "materials validation") && ok;
    ok = expectInvalidError(recipePureValidation(0, 0, 1, 16, 0), "skill out of range: 0 not in [1, 8]", "low skill validation") && ok;
    ok = expectInvalidError(recipePureValidation(9, 0, 1, 16, 0), "skill out of range: 9 not in [1, 8]", "high skill validation") && ok;
    ok = expectInvalidError(recipePureValidation(1, 111, 1, 16, 0), "level out of range: 111 not in [0, 110]", "high level validation") && ok;
    ok = expectInvalidError(recipePureValidation(1, 0, 0, 16, 0), "Mode out of range: 0 not in [1, 3]", "zero mode validation") && ok;
    ok = expectInvalidError(recipePureValidation(1, 0, 4, 16, 0), "Mode out of range: 4 not in [1, 3]", "campaign mode validation") && ok;
    ok = expectInvalidError(recipePureValidation(1, 0, 5, 16, 0), "Mode out of range: 5 not in [1, 3]", "submit campaign mode validation") && ok;
    ok = expectInvalidError(recipePureValidation(1, 0, 2, 16, 12), "Param4 out of range: 12 not in [0, 11]", "recipe list Param4 validation") && ok;
    ok = expectInvalidError(recipePureValidation(1, 0, 2, 17, 11), "Param1 is not a multiple of 16.", "recipe list pagination validation") && ok;
    ok = expectInvalidError(recipePureValidation(1, 0, 3, 17, 12), "Param4 out of range: 12 not in [0, 11]", "materials Param4 validation") && ok;

    return ok;
}

auto testEffectEndLayoutPayloadAndIgnoredScope() -> bool
{
    bool ok = true;

    ok = expectEqualString(GP_CLI_COMMAND_EFFECTEND::name, "GP_CLI_COMMAND_EFFECTEND", "EFFECTEND static name") && ok;
    ok = expectTrue(GP_CLI_COMMAND_EFFECTEND::packetId == PacketC2S::GP_CLI_COMMAND_EFFECTEND, "EFFECTEND static packetId") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_EFFECTEND), 16, "sizeof(GP_CLI_COMMAND_EFFECTEND)") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_EFFECTEND, header), 0, "EFFECTEND header offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_EFFECTEND, effectpara), 4, "effectpara offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_EFFECTEND, padding00), 8, "EFFECTEND padding00 offset") && ok;
    ok = expectBytes(encodedEffectEndPacket(),
                     EffectEndPacketBytes{ 0x59, 0x08, 0xEF, 0xBE,
                                           0x44, 0x33, 0x22, 0x11,
                                           0xA0, 0xA1, 0xA2, 0xA3,
                                           0xA4, 0xA5, 0xA6, 0xA7 },
                     "EFFECTEND encoded packet") &&
         ok;

    const auto packet = effectEndPacket();
    ok                = expectEqualInt(packet.effectpara, 0x11223344, "EFFECTEND effectpara storage") && ok;
    for (std::size_t i = 0; i < sizeof(packet.padding00); ++i)
    {
        ok = expectEqualInt(packet.padding00[i], 0xA0 + i, "EFFECTEND padding00 storage") && ok;
    }

    // GP_CLI_COMMAND_EFFECTEND::validate only blocks live BlockedState::InEvent.
    // There is no state-free scalar validation to exercise with PacketValidator(nullptr).
    return ok;
}

auto testDigLayoutPayloadAndIgnoredScope() -> bool
{
    bool ok = true;

    ok = expectEqualString(GP_CLI_COMMAND_DIG::name, "GP_CLI_COMMAND_DIG", "DIG static name") && ok;
    ok = expectTrue(GP_CLI_COMMAND_DIG::packetId == PacketC2S::GP_CLI_COMMAND_DIG, "DIG static packetId") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_DIG), 16, "sizeof(GP_CLI_COMMAND_DIG)") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_DIG, header), 0, "DIG header offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_DIG, UniqueNo), 4, "UniqueNo offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_DIG, para), 8, "para offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_DIG, ActIndex), 12, "ActIndex offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_DIG, mode), 14, "mode offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_DIG, padding00), 15, "DIG padding00 offset") && ok;
    ok = expectBytes(encodedDigPacket(),
                     DigPacketBytes{ 0x63, 0x08, 0xEF, 0xBE,
                                     0x44, 0x33, 0x22, 0x11,
                                     0x88, 0x77, 0x66, 0x55,
                                     0xAA, 0x99, 0xBB, 0xCC },
                     "DIG encoded packet") &&
         ok;

    const auto packet = digPacket();
    ok                = expectEqualInt(packet.UniqueNo, 0x11223344, "DIG UniqueNo storage") && ok;
    ok                = expectEqualInt(packet.para, 0x55667788, "DIG para storage") && ok;
    ok                = expectEqualInt(packet.ActIndex, 0x99AA, "DIG ActIndex storage") && ok;
    ok                = expectEqualInt(packet.mode, 0xBB, "DIG mode storage") && ok;
    ok                = expectEqualInt(packet.padding00, 0xCC, "DIG padding00 storage") && ok;

    // GP_CLI_COMMAND_DIG::validate only blocks live BlockedState::InEvent.
    // There is no state-free scalar validation to exercise with PacketValidator(nullptr).
    return ok;
}

} // namespace

auto runC2SRecipeMiscPacketSelfTests() -> bool
{
    bool ok = true;

    ok = testRecipeLayoutAndMetadata() && ok;
    ok = testRecipeModeEnumValuesAndValidationDomain() && ok;
    ok = testEffectEndLayoutPayloadAndIgnoredScope() && ok;
    ok = testDigLayoutPayloadAndIgnoredScope() && ok;

    return ok;
}
