#pragma once

namespace charvarincrementhelpers
{
struct Plan
{
    bool persistIncrement{};
    bool evictLocalCache{};

    constexpr auto operator==(const Plan&) const -> bool = default;
};

// MakePlan preserves IncrementCharVar's null-character behavior.
constexpr auto MakePlan(const bool hasCharacter) -> Plan
{
    if (!hasCharacter)
    {
        return {};
    }

    return {
        .persistIncrement = true,
        .evictLocalCache  = true,
    };
}
} // namespace charvarincrementhelpers
