#pragma once

#include <cstdint>

namespace charvarfetchhelpers
{
struct Plan
{
    std::int32_t value{};
    std::uint32_t expiry{};
    bool          deleteExpired{};

    constexpr auto operator==(const Plan&) const -> bool = default;
};

// MakePlan preserves FetchCharVar's row and expiry handling.
constexpr auto MakePlan(
    const bool          hasRow,
    const std::int32_t  value,
    const std::uint32_t expiry,
    const std::uint32_t now) -> Plan
{
    if (!hasRow)
    {
        return {};
    }
    if (expiry > 0 && expiry <= now)
    {
        return {
            .expiry        = expiry,
            .deleteExpired = true,
        };
    }
    return {
        .value  = value,
        .expiry = expiry,
    };
}
} // namespace charvarfetchhelpers
