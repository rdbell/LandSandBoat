#include "test_ranged_weapon_delay_1633.h"

#include "map/ranged_weapon_delay_capacity.h"

#include <iostream>

auto runRangedWeaponDelay1633SelfTests() -> bool
{
    using namespace rangedweapondelayhelpers;

    bool ok = true;

    // No weapons
    ok = ok && ResolveRangedBaseDelay(false, false, 0, false, false, 0, false) == 0;
    // Range present but zero damage falls through to ammo-only path
    ok = ok && ResolveRangedBaseDelay(true, false, 100, true, true, 20, true) == 20;
    ok = ok && ResolveRangedBaseDelay(true, false, 100, false, false, 0, true) == 0;

    // Range only, no TP ammo add
    ok = ok && ResolveRangedBaseDelay(true, true, 240, true, true, 30, false) == 240;
    // Range + ammo for TP
    ok = ok && ResolveRangedBaseDelay(true, true, 240, true, true, 30, true) == 270;
    // Ammo-only throw
    ok = ok && ResolveRangedBaseDelay(false, false, 0, true, true, 120, false) == 120;
    ok = ok && ResolveRangedBaseDelay(false, false, 0, true, true, 120, true) == 120;

    // Flat mod: (240 - 0) * 1000 / 120 = 2000
    ok = ok && ApplyRangedDelayFlatMod(240, 0) == 2000;
    // (240 - 12) * 1000 / 120 = 1900
    ok = ok && ApplyRangedDelayFlatMod(240, 12) == 1900;

    // DELAYP: 2000 * 1.0 = 2000; 2000 * 0.9 = 1800
    ok = ok && ApplyRangedDelayP(2000, 0) == 2000;
    ok = ok && ApplyRangedDelayP(2000, -10) == 1800;

    // Full forTPCalc: base 240+30=270 → (270-0)*1000/120 = 2250, no DELAYP
    ok = ok && ResolveRangedWeaponDelay(true, true, 240, true, true, 30, 0, -10, true) == 2250;
    // Full animation: base 240 → 2000 * 0.9 = 1800
    ok = ok && ResolveRangedWeaponDelay(true, true, 240, true, true, 30, 0, -10, false) == 1800;

    ok = ok && ResolveAmmoDelay(true, true, 480) == 240;
    ok = ok && ResolveAmmoDelay(true, false, 480) == 0;
    ok = ok && ResolveAmmoDelay(false, true, 480) == 0;

    if (!ok)
    {
        std::cerr << "ranged weapon delay 1633 self-test failed\n";
    }
    return ok;
}
