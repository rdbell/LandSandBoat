/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
  Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses/

===========================================================================
*/

#include "test_latent_effect.h"

#include "data/enums/latent.h"
#include "entities/char_entity.h"
#include "latent_capacity.h"
#include "latent_action_plan.h"
#include "latent_effect.h"
#include "latent_effect_container.h"

#include <iostream>

namespace
{

auto expectEqual(auto actual, auto expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "latent effect self-test failed: " << label << " got "
                  << static_cast<int>(actual) << " expected " << static_cast<int>(expected) << '\n';
        return false;
    }
    return true;
}

auto expectBool(bool actual, bool expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "latent effect self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto testLatentEffectConstructor() -> bool
{
    CLatentEffect effect(nullptr, xi::Latent::HpUnderPercent, 75, 4, Mod::DMG_RATING, 12);

    bool ok = true;
    ok      = expectBool(effect.GetOwner() == nullptr, true, "constructor owner") && ok;
    ok      = expectEqual(effect.GetConditionsID(), xi::Latent::HpUnderPercent, "constructor condition id") && ok;
    ok      = expectEqual(effect.GetConditionsValue(), static_cast<uint16>(75), "constructor condition value") && ok;
    ok      = expectEqual(effect.GetSlot(), static_cast<uint8>(4), "constructor slot") && ok;
    ok      = expectEqual(effect.GetModValue(), Mod::DMG_RATING, "constructor mod") && ok;
    ok      = expectEqual(effect.GetModPower(), static_cast<int16>(12), "constructor mod power") && ok;
    ok      = expectBool(effect.IsActivated(), false, "constructor inactive") && ok;
    return ok;
}

auto testLatentEffectSetters() -> bool
{
    CLatentEffect effect(nullptr, xi::Latent::HpUnderPercent, 75, 4, Mod::DMG_RATING, 12);

    effect.SetConditionsId(xi::Latent::JobLevelAbove);
    effect.SetConditionsValue(99);
    effect.SetSlot(16);
    effect.SetModValue(Mod::MOVE_SPEED_GEAR_BONUS);
    effect.SetModPower(-5);

    bool ok = true;
    ok      = expectEqual(effect.GetConditionsID(), xi::Latent::JobLevelAbove, "setter condition id") && ok;
    ok      = expectEqual(effect.GetConditionsValue(), static_cast<uint16>(99), "setter condition value") && ok;
    ok      = expectEqual(effect.GetSlot(), static_cast<uint8>(16), "setter slot") && ok;
    ok      = expectEqual(effect.GetModValue(), Mod::MOVE_SPEED_GEAR_BONUS, "setter mod") && ok;
    ok      = expectEqual(effect.GetModPower(), static_cast<int16>(-5), "setter mod power") && ok;
    return ok;
}

auto testModOnItemOnly() -> bool
{
    CLatentEffect effect(nullptr, xi::Latent::HpUnderPercent, 0, 0, Mod::NONE, 0);

    const Mod itemOnly[] = {
        Mod::DMG_RATING,
        Mod::ITEM_ADDEFFECT_TYPE,
        Mod::ITEM_SUBEFFECT,
        Mod::ITEM_ADDEFFECT_DMG,
        Mod::ITEM_ADDEFFECT_CHANCE,
        Mod::ITEM_ADDEFFECT_ELEMENT,
        Mod::ITEM_ADDEFFECT_STATUS,
        Mod::ITEM_ADDEFFECT_POWER,
        Mod::ITEM_ADDEFFECT_DURATION,
        Mod::ADDS_WEAPONSKILL,
        Mod::MOVE_SPEED_GEAR_BONUS,
        Mod::CRITHITRATE_ONLY_WEP,
    };

    bool ok = true;
    for (auto mod : itemOnly)
    {
        ok = expectBool(effect.ModOnItemOnly(mod), true, "item-only mod") && ok;
    }

    const Mod ownerMods[] = {
        Mod::NONE,
        Mod::DEF,
        Mod::MOVE_SPEED_STACKABLE,
        Mod::MAIN_DMG_RATING,
        Mod::ITEM_ADDEFFECT_SCRIPTED,
    };

    for (auto mod : ownerMods)
    {
        ok = expectBool(effect.ModOnItemOnly(mod), false, "owner mod") && ok;
    }
    return ok;
}

auto testLatentActivationPlan() -> bool
{
    bool ok = true;

    const auto alreadyActive = latenthelpers::PlanLatentActivation(true, false, false);
    ok                       = expectBool(alreadyActive == latenthelpers::LatentActivationPlan{}, true, "activation already-active plan") && ok;

    const auto                                ownerModifier = latenthelpers::PlanLatentActivation(false, false, false);
    const latenthelpers::LatentActivationPlan expectedOwner{
        .changed          = true,
        .addOwnerModifier = true,
        .markActivated    = true,
    };
    ok = expectBool(ownerModifier == expectedOwner, true, "activation owner plan") && ok;

    const auto                                itemModifier = latenthelpers::PlanLatentActivation(false, true, true);
    const latenthelpers::LatentActivationPlan expectedItem{
        .changed             = true,
        .addItemModifier     = true,
        .rebuildWeaponSkills = true,
        .pushCommandData     = true,
        .rememberItem        = true,
        .markActivated       = true,
    };
    ok = expectBool(itemModifier == expectedItem, true, "activation item plan") && ok;

    const auto                                missingItem = latenthelpers::PlanLatentActivation(false, true, false);
    const latenthelpers::LatentActivationPlan expectedMissing{
        .changed       = true,
        .markActivated = true,
    };
    ok = expectBool(missingItem == expectedMissing, true, "activation missing-item plan") && ok;

    return ok;
}

auto testLatentDeactivationPlan() -> bool
{
    bool ok = true;

    ok = expectBool(latenthelpers::PlanLatentDeactivation(false, false, false) == latenthelpers::LatentDeactivationPlan{}, true, "deactivation inactive plan") &&
         ok;

    const latenthelpers::LatentDeactivationPlan expectedOwner{
        .changed             = true,
        .removeOwnerModifier = true,
        .markDeactivated     = true,
    };
    ok = expectBool(latenthelpers::PlanLatentDeactivation(true, false, false) == expectedOwner, true, "deactivation owner plan") && ok;

    const latenthelpers::LatentDeactivationPlan expectedItem{
        .changed             = true,
        .removeItemModifier  = true,
        .rebuildWeaponSkills = true,
        .pushCommandData     = true,
        .markDeactivated     = true,
    };
    ok = expectBool(latenthelpers::PlanLatentDeactivation(true, true, true) == expectedItem, true, "deactivation item plan") && ok;

    const latenthelpers::LatentDeactivationPlan expectedMissing{
        .changed         = true,
        .markDeactivated = true,
    };
    ok = expectBool(latenthelpers::PlanLatentDeactivation(true, true, false) == expectedMissing, true, "deactivation missing-item plan") && ok;

    return ok;
}

auto testLatentChangeAccumulation() -> bool
{
    bool ok = true;
    ok      = expectBool(latenthelpers::AccumulateLatentChange(false, false), false, "no latent changed") && ok;
    ok      = expectBool(latenthelpers::AccumulateLatentChange(false, true), true, "first latent changed") && ok;
    ok      = expectBool(latenthelpers::AccumulateLatentChange(true, false), true, "prior change retained") && ok;
    ok      = expectBool(latenthelpers::AccumulateLatentChange(true, true), true, "additional change retained") && ok;
    return ok;
}

auto testLatentHealthUpdateSelection() -> bool
{
    bool ok = true;
    ok      = expectBool(latenthelpers::ProcessLatentListWantsHealthUpdate(false), false, "unchanged list skips health update") && ok;
    ok      = expectBool(latenthelpers::ProcessLatentListWantsHealthUpdate(true), true, "changed list requests health update") && ok;
    return ok;
}

auto testLatentTransitionPlan() -> bool
{
    using latenthelpers::LatentTransitionAction;

    bool ok = true;
    ok      = expectBool(latenthelpers::PlanLatentTransition(true, false) == LatentTransitionAction::Activate, true, "true inactive activates") && ok;
    ok      = expectBool(latenthelpers::PlanLatentTransition(true, true) == LatentTransitionAction::None, true, "true active unchanged") && ok;
    ok      = expectBool(latenthelpers::PlanLatentTransition(false, true) == LatentTransitionAction::Deactivate, true, "false active deactivates") && ok;
    ok      = expectBool(latenthelpers::PlanLatentTransition(false, false) == LatentTransitionAction::None, true, "false inactive unchanged") && ok;
    return ok;
}

auto testLatentEffectContainerBookkeeping() -> bool
{
    CLatentEffectContainer container(nullptr);

    bool ok = true;
    ok      = expectBool(container.HasAllLatentsActive(18), true, "empty scripted slot active") && ok;

    container.AddLatentEffect(xi::Latent::HpUnderPercent, 75, Mod::DEF, 12);
    ok = expectBool(container.HasAllLatentsActive(18), false, "scripted latent uses max slot") && ok;
    ok = expectBool(container.HasAllLatentsActive(17), true, "other slot ignores scripted latent") && ok;

    ok = expectBool(container.DelLatentEffect(xi::Latent::HpOverPercent, 75, Mod::DEF, 12), false, "condition id mismatch") && ok;
    ok = expectBool(container.DelLatentEffect(xi::Latent::HpUnderPercent, 74, Mod::DEF, 12), false, "condition value mismatch") && ok;
    ok = expectBool(container.DelLatentEffect(xi::Latent::HpUnderPercent, 75, Mod::ATT, 12), false, "modifier id mismatch") && ok;
    ok = expectBool(container.DelLatentEffect(xi::Latent::HpUnderPercent, 75, Mod::DEF, 13), false, "modifier power mismatch") && ok;
    ok = expectBool(container.HasAllLatentsActive(18), false, "mismatched deletes preserve latent") && ok;

    container.AddLatentEffect(xi::Latent::HpUnderPercent, 75, Mod::DEF, 12);
    ok = expectBool(container.DelLatentEffect(xi::Latent::HpUnderPercent, 75, Mod::DEF, 12), true, "first duplicate deleted") && ok;
    ok = expectBool(container.HasAllLatentsActive(18), false, "second duplicate remains") && ok;
    ok = expectBool(container.DelLatentEffect(xi::Latent::HpUnderPercent, 75, Mod::DEF, 12), true, "second duplicate deleted") && ok;
    ok = expectBool(container.HasAllLatentsActive(18), true, "all duplicates deleted") && ok;
    ok = expectBool(container.DelLatentEffect(xi::Latent::HpUnderPercent, 75, Mod::DEF, 12), false, "missing latent delete") && ok;

    container.AddLatentEffect(xi::Latent::MpUnder, 20, Mod::DEF, 3);
    container.AddLatentEffect(xi::Latent::TpOver, 1000, Mod::ATT, 4);
    container.DelLatentEffects(99, 17);
    ok = expectBool(container.HasAllLatentsActive(18), false, "other slot delete preserves latents") && ok;
    container.DelLatentEffects(99, 18);
    ok = expectBool(container.HasAllLatentsActive(18), true, "slot delete removes every latent") && ok;

    return ok;
}

auto testLatentEffectContainerEquipmentInsertion() -> bool
{
    CCharEntity owner;
    // The setter persists levels for TYPE_PC. Native self-tests run before the
    // SQLite test database is installed, so suppress only that unrelated write.
    owner.objtype = TYPE_NONE;
    owner.SetMLevel(49);
    CLatentEffectContainer container(&owner);

    std::vector<CItemEquipment::itemLatent> ordinary{
        { xi::Latent::HpUnderPercent, 75, Mod::DEF, 12 },
        { xi::Latent::MpUnder, 20, Mod::ATT, 7 },
    };

    bool ok = true;
    container.AddLatentEffects(ordinary, 50, 4);
    ok = expectBool(container.HasAllLatentsActive(4), true, "below-level ordinary latent skipped") && ok;

    owner.SetMLevel(50);
    container.AddLatentEffects(ordinary, 50, 4);
    ok = expectBool(container.HasAllLatentsActive(4), false, "required-level ordinary latent added") && ok;
    ok = expectBool(container.DelLatentEffect(xi::Latent::HpUnderPercent, 75, Mod::DEF, 12), true, "ordinary latent fields copied") && ok;
    ok = expectBool(container.DelLatentEffect(xi::Latent::MpUnder, 20, Mod::ATT, 7), true, "second ordinary latent copied") && ok;
    ok = expectBool(container.HasAllLatentsActive(4), true, "ordinary latent removed after exact delete") && ok;

    owner.SetMLevel(49);
    std::vector<CItemEquipment::itemLatent> jobLevelID{
        { xi::Latent::JobLevelAbove, 60, Mod::ATT, 3 },
    };
    container.AddLatentEffects(jobLevelID, 50, 5);
    ok = expectBool(container.HasAllLatentsActive(5), true, "job-level condition id does not bypass level gate") && ok;

    std::vector<CItemEquipment::itemLatent> jobLevelValue{
        { xi::Latent::HpUnderPercent, static_cast<uint16>(xi::Latent::JobLevelAbove), Mod::ATT, 4 },
    };
    container.AddLatentEffects(jobLevelValue, 50, 6);
    ok = expectBool(container.HasAllLatentsActive(6), false, "job-level enum value bypasses level gate") && ok;
    ok = expectBool(container.DelLatentEffect(xi::Latent::HpUnderPercent, static_cast<uint16>(xi::Latent::JobLevelAbove), Mod::ATT, 4), true,
                    "bypass latent fields copied") &&
         ok;

    return ok;
}

} // namespace

auto runLatentEffectSelfTests() -> bool
{
    bool ok = true;
    ok      = testLatentEffectConstructor() && ok;
    ok      = testLatentEffectSetters() && ok;
    ok      = testModOnItemOnly() && ok;
    ok      = testLatentActivationPlan() && ok;
    ok      = testLatentDeactivationPlan() && ok;
    ok      = testLatentChangeAccumulation() && ok;
    ok      = testLatentHealthUpdateSelection() && ok;
    ok      = testLatentTransitionPlan() && ok;
    ok      = testLatentEffectContainerBookkeeping() && ok;
    ok      = testLatentEffectContainerEquipmentInsertion() && ok;
    return ok;
}
