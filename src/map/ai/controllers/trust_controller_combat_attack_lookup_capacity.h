#pragma once

#include <cstdint>

namespace trustcontrollercombatattacklookup
{
template <typename CanAttack>
constexpr auto Resolve(const int16_t movementDistance, CanAttack&& canAttack) -> bool
{
    return movementDistance == 0 && canAttack();
}
} // namespace trustcontrollercombatattacklookup
