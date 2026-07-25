#pragma once

#include <cstdint>

// Pure CalculateEnspellDamage Tier 1 / Tier 2 base halves after injects.
// Parity: internal/attackutils EnspellTier2Cap / CalculateEnspellTier1Damage /
// CalculateEnspellTier2Damage / CalculateEnspellTierDamage (slice 0804 / 6767).
//
// Host residual: equip-exclude bonus, skill/merit/entity reads, writing
// ENSPELL_DMG after Tier 2 ramp, Tier 3/4 paths, % mult / resist / day / absorb.

namespace enspelldamagetierhelpers
{

// skill → Tier 2 potency cap (uint16 integer arithmetic).
//   skill <= 200: 2 * (3 + 6 * skill / 100)
//   skill > 200:  2 * (5 + 5 * skill / 100)
constexpr auto EnspellTier2Cap(const std::uint16_t skill) -> std::uint16_t
{
    std::uint16_t cap = static_cast<std::uint16_t>(3 + 6 * skill / 100);
    if (skill > 200)
    {
        cap = static_cast<std::uint16_t>(5 + 5 * skill / 100);
    }
    return static_cast<std::uint16_t>(cap * 2);
}

// Tier 1: cast-time base; no ENSPELL_DMG mutation.
// damage = enspellDMG + bonus + merit
constexpr auto CalculateEnspellTier1Damage(const std::int32_t enspellDMG,
                                           const std::int32_t bonus,
                                           const std::int32_t merit) -> std::int32_t
{
    return enspellDMG + bonus + merit;
}

// Tier 2 pure product. Returns hit damage (before % mult / resist) and the
// post-hit ENSPELL_DMG the host should write when it differs from input.
struct EnspellTier2Result
{
    std::int32_t damage;
    std::int32_t newEnspellDMG;
};

// cap = EnspellTier2Cap(skill)
// if enspellDMG > cap:  new = cap; damage = cap
// if enspellDMG == cap: new = enspellDMG; damage = cap
// if enspellDMG < cap:  new = enspellDMG + 1; damage = new - 1  // prior potency
// damage += bonus + merit * 2
constexpr auto CalculateEnspellTier2Damage(const std::int32_t enspellDMG,
                                           const std::uint16_t skill,
                                           const std::int32_t bonus,
                                           const std::int32_t merit) -> EnspellTier2Result
{
    const auto cap    = static_cast<std::int32_t>(EnspellTier2Cap(skill));
    auto       damage = static_cast<std::int32_t>(0);
    auto       newDMG = enspellDMG;

    if (enspellDMG > cap)
    {
        newDMG = cap;
        damage = cap;
    }
    else if (enspellDMG == cap)
    {
        damage = cap;
    }
    else // enspellDMG < cap
    {
        newDMG = enspellDMG + 1;
        damage = newDMG - 1;
    }

    damage += bonus;
    damage += merit * 2;
    return EnspellTier2Result{ damage, newDMG };
}

} // namespace enspelldamagetierhelpers
