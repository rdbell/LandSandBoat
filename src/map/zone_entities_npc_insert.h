#pragma once

namespace zoneentities
{

struct NpcInsertionPlan
{
    bool insert{};
    bool transportList{};
    bool warnDuplicate{};

    auto operator==(const NpcInsertionPlan&) const -> bool = default;
};

inline auto PlanNpcInsertion(const bool isNpc, const bool isShipModel, const bool duplicate) -> NpcInsertionPlan
{
    if (!isNpc)
    {
        return {};
    }
    return { true, isShipModel, duplicate };
}

} // namespace zoneentities
