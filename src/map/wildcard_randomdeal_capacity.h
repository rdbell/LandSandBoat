#pragma once

#include <algorithm>
#include <cstdint>
#include <optional>
#include <vector>

// Pure DoWildCardToEntity / DoRandomDealToEntity decision helpers.

namespace wildcardrandomdealhelpers
{

constexpr std::uint16_t RecastSpecial    = 0;
constexpr std::uint16_t RecastSpecial2   = 254;
constexpr std::uint16_t RecastRandomDeal = 196;

constexpr std::uint16_t WildCardTPRoll3 = 1000;
constexpr std::uint16_t WildCardTPRoll4 = 3000;

constexpr std::uint8_t RandomDealLoadedDeckBase = 50;
constexpr std::int16_t RandomDealTwoCap         = 50;

struct WildCardEffect
{
    bool                 setTP{ false };
    std::uint16_t        tpValue{ 0 };
    bool                 delSpecial{ false };
    bool                 delSpecial2{ false };
    bool                 setMPHalfFloor{ false };
    bool                 addFullMP{ false };
};

constexpr auto ResolveWildCard(const std::uint8_t roll,
                               const bool         isCORMain,
                               const std::int32_t maxMP,
                               const std::int32_t currentMP) -> WildCardEffect
{
    WildCardEffect e{};
    switch (roll)
    {
        case 3:
            e.setTP   = true;
            e.tpValue = WildCardTPRoll3;
            break;
        case 4:
            e.setTP   = true;
            e.tpValue = WildCardTPRoll4;
            break;
        case 5:
            if (!isCORMain)
            {
                e.delSpecial = true;
            }
            if (maxMP > 0 && currentMP < maxMP / 2)
            {
                e.setMPHalfFloor = true;
            }
            break;
        case 6:
            e.delSpecial2 = true;
            if (!isCORMain)
            {
                e.delSpecial = true;
            }
            e.addFullMP = true;
            break;
        default:
            break;
    }
    return e;
}

struct RandomDealRecast
{
    std::uint16_t id{ 0 };
    bool          active{ false };
};

inline auto BuildRandomDealLists(const std::vector<RandomDealRecast>& recasts,
                                 std::vector<int>&                    candidates,
                                 std::vector<int>&                    active) -> void
{
    candidates.clear();
    active.clear();
    for (std::size_t i = 0; i < recasts.size(); ++i)
    {
        const auto& r = recasts[i];
        if (r.id == RecastSpecial || r.id == RecastSpecial2 || r.id == RecastRandomDeal)
        {
            continue;
        }
        candidates.push_back(static_cast<int>(i));
        if (r.active)
        {
            active.push_back(static_cast<int>(i));
        }
    }
}

constexpr auto ShouldEvadeRandomDeal(const int candidateCount, const int activeCount) -> bool
{
    return candidateCount == 0 || activeCount == 0;
}

constexpr auto RandomDealTwoChance(const std::int16_t randomDealBonus) -> std::uint8_t
{
    if (randomDealBonus < 0)
    {
        return 0;
    }
    if (randomDealBonus > RandomDealTwoCap)
    {
        return static_cast<std::uint8_t>(RandomDealTwoCap);
    }
    return static_cast<std::uint8_t>(randomDealBonus);
}

constexpr auto LoadedDeckChance(const std::uint8_t loadedDeckMerit) -> std::uint8_t
{
    return static_cast<std::uint8_t>(RandomDealLoadedDeckBase + loadedDeckMerit);
}

struct RandomDealResult
{
    bool             success{ false };
    bool             useLoadedDeckPath{ false };
    bool             shuffleActive{ false };
    bool             shuffleCandidates{ false };
    std::vector<int> deleteIndices{};
    bool             notifyOtherTarget{ false };
};

inline auto ResolveRandomDeal(const int          candidateCount,
                              const int          activeCount,
                              const std::uint8_t loadedDeckMerit,
                              const std::uint8_t resetTwoChance,
                              const int          rollA,
                              const int          rollB,
                              const bool         casterIsTarget) -> RandomDealResult
{
    if (ShouldEvadeRandomDeal(candidateCount, activeCount))
    {
        return {};
    }

    RandomDealResult res{};
    res.notifyOtherTarget = !casterIsTarget;

    if (loadedDeckMerit > 0)
    {
        res.useLoadedDeckPath = true;
        auto chance           = LoadedDeckChance(loadedDeckMerit);
        if (activeCount > 1)
        {
            res.shuffleActive = true;
            chance            = 100;
        }
        if (chance < static_cast<std::uint8_t>(rollA))
        {
            return {};
        }
        res.success = true;
        res.deleteIndices.push_back(0);
        if (activeCount > 1 && resetTwoChance >= static_cast<std::uint8_t>(rollB))
        {
            res.deleteIndices.push_back(1);
        }
        return res;
    }

    res.success = true;
    if (candidateCount > 1)
    {
        res.shuffleCandidates = true;
    }
    res.deleteIndices.push_back(0);
    if (candidateCount > 1 && activeCount > 1 && static_cast<int>(resetTwoChance) >= rollB)
    {
        res.deleteIndices.push_back(1);
    }
    return res;
}

} // namespace wildcardrandomdealhelpers
