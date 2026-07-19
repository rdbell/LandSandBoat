#pragma once

namespace mobcontrollerambushdetection
{
// CanDetect reports whether ambush behavior immediately detects the target.
constexpr auto CanDetect(const bool ambushBehavior, const float distance, const bool hasSneak) -> bool
{
    return ambushBehavior && distance < 3.0f && !hasSneak;
}
} // namespace mobcontrollerambushdetection
