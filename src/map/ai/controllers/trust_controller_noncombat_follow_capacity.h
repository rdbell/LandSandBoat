#pragma once

#include <cstdint>

namespace trustcontrollernoncombatfollow
{
struct Plan
{
    bool  followMaster;
    bool  followPrevious;
    float distance;
};

constexpr auto Resolve(const uint8_t partyPosition, const bool previousExists, const bool previousIsSelf) -> Plan
{
    if (partyPosition == 0)
    {
        return { true, false, 3.0f };
    }
    if (previousExists && !previousIsSelf)
    {
        return { false, true, 4.0f };
    }
    return { true, false, 4.0f };
}
} // namespace trustcontrollernoncombatfollow
