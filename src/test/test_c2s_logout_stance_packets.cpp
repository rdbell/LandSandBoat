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

#include "test_c2s_logout_stance_packets.h"

#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "map/packets/c2s/0x0e7_reqlogout.h"
#include "map/packets/c2s/0x0e8_camp.h"
#include "map/packets/c2s/0x0ea_sit.h"
#include "map/packets/c2s/0x0eb_reqsubmapnum.h"

namespace
{

using ReqLogoutBytes    = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_REQLOGOUT)>;
using CampBytes         = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_CAMP)>;
using SitBytes          = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_SIT)>;
using ReqSubmapNumBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_REQSUBMAPNUM)>;

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s logout/stance packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s logout/stance packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectFalse(bool actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "c2s logout/stance packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s logout/stance packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(const std::array<std::uint8_t, N>& actual, const std::array<std::uint8_t, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s logout/stance packet self-test failed: " << label << " got";
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

auto validateReqLogoutPure(std::uint16_t mode, std::uint16_t kind) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.oneOf<GP_CLI_COMMAND_REQLOGOUT_MODE>(mode)
        .oneOf<GP_CLI_COMMAND_REQLOGOUT_KIND>(kind);
    return validator;
}

auto validateCampPure(std::uint32_t mode) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.oneOf<GP_CLI_COMMAND_REQLOGOUT_MODE>(static_cast<std::uint16_t>(mode));
    return validator;
}

auto validateSitPure(std::uint32_t mode) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.oneOf<GP_CLI_COMMAND_SIT_MODE>(mode);
    return validator;
}

auto validateReqSubmapNumPure() -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    return validator;
}

auto makeReqLogoutPacket() -> GP_CLI_COMMAND_REQLOGOUT
{
    auto packet         = GP_CLI_COMMAND_REQLOGOUT{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_REQLOGOUT);
    packet.header.size = sizeof(GP_CLI_COMMAND_REQLOGOUT) / 4U;
    packet.header.sync = 0xBEEF;
    packet.Mode        = static_cast<std::uint16_t>(GP_CLI_COMMAND_REQLOGOUT_MODE::ShutdownOn);
    packet.Kind        = static_cast<std::uint16_t>(GP_CLI_COMMAND_REQLOGOUT_KIND::Shutdown);
    return packet;
}

auto makeCampPacket() -> GP_CLI_COMMAND_CAMP
{
    auto packet         = GP_CLI_COMMAND_CAMP{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_CAMP);
    packet.header.size = sizeof(GP_CLI_COMMAND_CAMP) / 4U;
    packet.header.sync = 0xBEEF;
    packet.Mode        = static_cast<std::uint32_t>(GP_CLI_COMMAND_CAMP_MODE::On);
    return packet;
}

auto makeSitPacket() -> GP_CLI_COMMAND_SIT
{
    auto packet         = GP_CLI_COMMAND_SIT{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_SIT);
    packet.header.size = sizeof(GP_CLI_COMMAND_SIT) / 4U;
    packet.header.sync = 0xBEEF;
    packet.Mode        = static_cast<std::uint32_t>(GP_CLI_COMMAND_SIT_MODE::Off);
    return packet;
}

auto makeReqSubmapNumPacket() -> GP_CLI_COMMAND_REQSUBMAPNUM
{
    auto packet         = GP_CLI_COMMAND_REQSUBMAPNUM{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_REQSUBMAPNUM);
    packet.header.size = sizeof(GP_CLI_COMMAND_REQSUBMAPNUM) / 4U;
    packet.header.sync = 0xBEEF;
    return packet;
}

auto testLogoutStanceLayoutsAndMetadata() -> bool
{
    bool ok = true;

    ok = expectEqualString(GP_CLI_COMMAND_REQLOGOUT::name, "GP_CLI_COMMAND_REQLOGOUT", "REQLOGOUT name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_REQLOGOUT::packetId), 0x0E7, "REQLOGOUT packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_REQLOGOUT), 8, "REQLOGOUT sizeof") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_REQLOGOUT, Mode), 4, "REQLOGOUT Mode offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_REQLOGOUT, Kind), 6, "REQLOGOUT Kind offset") && ok;

    ok = expectEqualString(GP_CLI_COMMAND_CAMP::name, "GP_CLI_COMMAND_CAMP", "CAMP name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_CAMP::packetId), 0x0E8, "CAMP packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_CAMP), 8, "CAMP sizeof") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_CAMP, Mode), 4, "CAMP Mode offset") && ok;

    ok = expectEqualString(GP_CLI_COMMAND_SIT::name, "GP_CLI_COMMAND_SIT", "SIT name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_SIT::packetId), 0x0EA, "SIT packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_SIT), 8, "SIT sizeof") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_SIT, Mode), 4, "SIT Mode offset") && ok;

    ok = expectEqualString(GP_CLI_COMMAND_REQSUBMAPNUM::name, "GP_CLI_COMMAND_REQSUBMAPNUM", "REQSUBMAPNUM name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_REQSUBMAPNUM::packetId), 0x0EB, "REQSUBMAPNUM packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_REQSUBMAPNUM), 4, "REQSUBMAPNUM sizeof") && ok;
    return ok;
}

auto testLogoutStanceEncodedBytesAndPayloads() -> bool
{
    bool ok = true;

    ok = expectBytes(encodedPacketBytes(makeReqLogoutPacket()), ReqLogoutBytes{ 0xE7, 0x04, 0xEF, 0xBE, 0x03, 0x00, 0x03, 0x00 }, "REQLOGOUT encoded packet") && ok;
    ok = expectBytes(encodedPacketBytes(makeCampPacket()), CampBytes{ 0xE8, 0x04, 0xEF, 0xBE, 0x01, 0x00, 0x00, 0x00 }, "CAMP encoded packet") && ok;
    ok = expectBytes(encodedPacketBytes(makeSitPacket()), SitBytes{ 0xEA, 0x04, 0xEF, 0xBE, 0x02, 0x00, 0x00, 0x00 }, "SIT encoded packet") && ok;
    ok = expectBytes(encodedPacketBytes(makeReqSubmapNumPacket()), ReqSubmapNumBytes{ 0xEB, 0x02, 0xEF, 0xBE }, "REQSUBMAPNUM encoded packet") && ok;
    return ok;
}

auto testLogoutStanceConstantsAndValidation() -> bool
{
    bool ok = true;

    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_REQLOGOUT_MODE::Toggle), 0, "REQLOGOUT_MODE::Toggle") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_REQLOGOUT_MODE::LogoutOn), 1, "REQLOGOUT_MODE::LogoutOn") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_REQLOGOUT_MODE::Off), 2, "REQLOGOUT_MODE::Off") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_REQLOGOUT_MODE::ShutdownOn), 3, "REQLOGOUT_MODE::ShutdownOn") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_REQLOGOUT_KIND::Logout), 1, "REQLOGOUT_KIND::Logout") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_REQLOGOUT_KIND::Shutdown), 3, "REQLOGOUT_KIND::Shutdown") && ok;
    ok = expectValid(validateReqLogoutPure(0, 1), "REQLOGOUT logout toggle validation") && ok;
    ok = expectValid(validateReqLogoutPure(3, 3), "REQLOGOUT shutdown on validation") && ok;
    ok = expectInvalidError(validateReqLogoutPure(4, 1), "4 not a valid GP_CLI_COMMAND_REQLOGOUT_MODE value.", "REQLOGOUT invalid mode validation") && ok;
    ok = expectInvalidError(validateReqLogoutPure(2, 2), "2 not a valid GP_CLI_COMMAND_REQLOGOUT_KIND value.", "REQLOGOUT invalid kind validation") && ok;
    ok = expectInvalidError(validateReqLogoutPure(4, 2), "4 not a valid GP_CLI_COMMAND_REQLOGOUT_MODE value.", "REQLOGOUT validation order") && ok;

    ok = expectEqualInt(static_cast<std::uint32_t>(GP_CLI_COMMAND_CAMP_MODE::Toggle), 0, "CAMP_MODE::Toggle") && ok;
    ok = expectEqualInt(static_cast<std::uint32_t>(GP_CLI_COMMAND_CAMP_MODE::On), 1, "CAMP_MODE::On") && ok;
    ok = expectEqualInt(static_cast<std::uint32_t>(GP_CLI_COMMAND_CAMP_MODE::Off), 2, "CAMP_MODE::Off") && ok;
    ok = expectValid(validateCampPure(0), "CAMP toggle validation") && ok;
    ok = expectValid(validateCampPure(1), "CAMP on validation") && ok;
    ok = expectValid(validateCampPure(2), "CAMP off validation") && ok;
    ok = expectValid(validateCampPure(3), "CAMP reqlogout shutdown-mode validation") && ok;
    ok = expectValid(validateCampPure(0x10003), "CAMP narrowed valid validation") && ok;
    ok = expectInvalidError(validateCampPure(4), "4 not a valid GP_CLI_COMMAND_REQLOGOUT_MODE value.", "CAMP invalid mode validation") && ok;

    ok = expectEqualInt(static_cast<std::uint32_t>(GP_CLI_COMMAND_SIT_MODE::Toggle), 0, "SIT_MODE::Toggle") && ok;
    ok = expectEqualInt(static_cast<std::uint32_t>(GP_CLI_COMMAND_SIT_MODE::On), 1, "SIT_MODE::On") && ok;
    ok = expectEqualInt(static_cast<std::uint32_t>(GP_CLI_COMMAND_SIT_MODE::Off), 2, "SIT_MODE::Off") && ok;
    ok = expectValid(validateSitPure(0), "SIT toggle validation") && ok;
    ok = expectValid(validateSitPure(1), "SIT on validation") && ok;
    ok = expectValid(validateSitPure(2), "SIT off validation") && ok;
    ok = expectInvalidError(validateSitPure(3), "3 not a valid GP_CLI_COMMAND_SIT_MODE value.", "SIT invalid mode validation") && ok;
    ok = expectValid(validateReqSubmapNumPure(), "REQSUBMAPNUM pure validation") && ok;
    return ok;
}

auto expectLeaveGameTransition(
    const reqlogout::LeaveGameTransition& actual,
    reqlogout::LeaveGameAction            expectedAction,
    uint16_t                               expectedPower,
    std::chrono::seconds                   expectedDuration,
    std::chrono::seconds                   expectedTick,
    const std::string&                     label) -> bool
{
    bool ok = true;
    ok      = expectEqualInt(static_cast<uint8_t>(actual.action), static_cast<uint8_t>(expectedAction), label + " action") && ok;
    ok      = expectEqualInt(actual.power, expectedPower, label + " power") && ok;
    ok      = expectEqualInt(actual.duration.count(), expectedDuration.count(), label + " duration") && ok;
    ok      = expectEqualInt(actual.tick.count(), expectedTick.count(), label + " tick") && ok;
    return ok;
}

auto testReqLogoutLeaveGameTransitions() -> bool
{
    using namespace std::chrono_literals;
    using Action = reqlogout::LeaveGameAction;

    struct TestCase
    {
        uint16_t                    mode;
        uint16_t                    kind;
        bool                        hasExistingEffect;
        Action                      action;
        uint16_t                    power;
        std::chrono::seconds        duration;
        std::chrono::seconds        tick;
        const char*                 label;
    };

    const auto cases = std::array{
        TestCase{ 0, 1, false, Action::Add,         1, 5s, 0s, "logout toggle adds" },
        TestCase{ 0, 1, true,  Action::Remove,      0, 0s, 0s, "logout toggle removes" },
        TestCase{ 1, 1, false, Action::Add,         1, 5s, 0s, "logout on adds" },
        TestCase{ 1, 1, true,  Action::UpdatePower, 1, 0s, 0s, "logout on updates" },
        TestCase{ 2, 1, false, Action::None,        0, 0s, 0s, "logout off absent no-op" },
        TestCase{ 2, 1, true,  Action::Remove,      0, 0s, 0s, "logout off removes" },
        TestCase{ 3, 1, false, Action::None,        0, 0s, 0s, "logout shutdown-on absent no-op" },
        TestCase{ 3, 1, true,  Action::None,        0, 0s, 0s, "logout shutdown-on existing no-op" },
        TestCase{ 0, 3, false, Action::Add,         3, 5s, 0s, "shutdown toggle adds" },
        TestCase{ 0, 3, true,  Action::Remove,      0, 0s, 0s, "shutdown toggle removes" },
        TestCase{ 1, 3, false, Action::None,        0, 0s, 0s, "shutdown logout-on absent no-op" },
        TestCase{ 1, 3, true,  Action::None,        0, 0s, 0s, "shutdown logout-on existing no-op" },
        TestCase{ 2, 3, false, Action::None,        0, 0s, 0s, "shutdown off absent no-op" },
        TestCase{ 2, 3, true,  Action::Remove,      0, 0s, 0s, "shutdown off removes" },
        TestCase{ 3, 3, false, Action::Add,         3, 5s, 0s, "shutdown on adds" },
        TestCase{ 3, 3, true,  Action::UpdatePower, 3, 0s, 0s, "shutdown on updates" },
        TestCase{ 4, 1, false, Action::None,        0, 0s, 0s, "invalid mode no-op" },
        TestCase{ 0, 2, true,  Action::None,        0, 0s, 0s, "invalid kind no-op" },
    };

    bool ok = true;
    for (const auto& test : cases)
    {
        ok = expectLeaveGameTransition(
                 reqlogout::LeaveGameTransitionFor(test.mode, test.kind, test.hasExistingEffect),
                 test.action,
                 test.power,
                 test.duration,
                 test.tick,
                 test.label) &&
             ok;
    }
    return ok;
}

auto expectCampHealingTransition(
    const camp::HealingTransition& actual,
    camp::HealingAction            expectedAction,
    bool                           expectedClearStateStack,
    bool                           expectedDisengageAutomatonPet,
    std::chrono::seconds           expectedTick,
    const std::string&             label) -> bool
{
    bool ok = true;
    ok      = expectEqualInt(static_cast<uint8_t>(actual.action), static_cast<uint8_t>(expectedAction), label + " action") && ok;
    ok      = expectEqualInt(actual.clearStateStack, expectedClearStateStack, label + " clear state stack") && ok;
    ok      = expectEqualInt(actual.disengageAutomatonPet, expectedDisengageAutomatonPet, label + " disengage automaton pet") && ok;
    ok      = expectEqualInt(actual.tick.count(), expectedTick.count(), label + " tick") && ok;
    return ok;
}

auto testCampHealingTransitions() -> bool
{
    using namespace std::chrono_literals;
    using Action = camp::HealingAction;

    struct TestCase
    {
        uint32_t                    mode;
        bool                        isHealing;
        bool                        hasAutomatonPet;
        uint8_t                     healingTickDelay;
        Action                      action;
        bool                        clearStateStack;
        bool                        disengageAutomatonPet;
        std::chrono::seconds        tick;
        const char*                 label;
    };

    const auto cases = std::array{
        TestCase{ 0, false, false, 10, Action::Add,    true,  false, 10s, "toggle enables when not healing" },
        TestCase{ 0, false, true,  10, Action::Add,    true,  true,  10s, "toggle enables and disengages automaton" },
        TestCase{ 0, true,  true,  10, Action::Remove, false, false, 0s,  "toggle disables when healing" },
        TestCase{ 1, false, false, 7,  Action::Add,    true,  false, 7s,  "on enables" },
        TestCase{ 1, true,  true,  7,  Action::Add,    true,  true,  7s,  "on preserves process behavior when healing" },
        TestCase{ 2, false, true,  9,  Action::Remove, false, false, 0s,  "off disables when not healing" },
        TestCase{ 2, true,  true,  9,  Action::Remove, false, false, 0s,  "off disables when healing" },
        TestCase{ 3, false, true,  6,  Action::None,   false, false, 0s,  "legacy accepted shutdown mode no-op" },
        TestCase{ 4, false, true,  6,  Action::None,   false, false, 0s,  "invalid mode no-op" },
    };

    bool ok = true;
    for (const auto& test : cases)
    {
        ok = expectCampHealingTransition(
                 camp::HealingTransitionFor(test.mode, test.isHealing, test.hasAutomatonPet, test.healingTickDelay),
                 test.action,
                 test.clearStateStack,
                 test.disengageAutomatonPet,
                 test.tick,
                 test.label) &&
             ok;
    }
    return ok;
}

auto expectSitTransition(
    const sit::Transition& actual,
    uint8_t                expectedAnimation,
    bool                   expectedUpdatePet,
    const std::string&     label) -> bool
{
    bool ok = true;
    ok      = expectTrue(actual.removeHealingSilently, label + " removes healing silently") && ok;
    ok      = expectEqualInt(actual.characterAnimation, expectedAnimation, label + " character animation") && ok;
    ok      = expectTrue(actual.updateCharacterHP, label + " updates character HP") && ok;
    ok      = expectEqualInt(actual.updatePet, expectedUpdatePet, label + " updates pet") && ok;
    return ok;
}

auto testSitTransitions() -> bool
{
    constexpr auto animationNone = uint8_t{ 0 };  // ANIMATION_NONE
    constexpr auto animationSit  = uint8_t{ 47 }; // ANIMATION_SIT

    struct TestCase
    {
        uint32_t     mode;
        uint8_t      currentAnimation;
        sit::PetKind petKind;
        uint8_t      expectedAnimation;
        bool         expectedUpdatePet;
        const char*  label;
    };

    const auto cases = std::array{
        TestCase{ 0, animationNone, sit::PetKind::None,      animationSit,  false, "toggle sits without pet" },
        TestCase{ 0, animationSit,  sit::PetKind::Other,     animationNone, false, "toggle stands with other pet" },
        TestCase{ 1, 23,            sit::PetKind::WYVERN,    animationSit,  true,  "on sits and updates wyvern" },
        TestCase{ 2, animationSit,  sit::PetKind::AUTOMATON, animationNone, true,  "off stands and updates automaton" },
        TestCase{ 3, 23,             sit::PetKind::WYVERN,    23,             true,  "invalid mode preserves animation" },
    };

    bool ok = true;
    for (const auto& test : cases)
    {
        ok = expectSitTransition(
                 sit::TransitionFor(test.mode, test.currentAnimation, test.petKind),
                 test.expectedAnimation,
                 test.expectedUpdatePet,
                 test.label) &&
             ok;
    }
    return ok;
}

} // namespace

auto runC2SLogoutStancePacketSelfTests() -> bool
{
    return testLogoutStanceLayoutsAndMetadata() &&
           testLogoutStanceEncodedBytesAndPayloads() &&
           testLogoutStanceConstantsAndValidation() &&
           testReqLogoutLeaveGameTransitions() &&
           testCampHealingTransitions() &&
           testSitTransitions();
}
