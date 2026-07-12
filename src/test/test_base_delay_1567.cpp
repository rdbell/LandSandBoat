#include "test_base_delay_1567.h"

#include "map/base_delay_capacity.h"

#include <iostream>

namespace
{
using basedelayhelpers::GetBaseDelay;
using basedelayhelpers::GetBaseRangedDelay;
using basedelayhelpers::MeleeBaseDelayParams;
using basedelayhelpers::RangedBaseDelayParams;
using basedelayhelpers::UnequippedH2HBaseDelay;

auto Check() -> bool
{
    if (UnequippedH2HBaseDelay != 480)
    {
        return false;
    }

    // Melee unequipped / H2H / DW
    {
        MeleeBaseDelayParams p{};
        p.isPC = true;
        if (GetBaseDelay(p) != 480)
        {
            return false;
        }
        p.hasMainWeapon = true;
        p.mainDelay     = 480;
        p.isH2H         = true;
        p.hasSubWeapon  = true;
        p.subDelay      = 240;
        if (GetBaseDelay(p) != 480)
        {
            return false;
        }
        p.isH2H = false;
        if (GetBaseDelay(p) != 720)
        {
            return false;
        }
    }
    {
        MeleeBaseDelayParams p{};
        p.isMob         = true;
        p.hasMainWeapon = true;
        p.mainDelay     = 320;
        if (GetBaseDelay(p) != 320)
        {
            return false;
        }
    }
    {
        MeleeBaseDelayParams p{};
        p.hasMainWeapon = true;
        p.mainDelay     = 200;
        if (GetBaseDelay(p) != 480)
        {
            return false;
        }
    }

    // Ranged
    {
        RangedBaseDelayParams p{};
        p.isPC = true;
        if (GetBaseRangedDelay(p) != 0)
        {
            return false;
        }
        p.hasRanged        = true;
        p.rangedDelay      = 286;
        p.rangedIsRanged   = true;
        p.rangedIsThrowing = true;
        p.hasAmmo          = true;
        p.ammoDelay        = 100;
        if (GetBaseRangedDelay(p) != 286)
        {
            return false;
        }
        p.rangedIsThrowing = false;
        p.rangedDelay      = 360;
        p.ammoDelay        = 120;
        if (GetBaseRangedDelay(p) != 480)
        {
            return false;
        }
        p.hasAmmo = false;
        if (GetBaseRangedDelay(p) != 0)
        {
            return false;
        }
    }
    {
        RangedBaseDelayParams p{};
        p.isPC         = true;
        p.hasAmmo      = true;
        p.ammoDelay    = 192;
        p.ammoIsRanged = true;
        if (GetBaseRangedDelay(p) != 192)
        {
            return false;
        }
    }
    {
        RangedBaseDelayParams p{};
        p.isMob        = true;
        p.hasMobMain   = true;
        p.mobMainDelay = 400;
        if (GetBaseRangedDelay(p) != 400)
        {
            return false;
        }
    }

    return true;
}
} // namespace

auto runBaseDelay1567SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "base_delay_1567 self-tests failed\n";
        return false;
    }
    return true;
}
