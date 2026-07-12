#pragma once

#include <algorithm>
#include <cstdint>

// Pure battleutils::CheckMultiHits halves after weapon hit count inject.
// Parity: internal/attackutils multi_hits.go

namespace multihitshelpers
{

constexpr std::uint8_t MaxMultiHits = 8;

constexpr auto ShouldRejectNullWeapon(const bool weaponNull) -> bool
{
    return weaponNull;
}

constexpr auto ClampMultiHitRate(const std::int16_t rate) -> std::int16_t
{
    if (rate < 0)
    {
        return 0;
    }
    if (rate > 100)
    {
        return 100;
    }
    return rate;
}

constexpr auto StackMultiHitRates(const std::int16_t doubleMod,
                                  const std::int16_t tripleMod,
                                  const std::int16_t quadMod,
                                  const std::int16_t doubleMerit,
                                  const std::int16_t tripleMerit,
                                  const bool         hasDoubleTrait,
                                  const bool         hasTripleTrait,
                                  std::int16_t&      doubleRate,
                                  std::int16_t&      tripleRate,
                                  std::int16_t&      quadRate) -> void
{
    doubleRate = doubleMod;
    tripleRate = tripleMod;
    quadRate   = quadMod;
    if (hasTripleTrait)
    {
        tripleRate = static_cast<std::int16_t>(tripleRate + tripleMerit);
    }
    if (hasDoubleTrait)
    {
        doubleRate = static_cast<std::int16_t>(doubleRate + doubleMerit);
    }
    doubleRate = ClampMultiHitRate(doubleRate);
    tripleRate = ClampMultiHitRate(tripleRate);
    quadRate   = ClampMultiHitRate(quadRate);
}

// Exclusive DA/TA/QA expansion; rolls are 0..99 injects (may be unused if earlier arm procs).
constexpr auto ExpandMultiHits(const std::uint8_t baseHits,
                               std::int16_t       doubleRate,
                               std::int16_t       tripleRate,
                               std::int16_t       quadRate,
                               const std::uint8_t quadRoll,
                               const std::uint8_t tripleRoll,
                               const std::uint8_t doubleRoll) -> std::uint8_t
{
    doubleRate = ClampMultiHitRate(doubleRate);
    tripleRate = ClampMultiHitRate(tripleRate);
    quadRate   = ClampMultiHitRate(quadRate);

    auto num = baseHits;
    if (static_cast<std::int16_t>(quadRoll) < quadRate)
    {
        num = static_cast<std::uint8_t>(num + 3);
    }
    else if (static_cast<std::int16_t>(tripleRoll) < tripleRate)
    {
        num = static_cast<std::uint8_t>(num + 2);
    }
    else if (static_cast<std::int16_t>(doubleRoll) < doubleRate)
    {
        num = static_cast<std::uint8_t>(num + 1);
    }
    return num > MaxMultiHits ? MaxMultiHits : num;
}

constexpr auto HassoZanshinChance(const std::int16_t zanshinMod, const std::int16_t zanshinMerit, const bool isPC) -> std::int16_t
{
    auto z = zanshinMod;
    if (isPC)
    {
        z = static_cast<std::int16_t>(z + zanshinMerit);
    }
    return static_cast<std::int16_t>(z / 4);
}

constexpr auto ApplyHassoZanshinHit(std::uint8_t       num,
                                    const std::int16_t hassoZanshinBonus,
                                    const bool         hasHasso,
                                    const std::int16_t chance,
                                    const std::uint8_t roll) -> std::uint8_t
{
    if (hassoZanshinBonus > 0 && hasHasso && static_cast<std::int16_t>(roll) < chance)
    {
        num = static_cast<std::uint8_t>(num + 1);
    }
    return num > MaxMultiHits ? MaxMultiHits : num;
}

constexpr auto CapMultiHits(const std::uint8_t num) -> std::uint8_t
{
    return num > MaxMultiHits ? MaxMultiHits : num;
}

} // namespace multihitshelpers
