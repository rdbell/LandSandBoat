#pragma once

namespace mobcontrollerroampetfollow
{
// ShouldFollow reports whether a mob's pet should follow while the mob roams.
constexpr auto ShouldFollow(const bool hasPet, const bool petSpawned, const bool petEngaged) -> bool
{
    return hasPet && petSpawned && !petEngaged;
}
} // namespace mobcontrollerroampetfollow
