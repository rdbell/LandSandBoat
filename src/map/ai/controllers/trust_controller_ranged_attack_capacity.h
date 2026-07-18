#pragma once

#include <chrono>

namespace trustcontrollerrangedattack
{

inline constexpr auto DefaultDelay = std::chrono::seconds(10);

inline auto CanStart(std::chrono::steady_clock::duration sinceLastAttack, bool hasRangedWeapon,
                     std::chrono::steady_clock::duration weaponDelay, bool inTransit) -> bool
{
    const auto delay = hasRangedWeapon ? weaponDelay : DefaultDelay;
    return !inTransit && sinceLastAttack > delay;
}

} // namespace trustcontrollerrangedattack
