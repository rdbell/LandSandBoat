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

#include "test_map_msg_basic_enum.h"

#include <array>
#include <cstdint>
#include <iostream>
#include <string>
#include <type_traits>
#include <unordered_map>

#include "map/enums/msg_basic.h"

static_assert(std::is_same_v<std::underlying_type_t<MsgBasic>, std::uint16_t>);

namespace
{

struct MsgBasicCase
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
        std::cerr << "map msg basic enum self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto msgBasicCases() -> std::array<MsgBasicCase, 256>
{
    return {{
        { enumValue(MsgBasic::None), 0, "MsgBasic::None" },
        { enumValue(MsgBasic::AttackHits), 1, "MsgBasic::AttackHits" },
        { enumValue(MsgBasic::MagicDamage), 2, "MsgBasic::MagicDamage" },
        { enumValue(MsgBasic::StartsCastingSelf), 3, "MsgBasic::StartsCastingSelf" },
        { enumValue(MsgBasic::TargetOutOfRange), 4, "MsgBasic::TargetOutOfRange" },
        { enumValue(MsgBasic::UnableToSeeTarget), 5, "MsgBasic::UnableToSeeTarget" },
        { enumValue(MsgBasic::DefeatsTarget), 6, "MsgBasic::DefeatsTarget" },
        { enumValue(MsgBasic::MagicRecoversHP), 7, "MsgBasic::MagicRecoversHP" },
        { enumValue(MsgBasic::ExperiencePointsGained), 8, "MsgBasic::ExperiencePointsGained" },
        { enumValue(MsgBasic::LevelUp), 9, "MsgBasic::LevelUp" },
        { enumValue(MsgBasic::LevelDown), 11, "MsgBasic::LevelDown" },
        { enumValue(MsgBasic::AlreadyClaimed), 12, "MsgBasic::AlreadyClaimed" },
        { enumValue(MsgBasic::CounterAbsByShadow), 14, "MsgBasic::CounterAbsByShadow" },
        { enumValue(MsgBasic::AttackMisses), 15, "MsgBasic::AttackMisses" },
        { enumValue(MsgBasic::IsInterrupted), 16, "MsgBasic::IsInterrupted" },
        { enumValue(MsgBasic::UnableToCast), 18, "MsgBasic::UnableToCast" },
        { enumValue(MsgBasic::CallForHelp), 19, "MsgBasic::CallForHelp" },
        { enumValue(MsgBasic::FallsToGround), 20, "MsgBasic::FallsToGround" },
        { enumValue(MsgBasic::CannotCallForHelp), 22, "MsgBasic::CannotCallForHelp" },
        { enumValue(MsgBasic::LearnsNewSpell), 23, "MsgBasic::LearnsNewSpell" },
        { enumValue(MsgBasic::TargetRecoversHPSimple), 24, "MsgBasic::TargetRecoversHPSimple" },
        { enumValue(MsgBasic::ItemUse), 28, "MsgBasic::ItemUse" },
        { enumValue(MsgBasic::IsParalyzed), 29, "MsgBasic::IsParalyzed" },
        { enumValue(MsgBasic::TargetAnticipates), 30, "MsgBasic::TargetAnticipates" },
        { enumValue(MsgBasic::ShadowAbsorb), 31, "MsgBasic::ShadowAbsorb" },
        { enumValue(MsgBasic::TargetDodges), 32, "MsgBasic::TargetDodges" },
        { enumValue(MsgBasic::AttackCounteredDamage), 33, "MsgBasic::AttackCounteredDamage" },
        { enumValue(MsgBasic::NotEnoughMP), 34, "MsgBasic::NotEnoughMP" },
        { enumValue(MsgBasic::NoNinjaTools), 35, "MsgBasic::NoNinjaTools" },
        { enumValue(MsgBasic::LoseSight), 36, "MsgBasic::LoseSight" },
        { enumValue(MsgBasic::TooFarForExp), 37, "MsgBasic::TooFarForExp" },
        { enumValue(MsgBasic::SkillGain), 38, "MsgBasic::SkillGain" },
        { enumValue(MsgBasic::NeedDualWield), 39, "MsgBasic::NeedDualWield" },
        { enumValue(MsgBasic::CannotInThisArea), 40, "MsgBasic::CannotInThisArea" },
        { enumValue(MsgBasic::ReadiesWeaponskill), 43, "MsgBasic::ReadiesWeaponskill" },
        { enumValue(MsgBasic::SpikesEffectDmg), 44, "MsgBasic::SpikesEffectDmg" },
        { enumValue(MsgBasic::LearnsAbility), 45, "MsgBasic::LearnsAbility" },
        { enumValue(MsgBasic::CannotCastSpell), 47, "MsgBasic::CannotCastSpell" },
        { enumValue(MsgBasic::UnableToCastSpells), 49, "MsgBasic::UnableToCastSpells" },
        { enumValue(MsgBasic::MeritPointGained), 50, "MsgBasic::MeritPointGained" },
        { enumValue(MsgBasic::SkillLevelUp), 53, "MsgBasic::SkillLevelUp" },
        { enumValue(MsgBasic::UnableToUseItem), 56, "MsgBasic::UnableToUseItem" },
        { enumValue(MsgBasic::ItemFailsToActivate), 62, "MsgBasic::ItemFailsToActivate" },
        { enumValue(MsgBasic::DebugResistedSpell), 66, "MsgBasic::DebugResistedSpell" },
        { enumValue(MsgBasic::AttackCrit), 67, "MsgBasic::AttackCrit" },
        { enumValue(MsgBasic::TargetParries), 70, "MsgBasic::TargetParries" },
        { enumValue(MsgBasic::CannotPerformAction), 71, "MsgBasic::CannotPerformAction" },
        { enumValue(MsgBasic::MagicNoEffect), 75, "MsgBasic::MagicNoEffect" },
        { enumValue(MsgBasic::NoTargetInAreaOfEffect), 76, "MsgBasic::NoTargetInAreaOfEffect" },
        { enumValue(MsgBasic::UsesSangeTakesDamage), 77, "MsgBasic::UsesSangeTakesDamage" },
        { enumValue(MsgBasic::TooFarAway), 78, "MsgBasic::TooFarAway" },
        { enumValue(MsgBasic::DebugDoubleAttackProc), 79, "MsgBasic::DebugDoubleAttackProc" },
        { enumValue(MsgBasic::DebugTripleAttackProc), 80, "MsgBasic::DebugTripleAttackProc" },
        { enumValue(MsgBasic::IsParalyzed2), 84, "MsgBasic::IsParalyzed2" },
        { enumValue(MsgBasic::MagicResisted), 85, "MsgBasic::MagicResisted" },
        { enumValue(MsgBasic::UnableToUseJobAbility), 87, "MsgBasic::UnableToUseJobAbility" },
        { enumValue(MsgBasic::UnableToUseJobAbility2), 88, "MsgBasic::UnableToUseJobAbility2" },
        { enumValue(MsgBasic::UnableToUseWeaponskill), 89, "MsgBasic::UnableToUseWeaponskill" },
        { enumValue(MsgBasic::CannotUseItemOn), 92, "MsgBasic::CannotUseItemOn" },
        { enumValue(MsgBasic::MagicTeleport), 93, "MsgBasic::MagicTeleport" },
        { enumValue(MsgBasic::WaitLonger), 94, "MsgBasic::WaitLonger" },
        { enumValue(MsgBasic::PlayerDefeatedBy), 97, "MsgBasic::PlayerDefeatedBy" },
        { enumValue(MsgBasic::UsesJobAbility), 100, "MsgBasic::UsesJobAbility" },
        { enumValue(MsgBasic::UsesJobAbility2), 101, "MsgBasic::UsesJobAbility2" },
        { enumValue(MsgBasic::UsesRecoversHP), 102, "MsgBasic::UsesRecoversHP" },
        { enumValue(MsgBasic::SkillRecoversHP), 103, "MsgBasic::SkillRecoversHP" },
        { enumValue(MsgBasic::IsIntimidated), 106, "MsgBasic::IsIntimidated" },
        { enumValue(MsgBasic::UsesAbilityTakesDamage), 110, "MsgBasic::UsesAbilityTakesDamage" },
        { enumValue(MsgBasic::MagicFail), 114, "MsgBasic::MagicFail" },
        { enumValue(MsgBasic::UsesAbilityFortifiedUndead), 131, "MsgBasic::UsesAbilityFortifiedUndead" },
        { enumValue(MsgBasic::SpikesEffectHPDrain), 132, "MsgBasic::SpikesEffectHPDrain" },
        { enumValue(MsgBasic::UsesAbilityFortifiedArcana), 134, "MsgBasic::UsesAbilityFortifiedArcana" },
        { enumValue(MsgBasic::CharmSuccess), 136, "MsgBasic::CharmSuccess" },
        { enumValue(MsgBasic::CharmFail), 137, "MsgBasic::CharmFail" },
        { enumValue(MsgBasic::UsesAbilityFortifiedDemons), 148, "MsgBasic::UsesAbilityFortifiedDemons" },
        { enumValue(MsgBasic::TargetFortifiedDemons), 149, "MsgBasic::TargetFortifiedDemons" },
        { enumValue(MsgBasic::UsesAbilityFortifiedDragons), 150, "MsgBasic::UsesAbilityFortifiedDragons" },
        { enumValue(MsgBasic::TargetFortifiedDragons), 151, "MsgBasic::TargetFortifiedDragons" },
        { enumValue(MsgBasic::CannotOnThatTarget), 155, "MsgBasic::CannotOnThatTarget" },
        { enumValue(MsgBasic::UsesBarrageTakesDamage), 157, "MsgBasic::UsesBarrageTakesDamage" },
        { enumValue(MsgBasic::AbilityMisses), 158, "MsgBasic::AbilityMisses" },
        { enumValue(MsgBasic::AddEffectHPDrained), 161, "MsgBasic::AddEffectHPDrained" },
        { enumValue(MsgBasic::AddEffectMPDrained), 162, "MsgBasic::AddEffectMPDrained" },
        { enumValue(MsgBasic::AddEffectDamage), 163, "MsgBasic::AddEffectDamage" },
        { enumValue(MsgBasic::CheckDefault), 174, "MsgBasic::CheckDefault" },
        { enumValue(MsgBasic::UsesSkillTakesDamage), 185, "MsgBasic::UsesSkillTakesDamage" },
        { enumValue(MsgBasic::UsesSkillGainsEffect), 186, "MsgBasic::UsesSkillGainsEffect" },
        { enumValue(MsgBasic::UsesSkillHPDrained), 187, "MsgBasic::UsesSkillHPDrained" },
        { enumValue(MsgBasic::UsesSkillMisses), 188, "MsgBasic::UsesSkillMisses" },
        { enumValue(MsgBasic::UsesSkillNoEffect), 189, "MsgBasic::UsesSkillNoEffect" },
        { enumValue(MsgBasic::CannotUseWeaponskill), 190, "MsgBasic::CannotUseWeaponskill" },
        { enumValue(MsgBasic::CannotUseAnyWeaponskill), 191, "MsgBasic::CannotUseAnyWeaponskill" },
        { enumValue(MsgBasic::NotEnoughTP), 192, "MsgBasic::NotEnoughTP" },
        { enumValue(MsgBasic::UsesAbilityResistsDamage), 197, "MsgBasic::UsesAbilityResistsDamage" },
        { enumValue(MsgBasic::RequiresShield), 199, "MsgBasic::RequiresShield" },
        { enumValue(MsgBasic::TimeLeft), 202, "MsgBasic::TimeLeft" },
        { enumValue(MsgBasic::IsStatus), 203, "MsgBasic::IsStatus" },
        { enumValue(MsgBasic::CannotCharm), 210, "MsgBasic::CannotCharm" },
        { enumValue(MsgBasic::VeryDifficultCharm), 211, "MsgBasic::VeryDifficultCharm" },
        { enumValue(MsgBasic::DifficultToCharm), 212, "MsgBasic::DifficultToCharm" },
        { enumValue(MsgBasic::MightBeAbleCharm), 213, "MsgBasic::MightBeAbleCharm" },
        { enumValue(MsgBasic::ShouldBeAbleCharm), 214, "MsgBasic::ShouldBeAbleCharm" },
        { enumValue(MsgBasic::RequiresAPet), 215, "MsgBasic::RequiresAPet" },
        { enumValue(MsgBasic::NoRangedWeapon), 216, "MsgBasic::NoRangedWeapon" },
        { enumValue(MsgBasic::CannotSee), 217, "MsgBasic::CannotSee" },
        { enumValue(MsgBasic::MoveAndInterrupt), 218, "MsgBasic::MoveAndInterrupt" },
        { enumValue(MsgBasic::UsesSkillRecoversMP), 224, "MsgBasic::UsesSkillRecoversMP" },
        { enumValue(MsgBasic::UsesSkillMPDrained), 225, "MsgBasic::UsesSkillMPDrained" },
        { enumValue(MsgBasic::UsesSkillTPDrained), 226, "MsgBasic::UsesSkillTPDrained" },
        { enumValue(MsgBasic::MagicDrainsHP), 227, "MsgBasic::MagicDrainsHP" },
        { enumValue(MsgBasic::AddEffectAdditionalDamage), 229, "MsgBasic::AddEffectAdditionalDamage" },
        { enumValue(MsgBasic::MagicGainsEffect), 230, "MsgBasic::MagicGainsEffect" },
        { enumValue(MsgBasic::DrawnIn), 232, "MsgBasic::DrawnIn" },
        { enumValue(MsgBasic::ThatSomeonesPet), 235, "MsgBasic::ThatSomeonesPet" },
        { enumValue(MsgBasic::MagicStatus), 236, "MsgBasic::MagicStatus" },
        { enumValue(MsgBasic::MagicReceivesEffect), 237, "MsgBasic::MagicReceivesEffect" },
        { enumValue(MsgBasic::UsesSkillRecoversHPAreaOfEffect), 238, "MsgBasic::UsesSkillRecoversHPAreaOfEffect" },
        { enumValue(MsgBasic::UsesSkillStatus), 242, "MsgBasic::UsesSkillStatus" },
        { enumValue(MsgBasic::UsesSkillReceivesEffect), 243, "MsgBasic::UsesSkillReceivesEffect" },
        { enumValue(MsgBasic::CheckImpossibleToGauge), 249, "MsgBasic::CheckImpossibleToGauge" },
        { enumValue(MsgBasic::MagicBurstDamage), 252, "MsgBasic::MagicBurstDamage" },
        { enumValue(MsgBasic::ExpChain), 253, "MsgBasic::ExpChain" },
        { enumValue(MsgBasic::DebugSuccessChance), 255, "MsgBasic::DebugSuccessChance" },
        { enumValue(MsgBasic::GardeningSeedSown), 256, "MsgBasic::GardeningSeedSown" },
        { enumValue(MsgBasic::GardeningCrystalNone), 257, "MsgBasic::GardeningCrystalNone" },
        { enumValue(MsgBasic::GardeningCrystalUsed), 258, "MsgBasic::GardeningCrystalUsed" },
        { enumValue(MsgBasic::TargetRecoversHP2), 263, "MsgBasic::TargetRecoversHP2" },
        { enumValue(MsgBasic::TargetTakesDamage), 264, "MsgBasic::TargetTakesDamage" },
        { enumValue(MsgBasic::TargetGainsEffect), 266, "MsgBasic::TargetGainsEffect" },
        { enumValue(MsgBasic::TargetReceivesEffectAbility), 267, "MsgBasic::TargetReceivesEffectAbility" },
        { enumValue(MsgBasic::TargetTeleport), 273, "MsgBasic::TargetTeleport" },
        { enumValue(MsgBasic::MagicBurstDrainsHP), 274, "MsgBasic::MagicBurstDrainsHP" },
        { enumValue(MsgBasic::TargetRecoversMP), 276, "MsgBasic::TargetRecoversMP" },
        { enumValue(MsgBasic::TargetStatus), 277, "MsgBasic::TargetStatus" },
        { enumValue(MsgBasic::TargetReceivesEffect), 278, "MsgBasic::TargetReceivesEffect" },
        { enumValue(MsgBasic::TargetHPDrained), 281, "MsgBasic::TargetHPDrained" },
        { enumValue(MsgBasic::TargetEvades), 282, "MsgBasic::TargetEvades" },
        { enumValue(MsgBasic::TargetNoEffect), 283, "MsgBasic::TargetNoEffect" },
        { enumValue(MsgBasic::MagicResistedTarget), 284, "MsgBasic::MagicResistedTarget" },
        { enumValue(MsgBasic::TargetFortifiedUndead), 286, "MsgBasic::TargetFortifiedUndead" },
        { enumValue(MsgBasic::TargetFortifiedArcana), 287, "MsgBasic::TargetFortifiedArcana" },
        { enumValue(MsgBasic::UsesItemRecoversHPAreaOfEffect), 306, "MsgBasic::UsesItemRecoversHPAreaOfEffect" },
        { enumValue(MsgBasic::Needs2HWeapon), 307, "MsgBasic::Needs2HWeapon" },
        { enumValue(MsgBasic::SkillDrop), 310, "MsgBasic::SkillDrop" },
        { enumValue(MsgBasic::OutOfRangeUnableCast), 313, "MsgBasic::OutOfRangeUnableCast" },
        { enumValue(MsgBasic::AlreadyHasAPet), 315, "MsgBasic::AlreadyHasAPet" },
        { enumValue(MsgBasic::CannotUseInArea), 316, "MsgBasic::CannotUseInArea" },
        { enumValue(MsgBasic::UsesJobAbilityTakeDamage), 317, "MsgBasic::UsesJobAbilityTakeDamage" },
        { enumValue(MsgBasic::UsesItemRecoversHPAreaOfEffect2), 318, "MsgBasic::UsesItemRecoversHPAreaOfEffect2" },
        { enumValue(MsgBasic::UsesAbilityGainsEffect), 319, "MsgBasic::UsesAbilityGainsEffect" },
        { enumValue(MsgBasic::UsesAbilityReceivesEffect), 320, "MsgBasic::UsesAbilityReceivesEffect" },
        { enumValue(MsgBasic::UsesAbilityNoEffect), 323, "MsgBasic::UsesAbilityNoEffect" },
        { enumValue(MsgBasic::UsesButMisses), 324, "MsgBasic::UsesButMisses" },
        { enumValue(MsgBasic::ReadiesSkill), 326, "MsgBasic::ReadiesSkill" },
        { enumValue(MsgBasic::StartsCastingTarget), 327, "MsgBasic::StartsCastingTarget" },
        { enumValue(MsgBasic::TooFarAwayRed), 328, "MsgBasic::TooFarAwayRed" },
        { enumValue(MsgBasic::NoEffectOnPet), 336, "MsgBasic::NoEffectOnPet" },
        { enumValue(MsgBasic::NoJugPetItem), 337, "MsgBasic::NoJugPetItem" },
        { enumValue(MsgBasic::YourMountRefuses), 339, "MsgBasic::YourMountRefuses" },
        { enumValue(MsgBasic::TargetEffectDisappears), 343, "MsgBasic::TargetEffectDisappears" },
        { enumValue(MsgBasic::MustHaveFood), 347, "MsgBasic::MustHaveFood" },
        { enumValue(MsgBasic::RangedAttackHit), 352, "MsgBasic::RangedAttackHit" },
        { enumValue(MsgBasic::RangedAttackCrit), 353, "MsgBasic::RangedAttackCrit" },
        { enumValue(MsgBasic::RangedAttackMiss), 354, "MsgBasic::RangedAttackMiss" },
        { enumValue(MsgBasic::RangedAttackNoEffect), 355, "MsgBasic::RangedAttackNoEffect" },
        { enumValue(MsgBasic::FullInventory), 356, "MsgBasic::FullInventory" },
        { enumValue(MsgBasic::UsesSkillTPReduced), 362, "MsgBasic::UsesSkillTPReduced" },
        { enumValue(MsgBasic::TargetTPReduced), 363, "MsgBasic::TargetTPReduced" },
        { enumValue(MsgBasic::TargetMPDrained), 366, "MsgBasic::TargetMPDrained" },
        { enumValue(MsgBasic::TargetRecoversHP), 367, "MsgBasic::TargetRecoversHP" },
        { enumValue(MsgBasic::UsesSkillEffectDrained), 370, "MsgBasic::UsesSkillEffectDrained" },
        { enumValue(MsgBasic::LimitPointsGained), 371, "MsgBasic::LimitPointsGained" },
        { enumValue(MsgBasic::LimitChain), 372, "MsgBasic::LimitChain" },
        { enumValue(MsgBasic::SpikesEffectRecover), 373, "MsgBasic::SpikesEffectRecover" },
        { enumValue(MsgBasic::StatusSpikes), 374, "MsgBasic::StatusSpikes" },
        { enumValue(MsgBasic::UsesAbilityDispel), 378, "MsgBasic::UsesAbilityDispel" },
        { enumValue(MsgBasic::MeritIncrease), 380, "MsgBasic::MeritIncrease" },
        { enumValue(MsgBasic::MeritDecrease), 381, "MsgBasic::MeritDecrease" },
        { enumValue(MsgBasic::RangedAttackAbsorbs), 382, "MsgBasic::RangedAttackAbsorbs" },
        { enumValue(MsgBasic::SpikesEffectHeal), 383, "MsgBasic::SpikesEffectHeal" },
        { enumValue(MsgBasic::AddEffectRecoversHP), 384, "MsgBasic::AddEffectRecoversHP" },
        { enumValue(MsgBasic::TargetEffectDrained), 404, "MsgBasic::TargetEffectDrained" },
        { enumValue(MsgBasic::LearnsSpell), 419, "MsgBasic::LearnsSpell" },
        { enumValue(MsgBasic::RollMain), 420, "MsgBasic::RollMain" },
        { enumValue(MsgBasic::ReceivesEffectAbility), 421, "MsgBasic::ReceivesEffectAbility" },
        { enumValue(MsgBasic::RollMainFail), 422, "MsgBasic::RollMainFail" },
        { enumValue(MsgBasic::RollSubFail), 423, "MsgBasic::RollSubFail" },
        { enumValue(MsgBasic::DoubleUp), 424, "MsgBasic::DoubleUp" },
        { enumValue(MsgBasic::DoubleUpFail), 425, "MsgBasic::DoubleUpFail" },
        { enumValue(MsgBasic::DoubleUpBust), 426, "MsgBasic::DoubleUpBust" },
        { enumValue(MsgBasic::DoubleUpBustSub), 427, "MsgBasic::DoubleUpBustSub" },
        { enumValue(MsgBasic::NoEligibleRoll), 428, "MsgBasic::NoEligibleRoll" },
        { enumValue(MsgBasic::RollAlreadyActive), 429, "MsgBasic::RollAlreadyActive" },
        { enumValue(MsgBasic::MagicSteal), 430, "MsgBasic::MagicSteal" },
        { enumValue(MsgBasic::UsesAbilityRecharge), 435, "MsgBasic::UsesAbilityRecharge" },
        { enumValue(MsgBasic::TargetAbilitiesRecharged), 436, "MsgBasic::TargetAbilitiesRecharged" },
        { enumValue(MsgBasic::UsesAbilityRechargeTP), 437, "MsgBasic::UsesAbilityRechargeTP" },
        { enumValue(MsgBasic::TargetRechargedTP), 438, "MsgBasic::TargetRechargedTP" },
        { enumValue(MsgBasic::UsesAbilityRechargeMP), 439, "MsgBasic::UsesAbilityRechargeMP" },
        { enumValue(MsgBasic::TargetRechargedMP), 440, "MsgBasic::TargetRechargedMP" },
        { enumValue(MsgBasic::UsesAbilityEffect), 441, "MsgBasic::UsesAbilityEffect" },
        { enumValue(MsgBasic::LearnsNewAbility), 442, "MsgBasic::LearnsNewAbility" },
        { enumValue(MsgBasic::CannotUseItems), 445, "MsgBasic::CannotUseItems" },
        { enumValue(MsgBasic::CannotAttackTarget), 446, "MsgBasic::CannotAttackTarget" },
        { enumValue(MsgBasic::Requires2HForGrip), 512, "MsgBasic::Requires2HForGrip" },
        { enumValue(MsgBasic::NoFinishingMoves), 524, "MsgBasic::NoFinishingMoves" },
        { enumValue(MsgBasic::RetaliateShadowAbsorbs), 535, "MsgBasic::RetaliateShadowAbsorbs" },
        { enumValue(MsgBasic::RetaliateDamage), 536, "MsgBasic::RetaliateDamage" },
        { enumValue(MsgBasic::LevelSyncActivated), 540, "MsgBasic::LevelSyncActivated" },
        { enumValue(MsgBasic::LevelSyncNoExp), 545, "MsgBasic::LevelSyncNoExp" },
        { enumValue(MsgBasic::Obtains), 565, "MsgBasic::Obtains" },
        { enumValue(MsgBasic::PetCannotDoAction), 574, "MsgBasic::PetCannotDoAction" },
        { enumValue(MsgBasic::PetNotEnoughTP), 575, "MsgBasic::PetNotEnoughTP" },
        { enumValue(MsgBasic::RangedAttackSquarely), 576, "MsgBasic::RangedAttackSquarely" },
        { enumValue(MsgBasic::RangedAttackPummels), 577, "MsgBasic::RangedAttackPummels" },
        { enumValue(MsgBasic::TargetRegainsHP), 587, "MsgBasic::TargetRegainsHP" },
        { enumValue(MsgBasic::TargetRegainsMP), 588, "MsgBasic::TargetRegainsMP" },
        { enumValue(MsgBasic::PerfectCounterMiss), 592, "MsgBasic::PerfectCounterMiss" },
        { enumValue(MsgBasic::TreasureHunterUp), 603, "MsgBasic::TreasureHunterUp" },
        { enumValue(MsgBasic::CounterAbsorbedDmg), 606, "MsgBasic::CounterAbsorbedDmg" },
        { enumValue(MsgBasic::MagicCompleteResist), 655, "MsgBasic::MagicCompleteResist" },
        { enumValue(MsgBasic::SameEffectLuopan), 660, "MsgBasic::SameEffectLuopan" },
        { enumValue(MsgBasic::LuopanAlreadyPlaced), 661, "MsgBasic::LuopanAlreadyPlaced" },
        { enumValue(MsgBasic::RequireLuopan), 662, "MsgBasic::RequireLuopan" },
        { enumValue(MsgBasic::LuopanHPRateDown), 663, "MsgBasic::LuopanHPRateDown" },
        { enumValue(MsgBasic::LuopanHPRateUp), 664, "MsgBasic::LuopanHPRateUp" },
        { enumValue(MsgBasic::HasLuopanNoUse), 665, "MsgBasic::HasLuopanNoUse" },
        { enumValue(MsgBasic::RequireRune), 666, "MsgBasic::RequireRune" },
        { enumValue(MsgBasic::SwordplayGain), 667, "MsgBasic::SwordplayGain" },
        { enumValue(MsgBasic::VallationGain), 668, "MsgBasic::VallationGain" },
        { enumValue(MsgBasic::ValianceGainPartyMember), 669, "MsgBasic::ValianceGainPartyMember" },
        { enumValue(MsgBasic::LiementGain), 670, "MsgBasic::LiementGain" },
        { enumValue(MsgBasic::PflugGain), 671, "MsgBasic::PflugGain" },
        { enumValue(MsgBasic::GambitGain), 672, "MsgBasic::GambitGain" },
        { enumValue(MsgBasic::FeretoryCountdown), 679, "MsgBasic::FeretoryCountdown" },
        { enumValue(MsgBasic::ROERecord), 697, "MsgBasic::ROERecord" },
        { enumValue(MsgBasic::ROEProgress), 698, "MsgBasic::ROEProgress" },
        { enumValue(MsgBasic::TrustNoCastTrust), 700, "MsgBasic::TrustNoCastTrust" },
        { enumValue(MsgBasic::ROEStart), 704, "MsgBasic::ROEStart" },
        { enumValue(MsgBasic::ROETimed), 705, "MsgBasic::ROETimed" },
        { enumValue(MsgBasic::TrustPartyMessage), 711, "MsgBasic::TrustPartyMessage" },
        { enumValue(MsgBasic::CheckparamPrimary), 712, "MsgBasic::CheckparamPrimary" },
        { enumValue(MsgBasic::CheckparamAuxiliary), 713, "MsgBasic::CheckparamAuxiliary" },
        { enumValue(MsgBasic::CheckparamRange), 714, "MsgBasic::CheckparamRange" },
        { enumValue(MsgBasic::CheckparamDefense), 715, "MsgBasic::CheckparamDefense" },
        { enumValue(MsgBasic::TrustNoCallAlterEgos), 717, "MsgBasic::TrustNoCallAlterEgos" },
        { enumValue(MsgBasic::CapacityPointsGained), 718, "MsgBasic::CapacityPointsGained" },
        { enumValue(MsgBasic::JobPointGained), 719, "MsgBasic::JobPointGained" },
        { enumValue(MsgBasic::JobPointsIncrease), 720, "MsgBasic::JobPointsIncrease" },
        { enumValue(MsgBasic::CheckparamIlvl), 731, "MsgBasic::CheckparamIlvl" },
        { enumValue(MsgBasic::CheckparamName), 733, "MsgBasic::CheckparamName" },
        { enumValue(MsgBasic::CapacityChain), 735, "MsgBasic::CapacityChain" },
        { enumValue(MsgBasic::ROEUnable), 742, "MsgBasic::ROEUnable" },
        { enumValue(MsgBasic::AutoExceedsCapacity), 745, "MsgBasic::AutoExceedsCapacity" },
        { enumValue(MsgBasic::MountRequiredLevel), 773, "MsgBasic::MountRequiredLevel" },
        { enumValue(MsgBasic::AlterEgoUpgrade), 828, "MsgBasic::AlterEgoUpgrade" },
    }};
}

auto testMsgBasicValuesAndAliases() -> bool
{
    const auto tests = msgBasicCases();
    bool       ok    = expectEqualInt(tests.size(), 256, "MsgBasic catalog count");

    std::unordered_map<std::uint64_t, std::string> seen;
    for (const auto& test : tests)
    {
        ok = expectEqualInt(test.actual, test.expected, test.label) && ok;

        const auto [_, inserted] = seen.emplace(test.actual, test.label);
        if (!inserted)
        {
            std::cerr << "map msg basic enum self-test failed: " << test.label << " aliases " << test.actual << '\n';
            ok = false;
        }
    }

    return ok;
}

} // namespace

auto runMapMsgBasicEnumSelfTests() -> bool
{
    return testMsgBasicValuesAndAliases();
}
