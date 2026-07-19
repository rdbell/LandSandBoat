#pragma once

namespace mobcontrollerengagepet
{
// ShouldEngage reports whether a mob's pet should join its newly engaged target.
constexpr auto ShouldEngage(const bool mobEngaged, const bool hasPet, const bool petEngaged) -> bool
{
    return mobEngaged && hasPet && !petEngaged;
}
} // namespace mobcontrollerengagepet
