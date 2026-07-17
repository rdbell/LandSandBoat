#include "test_darkixion_break_horn_2885.h"

#include "map/darkixion_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "darkixion CanBreakHorn 2885 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline checkHornBreak pure gate (before 5% roll) for dual-wire cross-check:
//   !busy && (animSub == NORMAL || animSub == GLOWING) && attackerInFront
auto inlineCanBreakHorn(const bool busy, const int32 animSub, const bool attackerInFront) -> bool
{
    return !busy && (animSub == darkixionhelpers::kAnimNormal || animSub == darkixionhelpers::kAnimGlowing) && attackerInFront;
}

} // namespace

// Pure dual-wire expansion for darkixionhelpers::CanBreakHorn
// (Lua checkHornBreak gate before 5% roll; slice 2885).
auto runDarkixionBreakHorn2885SelfTests() -> bool
{
    using darkixionhelpers::CanBreakHorn;
    using darkixionhelpers::kAnimGlowing;
    using darkixionhelpers::kAnimHornBroken;
    using darkixionhelpers::kAnimNormal;
    using darkixionhelpers::kAnimTrample;

    bool ok = true;

    // Animation-sub catalog pins (match Go AnimNormal / AnimGlowing / …).
    ok = expect(kAnimNormal == 0, "AnimNormal pin") && ok;
    ok = expect(kAnimTrample == 1, "AnimTrample pin") && ok;
    ok = expect(kAnimHornBroken == 2, "AnimHornBroken pin") && ok;
    ok = expect(kAnimGlowing == 3, "AnimGlowing pin") && ok;

    // Primary residual 0985 pins for CanBreakHorn.
    ok = expect(CanBreakHorn(false, kAnimNormal, true), "normal+front should pass") && ok;
    ok = expect(CanBreakHorn(false, kAnimGlowing, true), "glowing+front should pass") && ok;
    ok = expect(!CanBreakHorn(true, kAnimNormal, true), "busy should fail") && ok;
    ok = expect(!CanBreakHorn(false, kAnimHornBroken, true), "broken should fail") && ok;
    ok = expect(!CanBreakHorn(false, kAnimTrample, true), "trample should fail") && ok;
    ok = expect(!CanBreakHorn(false, kAnimNormal, false), "not in front should fail") && ok;

    // Dual-wire matches inline formula across a table.
    const struct
    {
        bool        busy;
        int32       animSub;
        bool        attackerInFront;
        bool        want;
        const char* label;
    } cases[] = {
        { false, kAnimNormal, true, true, "table normal+front" },
        { false, kAnimGlowing, true, true, "table glowing+front" },
        { true, kAnimNormal, true, false, "table busy normal" },
        { true, kAnimGlowing, true, false, "table busy glowing" },
        { false, kAnimHornBroken, true, false, "table horn broken" },
        { false, kAnimTrample, true, false, "table trample" },
        { false, kAnimNormal, false, false, "table normal not front" },
        { false, kAnimGlowing, false, false, "table glowing not front" },
        { true, kAnimHornBroken, false, false, "table all fail" },
        { false, 99, true, false, "table unknown anim" },
        { false, -1, true, false, "table negative anim" },
        { false, kAnimNormal, true, true, "table residual normal pin" },
        { false, kAnimGlowing, true, true, "table residual glowing pin" },
    };

    for (const auto& c : cases)
    {
        const bool got       = CanBreakHorn(c.busy, c.animSub, c.attackerInFront);
        const bool inlineGot = inlineCanBreakHorn(c.busy, c.animSub, c.attackerInFront);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineGot, "dual-wire free == inline Lua formula") && ok;
    }

    // Host compose: busy / anim / front flags are independent injects; only
    // the pure conjunction of all three succeeds (5% roll still host-owned).
    const struct
    {
        bool        busy;
        int32       animSub;
        bool        attackerInFront;
        bool        want;
        const char* label;
    } composeCases[] = {
        { false, kAnimNormal, true, true, "compose eligible normal" },
        { false, kAnimGlowing, true, true, "compose eligible glowing" },
        { true, kAnimNormal, true, false, "compose busy blocks" },
        { false, kAnimTrample, true, false, "compose wrong anim blocks" },
        { false, kAnimNormal, false, false, "compose not-front blocks" },
    };

    for (const auto& c : composeCases)
    {
        const bool got       = CanBreakHorn(c.busy, c.animSub, c.attackerInFront);
        const bool inlineGot = inlineCanBreakHorn(c.busy, c.animSub, c.attackerInFront);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineGot, "compose dual-wire free == inline") && ok;
    }

    return ok;
}
