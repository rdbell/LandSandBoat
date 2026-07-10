/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_status_effect_data.h"

#include "map/data/loader.h"

#include "data/enums/effect_overwrite.h"
#include "data/enums/element.h"
#include "data/enums/status_effect.h"
#include "data/enums/status_effect_flag.h"

#include <cstdint>
#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const std::string& label) -> bool
{
    if (!condition)
    {
        std::cerr << "status-effect data self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto testStatusEffectData() -> bool
{
    const auto data = LoadStatusEffects();
    bool       ok   = true;

    ok = expect(data.size() == 668, "status effect row count") && ok;

    const auto& poison = data.at(3);
    ok                  = expect(poison.Name == "poison", "poison default name") && ok;
    ok                  = expect(poison.Flags == (xi::StatusEffectFlag::Death | xi::StatusEffectFlag::Waltzable | xi::StatusEffectFlag::NoCancel), "poison flags") && ok;
    ok                  = expect(poison.Overwrite == xi::EffectOverwrite::Never, "poison overwrite") && ok;
    ok                  = expect(poison.Element == xi::Element::Water, "poison element") && ok;
    ok                  = expect(poison.MinDuration == 1, "poison minimum duration") && ok;
    ok                  = expect(poison.SortKey == 0, "poison raw sort key") && ok;
    ok                  = expect(poison.WearOffMessageId == 204, "poison wear-off message") && ok;

    const auto& sleep = data.at(2);
    ok                = expect(sleep.Name == "sleep", "sleep override name") && ok;
    ok                = expect(sleep.ExclusionGroup == xi::StatusEffect::SleepI, "sleep exclusion group") && ok;
    ok                = expect(sleep.Negative == xi::StatusEffect::SleepIi, "sleep negative effect") && ok;
    ok                = expect(sleep.Block == xi::StatusEffect::Lullaby, "sleep block effect") && ok;
    ok                = expect(sleep.Overwrite == xi::EffectOverwrite::Higher, "sleep overwrite") && ok;
    ok                = expect(sleep.Element == xi::Element::Dark, "sleep element") && ok;

    const auto& none = data.at(255);
    ok               = expect(none.Name == "none", "none default name") && ok;
    ok               = expect(none.Flags == xi::StatusEffectFlag::None, "none flags default") && ok;
    ok               = expect(none.Overwrite == xi::EffectOverwrite::EqualHigher, "none overwrite default") && ok;
    ok               = expect(none.SortKey == 0, "none sort key default") && ok;
    ok               = expect(none.WearOffMessageId == 0, "none wear-off default") && ok;

    const auto& aura = data.at(808);
    ok               = expect(aura.Name == "trust_aura_haste", "aura default name") && ok;
    ok               = expect(aura.Flags == (xi::StatusEffectFlag::Death | xi::StatusEffectFlag::NoCancel | xi::StatusEffectFlag::HideTimer), "aura flags") && ok;

    return ok;
}

} // namespace

auto runStatusEffectDataSelfTests() -> bool
{
    return testStatusEffectData();
}
