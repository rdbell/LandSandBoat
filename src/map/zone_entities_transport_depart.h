#pragma once

#include <string>
#include <string_view>

namespace zoneentities
{

struct TransportDepartureRecoveryPlan
{
    bool        clearTarget{};
    bool        setScriptFile{};
    std::string scriptFile{};

    auto operator==(const TransportDepartureRecoveryPlan&) const -> bool = default;
};

inline auto PlanTransportDepartureRecovery(const bool hasTarget, const std::string_view scriptFile) -> TransportDepartureRecoveryPlan
{
    if (!hasTarget)
    {
        return {};
    }

    TransportDepartureRecoveryPlan plan{ .clearTarget = true };
    const auto                     deleteStart = scriptFile.find("npcs/");
    const auto                     deleteEnd   = scriptFile.find(".lua");
    if (deleteStart != std::string_view::npos && deleteEnd != std::string_view::npos)
    {
        plan.setScriptFile = true;
        plan.scriptFile    = scriptFile;
        plan.scriptFile.replace(deleteStart, deleteEnd - deleteStart, "Zone");
    }
    return plan;
}

} // namespace zoneentities
