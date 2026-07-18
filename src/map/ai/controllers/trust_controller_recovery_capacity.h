#pragma once

#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>

namespace trustcontrollerrecovery
{

inline constexpr std::array Delays = { std::chrono::seconds(15), std::chrono::seconds(10), std::chrono::seconds(10), std::chrono::seconds(3) };

struct Plan
{
    bool        recover;
    std::size_t nextHealingTick;
};

inline auto Resolve(bool canRest, bool needsRecovery, std::chrono::steady_clock::duration lastAttackedAgo,
                    std::chrono::steady_clock::duration combatEndedAgo, std::chrono::steady_clock::duration lastRecoveryAgo,
                    std::size_t healingTick) -> Plan
{
    healingTick = std::min(healingTick, Delays.size() - 1);
    if (!canRest || !needsRecovery || lastAttackedAgo <= Delays.front() || combatEndedAgo <= Delays.front() || lastRecoveryAgo <= Delays.at(healingTick))
    {
        return {};
    }

    return { true, std::min(healingTick + 1, Delays.size() - 1) };
}

inline auto Amount(uint32_t maximum) -> uint32_t
{
    return static_cast<uint32_t>(maximum * 0.05);
}

} // namespace trustcontrollerrecovery
