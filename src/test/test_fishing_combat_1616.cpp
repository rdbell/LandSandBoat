#include "test_fishing_combat_1616.h"

#include "map/fishing_combat_capacity.h"

#include <iostream>

namespace
{
using namespace fishingcombathelpers;

auto Check() -> bool
{
    // Stamina: skill 50 count 1 roll 100 → 100*((50+36)/2)=4300
    if (CalculateStamina(50, 1, 100) != 4300)
    {
        return false;
    }
    // skill 100 count 2 roll 100 → floor(1.1*100)=110 → 100*((110+36)/2)=7300
    if (CalculateStamina(100, 2, 100) != 7300)
    {
        return false;
    }
    // negative skill wrap: skill -14 count 1 roll 100 → 100*((-14+36)/2)=1100
    if (CalculateStamina(-14, 1, 100) != 1100)
    {
        return false;
    }

    // Attack: difficulty 10, fishAttack 100, non-legendary → floor(10*1.0*20)=200
    if (CalculateAttack(false, 10, 100, 50) != 200)
    {
        return false;
    }
    // legendary adds lgdBonusAtk: floor(10 * 1.5 * 20)=300
    if (CalculateAttack(true, 10, 100, 50) != 300)
    {
        return false;
    }

    // Heal: attack 200, recovery 100 → floor((200/20)*(100/100))=10 → *10 = 100
    if (CalculateHeal(false, 10, 100, 0, 100) != 100)
    {
        return false;
    }

    // Gear: apron +3, anglers gloves +1
    if (LuckyTimingGearBonus(GearFishermansApron, GearAnglersGloves, 0, 0) != 4.0f)
    {
        return false;
    }

    // Lucky timing floor: skill equal, moon 1 roll 1 hour 12 → base 10 + skill0 + moon(5+1) + hour3 = 19, min 5
    // fishingSkill 50 catchSkill 50: fishing+10>catch → floor((60-50)/20)=0
    if (CalculateLuckyTiming(50, 50, 0, 0, 0, false, false, 0, 0.0f, 1, 12, 1) != 19)
    {
        return false;
    }
    // floor at 5
    if (CalculateLuckyTiming(1, 50, 0, 0, 0, false, false, 0, 0.0f, 0, 0, 0) < 5)
    {
        return false;
    }
    return true;
}
} // namespace

auto runFishingCombat1616SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "fishing_combat_1616 self-tests failed\n";
        return false;
    }
    return true;
}
