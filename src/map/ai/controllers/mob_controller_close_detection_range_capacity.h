#pragma once

namespace mobcontrollerclosedetectionrange
{
// IsInRange reports whether a target is close enough for low-HP and action-state detection.
constexpr auto IsInRange(const float distance) -> bool
{
    return distance <= 20.0f;
}
} // namespace mobcontrollerclosedetectionrange
