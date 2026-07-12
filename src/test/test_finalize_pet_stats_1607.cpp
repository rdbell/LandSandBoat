#include "test_finalize_pet_stats_1607.h"

#include "map/finalize_pet_stats_capacity.h"

#include <iostream>

namespace
{
using namespace finalizepetstatshelpers;

auto Check() -> bool
{
    if (MEVASkillRank != 7)
    {
        return false;
    }
    if (MEVALevel(50) != 50 || MEVALevel(99) != 99 || MEVALevel(119) != 99)
    {
        return false;
    }
    if (ComposeMEVA(100, 20) != 120 || ComposeMEVA(0, -10) != -10)
    {
        return false;
    }
    if (StoutServantDamage(5) != -500 || StoutServantDamage(0) != 0 || StoutServantDamage(15) != -1500)
    {
        return false;
    }
    return true;
}
} // namespace

auto runFinalizePetStats1607SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "finalize_pet_stats_1607 self-tests failed\n";
        return false;
    }
    return true;
}
