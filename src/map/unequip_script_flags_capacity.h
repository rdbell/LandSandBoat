#pragma once

#include <cstdint>
#include <span>

namespace unequipscriptflagshelpers
{
constexpr uint16_t ScriptEquip = 0x0001;

struct Plan
{
    bool     recomputeEquipFlag{};
    uint16_t equipFlag{};
};

constexpr Plan PlanFor(const uint16_t removedScriptType, const std::span<const uint16_t> remainingScriptTypes)
{
    if ((removedScriptType & ScriptEquip) == 0)
    {
        return {};
    }
    uint16_t equipFlag{};
    for (const auto scriptType : remainingScriptTypes)
    {
        equipFlag |= scriptType;
    }
    return { .recomputeEquipFlag = true, .equipFlag = equipFlag };
}
} // namespace unequipscriptflagshelpers
