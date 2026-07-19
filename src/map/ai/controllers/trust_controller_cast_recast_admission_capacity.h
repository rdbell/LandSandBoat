#pragma once

namespace trustcontrollercastrecastadmission
{
// CanStart reports whether a trust spell is not on magic recast.
constexpr auto CanStart(const bool hasRecast) -> bool
{
    return !hasRecast;
}
} // namespace trustcontrollercastrecastadmission
