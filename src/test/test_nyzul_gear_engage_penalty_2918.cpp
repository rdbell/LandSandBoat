#include "test_nyzul_gear_engage_penalty_2918.h"

#include "map/nyzul_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "nyzul gear engage penalty 2918 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua onGearEngage penalty gate for dual-wire checks:
//   gearObjective == AVOID_AGRO and CE == 0 and VE == 0 and initialAgro == 0
auto inlineShouldApplyGearEngagePenalty(const int32 gearObjective, const int32 ce, const int32 ve, const int32 initialAgro) -> bool
{
    return gearObjective == nyzulhelpers::GearObjectiveAvoidAgro &&
           ce == 0 &&
           ve == 0 &&
           initialAgro == 0;
}

} // namespace

// Pure dual-wire expansion for nyzulhelpers::ShouldApplyGearEngagePenalty
// (Lua onGearEngage AVOID_AGRO / CE / VE / initialAgro gate; slice 2918).
auto runNyzulGearEngagePenalty2918SelfTests() -> bool
{
    using nyzulhelpers::GearObjectiveAvoidAgro;
    using nyzulhelpers::ShouldApplyGearEngagePenalty;

    bool ok = true;

    // Gear objective catalog pin (match Go GearObjectiveAvoidAgro).
    ok = expect(GearObjectiveAvoidAgro == 1, "GearObjectiveAvoidAgro pin") && ok;

    // Residual 1088 ShouldApplyGearEngagePenalty pins.
    ok = expect(ShouldApplyGearEngagePenalty(GearObjectiveAvoidAgro, 0, 0, 0), "engage clean agro") && ok;
    ok = expect(!ShouldApplyGearEngagePenalty(GearObjectiveAvoidAgro, 1, 0, 0), "engage CE>0") && ok;
    ok = expect(!ShouldApplyGearEngagePenalty(GearObjectiveAvoidAgro, 0, 1, 0), "engage VE>0") && ok;
    ok = expect(!ShouldApplyGearEngagePenalty(GearObjectiveAvoidAgro, 0, 0, 1), "engage already agro") && ok;
    ok = expect(!ShouldApplyGearEngagePenalty(2, 0, 0, 0), "engage wrong objective DO_NOT_DESTROY") && ok;

    // --- ShouldApplyGearEngagePenalty table ---
    const struct
    {
        int32       obj;
        int32       ce;
        int32       ve;
        int32       agro;
        bool        want;
        const char* label;
    } cases[] = {
        { GearObjectiveAvoidAgro, 0, 0, 0, true, "AVOID_AGRO clean first engage" },
        { 1, 0, 0, 0, true, "literal AVOID_AGRO 1 clean" },
        { GearObjectiveAvoidAgro, 1, 0, 0, false, "CE>0 blocks" },
        { GearObjectiveAvoidAgro, 0, 1, 0, false, "VE>0 blocks" },
        { GearObjectiveAvoidAgro, 0, 0, 1, false, "initialAgro already set" },
        { GearObjectiveAvoidAgro, 1, 1, 0, false, "CE and VE both set" },
        { GearObjectiveAvoidAgro, 0, 0, 2, false, "initialAgro non-zero" },
        { 2, 0, 0, 0, false, "DO_NOT_DESTROY wrong objective" },
        { 0, 0, 0, 0, false, "gearObjective 0 (unset)" },
        { GearObjectiveAvoidAgro, -1, 0, 0, false, "negative CE blocks" },
        { GearObjectiveAvoidAgro, 0, -1, 0, false, "negative VE blocks" },
        { GearObjectiveAvoidAgro, 0, 0, -1, false, "negative initialAgro blocks" },
        { GearObjectiveAvoidAgro, 5, 0, 0, false, "large CE blocks" },
        { GearObjectiveAvoidAgro, 0, 99, 0, false, "large VE blocks" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldApplyGearEngagePenalty(c.obj, c.ce, c.ve, c.agro);
        const bool inlineC = inlineShouldApplyGearEngagePenalty(c.obj, c.ce, c.ve, c.agro);
        const bool compose = c.obj == GearObjectiveAvoidAgro &&
                             c.ce == 0 && c.ve == 0 && c.agro == 0;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineC, "dual-wire free == inline Lua formula") && ok;
        ok = expect(got == compose, "dual-wire free == AVOID_AGRO compose") && ok;
    }

    // Host compose: only AVOID_AGRO + zero CE/VE/initialAgro applies penalty.
    const struct
    {
        int32       obj;
        int32       ce;
        int32       ve;
        int32       agro;
        bool        want;
        const char* label;
    } composeCases[] = {
        { GearObjectiveAvoidAgro, 0, 0, 0, true, "compose clean engage applies" },
        { GearObjectiveAvoidAgro, 1, 0, 0, false, "compose CE>0 blocks" },
        { GearObjectiveAvoidAgro, 0, 1, 0, false, "compose VE>0 blocks" },
        { GearObjectiveAvoidAgro, 0, 0, 1, false, "compose initialAgro blocks" },
        { 2, 0, 0, 0, false, "compose DO_NOT_DESTROY blocks" },
        { 0, 0, 0, 0, false, "compose unset objective blocks" },
        { 1, 0, 0, 0, true, "compose literal 1 applies" },
        { GearObjectiveAvoidAgro, 10, 20, 0, false, "compose high CE/VE blocks" },
        { GearObjectiveAvoidAgro, 0, 0, 99, false, "compose high initialAgro blocks" },
    };

    for (const auto& c : composeCases)
    {
        const bool got     = ShouldApplyGearEngagePenalty(c.obj, c.ce, c.ve, c.agro);
        const bool inlineC = inlineShouldApplyGearEngagePenalty(c.obj, c.ce, c.ve, c.agro);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineC, "compose dual-wire free == inline") && ok;
    }

    return ok;
}
