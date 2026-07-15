#pragma once

#include <algorithm>
#include <cstdint>

// Pure primary-stat resolution from CBattleEntity::STR/DEX/VIT/AGI/INT/MND/CHR.
// Parity: internal/primarystat (slice 1637).

namespace primarystathelpers
{

// Hard clamp bounds applied by every primary-stat getter.
constexpr std::int32_t PrimaryStatClampMin = 0;
constexpr std::int32_t PrimaryStatClampMax = 999;

// ClampPrimaryStat mirrors std::clamp(sum, 0, 999) → uint16.
inline auto ClampPrimaryStat(const std::int32_t sum) -> std::uint16_t
{
    return static_cast<std::uint16_t>(std::clamp(sum, PrimaryStatClampMin, PrimaryStatClampMax));
}

// ResolvePrimaryStat mirrors base + mod then clamp for DEX/VIT/AGI/INT/MND/CHR
// (and STR when the main weapon is not two-handed).
// Production: base is stats.X (uint16); mod is m_modStat[Mod::X] (int16).
inline auto ResolvePrimaryStat(const std::int16_t base, const std::int16_t mod) -> std::uint16_t
{
    return ClampPrimaryStat(static_cast<std::int32_t>(base) + static_cast<std::int32_t>(mod));
}

// ResolveSTR mirrors CBattleEntity::STR().
// When mainIsTwoHanded, Mod::TWOHAND_STR is included (Hasso path); otherwise
// only base STR + Mod::STR (twoHandStrMod is ignored).
inline auto ResolveSTR(
    const std::int16_t base,
    const std::int16_t strMod,
    const std::int16_t twoHandStrMod,
    const bool         mainIsTwoHanded) -> std::uint16_t
{
    if (mainIsTwoHanded)
    {
        return ClampPrimaryStat(
            static_cast<std::int32_t>(base) +
            static_cast<std::int32_t>(strMod) +
            static_cast<std::int32_t>(twoHandStrMod));
    }
    return ResolvePrimaryStat(base, strMod);
}

} // namespace primarystathelpers
