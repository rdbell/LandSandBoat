#pragma once

#include <cstdint>

// Pure petutils::PerpetuationCost base MP ladders by pet id + level.
// Parity: internal/petutils perpetuation.go (slice 0810 pure; 1613 production wire).

namespace perpetuationhelpers
{

// PETID pins (petutils.h).
constexpr std::uint32_t PetIDDarkSpirit = 7;
constexpr std::uint32_t PetIDCarbuncle  = 8;
constexpr std::uint32_t PetIDFenrir     = 9;
constexpr std::uint32_t PetIDDiabolos   = 16;
constexpr std::uint32_t PetIDCaitSith   = 20;
constexpr std::uint32_t PetIDSiren      = 76;

constexpr auto SpiritCost(const std::uint8_t level) -> std::int16_t
{
    if (level < 19)
    {
        return 1;
    }
    if (level < 38)
    {
        return 2;
    }
    if (level < 57)
    {
        return 3;
    }
    if (level < 75)
    {
        return 4;
    }
    if (level < 81)
    {
        return 5;
    }
    if (level < 91)
    {
        return 6;
    }
    return 7;
}

constexpr auto CarbuncleCost(const std::uint8_t level) -> std::int16_t
{
    if (level < 10)
    {
        return 1;
    }
    if (level < 18)
    {
        return 2;
    }
    if (level < 27)
    {
        return 3;
    }
    if (level < 36)
    {
        return 4;
    }
    if (level < 45)
    {
        return 5;
    }
    if (level < 54)
    {
        return 6;
    }
    if (level < 63)
    {
        return 7;
    }
    if (level < 72)
    {
        return 8;
    }
    if (level < 81)
    {
        return 9;
    }
    if (level < 91)
    {
        return 10;
    }
    return 11;
}

constexpr auto FenrirCost(const std::uint8_t level) -> std::int16_t
{
    if (level < 8)
    {
        return 1;
    }
    if (level < 15)
    {
        return 2;
    }
    if (level < 22)
    {
        return 3;
    }
    if (level < 30)
    {
        return 4;
    }
    if (level < 37)
    {
        return 5;
    }
    if (level < 45)
    {
        return 6;
    }
    if (level < 51)
    {
        return 7;
    }
    if (level < 59)
    {
        return 8;
    }
    if (level < 66)
    {
        return 9;
    }
    if (level < 73)
    {
        return 10;
    }
    if (level < 81)
    {
        return 11;
    }
    if (level < 91)
    {
        return 12;
    }
    return 13;
}

constexpr auto AvatarCost(const std::uint8_t level) -> std::int16_t
{
    if (level < 10)
    {
        return 3;
    }
    if (level < 19)
    {
        return 4;
    }
    if (level < 28)
    {
        return 5;
    }
    if (level < 38)
    {
        return 6;
    }
    if (level < 47)
    {
        return 7;
    }
    if (level < 56)
    {
        return 8;
    }
    if (level < 65)
    {
        return 9;
    }
    if (level < 68)
    {
        return 10;
    }
    if (level < 71)
    {
        return 11;
    }
    if (level < 74)
    {
        return 12;
    }
    if (level < 81)
    {
        return 13;
    }
    if (level < 91)
    {
        return 14;
    }
    return 15;
}

// Full PerpetuationCost branch order matching petutils.cpp.
constexpr auto PerpetuationCost(const std::uint32_t id, const std::uint8_t level) -> std::int16_t
{
    if (id <= PetIDDarkSpirit)
    {
        return SpiritCost(level);
    }
    if (id == PetIDCarbuncle || id == PetIDCaitSith)
    {
        return CarbuncleCost(level);
    }
    if (id == PetIDFenrir)
    {
        return FenrirCost(level);
    }
    // Covers Ifrit..Diabolos after earlier branches; Siren explicit.
    if (id <= PetIDDiabolos || id == PetIDSiren)
    {
        return AvatarCost(level);
    }
    return 0;
}

} // namespace perpetuationhelpers
