#pragma once

#include <cstdint>

// Pure CBattleEntity::inMogHouse / CCharEntity moghouse gate.
// Parity: internal/inmoghouse (slice 1670).

namespace inmoghousehelpers
{

// InMogHouse: non-PC → false; PC → moghouseID != 0.
constexpr auto InMogHouse(const bool isPC, const std::uint32_t moghouseID) -> bool
{
    if (!isPC)
    {
        return false;
    }
    return moghouseID != 0;
}

} // namespace inmoghousehelpers
