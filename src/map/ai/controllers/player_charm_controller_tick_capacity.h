#pragma once

#include <cstdint>

namespace playercharmcontrollertick
{

enum class Route : uint8_t
{
    RemoveCharm,
    Combat,
    Roam,
};

inline auto Resolve(bool hasMaster, bool masterAlive, bool ownerEngaged) -> Route
{
    if (!hasMaster || !masterAlive)
    {
        return Route::RemoveCharm;
    }
    return ownerEngaged ? Route::Combat : Route::Roam;
}

} // namespace playercharmcontrollertick
