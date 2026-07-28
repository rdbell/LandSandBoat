#pragma once

namespace instanceloader
{

// LoadPlan is CInstanceLoader::LoadInstance's outer failed-instance gate.
struct LoadPlan
{
    bool loadEntities{};
    bool finalize{};

    auto operator==(const LoadPlan&) const -> bool = default;
};

inline auto PlanLoad(const bool instanceFailed) -> LoadPlan
{
    return { !instanceFailed, !instanceFailed };
}

} // namespace instanceloader
