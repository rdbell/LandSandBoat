#pragma once

namespace latenthelpers
{

enum class LatentTransitionAction
{
    None,
    Activate,
    Deactivate,
};

enum class LatentActivationAction
{
    AddItemModifier,
    RebuildWeaponSkills,
    PushCommandData,
    RememberItem,
    AddOwnerModifier,
    MarkActivated,
};

enum class LatentDeactivationAction
{
    RemoveItemModifier,
    RebuildWeaponSkills,
    PushCommandData,
    RemoveOwnerModifier,
    MarkDeactivated,
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

template <typename Apply>
constexpr auto ApplyLatentActivationPlan(const LatentActivationPlan& plan, Apply&& apply) -> bool
{
    if (!plan.changed)
    {
        return false;
    }
    if (plan.addItemModifier)
    {
        apply(LatentActivationAction::AddItemModifier);
    }
    if (plan.rebuildWeaponSkills)
    {
        apply(LatentActivationAction::RebuildWeaponSkills);
    }
    if (plan.pushCommandData)
    {
        apply(LatentActivationAction::PushCommandData);
    }
    if (plan.rememberItem)
    {
        apply(LatentActivationAction::RememberItem);
    }
    if (plan.addOwnerModifier)
    {
        apply(LatentActivationAction::AddOwnerModifier);
    }
    if (plan.markActivated)
    {
        apply(LatentActivationAction::MarkActivated);
    }
    return true;
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

template <typename Apply>
constexpr auto ApplyLatentDeactivationPlan(const LatentDeactivationPlan& plan, Apply&& apply) -> bool
{
    if (!plan.changed)
    {
        return false;
    }
    if (plan.removeItemModifier)
    {
        apply(LatentDeactivationAction::RemoveItemModifier);
    }
    if (plan.rebuildWeaponSkills)
    {
        apply(LatentDeactivationAction::RebuildWeaponSkills);
    }
    if (plan.pushCommandData)
    {
        apply(LatentDeactivationAction::PushCommandData);
    }
    if (plan.removeOwnerModifier)
    {
        apply(LatentDeactivationAction::RemoveOwnerModifier);
    }
    if (plan.markDeactivated)
    {
        apply(LatentDeactivationAction::MarkDeactivated);
    }
    return true;
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
