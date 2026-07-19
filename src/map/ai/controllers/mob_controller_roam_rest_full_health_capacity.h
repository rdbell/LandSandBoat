#pragma once

namespace mobcontrollerroamrestfullhealth
{
// ShouldResetExperience reports whether a completed rest restores normal experience settings.
constexpr auto ShouldResetExperience(const unsigned char hpPercent) -> bool
{
    return hpPercent == 100;
}
} // namespace mobcontrollerroamrestfullhealth
