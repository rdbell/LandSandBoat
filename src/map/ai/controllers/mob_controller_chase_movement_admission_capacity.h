#pragma once

namespace mobcontrollerchasemovementadmission
{
// CanEnter reports whether Move may begin its chase teleport or path work.
constexpr auto CanEnter(const bool hasSpeed, const bool movementAllowed, const bool cooldownReady) -> bool
{
    return hasSpeed && movementAllowed && cooldownReady;
}
} // namespace mobcontrollerchasemovementadmission
