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

#include "test_c2s_action_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include "map/packets/c2s/0x01a_action.h"

namespace
{

using PacketBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_ACTION)>;

struct EnumCase
{
    std::uint64_t actual;
    std::uint64_t expected;
    std::string   label;
};

template <typename T>
auto enumValue(T value) -> std::uint64_t
{
    return static_cast<std::uint64_t>(value);
}

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s ACTION packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s ACTION packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectFalse(bool actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "c2s ACTION packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s ACTION packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

auto expectBytes(const PacketBytes& actual, const PacketBytes& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s ACTION packet self-test failed: " << label << " got";
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

auto expectValidation(const PacketValidationResult& result, bool expectedValid, std::string_view expectedError, const std::string& label) -> bool
{
    bool ok = true;
    if (expectedValid)
    {
        ok = expectTrue(result.valid(), label + " valid") && ok;
    }
    else
    {
        ok = expectFalse(result.valid(), label + " valid") && ok;
    }
    ok = expectEqualString(result.errorString(), expectedError, label + " error string") && ok;
    return ok;
}

auto expectAll(const std::vector<EnumCase>& tests) -> bool
{
    bool ok = true;
    for (const auto& test : tests)
    {
        ok = expectEqualInt(test.actual, test.expected, test.label) && ok;
    }
    return ok;
}

auto encodedRawAction() -> PacketBytes
{
    auto packet         = GP_CLI_COMMAND_ACTION{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_ACTION);
    packet.header.size = sizeof(GP_CLI_COMMAND_ACTION) / 4;
    packet.header.sync = 0xBEEF;
    packet.UniqueNo    = 0x11223344;
    packet.ActIndex    = 0x5566;
    packet.ActionID    = GP_CLI_COMMAND_ACTION_ACTIONID::Weaponskill;
    packet.ActionBuf[0] = 0x01020304;
    packet.ActionBuf[1] = 0xA0B0C0D0;
    packet.ActionBuf[2] = 0xFFEEDDCC;
    packet.ActionBuf[3] = 0x87654321;

    auto bytes = PacketBytes{};
    std::memcpy(bytes.data(), &packet, bytes.size());
    return bytes;
}

auto encodedCastMagicAction() -> PacketBytes
{
    auto packet          = GP_CLI_COMMAND_ACTION{};
    packet.header.id    = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_ACTION);
    packet.header.size  = sizeof(GP_CLI_COMMAND_ACTION) / 4;
    packet.header.sync  = 0xBEEF;
    packet.UniqueNo     = 0x01020304;
    packet.ActIndex     = 0x0506;
    packet.ActionID     = GP_CLI_COMMAND_ACTION_ACTIONID::CastMagic;
    packet.CastMagic.SpellId = 0xABCDEF01;
    packet.CastMagic.PosX    = 1.5F;
    packet.CastMagic.PosZ    = -2.25F;
    packet.CastMagic.PosY    = 0.125F;

    auto bytes = PacketBytes{};
    std::memcpy(bytes.data(), &packet, bytes.size());
    return bytes;
}

auto encodedBlockAidAction() -> PacketBytes
{
    auto packet              = GP_CLI_COMMAND_ACTION{};
    packet.header.id        = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_ACTION);
    packet.header.size      = sizeof(GP_CLI_COMMAND_ACTION) / 4;
    packet.header.sync      = 0xBEEF;
    packet.UniqueNo         = 0x11223344;
    packet.ActIndex         = 0x5566;
    packet.ActionID         = GP_CLI_COMMAND_ACTION_ACTIONID::Blockaid;
    packet.BlockAid.StatusId = GP_CLI_COMMAND_ACTION_BLOCKAID::Toggle;
    packet.BlockAid.unknown  = 0xDEADBEEF;

    auto bytes = PacketBytes{};
    std::memcpy(bytes.data(), &packet, bytes.size());
    return bytes;
}

auto testDeclarationMetadataAndLayout() -> bool
{
    bool ok = true;

    ok = expectEqualString(GP_CLI_COMMAND_ACTION::name, "GP_CLI_COMMAND_ACTION", "static name") && ok;
    ok = expectTrue(GP_CLI_COMMAND_ACTION::packetId == PacketC2S::GP_CLI_COMMAND_ACTION, "static packetId") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_HEADER), 4, "sizeof(GP_CLI_HEADER)") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_ACTION), 28, "sizeof(GP_CLI_COMMAND_ACTION)") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ACTION, header), 0, "header offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ACTION, UniqueNo), 4, "UniqueNo offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ACTION, ActIndex), 8, "ActIndex offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ACTION, ActionID), 10, "ActionID offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ACTION, ActionBuf), 12, "ActionBuf offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ACTION, CastMagic), 12, "CastMagic union offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ACTION, Weaponskill), 12, "Weaponskill union offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ACTION, JobAbility), 12, "JobAbility union offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ACTION, MonsterSkill), 12, "MonsterSkill union offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ACTION, HomepointMenu), 12, "HomepointMenu union offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ACTION, TractorMenu), 12, "TractorMenu union offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ACTION, BlockAid), 12, "BlockAid union offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ACTION, Mount), 12, "Mount union offset") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_ACTION{}.ActionBuf), 16, "sizeof(ActionBuf)") && ok;

    return ok;
}

auto testActionIDEnumValues() -> bool
{
    return expectAll({
        { enumValue(GP_CLI_COMMAND_ACTION_ACTIONID::Talk), 0x00, "ActionID::Talk" },
        { enumValue(GP_CLI_COMMAND_ACTION_ACTIONID::Attack), 0x02, "ActionID::Attack" },
        { enumValue(GP_CLI_COMMAND_ACTION_ACTIONID::CastMagic), 0x03, "ActionID::CastMagic" },
        { enumValue(GP_CLI_COMMAND_ACTION_ACTIONID::AttackOff), 0x04, "ActionID::AttackOff" },
        { enumValue(GP_CLI_COMMAND_ACTION_ACTIONID::Help), 0x05, "ActionID::Help" },
        { enumValue(GP_CLI_COMMAND_ACTION_ACTIONID::Weaponskill), 0x07, "ActionID::Weaponskill" },
        { enumValue(GP_CLI_COMMAND_ACTION_ACTIONID::JobAbility), 0x09, "ActionID::JobAbility" },
        { enumValue(GP_CLI_COMMAND_ACTION_ACTIONID::HomepointMenu), 0x0B, "ActionID::HomepointMenu" },
        { enumValue(GP_CLI_COMMAND_ACTION_ACTIONID::Assist), 0x0C, "ActionID::Assist" },
        { enumValue(GP_CLI_COMMAND_ACTION_ACTIONID::RaiseMenu), 0x0D, "ActionID::RaiseMenu" },
        { enumValue(GP_CLI_COMMAND_ACTION_ACTIONID::Fish), 0x0E, "ActionID::Fish" },
        { enumValue(GP_CLI_COMMAND_ACTION_ACTIONID::ChangeTarget), 0x0F, "ActionID::ChangeTarget" },
        { enumValue(GP_CLI_COMMAND_ACTION_ACTIONID::Shoot), 0x10, "ActionID::Shoot" },
        { enumValue(GP_CLI_COMMAND_ACTION_ACTIONID::ChocoboDig), 0x11, "ActionID::ChocoboDig" },
        { enumValue(GP_CLI_COMMAND_ACTION_ACTIONID::Dismount), 0x12, "ActionID::Dismount" },
        { enumValue(GP_CLI_COMMAND_ACTION_ACTIONID::TractorMenu), 0x13, "ActionID::TractorMenu" },
        { enumValue(GP_CLI_COMMAND_ACTION_ACTIONID::SendResRdy), 0x14, "ActionID::SendResRdy" },
        { enumValue(GP_CLI_COMMAND_ACTION_ACTIONID::Quarry), 0x15, "ActionID::Quarry" },
        { enumValue(GP_CLI_COMMAND_ACTION_ACTIONID::Sprint), 0x16, "ActionID::Sprint" },
        { enumValue(GP_CLI_COMMAND_ACTION_ACTIONID::Scout), 0x17, "ActionID::Scout" },
        { enumValue(GP_CLI_COMMAND_ACTION_ACTIONID::Blockaid), 0x18, "ActionID::Blockaid" },
        { enumValue(GP_CLI_COMMAND_ACTION_ACTIONID::MonsterSkill), 0x19, "ActionID::MonsterSkill" },
        { enumValue(GP_CLI_COMMAND_ACTION_ACTIONID::Mount), 0x1A, "ActionID::Mount" },
    });
}

auto testMenuStatusEnumValues() -> bool
{
    return expectAll({
        { enumValue(GP_CLI_COMMAND_ACTION_BLOCKAID::Disable), 0, "BlockAid::Disable" },
        { enumValue(GP_CLI_COMMAND_ACTION_BLOCKAID::Enable), 1, "BlockAid::Enable" },
        { enumValue(GP_CLI_COMMAND_ACTION_BLOCKAID::Toggle), 2, "BlockAid::Toggle" },
        { enumValue(GP_CLI_COMMAND_ACTION_HOMEPOINTMENU::Accept), 0, "HomepointMenu::Accept" },
        { enumValue(GP_CLI_COMMAND_ACTION_HOMEPOINTMENU::MonstrosityCancel), 1, "HomepointMenu::MonstrosityCancel" },
        { enumValue(GP_CLI_COMMAND_ACTION_HOMEPOINTMENU::MonstrosityRetry), 2, "HomepointMenu::MonstrosityRetry" },
        { enumValue(GP_CLI_COMMAND_ACTION_RAISEMENU::Accept), 0, "RaiseMenu::Accept" },
        { enumValue(GP_CLI_COMMAND_ACTION_RAISEMENU::Reject), 1, "RaiseMenu::Reject" },
        { enumValue(GP_CLI_COMMAND_ACTION_TRACTORMENU::Accept), 0, "TractorMenu::Accept" },
        { enumValue(GP_CLI_COMMAND_ACTION_TRACTORMENU::Reject), 1, "TractorMenu::Reject" },
    });
}

auto testUnionPayloadLayouts() -> bool
{
    bool ok = true;

    ok = expectEqualInt(sizeof(ACTIONBUF_CASTMAGIC), 16, "sizeof(ACTIONBUF_CASTMAGIC)") && ok;
    ok = expectEqualInt(offsetof(ACTIONBUF_CASTMAGIC, SpellId), 0, "CastMagic SpellId offset") && ok;
    ok = expectEqualInt(offsetof(ACTIONBUF_CASTMAGIC, PosX), 4, "CastMagic PosX offset") && ok;
    ok = expectEqualInt(offsetof(ACTIONBUF_CASTMAGIC, PosZ), 8, "CastMagic PosZ offset") && ok;
    ok = expectEqualInt(offsetof(ACTIONBUF_CASTMAGIC, PosY), 12, "CastMagic PosY offset") && ok;

    ok = expectEqualInt(sizeof(ACTIONBUF_WEAPONSKILL), 16, "sizeof(ACTIONBUF_WEAPONSKILL)") && ok;
    ok = expectEqualInt(sizeof(ACTIONBUF_JOBABILITY), 16, "sizeof(ACTIONBUF_JOBABILITY)") && ok;
    ok = expectEqualInt(sizeof(ACTIONBUF_MONSTERSKILL), 16, "sizeof(ACTIONBUF_MONSTERSKILL)") && ok;
    ok = expectEqualInt(offsetof(ACTIONBUF_WEAPONSKILL, SkillId), 0, "Weaponskill SkillId offset") && ok;
    ok = expectEqualInt(offsetof(ACTIONBUF_WEAPONSKILL, unused), 4, "Weaponskill unused offset") && ok;
    ok = expectEqualInt(offsetof(ACTIONBUF_WEAPONSKILL, unknown), 12, "Weaponskill unknown offset") && ok;

    ok = expectEqualInt(sizeof(ACTIONBUF_HOMEPOINTMENU), 16, "sizeof(ACTIONBUF_HOMEPOINTMENU)") && ok;
    ok = expectEqualInt(offsetof(ACTIONBUF_HOMEPOINTMENU, StatusId), 0, "HomepointMenu StatusId offset") && ok;
    ok = expectEqualInt(offsetof(ACTIONBUF_HOMEPOINTMENU, unused), 4, "HomepointMenu unused offset") && ok;
    ok = expectEqualInt(offsetof(ACTIONBUF_HOMEPOINTMENU, unknown), 12, "HomepointMenu unknown offset") && ok;

    ok = expectEqualInt(sizeof(ACTIONBUF_TRACTORMENU), 16, "sizeof(ACTIONBUF_TRACTORMENU)") && ok;
    ok = expectEqualInt(offsetof(ACTIONBUF_TRACTORMENU, StatusId), 0, "TractorMenu StatusId offset") && ok;
    ok = expectEqualInt(offsetof(ACTIONBUF_TRACTORMENU, unused), 4, "TractorMenu unused offset") && ok;
    ok = expectEqualInt(offsetof(ACTIONBUF_TRACTORMENU, unknown), 12, "TractorMenu unknown offset") && ok;

    ok = expectEqualInt(sizeof(ACTIONBUF_BLOCKAID), 16, "sizeof(ACTIONBUF_BLOCKAID)") && ok;
    ok = expectEqualInt(offsetof(ACTIONBUF_BLOCKAID, StatusId), 0, "BlockAid StatusId offset") && ok;
    ok = expectEqualInt(offsetof(ACTIONBUF_BLOCKAID, unused), 4, "BlockAid unused offset") && ok;
    ok = expectEqualInt(offsetof(ACTIONBUF_BLOCKAID, unknown), 12, "BlockAid unknown offset") && ok;

    ok = expectEqualInt(sizeof(ACTIONBUF_MOUNT), 16, "sizeof(ACTIONBUF_MOUNT)") && ok;
    ok = expectEqualInt(offsetof(ACTIONBUF_MOUNT, MountId), 0, "Mount MountId offset") && ok;
    ok = expectEqualInt(offsetof(ACTIONBUF_MOUNT, unused), 4, "Mount unused offset") && ok;
    ok = expectEqualInt(offsetof(ACTIONBUF_MOUNT, unknown), 12, "Mount unknown offset") && ok;

    return ok;
}

auto testEncodedBytes() -> bool
{
    bool ok = true;

    ok = expectBytes(encodedRawAction(),
                     PacketBytes{ 0x1A, 0x0E, 0xEF, 0xBE,
                                  0x44, 0x33, 0x22, 0x11,
                                  0x66, 0x55, 0x07, 0x00,
                                  0x04, 0x03, 0x02, 0x01,
                                  0xD0, 0xC0, 0xB0, 0xA0,
                                  0xCC, 0xDD, 0xEE, 0xFF,
                                  0x21, 0x43, 0x65, 0x87 },
                     "raw ActionBuf encoded bytes") &&
         ok;

    ok = expectBytes(encodedCastMagicAction(),
                     PacketBytes{ 0x1A, 0x0E, 0xEF, 0xBE,
                                  0x04, 0x03, 0x02, 0x01,
                                  0x06, 0x05, 0x03, 0x00,
                                  0x01, 0xEF, 0xCD, 0xAB,
                                  0x00, 0x00, 0xC0, 0x3F,
                                  0x00, 0x00, 0x10, 0xC0,
                                  0x00, 0x00, 0x00, 0x3E },
                     "CastMagic encoded bytes") &&
         ok;

    ok = expectBytes(encodedBlockAidAction(),
                     PacketBytes{ 0x1A, 0x0E, 0xEF, 0xBE,
                                  0x44, 0x33, 0x22, 0x11,
                                  0x66, 0x55, 0x18, 0x00,
                                  0x02, 0x00, 0x00, 0x00,
                                  0x00, 0x00, 0x00, 0x00,
                                  0x00, 0x00, 0x00, 0x00,
                                  0xEF, 0xBE, 0xAD, 0xDE },
                     "BlockAid encoded bytes") &&
         ok;

    return ok;
}

auto testValidation() -> bool
{
    auto validTalk = GP_CLI_COMMAND_ACTION{};
    validTalk.ActionID = GP_CLI_COMMAND_ACTION_ACTIONID::Talk;

    auto validBlockAid = GP_CLI_COMMAND_ACTION{};
    validBlockAid.ActionID = GP_CLI_COMMAND_ACTION_ACTIONID::Blockaid;

    auto invalid = GP_CLI_COMMAND_ACTION{};
    invalid.ActionID = static_cast<GP_CLI_COMMAND_ACTION_ACTIONID>(0x01);

    bool ok = true;
    ok      = expectValidation(validTalk.validate(nullptr, nullptr), true, "", "Talk ActionID validation") && ok;
    ok      = expectValidation(validBlockAid.validate(nullptr, nullptr), true, "", "Blockaid ActionID validation") && ok;
    ok      = expectValidation(invalid.validate(nullptr, nullptr), false, "1 not a valid GP_CLI_COMMAND_ACTION_ACTIONID value.", "invalid ActionID validation") && ok;
    return ok;
}

} // namespace

auto runC2SActionPacketSelfTests() -> bool
{
    bool ok = true;

    ok = testDeclarationMetadataAndLayout() && ok;
    ok = testActionIDEnumValues() && ok;
    ok = testMenuStatusEnumValues() && ok;
    ok = testUnionPayloadLayouts() && ok;
    ok = testEncodedBytes() && ok;
    ok = testValidation() && ok;

    return ok;
}
