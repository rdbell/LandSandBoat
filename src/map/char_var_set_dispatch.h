#pragma once

#include <cstdint>

namespace charvarsetdispatchhelpers
{
enum class Action : std::uint8_t
{
    UpdateLocalCache,
    PersistAndBroadcast,
};

// ActionFor preserves SetCharVar's online/offline dispatch.
constexpr auto ActionFor(const bool isOnline) -> Action
{
    if (isOnline)
    {
        return Action::UpdateLocalCache;
    }

    return Action::PersistAndBroadcast;
}
} // namespace charvarsetdispatchhelpers
