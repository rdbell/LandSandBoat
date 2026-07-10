/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_status_effect_container.h"

#include <memory>
#include <set>
#include <iostream>
#include <string>
#include <vector>

#define private public
#include "map/status_effect_container.h"
#undef private

namespace
{

using namespace std::chrono_literals;

auto expect(const bool condition, const std::string& label) -> bool
{
    if (!condition)
    {
        std::cerr << "status-effect container self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto makeEffect(CStatusEffectContainer& container,
                xi::StatusEffect         id,
                uint8                    slot,
                timer::time_point       start,
                uint32                   subID,
                uint16                   sourceParam,
                xi::StatusEffectFlag     flags,
                timer::duration          duration,
                EffectSourceType         sourceType = SOURCE_NONE) -> CStatusEffect*
{
    auto effect = std::make_unique<CStatusEffect>(id, 0, 0, 0s, duration, subID, 0, 0, 0, flags, sourceType, sourceParam, 0, slot);
    effect->SetStartTime(start);
    auto* result = effect.get();
    container.m_StatusEffectSet.insert(std::move(effect));
    return result;
}

auto testQueriesAndOrdering() -> bool
{
    CStatusEffectContainer container(nullptr);
    const auto             base = timer::time_point{ 0s };

    auto* oldPoison = makeEffect(container, xi::StatusEffect::Poison, 3, base, 7, 42, xi::StatusEffectFlag::Dispelable, 30s, SOURCE_MOB);
    auto* newPoison = makeEffect(container, xi::StatusEffect::Poison, 1, base + 1s, 9, 77, xi::StatusEffectFlag::Dispelable, 30s, SOURCE_FOOD);
    auto* deleted   = makeEffect(container, xi::StatusEffect::Blindness, 2, base + 2s, 0, 0, xi::StatusEffectFlag::Erasable, 60s);
    makeEffect(container, xi::StatusEffect::Paralysis, 4, base + 3s, 0, 0, xi::StatusEffectFlag::Dispelable, 0s);
    deleted->markDeleted();

    bool ok = true;
    ok      = expect(container.m_StatusEffectSet.size() == 4, "stored effect count") && ok;

    std::vector<xi::StatusEffect> ordered;
    std::vector<bool>             deletedFlags;
    container.ForEachEffect(
        [&](CStatusEffect& effect)
        {
            ordered.emplace_back(effect.GetStatusID());
            deletedFlags.emplace_back(effect.isDeleted());
        });
    ok = expect(ordered == std::vector<xi::StatusEffect>{ xi::StatusEffect::Poison, xi::StatusEffect::Blindness, xi::StatusEffect::Poison, xi::StatusEffect::Paralysis }, "ordered iteration") && ok;
    ok = expect(deletedFlags == std::vector<bool>{ false, true, false, false }, "deferred deletion iteration") && ok;

    ok = expect(container.GetStatusEffect(xi::StatusEffect::Poison) == newPoison, "first active ID lookup") && ok;
    ok = expect(container.GetStatusEffect(xi::StatusEffect::Blindness) == nullptr, "deleted ID lookup") && ok;
    ok = expect(container.GetStatusEffect(xi::StatusEffect::Poison, 9) == newPoison, "sub-ID lookup") && ok;
    ok = expect(container.GetStatusEffectBySource(xi::StatusEffect::Poison, SOURCE_MOB, 42) == oldPoison, "source lookup") && ok;
    ok = expect(container.HasStatusEffect(xi::StatusEffect::Poison), "ID presence") && ok;
    ok = expect(!container.HasStatusEffect(xi::StatusEffect::Blindness), "deleted ID absence") && ok;
    ok = expect(container.HasStatusEffect({ xi::StatusEffect::Blindness, xi::StatusEffect::Paralysis }), "any-ID presence") && ok;
    ok = expect(container.HasStatusEffectByFlag(xi::StatusEffectFlag::Dispelable | xi::StatusEffectFlag::Erasable), "flag presence") && ok;
    ok = expect(container.GetEffectsCount(xi::StatusEffect::Poison) == 2, "ID count") && ok;
    ok = expect(container.GetEffectsCountWithFlag(xi::StatusEffectFlag::Dispelable | xi::StatusEffectFlag::Erasable) == 2, "positive-duration flag count") && ok;
    ok = expect(container.GetLowestFreeSlot() == 2, "lowest free deleted slot") && ok;

    const auto range = container.GetStatusEffectsInIDRange(xi::StatusEffect::Poison, xi::StatusEffect::Blindness);
    ok = expect(range == std::vector<xi::StatusEffect>{ xi::StatusEffect::Poison, xi::StatusEffect::Poison, xi::StatusEffect::Paralysis }, "inclusive ID range") && ok;
    ok = expect(container.GetStatusEffectCountInIDRange(xi::StatusEffect::Poison, xi::StatusEffect::Paralysis) == 3, "inclusive ID range count") && ok;
    ok = expect(container.GetNewestStatusEffectInIDRange(xi::StatusEffect::Poison, xi::StatusEffect::Paralysis) == xi::StatusEffect::Paralysis, "newest range effect") && ok;
    ok = expect(container.GetNewestStatusEffectInIDRange(xi::StatusEffect::None, xi::StatusEffect::None) == xi::StatusEffect::None, "empty range effect") && ok;

    CStatusEffectContainer endpoint(nullptr);
    makeEffect(endpoint, xi::StatusEffect::Blindness, 1, base, 0, 0, xi::StatusEffectFlag::None, 1s);
    ok = expect(endpoint.GetStatusEffectsInIDRange(xi::StatusEffect::Blindness, xi::StatusEffect::Blindness) == std::vector<xi::StatusEffect>{ xi::StatusEffect::Blindness }, "active inclusive upper endpoint") && ok;
    return ok;
}

} // namespace

auto runStatusEffectContainerSelfTests() -> bool
{
    return testQueriesAndOrdering();
}
