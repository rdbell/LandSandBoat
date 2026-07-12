#pragma once

#include <cstdint>

// Pure GetBaseDelay / GetBaseRangedDelay assembly after weapon delay injects.
// Parity: internal/attackutils base_delay.go
// Game delay units (getBaseDelay), not ms-style GetWeaponDelay.

namespace basedelayhelpers
{

constexpr std::uint16_t UnequippedH2HBaseDelay = 480;

struct MeleeBaseDelayParams
{
    bool          isPC{};
    bool          isMob{};
    bool          hasMainWeapon{};
    std::uint16_t mainDelay{};
    bool          isH2H{};
    bool          hasSubWeapon{};
    std::uint16_t subDelay{};
};

constexpr auto GetBaseDelay(const MeleeBaseDelayParams& p) -> std::uint16_t
{
    std::uint16_t base = UnequippedH2HBaseDelay;
    if (p.isPC)
    {
        if (!p.hasMainWeapon)
        {
            return base;
        }
        if (p.isH2H)
        {
            return p.mainDelay;
        }
        base = p.mainDelay;
        if (p.hasSubWeapon)
        {
            base = static_cast<std::uint16_t>(base + p.subDelay);
        }
        return base;
    }
    if (p.isMob && p.hasMainWeapon)
    {
        return p.mainDelay;
    }
    return base;
}

struct RangedBaseDelayParams
{
    bool          isPC{};
    bool          isMob{};
    bool          hasRanged{};
    std::uint16_t rangedDelay{};
    bool          rangedIsRanged{};
    bool          rangedIsThrowing{};
    bool          hasAmmo{};
    std::uint16_t ammoDelay{};
    bool          ammoIsRanged{};
    bool          hasMobMain{};
    std::uint16_t mobMainDelay{};
};

constexpr auto GetBaseRangedDelay(const RangedBaseDelayParams& p) -> std::uint16_t
{
    if (p.isPC)
    {
        if (p.hasRanged && p.rangedIsRanged)
        {
            if (p.rangedIsThrowing)
            {
                return p.rangedDelay;
            }
            if (p.hasAmmo)
            {
                return static_cast<std::uint16_t>(p.rangedDelay + p.ammoDelay);
            }
            return 0;
        }
        if (p.hasAmmo && p.ammoIsRanged)
        {
            return p.ammoDelay;
        }
        return 0;
    }
    if (p.isMob && p.hasMobMain)
    {
        return p.mobMainDelay;
    }
    return 0;
}

} // namespace basedelayhelpers
