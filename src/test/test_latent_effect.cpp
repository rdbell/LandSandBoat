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
#include "latent_effect.h"

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

} // namespace

auto runLatentEffectSelfTests() -> bool
{
    bool ok = true;
    ok      = testLatentEffectConstructor() && ok;
    ok      = testLatentEffectSetters() && ok;
    ok      = testModOnItemOnly() && ok;
    return ok;
}
