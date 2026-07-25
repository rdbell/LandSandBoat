#pragma once

#include <cstdint>

// Pure SetUnityLeader policy from charutils.

namespace unityleaderhelpers
{
struct UnityLeaderChangePlan
{
    bool    accepted{};
    uint8_t newLeader{};
    bool    removeExistingMember{};
    bool    addNewMember{};
    bool    persist{};

    constexpr auto operator==(const UnityLeaderChangePlan&) const -> bool = default;
};

// PlanUnityLeaderChange mirrors SetUnityLeader after the caller resolves
// whether a Unity-chat membership currently exists.
constexpr auto PlanUnityLeaderChange(const uint8_t leaderID, const bool hasUnityChat) -> UnityLeaderChangePlan
{
    if (leaderID < 1 || leaderID > 11)
    {
        return {};
    }

    return {
        .accepted             = true,
        .newLeader            = leaderID,
        .removeExistingMember = hasUnityChat,
        .addNewMember         = true,
        .persist              = true,
    };
}
} // namespace unityleaderhelpers
