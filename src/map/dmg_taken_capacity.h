#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>

// Pure Magic/Physical/Ranged DmgTaken resist products and absorb/null outcomes.
// Parity: internal/dmgtaken + internal/absorbnull

namespace dmgtakenhelpers
{

enum class AbsorbNullOutcome : std::uint8_t
{
    Pass = 0,
    Absorb,
    Null,
};

// Magic resist product (after Liement short-circuit).
inline auto MagicResist(const std::int32_t damage,
                        const std::int16_t udmgMagic,
                        const std::int16_t dmgMagic,
                        const std::int16_t dmg,
                        const std::int16_t dmgMagicII) -> std::int32_t
{
    auto d = damage;
    auto resist = 1.0f + static_cast<float>(udmgMagic) / 10000.0f;
    resist      = std::max(resist, 0.0f);
    d           = static_cast<std::int32_t>(static_cast<float>(d) * resist);

    resist = 1.0f + static_cast<float>(dmgMagic) / 10000.0f + static_cast<float>(dmg) / 10000.0f;
    resist = std::max(resist, 0.5f);
    resist += static_cast<float>(dmgMagicII) / 10000.0f;
    resist = std::max(resist, 0.125f);
    return static_cast<std::int32_t>(static_cast<float>(d) * resist);
}

inline auto PhysicalResist(const std::int32_t damage,
                           const std::int16_t udmgPhys,
                           const std::int16_t dmgPhys,
                           const std::int16_t dmg,
                           const std::int16_t dmgPhysII,
                           const std::int16_t autoEqualizer,
                           const std::int32_t maxHP) -> std::int32_t
{
    auto d = damage;
    auto resist = 1.0f + static_cast<float>(udmgPhys) / 10000.0f;
    resist      = std::max(resist, 0.0f);
    d           = static_cast<std::int32_t>(static_cast<float>(d) * resist);

    resist = 1.0f + static_cast<float>(dmgPhys) / 10000.0f + static_cast<float>(dmg) / 10000.0f;
    resist = std::max(resist, 0.5f);
    resist += static_cast<float>(dmgPhysII) / 10000.0f;
    d = static_cast<std::int32_t>(static_cast<float>(d) * resist);

    if (d > 0 && autoEqualizer > 0)
    {
        auto reductionRate = std::floor(static_cast<float>(d) / static_cast<float>(maxHP) * static_cast<float>(autoEqualizer)) / 100.0f;
        reductionRate      = std::min(reductionRate, 0.90f);
        d                  = static_cast<std::int32_t>(std::floor(static_cast<float>(d) * (1.0f - reductionRate)));
    }
    return d;
}

inline auto RangedResist(const std::int32_t damage,
                         const std::int16_t udmgRange,
                         const std::int16_t dmgRange,
                         const std::int16_t dmg,
                         const std::int16_t autoEqualizer,
                         const std::int32_t maxHP) -> std::int32_t
{
    // Ranged: no type-II term.
    return PhysicalResist(damage, udmgRange, dmgRange, dmg, 0, autoEqualizer, maxHP);
}

constexpr auto MagicalOutcome(const bool absorbAll,
                              const bool absorbMagic,
                              const bool absorbElement,
                              const bool hasElement,
                              const bool nullAll,
                              const bool nullMagic,
                              const bool nullElement) -> AbsorbNullOutcome
{
    if (absorbAll || absorbMagic || (hasElement && absorbElement))
    {
        return AbsorbNullOutcome::Absorb;
    }
    if (nullAll || nullMagic || (hasElement && nullElement))
    {
        return AbsorbNullOutcome::Null;
    }
    return AbsorbNullOutcome::Pass;
}

constexpr auto PhysicalOutcome(const bool absorbAll, const bool absorbPhys, const bool nullAll, const bool nullPhys) -> AbsorbNullOutcome
{
    if (absorbAll || absorbPhys)
    {
        return AbsorbNullOutcome::Absorb;
    }
    if (nullAll || nullPhys)
    {
        return AbsorbNullOutcome::Null;
    }
    return AbsorbNullOutcome::Pass;
}

constexpr auto RangedOutcome(const bool absorbAll, const bool absorbPhys, const bool nullAll, const bool nullRanged) -> AbsorbNullOutcome
{
    if (absorbAll || absorbPhys)
    {
        return AbsorbNullOutcome::Absorb;
    }
    if (nullAll || nullRanged)
    {
        return AbsorbNullOutcome::Null;
    }
    return AbsorbNullOutcome::Pass;
}

constexpr auto ApplyAbsorbNull(const std::int32_t damage, const AbsorbNullOutcome o) -> std::int32_t
{
    switch (o)
    {
        case AbsorbNullOutcome::Absorb:
            return -damage;
        case AbsorbNullOutcome::Null:
            return 0;
        default:
            return damage;
    }
}

// Liement short-circuit: when factor < 0, damage * liement.
constexpr auto ShouldLiementShortCircuit(const float liement) -> bool
{
    return liement < 0.0f;
}

inline auto ApplyLiement(const std::int32_t damage, const float liement) -> std::int32_t
{
    return static_cast<std::int32_t>(static_cast<float>(damage) * liement);
}

} // namespace dmgtakenhelpers
