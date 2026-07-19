#pragma once

namespace mobcontrollertypetwoteleport
{
inline auto CanStart(bool hasSkill, bool withinRange) -> bool
{
    return hasSkill && withinRange;
}
} // namespace mobcontrollertypetwoteleport
