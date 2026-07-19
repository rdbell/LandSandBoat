#pragma once

namespace mobcontrollerteleportwindow
{
inline auto CanStartTypeOne(bool outsideAttackRange, bool within30, bool cooldownReady) -> bool
{
    return outsideAttackRange && within30 && cooldownReady;
}
} // namespace mobcontrollerteleportwindow
