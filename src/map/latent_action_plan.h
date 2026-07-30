#pragma once

namespace latenthelpers
{

enum class LatentTransitionAction
{
    None,
    Activate,
    Deactivate,
};

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

struct LatentDeactivationPlan
{
    bool changed{};
    bool removeItemModifier{};
    bool removeOwnerModifier{};
    bool rebuildWeaponSkills{};
    bool pushCommandData{};
    bool markDeactivated{};

    constexpr auto operator==(const LatentDeactivationPlan&) const -> bool = default;
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

constexpr auto PlanLatentDeactivation(const bool activated, const bool itemOnly, const bool hasRememberedItem) -> LatentDeactivationPlan
{
    if (!activated)
    {
        return {};
    }

    LatentDeactivationPlan plan{
        .changed         = true,
        .markDeactivated = true,
    };

    if (!itemOnly)
    {
        plan.removeOwnerModifier = true;
        return plan;
    }

    if (hasRememberedItem)
    {
        plan.removeItemModifier  = true;
        plan.rebuildWeaponSkills = true;
        plan.pushCommandData     = true;
    }

    return plan;
}

constexpr auto PlanLatentTransition(const bool expression, const bool activated) -> LatentTransitionAction
{
    if (expression)
    {
        return activated ? LatentTransitionAction::None : LatentTransitionAction::Activate;
    }

    return activated ? LatentTransitionAction::Deactivate : LatentTransitionAction::None;
}

template <typename Activate, typename Deactivate>
constexpr auto ApplyLatentTransition(const LatentTransitionAction action, Activate&& activate, Deactivate&& deactivate) -> bool
{
    switch (action)
    {
        case LatentTransitionAction::Activate:
            return activate();
        case LatentTransitionAction::Deactivate:
            return deactivate();
        case LatentTransitionAction::None:
        default:
            return false;
    }
}

} // namespace latenthelpers
