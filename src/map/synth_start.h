#pragma once

#include <array>
#include <cstdint>

namespace synthstarthelpers
{

inline constexpr std::uint8_t CraftSkillWoodworking = 49;

struct Plan
{
    bool recordLastSynthTime{};
    bool startTransaction{};
    bool warnClaimFailure{};
    bool sendCancelBadRecipe{};
    bool consumeCrystal{};
    bool resolveResult{};
    bool setAnimationSynth{};
    bool setUpdateHP{};
    bool sendCharStatus{};
    bool startCharacterSynth{};
    bool broadcastSynthEffect{};

    constexpr auto operator==(const Plan&) const -> bool = default;
};

constexpr auto MakePlan(const bool recipeResolved, const bool transactionStarted) -> Plan
{
    Plan plan{ .recordLastSynthTime = true };
    if (!recipeResolved)
    {
        return plan;
    }

    plan.startTransaction = true;
    if (!transactionStarted)
    {
        plan.warnClaimFailure    = true;
        plan.sendCancelBadRecipe = true;
        return plan;
    }

    plan.consumeCrystal       = true;
    plan.resolveResult        = true;
    plan.setAnimationSynth    = true;
    plan.setUpdateHP          = true;
    plan.sendCharStatus       = true;
    plan.startCharacterSynth  = true;
    plan.broadcastSynthEffect = true;
    return plan;
}

constexpr auto HighestRequiredCraft(const std::array<std::uint8_t, 8>& requirements) -> std::uint8_t
{
    std::uint8_t craft{};
    std::uint8_t highest{};
    for (std::uint8_t index = 0; index < requirements.size(); ++index)
    {
        if (requirements[index] > highest)
        {
            craft   = CraftSkillWoodworking + index;
            highest = requirements[index];
        }
    }
    return craft;
}

} // namespace synthstarthelpers
