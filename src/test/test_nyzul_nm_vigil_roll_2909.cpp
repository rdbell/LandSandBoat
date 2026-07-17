#include "test_nyzul_nm_vigil_roll_2909.h"

#include "map/nyzul_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "nyzul nm vigil roll 2909 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua vigilWeaponDrop non-100 NM roll gate for dual-wire checks:
// math.random(1, 100) <= 20 and ENABLE_VIGIL_DROPS
//   → enableVigilDrops && roll >= 1 && roll <= 20
auto inlineShouldRollNMVigilWeapon(const int32 roll1to100, const bool enableVigilDrops) -> bool
{
    return enableVigilDrops && roll1to100 >= 1 && roll1to100 <= 20;
}

} // namespace

// Pure dual-wire expansion for nyzulhelpers::ShouldRollNMVigilWeapon
// (Lua vigilWeaponDrop non-floor-100 NM 20% roll).
auto runNyzulNMVigilRoll2909SelfTests() -> bool
{
    using nyzulhelpers::ShouldRollNMVigilWeapon;
    using nyzulhelpers::VigilNMDropChancePercent;

    bool ok = true;

    ok = expect(VigilNMDropChancePercent == 20, "VigilNMDropChancePercent == 20") && ok;

    const struct
    {
        int32       roll;
        bool        enable;
        bool        want;
        const char* label;
    } cases[] = {
        { 1, true, true, "min success roll" },
        { 20, true, true, "threshold success roll" },
        { 10, true, true, "mid success roll" },
        { 21, true, false, "just above threshold" },
        { 100, true, false, "max miss roll" },
        { 50, true, false, "mid miss roll" },
        { 0, true, false, "roll 0 out of range" },
        { -1, true, false, "negative roll" },
        { 2, true, true, "success roll 2" },
        { 19, true, true, "success roll 19" },
        { 101, true, false, "above max range" },
        { 1, false, false, "min roll vigil disabled" },
        { 20, false, false, "threshold roll vigil disabled" },
        { 21, false, false, "miss roll vigil disabled" },
        { 0, false, false, "out of range and disabled" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRollNMVigilWeapon(c.roll, c.enable);
        const bool inlineF = inlineShouldRollNMVigilWeapon(c.roll, c.enable);
        const bool pure    = c.enable && c.roll >= 1 && c.roll <= VigilNMDropChancePercent;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == pure, "dual-wire free == enable&&roll in 1..percent") && ok;
        ok = expect(got == inlineF, "dual-wire free == inline Lua formula") && ok;
    }

    // Residual 1088 pins.
    ok = expect(ShouldRollNMVigilWeapon(20, true), "residual threshold success") && ok;
    ok = expect(ShouldRollNMVigilWeapon(1, true), "residual min success") && ok;
    ok = expect(!ShouldRollNMVigilWeapon(21, true), "residual above-threshold miss") && ok;
    ok = expect(!ShouldRollNMVigilWeapon(20, false), "residual vigil disabled") && ok;

    // Dense compose range identity: rolls -2..101 × enable true/false.
    for (const bool enable : { true, false })
    {
        for (int32 roll = -2; roll <= 101; ++roll)
        {
            const bool got  = ShouldRollNMVigilWeapon(roll, enable);
            const bool want = enable && roll >= 1 && roll <= VigilNMDropChancePercent;
            ok = expect(got == want, "compose range free == formula") && ok;
            ok = expect(got == inlineShouldRollNMVigilWeapon(roll, enable),
                        "compose range free == inline") &&
                 ok;
        }
    }

    return ok;
}
