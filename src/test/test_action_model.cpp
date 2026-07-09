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

#include "test_action_model.h"

#include "map/action/action.h"
#include "map/enums/four_cc.h"

#include <array>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <limits>
#include <type_traits>
#include <variant>
#include <vector>

namespace
{

using namespace std::chrono_literals;

static_assert(MAX_ACTION_TARGETS == 64);
static_assert(std::is_same_v<decltype(action_t{}.actorId), uint32>);
static_assert(std::is_same_v<decltype(action_t{}.actionid), uint32>);
static_assert(std::is_same_v<decltype(action_t{}.recast), timer::duration>);
static_assert(std::is_same_v<decltype(action_t{}.spellgroup), SPELLGROUP>);
static_assert(std::is_enum_v<SPELLGROUP>);
static_assert(sizeof(SPELLGROUP) >= sizeof(std::uint8_t));
static_assert(static_cast<int>(SPELLGROUP_NONE) == 0);
static_assert(static_cast<int>(SPELLGROUP_SONG) == 1);
static_assert(static_cast<int>(SPELLGROUP_BLACK) == 2);
static_assert(static_cast<int>(SPELLGROUP_BLUE) == 3);
static_assert(static_cast<int>(SPELLGROUP_NINJUTSU) == 4);
static_assert(static_cast<int>(SPELLGROUP_SUMMONING) == 5);
static_assert(static_cast<int>(SPELLGROUP_WHITE) == 6);
static_assert(static_cast<int>(SPELLGROUP_GEOMANCY) == 7);
static_assert(static_cast<int>(SPELLGROUP_TRUST) == 8);
static_assert(std::is_same_v<decltype(action_target_t{}.actorId), uint32>);
static_assert(std::is_same_v<decltype(action_result_t{}.kind), uint8_t>);
static_assert(std::is_same_v<decltype(action_result_t{}.param), int32>);
static_assert(std::is_same_v<decltype(action_result_t{}.addEffectInfo), uint8_t>);
static_assert(std::is_same_v<decltype(action_result_t{}.addEffectParam), int32>);
static_assert(std::is_same_v<decltype(action_result_t{}.spikesInfo), uint8_t>);
static_assert(std::is_same_v<decltype(action_result_t{}.spikesParam), uint16>);

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "action model self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto testDefaults() -> bool
{
    const auto action = action_t{};
    const auto target = action_target_t{};
    const auto result = action_result_t{};

    bool ok = true;
    ok      = expect(action.actorId == 0, "default action actor ID") && ok;
    ok      = expect(action.actiontype == ActionCategory::None, "default action category") && ok;
    ok      = expect(action.actionid == 0, "default action ID") && ok;
    ok      = expect(action.recast == timer::duration::zero(), "default action recast") && ok;
    ok      = expect(action.spellgroup == SPELLGROUP_NONE, "default action spell group") && ok;
    ok      = expect(action.targets.empty(), "default action targets") && ok;
    ok      = expect(target.actorId == 0 && target.results.empty(), "default target") && ok;

    ok = expect(result.resolution == ActionResolution::Hit, "default resolution") && ok;
    ok = expect(result.kind == 0, "default kind") && ok;
    ok = expect(result.animation == ActionAnimation::None, "default animation") && ok;
    ok = expect(result.info == ActionInfo::None, "default info") && ok;
    ok = expect(result.hitDistortion == HitDistortion::None, "default hit distortion") && ok;
    ok = expect(result.knockback == Knockback::None, "default knockback") && ok;
    ok = expect(result.param == 0 && result.messageID == MsgBasic::None, "default value and message") && ok;
    ok = expect(result.modifier == ActionModifier::None, "default modifier") && ok;
    ok = expect(std::holds_alternative<ActionProcAddEffect>(result.additionalEffect), "default proc alternative") && ok;
    ok = expect(std::get<ActionProcAddEffect>(result.additionalEffect) == ActionProcAddEffect::None, "default proc value") && ok;
    ok = expect(!result.hasAdditionalEffect(), "default proc presence") && ok;
    ok = expect(result.addEffectInfo == 0 && result.addEffectParam == 0 && result.addEffectMessage == MsgBasic::None, "default proc fields") && ok;
    ok = expect(result.spikesEffect == ActionReactKind::None && result.spikesInfo == 0 && result.spikesParam == 0 && result.spikesMessage == MsgBasic::None, "default reaction fields") && ok;
    return ok;
}

auto testOrderingMutationAndCopy() -> bool
{
    auto action = action_t{};
    {
        auto& firstTarget             = action.addTarget(0x11111111);
        firstTarget.addResult().param = 10;
        firstTarget.addResult().param = 20;
    }
    action.addTarget(0x22222222).addResult().param = 30;

    auto targetIds = std::vector<uint32>{};
    action.ForEachTarget([&](const action_target_t& target)
                         {
                             targetIds.push_back(target.actorId);
                         });

    auto params = std::vector<int32>{};
    action.ForEachResult([&](action_result_t& result)
                         {
                             params.push_back(result.param);
                             result.kind = static_cast<uint8_t>(params.size());
                         });

    bool ok = true;
    ok      = expect(targetIds == std::vector<uint32>{ 0x11111111, 0x22222222 }, "target insertion order") && ok;
    ok      = expect(params == std::vector<int32>{ 10, 20, 30 }, "nested result insertion order") && ok;
    ok      = expect(action.targets[0].results[0].kind == 1 && action.targets[0].results[1].kind == 2 && action.targets[1].results[0].kind == 3, "mutation through result traversal") && ok;

    auto copied                         = action;
    copied.targets[0].actorId           = 0xAAAAAAAA;
    copied.targets[0].results[0].param  = 99;
    copied.targets[0].addResult().param = 100;
    ok                                  = expect(action.targets[0].actorId == 0x11111111, "copied target storage is independent") && ok;
    ok                                  = expect(action.targets[0].results[0].param == 10 && action.targets[0].results.size() == 2, "copied result storage is independent") && ok;
    return ok;
}

auto testNormalize() -> bool
{
    struct TestCase
    {
        ActionCategory category;
        uint8_t        expectedKind;
    };

    constexpr auto cases = std::array{
        TestCase{ ActionCategory::None, 7 },
        TestCase{ ActionCategory::BasicAttack, 1 },
        TestCase{ ActionCategory::RangedFinish, 2 },
        TestCase{ ActionCategory::SkillFinish, 3 },
        TestCase{ ActionCategory::MagicFinish, 7 },
        TestCase{ ActionCategory::ItemFinish, 1 },
        TestCase{ ActionCategory::AbilityFinish, 2 },
        TestCase{ ActionCategory::SkillStart, 7 },
        TestCase{ ActionCategory::MagicStart, 7 },
        TestCase{ ActionCategory::ItemStart, 7 },
        TestCase{ ActionCategory::AbilityStart, 7 },
        TestCase{ ActionCategory::MobSkillFinish, 3 },
        TestCase{ ActionCategory::RangedStart, 7 },
        TestCase{ ActionCategory::PetSkillFinish, 3 },
        TestCase{ ActionCategory::Dancer, 2 },
        TestCase{ ActionCategory::RuneFencer, 3 },
    };

    bool ok = true;
    for (const auto& test : cases)
    {
        auto action                          = action_t{};
        action.actiontype                    = test.category;
        action.actionid                      = 0xDEADBEEF;
        action.recast                        = 90s;
        action.spellgroup                    = SPELLGROUP_BLACK;
        action.addTarget(1).addResult().kind = 7;
        action.addTarget(2).addResult().kind = 7;

        action.normalize();

        const auto expectedRecast = test.category == ActionCategory::MagicFinish ? 90s : 0s;
        const auto expectedId     = test.category == ActionCategory::BasicAttack ? static_cast<uint32>(FourCC::BasicAttack) : 0xDEADBEEF;
        ok                        = expect(action.recast == expectedRecast, "normalized recast") && ok;
        ok                        = expect(action.actionid == expectedId, "normalized action ID") && ok;
        ok                        = expect(action.targets[0].results[0].kind == test.expectedKind && action.targets[1].results[0].kind == test.expectedKind, "normalized result kind") && ok;
        ok                        = expect(action.spellgroup == SPELLGROUP_BLACK, "normalize preserves spell group") && ok;
    }
    return ok;
}

auto testRecordSkillchain() -> bool
{
    bool ok = true;

    auto damage = action_result_t{};
    damage.recordSkillchain(ActionProcSkillChain::Light, 123);
    ok = expect(std::holds_alternative<ActionProcSkillChain>(damage.additionalEffect), "damage skillchain alternative") && ok;
    ok = expect(std::get<ActionProcSkillChain>(damage.additionalEffect) == ActionProcSkillChain::Light, "damage skillchain value") && ok;
    ok = expect(damage.addEffectParam == 123 && damage.addEffectMessage == static_cast<MsgBasic>(288), "damage skillchain fields") && ok;
    ok = expect(damage.hasAdditionalEffect(), "damage skillchain presence") && ok;

    auto absorb = action_result_t{};
    absorb.recordSkillchain(ActionProcSkillChain::Umbra, std::numeric_limits<int16_t>::min());
    ok = expect(absorb.addEffectParam == 32768 && absorb.addEffectMessage == static_cast<MsgBasic>(400), "absorbed skillchain int16 boundary") && ok;

    auto none = action_result_t{};
    none.recordSkillchain(ActionProcSkillChain::None, 0);
    ok = expect(none.addEffectParam == 0 && none.addEffectMessage == static_cast<MsgBasic>(287), "none skillchain fields") && ok;
    ok = expect(!none.hasAdditionalEffect(), "none skillchain remains absent on wire") && ok;
    return ok;
}

} // namespace

auto runActionModelSelfTests() -> bool
{
    bool ok = true;
    ok      = testDefaults() && ok;
    ok      = testOrderingMutationAndCopy() && ok;
    ok      = testNormalize() && ok;
    ok      = testRecordSkillchain() && ok;
    return ok;
}
