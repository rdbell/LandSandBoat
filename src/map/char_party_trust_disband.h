#pragma once

#include <cstdint>

namespace partytrustdisbandhelpers
{
struct Plan
{
    bool checkMemberCountAcrossProcesses{};

    constexpr auto operator==(const Plan&) const -> bool = default;
};

// MakePlan preserves ReloadParty's local admission to the final
// trust-release disband check.
constexpr auto MakePlan(const bool hasParty, const bool hasOnlyOneMember, const bool hasNoTrusts) -> Plan
{
    return { .checkMemberCountAcrossProcesses = hasParty && hasOnlyOneMember && hasNoTrusts };
}

// ShouldDisband preserves ReloadParty's cross-process party-size gate.
constexpr auto ShouldDisband(const std::uint32_t memberCountAcrossProcesses) -> bool
{
    return memberCountAcrossProcesses == 1;
}
} // namespace partytrustdisbandhelpers
