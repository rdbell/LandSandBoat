#pragma once

#include <string>
#include <utility>

namespace forcedsynthhelpers
{

struct Plan
{
    std::string sourceFunction;
    std::string characterName;
    bool        criticalFail = true;

    auto operator==(const Plan&) const -> bool = default;
};

inline auto MakePlan(std::string sourceFunction, std::string characterName) -> Plan
{
    return { std::move(sourceFunction), std::move(characterName), true };
}

} // namespace forcedsynthhelpers
