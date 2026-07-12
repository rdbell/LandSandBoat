#include "test_wildcard_randomdeal_1530.h"

#include "map/wildcard_randomdeal_capacity.h"

#include <iostream>
#include <vector>

namespace
{
using wildcardrandomdealhelpers::BuildRandomDealLists;
using wildcardrandomdealhelpers::LoadedDeckChance;
using wildcardrandomdealhelpers::RandomDealRecast;
using wildcardrandomdealhelpers::RandomDealTwoChance;
using wildcardrandomdealhelpers::ResolveRandomDeal;
using wildcardrandomdealhelpers::ResolveWildCard;
using wildcardrandomdealhelpers::ShouldEvadeRandomDeal;

auto Check() -> bool
{
    {
        const auto e = ResolveWildCard(3, true, 100, 50);
        if (!e.setTP || e.tpValue != 1000)
        {
            return false;
        }
    }
    {
        const auto e = ResolveWildCard(4, false, 100, 50);
        if (!e.setTP || e.tpValue != 3000)
        {
            return false;
        }
    }
    {
        const auto e = ResolveWildCard(5, true, 100, 40);
        if (e.delSpecial || !e.setMPHalfFloor)
        {
            return false;
        }
    }
    {
        const auto e = ResolveWildCard(5, false, 100, 40);
        if (!e.delSpecial || !e.setMPHalfFloor)
        {
            return false;
        }
    }
    {
        const auto e = ResolveWildCard(5, false, 100, 50);
        if (e.setMPHalfFloor)
        {
            return false;
        }
    }
    {
        const auto e = ResolveWildCard(6, true, 200, 0);
        if (!e.delSpecial2 || e.delSpecial || !e.addFullMP)
        {
            return false;
        }
    }
    {
        const auto e = ResolveWildCard(6, false, 200, 0);
        if (!e.delSpecial2 || !e.delSpecial || !e.addFullMP)
        {
            return false;
        }
    }

    {
        std::vector<RandomDealRecast> recasts{
            { 0, true },
            { 10, true },
            { 11, false },
            { 254, true },
            { 196, true },
            { 12, true },
        };
        std::vector<int> cands;
        std::vector<int> active;
        BuildRandomDealLists(recasts, cands, active);
        if (cands.size() != 3 || cands[0] != 1 || cands[1] != 2 || cands[2] != 5)
        {
            return false;
        }
        if (active.size() != 2 || active[0] != 1 || active[1] != 5)
        {
            return false;
        }
    }

    if (!ShouldEvadeRandomDeal(0, 1) || !ShouldEvadeRandomDeal(1, 0) || ShouldEvadeRandomDeal(1, 1))
    {
        return false;
    }
    if (RandomDealTwoChance(60) != 50 || LoadedDeckChance(10) != 60)
    {
        return false;
    }

    if (ResolveRandomDeal(0, 0, 5, 10, 0, 0, true).success)
    {
        return false;
    }
    if (ResolveRandomDeal(2, 1, 5, 10, 99, 0, true).success)
    {
        return false;
    }
    {
        const auto r = ResolveRandomDeal(3, 1, 5, 10, 54, 0, true);
        if (!r.success || !r.useLoadedDeckPath || r.shuffleActive || r.deleteIndices.size() != 1)
        {
            return false;
        }
    }
    {
        const auto r = ResolveRandomDeal(3, 2, 5, 50, 0, 50, false);
        if (!r.success || !r.shuffleActive || r.deleteIndices.size() != 2 || !r.notifyOtherTarget)
        {
            return false;
        }
    }
    {
        const auto r = ResolveRandomDeal(1, 1, 0, 0, 0, 0, true);
        if (!r.success || r.useLoadedDeckPath || r.shuffleCandidates || r.deleteIndices.size() != 1)
        {
            return false;
        }
    }
    {
        const auto r = ResolveRandomDeal(3, 2, 0, 50, 0, 50, false);
        if (!r.success || !r.shuffleCandidates || r.deleteIndices.size() != 2)
        {
            return false;
        }
    }
    {
        const auto r = ResolveRandomDeal(3, 2, 0, 50, 0, 51, true);
        if (r.deleteIndices.size() != 1)
        {
            return false;
        }
    }
    return true;
}
} // namespace

auto runWildCardRandomDeal1530SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "wildcard_randomdeal_1530 self-tests failed\n";
        return false;
    }
    return true;
}
