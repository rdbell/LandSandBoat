#pragma once

namespace mobcontrollertypeoneteleportadmission
{
// CanDispatch reports whether an open type-one teleport window can start.
constexpr auto CanDispatch(const bool windowOpen, const bool hasTeleportSkill) -> bool
{
    return windowOpen && hasTeleportSkill;
}
} // namespace mobcontrollertypeoneteleportadmission
