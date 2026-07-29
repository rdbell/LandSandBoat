#pragma once

namespace latenthelpers
{

struct LatentActivationPlan
{
    bool changed{};
    bool addItemModifier{};
    bool addOwnerModifier{};
    bool rebuildWeaponSkills{};
    bool pushCommandData{};
    bool rememberItem{};
    bool markActivated{};

    constexpr auto operator==(const LatentActivationPlan&) const -> bool = default;
};

constexpr auto PlanLatentActivation(const bool alreadyActivated, const bool itemOnly, const bool hasEquippedItem) -> LatentActivationPlan
{
    if (alreadyActivated)
    {
        return {};
    }

    LatentActivationPlan plan{
        .changed       = true,
        .markActivated = true,
    };

    if (!itemOnly)
    {
        plan.addOwnerModifier = true;
        return plan;
    }

    if (hasEquippedItem)
    {
        plan.addItemModifier     = true;
        plan.rebuildWeaponSkills = true;
        plan.pushCommandData     = true;
        plan.rememberItem        = true;
    }

    return plan;
}

} // namespace latenthelpers
