#pragma once

#include <cmath>
#include <cstdint>

// Pure CalculateLuopanStats HP / visual pins.
// Parity: internal/luopan stats.go (slice 1610).

namespace luopanstatshelpers
{

// Entity flags pin (visual size; pets do not yet use DB entity flags).
constexpr std::uint32_t EntityFlags = 0x0000008B;

// baseSpeed pin (sit, do nothing).
constexpr std::uint8_t BaseSpeed = 0;

// Bolster JP max-HP scale per point (0.03 = 3%).
constexpr float BolsterHPScalePerJP = 0.03f;

// Base max HP: floor((250 * mLvl) / 15) via integer division.
constexpr auto BaseMaxHP(const std::uint8_t mLvl) -> std::uint32_t
{
    return static_cast<std::uint32_t>((250 * static_cast<unsigned>(mLvl)) / 15);
}

// Bolster bonus: floor(baseMaxHP * (0.03 * bolsterJP)).
inline auto BolsterMaxHPBonus(const std::uint32_t baseMaxHP, const std::uint8_t bolsterJP) -> std::uint32_t
{
    if (bolsterJP == 0 || baseMaxHP == 0)
    {
        return 0;
    }
    return static_cast<std::uint32_t>(std::floor(static_cast<double>(baseMaxHP) * (static_cast<double>(BolsterHPScalePerJP) * static_cast<double>(bolsterJP))));
}

// Full max HP with optional Bolster.
inline auto MaxHP(const std::uint8_t mLvl, const bool hasBolster, const std::uint8_t bolsterJP) -> std::uint32_t
{
    const auto base = BaseMaxHP(mLvl);
    if (!hasBolster)
    {
        return base;
    }
    return base + BolsterMaxHPBonus(base, bolsterJP);
}

} // namespace luopanstatshelpers
