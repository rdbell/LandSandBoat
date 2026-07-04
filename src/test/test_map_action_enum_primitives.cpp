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

#include "test_map_action_enum_primitives.h"

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "map/enums/action/animation.h"
#include "map/enums/action/category.h"
#include "map/enums/action/hit_distortion.h"
#include "map/enums/action/info.h"
#include "map/enums/action/knockback.h"
#include "map/enums/action/modifier.h"
#include "map/enums/action/proc_kind.h"
#include "map/enums/action/react_kind.h"
#include "map/enums/action/resolution.h"

namespace
{

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
        std::cerr << "map action enum primitive self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "map action enum primitive self-test failed: " << label << '\n';
        return false;
    }
    return true;
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

auto testActionCategories() -> bool
{
    return expectAll({
        { enumValue(ActionCategory::None), 0, "ActionCategory::None" },
        { enumValue(ActionCategory::BasicAttack), 1, "ActionCategory::BasicAttack" },
        { enumValue(ActionCategory::RangedFinish), 2, "ActionCategory::RangedFinish" },
        { enumValue(ActionCategory::SkillFinish), 3, "ActionCategory::SkillFinish" },
        { enumValue(ActionCategory::MagicFinish), 4, "ActionCategory::MagicFinish" },
        { enumValue(ActionCategory::ItemFinish), 5, "ActionCategory::ItemFinish" },
        { enumValue(ActionCategory::AbilityFinish), 6, "ActionCategory::AbilityFinish" },
        { enumValue(ActionCategory::SkillStart), 7, "ActionCategory::SkillStart" },
        { enumValue(ActionCategory::MagicStart), 8, "ActionCategory::MagicStart" },
        { enumValue(ActionCategory::ItemStart), 9, "ActionCategory::ItemStart" },
        { enumValue(ActionCategory::AbilityStart), 10, "ActionCategory::AbilityStart" },
        { enumValue(ActionCategory::MobSkillFinish), 11, "ActionCategory::MobSkillFinish" },
        { enumValue(ActionCategory::RangedStart), 12, "ActionCategory::RangedStart" },
        { enumValue(ActionCategory::PetSkillFinish), 13, "ActionCategory::PetSkillFinish" },
        { enumValue(ActionCategory::Dancer), 14, "ActionCategory::Dancer" },
        { enumValue(ActionCategory::RuneFencer), 15, "ActionCategory::RuneFencer" },
    });
}

auto testActionAnimations() -> bool
{
    return expectAll({
        { enumValue(ActionAnimation::None), 0, "ActionAnimation::None" },
        { enumValue(ActionAnimation::PetSkillStart), 94, "ActionAnimation::PetSkillStart" },
        { enumValue(ActionAnimation::SkillStart), 121, "ActionAnimation::SkillStart" },
        { enumValue(ActionAnimation::Teleport), 122, "ActionAnimation::Teleport" },
        { enumValue(ActionAnimation::Raise3), 496, "ActionAnimation::Raise3" },
        { enumValue(ActionAnimation::SkillInterrupt), 508, "ActionAnimation::SkillInterrupt" },
        { enumValue(ActionAnimation::Raise), 511, "ActionAnimation::Raise" },
        { enumValue(ActionAnimation::Raise2), 512, "ActionAnimation::Raise2" },
        { enumValue(ActionAnimation::RegainHP), 772, "ActionAnimation::RegainHP" },
        { enumValue(ActionAnimation::RegainMP), 773, "ActionAnimation::RegainMP" },
        { enumValue(ActionAnimation::Arise), 847, "ActionAnimation::Arise" },
        { enumValue(ActionAnimation::RedTrigger), 1806, "ActionAnimation::RedTrigger" },
        { enumValue(ActionAnimation::YellowTrigger), 1807, "ActionAnimation::YellowTrigger" },
        { enumValue(ActionAnimation::BlueTrigger), 1808, "ActionAnimation::BlueTrigger" },
        { enumValue(ActionAnimation::WhiteTrigger), 1946, "ActionAnimation::WhiteTrigger" },
    });
}

auto testActionInfoValuesAliasesAndFlags() -> bool
{
    bool ok = expectAll({
        { enumValue(ActionInfo::None), 0, "ActionInfo::None" },
        { enumValue(ActionInfo::Defeated), 1, "ActionInfo::Defeated" },
        { enumValue(ActionInfo::CriticalHit), 2, "ActionInfo::CriticalHit" },
        { enumValue(ActionInfo::UnknownAoE), 4, "ActionInfo::UnknownAoE" },
        { enumValue(ActionInfo::Jump), 4, "ActionInfo::Jump" },
        { enumValue(ActionInfo::KonzenIttai), 5, "ActionInfo::KonzenIttai" },
        { enumValue(ActionInfo::WildFlourish), 5, "ActionInfo::WildFlourish" },
        { enumValue(ActionInfo::QuickStep), 5, "ActionInfo::QuickStep" },
        { enumValue(ActionInfo::BoxStep), 6, "ActionInfo::BoxStep" },
        { enumValue(ActionInfo::DesperateFlourish), 6, "ActionInfo::DesperateFlourish" },
        { enumValue(ActionInfo::StutterStep), 7, "ActionInfo::StutterStep" },
        { enumValue(ActionInfo::ViolentFlourish), 7, "ActionInfo::ViolentFlourish" },
        { enumValue(ActionInfo::FeatherStep), 8, "ActionInfo::FeatherStep" },
        { enumValue(ActionInfo::ElementalMix), 0, "ActionInfo::ElementalMix" },
        { enumValue(ActionInfo::Ignis), 1, "ActionInfo::Ignis" },
        { enumValue(ActionInfo::Gelus), 2, "ActionInfo::Gelus" },
        { enumValue(ActionInfo::Flabra), 3, "ActionInfo::Flabra" },
        { enumValue(ActionInfo::Tellus), 4, "ActionInfo::Tellus" },
        { enumValue(ActionInfo::Suplor), 5, "ActionInfo::Suplor" },
        { enumValue(ActionInfo::Unda), 6, "ActionInfo::Unda" },
        { enumValue(ActionInfo::Lux), 7, "ActionInfo::Lux" },
        { enumValue(ActionInfo::Tenebrae), 8, "ActionInfo::Tenebrae" },
    });

    ok = expectTrue(ActionInfo::UnknownAoE == ActionInfo::Jump && ActionInfo::UnknownAoE == ActionInfo::Tellus, "ActionInfo value 4 aliases") && ok;
    ok = expectTrue(ActionInfo::KonzenIttai == ActionInfo::WildFlourish && ActionInfo::KonzenIttai == ActionInfo::QuickStep && ActionInfo::KonzenIttai == ActionInfo::Suplor, "ActionInfo value 5 aliases") && ok;
    ok = expectTrue(ActionInfo::BoxStep == ActionInfo::DesperateFlourish && ActionInfo::BoxStep == ActionInfo::Unda, "ActionInfo value 6 aliases") && ok;
    ok = expectTrue(ActionInfo::StutterStep == ActionInfo::ViolentFlourish && ActionInfo::StutterStep == ActionInfo::Lux, "ActionInfo value 7 aliases") && ok;
    ok = expectTrue(ActionInfo::None == ActionInfo::ElementalMix, "ActionInfo zero aliases") && ok;

    const auto flags = enumValue(ActionInfo::Defeated) | enumValue(ActionInfo::CriticalHit) | enumValue(ActionInfo::Jump);
    ok               = expectEqualInt(flags, 0x07, "ActionInfo combined flags") && ok;
    ok               = expectTrue((flags & enumValue(ActionInfo::CriticalHit)) == enumValue(ActionInfo::CriticalHit), "ActionInfo has CriticalHit") && ok;
    ok               = expectTrue((flags & enumValue(ActionInfo::FeatherStep)) == 0, "ActionInfo missing FeatherStep") && ok;
    return ok;
}

auto testModifiersAndResolution() -> bool
{
    bool ok = expectAll({
        { enumValue(ActionModifier::None), 0x00, "ActionModifier::None" },
        { enumValue(ActionModifier::Cover), 0x01, "ActionModifier::Cover" },
        { enumValue(ActionModifier::Resist), 0x02, "ActionModifier::Resist" },
        { enumValue(ActionModifier::MagicBurst), 0x04, "ActionModifier::MagicBurst" },
        { enumValue(ActionModifier::Immunobreak), 0x08, "ActionModifier::Immunobreak" },
        { enumValue(ActionModifier::CriticalHit), 0x10, "ActionModifier::CriticalHit" },
        { enumValue(ActionResolution::Hit), 0, "ActionResolution::Hit" },
        { enumValue(ActionResolution::Miss), 1, "ActionResolution::Miss" },
        { enumValue(ActionResolution::Guard), 2, "ActionResolution::Guard" },
        { enumValue(ActionResolution::Parry), 3, "ActionResolution::Parry" },
        { enumValue(ActionResolution::Block), 4, "ActionResolution::Block" },
    });

    const auto flags = enumValue(ActionModifier::Cover) | enumValue(ActionModifier::MagicBurst) | enumValue(ActionModifier::CriticalHit);
    ok               = expectEqualInt(flags, 0x15, "ActionModifier combined flags") && ok;
    ok               = expectTrue((flags & enumValue(ActionModifier::MagicBurst)) == enumValue(ActionModifier::MagicBurst), "ActionModifier has MagicBurst") && ok;
    ok               = expectTrue((flags & enumValue(ActionModifier::Resist)) == 0, "ActionModifier missing Resist") && ok;
    return ok;
}

auto testReactAndScaleEnums() -> bool
{
    return expectAll({
        { enumValue(ActionReactKind::None), 0, "ActionReactKind::None" },
        { enumValue(ActionReactKind::BlazeSpikes), 1, "ActionReactKind::BlazeSpikes" },
        { enumValue(ActionReactKind::IceSpikes), 2, "ActionReactKind::IceSpikes" },
        { enumValue(ActionReactKind::DreadSpikes), 3, "ActionReactKind::DreadSpikes" },
        { enumValue(ActionReactKind::CurseSpikes), 4, "ActionReactKind::CurseSpikes" },
        { enumValue(ActionReactKind::ShockSpikes), 5, "ActionReactKind::ShockSpikes" },
        { enumValue(ActionReactKind::ReprisalSpikes), 6, "ActionReactKind::ReprisalSpikes" },
        { enumValue(ActionReactKind::WindSpikes), 7, "ActionReactKind::WindSpikes" },
        { enumValue(ActionReactKind::EarthSpikes), 8, "ActionReactKind::EarthSpikes" },
        { enumValue(ActionReactKind::WaterSpikes), 9, "ActionReactKind::WaterSpikes" },
        { enumValue(ActionReactKind::DeathSpikes), 10, "ActionReactKind::DeathSpikes" },
        { enumValue(ActionReactKind::Counter), 63, "ActionReactKind::Counter" },
        { enumValue(HitDistortion::None), 0, "HitDistortion::None" },
        { enumValue(HitDistortion::Light), 1, "HitDistortion::Light" },
        { enumValue(HitDistortion::Medium), 2, "HitDistortion::Medium" },
        { enumValue(HitDistortion::Heavy), 3, "HitDistortion::Heavy" },
        { enumValue(Knockback::None), 0, "Knockback::None" },
        { enumValue(Knockback::Level1), 1, "Knockback::Level1" },
        { enumValue(Knockback::Level2), 2, "Knockback::Level2" },
        { enumValue(Knockback::Level3), 3, "Knockback::Level3" },
        { enumValue(Knockback::Level4), 4, "Knockback::Level4" },
        { enumValue(Knockback::Level5), 5, "Knockback::Level5" },
        { enumValue(Knockback::Level6), 6, "Knockback::Level6" },
        { enumValue(Knockback::Level7), 7, "Knockback::Level7" },
    });
}

auto testProcAddEffectValuesAndAliases() -> bool
{
    bool ok = expectAll({
        { enumValue(ActionProcAddEffect::None), 0, "ActionProcAddEffect::None" },
        { enumValue(ActionProcAddEffect::FireDamage), 1, "ActionProcAddEffect::FireDamage" },
        { enumValue(ActionProcAddEffect::IceDamage), 2, "ActionProcAddEffect::IceDamage" },
        { enumValue(ActionProcAddEffect::WindDamage), 3, "ActionProcAddEffect::WindDamage" },
        { enumValue(ActionProcAddEffect::EarthDamage), 4, "ActionProcAddEffect::EarthDamage" },
        { enumValue(ActionProcAddEffect::LightningDamage), 5, "ActionProcAddEffect::LightningDamage" },
        { enumValue(ActionProcAddEffect::WaterDamage), 6, "ActionProcAddEffect::WaterDamage" },
        { enumValue(ActionProcAddEffect::LightDamage), 7, "ActionProcAddEffect::LightDamage" },
        { enumValue(ActionProcAddEffect::DarkDamage), 8, "ActionProcAddEffect::DarkDamage" },
        { enumValue(ActionProcAddEffect::Sleep), 9, "ActionProcAddEffect::Sleep" },
        { enumValue(ActionProcAddEffect::Poison), 10, "ActionProcAddEffect::Poison" },
        { enumValue(ActionProcAddEffect::Paralyze), 11, "ActionProcAddEffect::Paralyze" },
        { enumValue(ActionProcAddEffect::Addle), 11, "ActionProcAddEffect::Addle" },
        { enumValue(ActionProcAddEffect::Amnesia), 11, "ActionProcAddEffect::Amnesia" },
        { enumValue(ActionProcAddEffect::Blind), 12, "ActionProcAddEffect::Blind" },
        { enumValue(ActionProcAddEffect::Silence), 13, "ActionProcAddEffect::Silence" },
        { enumValue(ActionProcAddEffect::Petrify), 14, "ActionProcAddEffect::Petrify" },
        { enumValue(ActionProcAddEffect::Plague), 15, "ActionProcAddEffect::Plague" },
        { enumValue(ActionProcAddEffect::Stun), 16, "ActionProcAddEffect::Stun" },
        { enumValue(ActionProcAddEffect::Curse), 17, "ActionProcAddEffect::Curse" },
        { enumValue(ActionProcAddEffect::Weaken), 18, "ActionProcAddEffect::Weaken" },
        { enumValue(ActionProcAddEffect::DefenseDown), 18, "ActionProcAddEffect::DefenseDown" },
        { enumValue(ActionProcAddEffect::EvasionDown), 18, "ActionProcAddEffect::EvasionDown" },
        { enumValue(ActionProcAddEffect::AttackDown), 18, "ActionProcAddEffect::AttackDown" },
        { enumValue(ActionProcAddEffect::Slow), 18, "ActionProcAddEffect::Slow" },
        { enumValue(ActionProcAddEffect::Death), 19, "ActionProcAddEffect::Death" },
        { enumValue(ActionProcAddEffect::Shield), 20, "ActionProcAddEffect::Shield" },
        { enumValue(ActionProcAddEffect::HPDrain), 21, "ActionProcAddEffect::HPDrain" },
        { enumValue(ActionProcAddEffect::MPDrain), 22, "ActionProcAddEffect::MPDrain" },
        { enumValue(ActionProcAddEffect::TPDrain), 22, "ActionProcAddEffect::TPDrain" },
        { enumValue(ActionProcAddEffect::StatusDrain), 22, "ActionProcAddEffect::StatusDrain" },
        { enumValue(ActionProcAddEffect::Haste), 23, "ActionProcAddEffect::Haste" },
        { enumValue(ActionProcAddEffect::ImpairsEvasion), 0, "ActionProcAddEffect::ImpairsEvasion" },
        { enumValue(ActionProcAddEffect::Bind), 0, "ActionProcAddEffect::Bind" },
        { enumValue(ActionProcAddEffect::Weight), 0, "ActionProcAddEffect::Weight" },
        { enumValue(ActionProcAddEffect::Auspice), 0, "ActionProcAddEffect::Auspice" },
    });

    ok = expectTrue(ActionProcAddEffect::Paralyze == ActionProcAddEffect::Addle && ActionProcAddEffect::Paralyze == ActionProcAddEffect::Amnesia, "ActionProcAddEffect value 11 aliases") && ok;
    ok = expectTrue(ActionProcAddEffect::Weaken == ActionProcAddEffect::DefenseDown && ActionProcAddEffect::Weaken == ActionProcAddEffect::EvasionDown && ActionProcAddEffect::Weaken == ActionProcAddEffect::AttackDown && ActionProcAddEffect::Weaken == ActionProcAddEffect::Slow, "ActionProcAddEffect value 18 aliases") && ok;
    ok = expectTrue(ActionProcAddEffect::MPDrain == ActionProcAddEffect::TPDrain && ActionProcAddEffect::MPDrain == ActionProcAddEffect::StatusDrain, "ActionProcAddEffect value 22 aliases") && ok;
    ok = expectTrue(ActionProcAddEffect::None == ActionProcAddEffect::ImpairsEvasion && ActionProcAddEffect::None == ActionProcAddEffect::Bind && ActionProcAddEffect::None == ActionProcAddEffect::Weight && ActionProcAddEffect::None == ActionProcAddEffect::Auspice, "ActionProcAddEffect zero aliases") && ok;
    return ok;
}

auto testProcSkillChainValues() -> bool
{
    return expectAll({
        { enumValue(ActionProcSkillChain::None), 0, "ActionProcSkillChain::None" },
        { enumValue(ActionProcSkillChain::Light), 1, "ActionProcSkillChain::Light" },
        { enumValue(ActionProcSkillChain::Darkness), 2, "ActionProcSkillChain::Darkness" },
        { enumValue(ActionProcSkillChain::Gravitation), 3, "ActionProcSkillChain::Gravitation" },
        { enumValue(ActionProcSkillChain::Fragmentation), 4, "ActionProcSkillChain::Fragmentation" },
        { enumValue(ActionProcSkillChain::Distortion), 5, "ActionProcSkillChain::Distortion" },
        { enumValue(ActionProcSkillChain::Fusion), 6, "ActionProcSkillChain::Fusion" },
        { enumValue(ActionProcSkillChain::Compression), 7, "ActionProcSkillChain::Compression" },
        { enumValue(ActionProcSkillChain::Liquefaction), 8, "ActionProcSkillChain::Liquefaction" },
        { enumValue(ActionProcSkillChain::Induration), 9, "ActionProcSkillChain::Induration" },
        { enumValue(ActionProcSkillChain::Reverberation), 10, "ActionProcSkillChain::Reverberation" },
        { enumValue(ActionProcSkillChain::Transfixion), 11, "ActionProcSkillChain::Transfixion" },
        { enumValue(ActionProcSkillChain::Scission), 12, "ActionProcSkillChain::Scission" },
        { enumValue(ActionProcSkillChain::Detonation), 13, "ActionProcSkillChain::Detonation" },
        { enumValue(ActionProcSkillChain::Impaction), 14, "ActionProcSkillChain::Impaction" },
        { enumValue(ActionProcSkillChain::Radiance), 15, "ActionProcSkillChain::Radiance" },
        { enumValue(ActionProcSkillChain::Umbra), 16, "ActionProcSkillChain::Umbra" },
    });
}

} // namespace

auto runMapActionEnumPrimitiveSelfTests() -> bool
{
    return testActionCategories() &&
           testActionAnimations() &&
           testActionInfoValuesAliasesAndFlags() &&
           testModifiersAndResolution() &&
           testReactAndScaleEnums() &&
           testProcAddEffectValuesAndAliases() &&
           testProcSkillChainValues();
}
