#pragma once

namespace mobcontrollersightmode
{
// IsEnabled reports whether ordinary or forced sight detection is active.
constexpr auto IsEnabled(const bool hasSightDetection, const bool forceSight) -> bool
{
    return hasSightDetection || forceSight;
}
} // namespace mobcontrollersightmode
