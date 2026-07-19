#pragma once

#include <cstdint>

namespace trustcontrollercombatdeclumpdisplacement
{
constexpr auto Amount(const float randomAmount, const uint8_t partyPosition) -> float
{
    return randomAmount * ((partyPosition % 2) ? 1.0f : -1.0f);
}
} // namespace trustcontrollercombatdeclumpdisplacement
