#pragma once

namespace zoneentities
{

enum class DynamicTargidOwner
{
    None,
    Instance,
    Zone,
};

struct SummonInsertionPlan
{
    bool               insert{};
    DynamicTargidOwner targidOwner{ DynamicTargidOwner::None };
    bool               resetSpawnAnimation{};

    auto operator==(const SummonInsertionPlan&) const -> bool = default;
};

inline auto PlanSummonInsertion(const bool entityPresent, const bool hasInstance) -> SummonInsertionPlan
{
    if (!entityPresent)
    {
        return {};
    }
    return { true, hasInstance ? DynamicTargidOwner::Instance : DynamicTargidOwner::Zone, true };
}

} // namespace zoneentities
