#pragma once

#include <cstdint>
#include <functional>

namespace chartimeddeathhelpers
{

struct Input
{
    bool         hasWeakness{};
    std::uint8_t weaknessLevel{};
    std::uint8_t hasRaise{};
    bool         reraiseI{};
    bool         reraiseII{};
    bool         reraiseIII{};
    bool         mijinReraise{};
};

struct Plan
{
    bool         removeWeakness{};
    std::uint8_t weaknessLevel{};
    std::uint8_t hasRaise{};
};

constexpr auto Build(const Input& input) -> Plan
{
    Plan plan{
        .removeWeakness = input.hasWeakness,
        .hasRaise       = input.hasRaise,
    };
    if (input.hasWeakness)
    {
        plan.weaknessLevel = static_cast<std::uint8_t>(input.weaknessLevel + 1);
    }
    if (input.reraiseI)
    {
        plan.hasRaise = 1;
    }
    if (input.reraiseII)
    {
        plan.hasRaise = 2;
    }
    if (input.reraiseIII)
    {
        plan.hasRaise = 3;
    }
    if (plan.hasRaise == 0 && input.mijinReraise)
    {
        plan.hasRaise = 1;
    }
    return plan;
}

template <typename Duration,
          typename ClearTrusts,
          typename RemoveWeakness,
          typename SetWeaknessLevel,
          typename SetDeathSyncTime,
          typename ClearAIStateStack,
          typename InternalDie,
          typename ResetAllegiance,
          typename SetHasRaise,
          typename IncrementKnockouts,
          typename BaseDie>
inline void Apply(const Plan& plan,
                  const Duration duration,
                  ClearTrusts&& clearTrusts,
                  RemoveWeakness&& removeWeakness,
                  SetWeaknessLevel&& setWeaknessLevel,
                  SetDeathSyncTime&& setDeathSyncTime,
                  ClearAIStateStack&& clearAIStateStack,
                  InternalDie&& internalDie,
                  ResetAllegiance&& resetAllegiance,
                  SetHasRaise&& setHasRaise,
                  IncrementKnockouts&& incrementKnockouts,
                  BaseDie&& baseDie)
{
    std::invoke(clearTrusts);
    if (plan.removeWeakness)
    {
        std::invoke(removeWeakness);
    }
    std::invoke(setWeaknessLevel, plan.weaknessLevel);
    std::invoke(setDeathSyncTime);
    std::invoke(clearAIStateStack);
    std::invoke(internalDie, duration);
    std::invoke(resetAllegiance);
    std::invoke(setHasRaise, plan.hasRaise);
    std::invoke(incrementKnockouts);
    std::invoke(baseDie);
}

} // namespace chartimeddeathhelpers
