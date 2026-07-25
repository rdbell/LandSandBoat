#pragma once

#include "enums/key_items.h"

#include <array>
#include <cstdint>

namespace skilluprovhelpers
{
constexpr std::array<KeyItem, 3> SkillUpIncreaseKeyItems = {
    KeyItem::RHAPSODY_IN_WHITE,
    KeyItem::RHAPSODY_IN_CRIMSON,
    KeyItem::RHAPSODY_IN_FUCHSIA,
};

struct Facts
{
    std::array<bool, SkillUpIncreaseKeyItems.size()> held{};
};

struct Plan
{
    uint8_t heldCount{};
};

constexpr auto PlanFor(const Facts& facts) -> Plan
{
    Plan plan;
    for (const auto hasKeyItem : facts.held)
    {
        if (hasKeyItem)
        {
            plan.heldCount += 1;
        }
    }
    return plan;
}
} // namespace skilluprovhelpers
