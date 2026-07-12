#pragma once

#include <cstdint>

// Pure GetGender formula from CCharEntity:
//   (look.race % 2) ^ (look.race > 6)

namespace chargenderhelpers
{

// FromRace mirrors CCharEntity::GetGender race-bit formula.
// Races 1-6: odd=male(1), even=female(0). Races 7+ flip (Mithra=0, Galka=1).
constexpr auto FromRace(const std::uint8_t race) -> std::uint8_t
{
    return static_cast<std::uint8_t>((race % 2) ^ static_cast<std::uint8_t>(race > 6));
}

} // namespace chargenderhelpers
