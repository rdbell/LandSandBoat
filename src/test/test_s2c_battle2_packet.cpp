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

#include "test_s2c_battle2_packet.h"

#include <array>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "common/utils.h"
#include "map/action/action.h"
#include "map/enums/four_cc.h"
#include "map/packets/s2c/0x028_battle2.h"

namespace
{

using namespace std::chrono_literals;

constexpr auto battle2WorkSizeOffset = sizeof(GP_SERV_HEADER);
constexpr auto battle2BitOffset      = 8 * (sizeof(GP_SERV_HEADER) + 1);

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto readBits(CBasicPacket& packet, std::uint32_t& bitOffset, const uint8 bitCount) -> std::uint64_t
{
    const auto value = unpackBitsBE(packetData(packet), bitOffset, bitCount);
    bitOffset += bitCount;
    return value;
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c battle2 packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c battle2 packet self-test failed: " << label << " got";
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
            std::cerr << "s2c battle2 packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto makeFullAction() -> action_t
{
    auto action       = action_t{};
    action.actorId    = 0x11223344;
    action.actiontype = ActionCategory::MagicFinish;
    action.actionid   = 0x55667788;
    action.recast     = std::chrono::seconds{ 37 };

    auto& target            = action.addTarget(0xAABBCCDD);
    auto& result            = target.addResult();
    result.resolution       = ActionResolution::Guard;
    result.kind             = 3;
    result.animation        = static_cast<ActionAnimation>(0x345);
    result.info             = static_cast<ActionInfo>(static_cast<uint8_t>(ActionInfo::Defeated) | static_cast<uint8_t>(ActionInfo::CriticalHit));
    result.hitDistortion    = HitDistortion::Heavy;
    result.knockback        = Knockback::Level5;
    result.param            = -1;
    result.messageID        = MsgBasic::TargetTakesDamage;
    result.modifier         = static_cast<ActionModifier>(static_cast<std::uint32_t>(ActionModifier::MagicBurst) | static_cast<std::uint32_t>(ActionModifier::Immunobreak));
    result.additionalEffect = ActionProcAddEffect::FireDamage;
    result.addEffectInfo    = 0x0F;
    result.addEffectParam   = -2;
    result.addEffectMessage = MsgBasic::AddEffectDamage;
    result.spikesEffect     = ActionReactKind::Counter;
    result.spikesInfo       = 0x0A;
    result.spikesParam      = 0x3FFE;
    result.spikesMessage    = MsgBasic::SpikesEffectDmg;
    return action;
}

auto testBattle2FullActionPacking() -> bool
{
    auto action = makeFullAction();
    auto packet = GP_SERV_COMMAND_BATTLE2(action);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x028, "BATTLE2 type") && ok;
    ok      = expectEqualUInt(packet.getSize(), 44, "BATTLE2 size") && ok;
    ok      = expectEqualUInt(packetData(packet)[battle2WorkSizeOffset], 43, "BATTLE2 work size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x28, 0x16, 0xEF, 0xBE }, "BATTLE2 header") && ok;

    std::uint32_t bitOffset = battle2BitOffset;
    ok                      = expectEqualUInt(readBits(packet, bitOffset, 32), 0x11223344, "action actorId") && ok;
    ok                      = expectEqualUInt(readBits(packet, bitOffset, 6), 1, "action target count") && ok;
    ok                      = expectEqualUInt(readBits(packet, bitOffset, 4), 0, "action result summary") && ok;
    ok                      = expectEqualUInt(readBits(packet, bitOffset, 4), static_cast<std::uint8_t>(ActionCategory::MagicFinish), "action category") && ok;
    ok                      = expectEqualUInt(readBits(packet, bitOffset, 32), 0x55667788, "action id") && ok;
    ok                      = expectEqualUInt(readBits(packet, bitOffset, 32), 37, "action recast seconds") && ok;

    ok = expectEqualUInt(readBits(packet, bitOffset, 32), 0xAABBCCDD, "target actorId") && ok;
    ok = expectEqualUInt(readBits(packet, bitOffset, 4), 1, "target result count") && ok;
    ok = expectEqualUInt(readBits(packet, bitOffset, 3), static_cast<std::uint8_t>(ActionResolution::Guard), "result resolution") && ok;
    ok = expectEqualUInt(readBits(packet, bitOffset, 2), 3, "result kind") && ok;
    ok = expectEqualUInt(readBits(packet, bitOffset, 12), 0x345, "result animation") && ok;
    ok = expectEqualUInt(readBits(packet, bitOffset, 5), 3, "result info") && ok;
    ok = expectEqualUInt(readBits(packet, bitOffset, 2), static_cast<std::uint8_t>(HitDistortion::Heavy), "result hit distortion") && ok;
    ok = expectEqualUInt(readBits(packet, bitOffset, 3), static_cast<std::uint8_t>(Knockback::Level5), "result knockback") && ok;
    ok = expectEqualUInt(readBits(packet, bitOffset, 17), 0x1FFFF, "result param") && ok;
    ok = expectEqualUInt(readBits(packet, bitOffset, 10), static_cast<std::uint16_t>(MsgBasic::TargetTakesDamage), "result message") && ok;
    ok = expectEqualUInt(readBits(packet, bitOffset, 31), 0x0C, "result modifier") && ok;

    ok = expectEqualUInt(readBits(packet, bitOffset, 1), 1, "result has additional effect") && ok;
    ok = expectEqualUInt(readBits(packet, bitOffset, 6), static_cast<std::uint8_t>(ActionProcAddEffect::FireDamage), "result additional effect kind") && ok;
    ok = expectEqualUInt(readBits(packet, bitOffset, 4), 0x0F, "result additional effect info") && ok;
    ok = expectEqualUInt(readBits(packet, bitOffset, 17), 0x1FFFE, "result additional effect param") && ok;
    ok = expectEqualUInt(readBits(packet, bitOffset, 10), static_cast<std::uint16_t>(MsgBasic::AddEffectDamage), "result additional effect message") && ok;

    ok = expectEqualUInt(readBits(packet, bitOffset, 1), 1, "result has reaction") && ok;
    ok = expectEqualUInt(readBits(packet, bitOffset, 6), static_cast<std::uint8_t>(ActionReactKind::Counter), "result reaction kind") && ok;
    ok = expectEqualUInt(readBits(packet, bitOffset, 4), 0x0A, "result reaction info") && ok;
    ok = expectEqualUInt(readBits(packet, bitOffset, 14), 0x3FFE, "result reaction param") && ok;
    ok = expectEqualUInt(readBits(packet, bitOffset, 10), static_cast<std::uint16_t>(MsgBasic::SpikesEffectDmg), "result reaction message") && ok;

    ok = expectEqualUInt(bitOffset, 344, "BATTLE2 final bit offset") && ok;
    ok = expectZeroRange(packet, packet.getSize(), PACKET_SIZE, "BATTLE2 tail") && ok;
    return ok;
}

auto testBattle2NormalizesBasicAttack() -> bool
{
    auto action       = action_t{};
    action.actorId    = 0x01020304;
    action.actiontype = ActionCategory::BasicAttack;
    action.actionid   = 0xDEADBEEF;
    action.recast     = std::chrono::seconds{ 90 };

    auto& target     = action.addTarget(0x05060708);
    auto& result     = target.addResult();
    result.kind      = 0;
    result.messageID = MsgBasic::AttackHits;

    auto packet = GP_SERV_COMMAND_BATTLE2(action);

    bool ok = true;
    ok      = expectEqualUInt(packet.getSize(), 36, "normalized BATTLE2 size") && ok;
    ok      = expectEqualUInt(packetData(packet)[battle2WorkSizeOffset], 35, "normalized BATTLE2 work size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x28, 0x12, 0x00, 0x00 }, "normalized BATTLE2 header") && ok;

    std::uint32_t bitOffset = battle2BitOffset;
    ok                      = expectEqualUInt(readBits(packet, bitOffset, 32), 0x01020304, "normalized actorId") && ok;
    ok                      = expectEqualUInt(readBits(packet, bitOffset, 6), 1, "normalized target count") && ok;
    ok                      = expectEqualUInt(readBits(packet, bitOffset, 4), 0, "normalized result summary") && ok;
    ok                      = expectEqualUInt(readBits(packet, bitOffset, 4), static_cast<std::uint8_t>(ActionCategory::BasicAttack), "normalized category") && ok;
    ok                      = expectEqualUInt(readBits(packet, bitOffset, 32), static_cast<std::uint32_t>(FourCC::BasicAttack), "normalized action id") && ok;
    ok                      = expectEqualUInt(readBits(packet, bitOffset, 32), 0, "normalized recast") && ok;
    ok                      = expectEqualUInt(readBits(packet, bitOffset, 32), 0x05060708, "normalized target actorId") && ok;
    ok                      = expectEqualUInt(readBits(packet, bitOffset, 4), 1, "normalized result count") && ok;
    ok                      = expectEqualUInt(readBits(packet, bitOffset, 3), static_cast<std::uint8_t>(ActionResolution::Hit), "normalized result resolution") && ok;
    ok                      = expectEqualUInt(readBits(packet, bitOffset, 2), 1, "normalized result kind") && ok;
    return ok;
}

auto testBattle2FloorsNegativeFractionalRecast() -> bool
{
    struct TestCase
    {
        std::chrono::milliseconds recast;
        std::uint32_t             expected;
        std::string               label;
    };

    const auto cases = std::array{
        TestCase{ -500ms, 0xFFFFFFFF, "negative half-second recast" },
        TestCase{ -1500ms, 0xFFFFFFFE, "negative one-and-a-half-second recast" },
    };

    bool ok = true;
    for (const auto& test : cases)
    {
        auto action       = action_t{};
        action.actiontype = ActionCategory::MagicFinish;
        action.recast     = test.recast;

        auto          packet     = GP_SERV_COMMAND_BATTLE2(action);
        std::uint32_t bitOffset  = battle2BitOffset + 32 + 6 + 4 + 4 + 32;
        const auto    wireRecast = readBits(packet, bitOffset, 32);
        ok                       = expectEqualUInt(wireRecast, test.expected, test.label) && ok;
    }
    return ok;
}

auto testBattle2ProcVariantsShareWireValue() -> bool
{
    const auto effects = std::array<ActionProcKind, 2>{
        ActionProcKind{ ActionProcAddEffect::Stun },
        ActionProcKind{ ActionProcSkillChain::Umbra },
    };

    bool ok = true;
    for (const auto& effect : effects)
    {
        auto action                                      = action_t{};
        action.actiontype                                = ActionCategory::MagicFinish;
        action.addTarget(1).addResult().additionalEffect = effect;

        auto          packet    = GP_SERV_COMMAND_BATTLE2(action);
        std::uint32_t bitOffset = battle2BitOffset + 32 + 6 + 4 + 4 + 32 + 32 + 32 + 4 + 85;
        ok                      = expectEqualUInt(readBits(packet, bitOffset, 1), 1, "proc variant presence") && ok;
        ok                      = expectEqualUInt(readBits(packet, bitOffset, 6), 16, "proc variant shared wire value") && ok;
    }
    return ok;
}

auto testBattle2CapsTargetsAndResults() -> bool
{
    auto manyTargets       = action_t{};
    manyTargets.actorId    = 0x11111111;
    manyTargets.actiontype = ActionCategory::AbilityStart;
    for (std::uint32_t i = 0; i < 16; ++i)
    {
        manyTargets.addTarget(0x20000000 + i);
    }

    auto targetPacket = GP_SERV_COMMAND_BATTLE2(manyTargets);

    bool ok = true;
    ok      = expectEqualUInt(packetData(targetPacket)[battle2WorkSizeOffset], 87, "target cap work size") && ok;

    std::uint32_t bitOffset = battle2BitOffset + 32;
    ok                      = expectEqualUInt(readBits(targetPacket, bitOffset, 6), 15, "target cap count") && ok;

    bitOffset = battle2BitOffset + 32 + 6 + 4 + 4 + 32 + 32;
    for (std::uint32_t i = 0; i < 15; ++i)
    {
        ok = expectEqualUInt(readBits(targetPacket, bitOffset, 32), 0x20000000 + i, "capped target actorId") && ok;
        ok = expectEqualUInt(readBits(targetPacket, bitOffset, 4), 0, "capped target result count") && ok;
    }

    auto manyResults       = action_t{};
    manyResults.actorId    = 0x33333333;
    manyResults.actiontype = ActionCategory::MagicStart;
    auto& target           = manyResults.addTarget(0x44444444);
    for (std::uint32_t i = 0; i < 9; ++i)
    {
        auto& result      = target.addResult();
        result.messageID  = MsgBasic::AttackHits;
        result.param      = static_cast<int32>(i);
        result.resolution = ActionResolution::Hit;
    }

    auto resultPacket = GP_SERV_COMMAND_BATTLE2(manyResults);
    bitOffset         = battle2BitOffset + 32 + 6 + 4 + 4 + 32 + 32 + 32;
    ok                = expectEqualUInt(readBits(resultPacket, bitOffset, 4), 8, "result cap count") && ok;
    return ok;
}

} // namespace

auto runS2CBattle2PacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testBattle2FullActionPacking() && ok;
    ok      = testBattle2NormalizesBasicAttack() && ok;
    ok      = testBattle2FloorsNegativeFractionalRecast() && ok;
    ok      = testBattle2ProcVariantsShareWireValue() && ok;
    ok      = testBattle2CapsTargetsAndResults() && ok;
    return ok;
}
