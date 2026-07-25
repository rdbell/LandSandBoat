#pragma once

namespace invisibleremovalhelpers
{
// ShouldRemoveInvisible preserves RemoveInvisible's host-admission gate.
constexpr auto ShouldRemoveInvisible(const bool characterPresent, const bool statusEffectContainerPresent) -> bool
{
    return characterPresent && statusEffectContainerPresent;
}
} // namespace invisibleremovalhelpers
