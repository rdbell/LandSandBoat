#include "test_darkixion_can_break_horn_3154.h"

#include "map/darkixion_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "darkixion CanBreakHorn 3154 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline checkHornBreak pure gate (before 5% roll) for dual-wire cross-check
// (slice 3154; residual 2885):
//   !busy && (animSub == NORMAL || animSub == GLOWING) && attackerInFront
auto inlineCanBreakHorn(const bool busy, const int32 animSub, const bool attackerInFront) -> bool
{
    return !busy && (animSub == darkixionhelpers::kAnimNormal || animSub == darkixionhelpers::kAnimGlowing) &&
           attackerInFront;
}

// Positive if/else pin matching free function / capacity body (slice 3154).
// Avoid De Morgan rewrites of the compound-not conjunction (QF1001).
auto pinCanBreakHorn(const bool busy, const int32 animSub, const bool attackerInFront) -> bool
{
    if (!busy)
    {
        if (animSub == darkixionhelpers::kAnimNormal || animSub == darkixionhelpers::kAnimGlowing)
        {
            if (attackerInFront)
            {
                return true;
            }
        }
    }
    return false;
}

} // namespace

// Pure dual-wire expansion for darkixionhelpers::CanBreakHorn
// (Lua checkHornBreak gate before 5% roll; OmegaXI internal/darkixion;
// dedicated slice 3154).
//
// Coverage:
//   - not-busy + NORMAL/GLOWING + front → break true
//   - busy / wrong anim / not front → break false
//   - free == inline == pin (positive if/else)
//   - residual 0985 / 2885 pins still hold
//   - poles busy / anim / front
auto runDarkixionCanBreakHorn3154SelfTests() -> bool
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

    // Residual 0985 / 2885 CanBreakHorn pins still hold under dual-wire.
    ok = expect(CanBreakHorn(false, kAnimNormal, true), "residual normal+front should pass") && ok;
    ok = expect(CanBreakHorn(false, kAnimGlowing, true), "residual glowing+front should pass") && ok;
    ok = expect(!CanBreakHorn(true, kAnimNormal, true), "residual busy should fail") && ok;
    ok = expect(!CanBreakHorn(false, kAnimHornBroken, true), "residual broken should fail") && ok;
    ok = expect(!CanBreakHorn(false, kAnimTrample, true), "residual trample should fail") && ok;
    ok = expect(!CanBreakHorn(false, kAnimNormal, false), "residual not in front should fail") && ok;

    // --- Eligible break path ---
    ok = expect(CanBreakHorn(false, kAnimNormal, true), "eligible not-busy normal+front") && ok;
    ok = expect(CanBreakHorn(false, kAnimGlowing, true), "eligible not-busy glowing+front") && ok;
    ok = expect(CanBreakHorn(false, 0, true), "eligible literal NORMAL 0 + front") && ok;
    ok = expect(CanBreakHorn(false, 3, true), "eligible literal GLOWING 3 + front") && ok;

    // --- Blocked paths ---
    ok = expect(!CanBreakHorn(true, kAnimNormal, true), "blocked busy normal") && ok;
    ok = expect(!CanBreakHorn(true, kAnimGlowing, true), "blocked busy glowing") && ok;
    ok = expect(!CanBreakHorn(false, kAnimTrample, true), "blocked trample") && ok;
    ok = expect(!CanBreakHorn(false, kAnimHornBroken, true), "blocked broken") && ok;
    ok = expect(!CanBreakHorn(false, kAnimNormal, false), "blocked normal not front") && ok;
    ok = expect(!CanBreakHorn(false, kAnimGlowing, false), "blocked glowing not front") && ok;
    ok = expect(!CanBreakHorn(false, 99, true), "blocked unknown anim") && ok;
    ok = expect(!CanBreakHorn(false, -1, true), "blocked negative anim") && ok;
    ok = expect(!CanBreakHorn(true, kAnimHornBroken, false), "blocked all poles fail") && ok;

    // --- Composition table: free == inline == pin (positive if/else) ---
    // Poles busy / anim / front.
    const struct
    {
        bool        busy;
        int32       animSub;
        bool        attackerInFront;
        bool        want;
        const char* label;
    } cases[] = {
        // eligible
        { false, kAnimNormal, true, true, "table not-busy normal+front" },
        { false, kAnimGlowing, true, true, "table not-busy glowing+front" },
        { false, 0, true, true, "table literal NORMAL 0 + front" },
        { false, 3, true, true, "table literal GLOWING 3 + front" },
        // busy blocks
        { true, kAnimNormal, true, false, "table busy normal blocks" },
        { true, kAnimGlowing, true, false, "table busy glowing blocks" },
        { true, kAnimNormal, false, false, "table busy + not front blocks" },
        // wrong anim blocks
        { false, kAnimTrample, true, false, "table trample + front blocks" },
        { false, kAnimHornBroken, true, false, "table broken + front blocks" },
        { false, 99, true, false, "table unknown anim blocks" },
        { false, -1, true, false, "table negative anim blocks" },
        { false, 1, true, false, "table literal TRAMPLE 1 blocks" },
        { false, 2, true, false, "table literal HORN_BROKEN 2 blocks" },
        // not in front blocks
        { false, kAnimNormal, false, false, "table normal not front blocks" },
        { false, kAnimGlowing, false, false, "table glowing not front blocks" },
        // combined fail poles
        { true, kAnimTrample, false, false, "table busy trample not-front" },
        { true, kAnimHornBroken, false, false, "table busy broken not-front" },
        { true, 99, false, false, "table busy unknown not-front" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanBreakHorn(c.busy, c.animSub, c.attackerInFront);
        const bool inlineC = inlineCanBreakHorn(c.busy, c.animSub, c.attackerInFront);
        const bool pinGot  = pinCanBreakHorn(c.busy, c.animSub, c.attackerInFront);
        const bool compose =
            !c.busy && (c.animSub == kAnimNormal || c.animSub == kAnimGlowing) && c.attackerInFront;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineC, "dual-wire free==inline") && ok;
        ok = expect(got == pinGot, "dual-wire free==pin (positive if/else)") && ok;
        ok = expect(got == compose, "formula free==!busy&&(NORMAL||GLOWING)&&front") && ok;
    }

    // Free == pin across residual poles (positive if/else pin form).
    ok = expect(CanBreakHorn(false, kAnimNormal, true) == pinCanBreakHorn(false, kAnimNormal, true),
                "free == pin residual normal+front") &&
         ok;
    ok = expect(CanBreakHorn(false, kAnimGlowing, true) == pinCanBreakHorn(false, kAnimGlowing, true),
                "free == pin residual glowing+front") &&
         ok;
    ok = expect(CanBreakHorn(true, kAnimNormal, true) == pinCanBreakHorn(true, kAnimNormal, true),
                "free == pin residual busy") &&
         ok;
    ok = expect(CanBreakHorn(false, kAnimHornBroken, true) == pinCanBreakHorn(false, kAnimHornBroken, true),
                "free == pin residual broken") &&
         ok;
    ok = expect(CanBreakHorn(false, kAnimNormal, false) == pinCanBreakHorn(false, kAnimNormal, false),
                "free == pin residual not front") &&
         ok;

    // Dense compose identity over representative busy / anim / front poles.
    const bool composeBusy[]  = { false, true };
    const int32 composeAnims[] = { -1, 0, kAnimNormal, kAnimTrample, kAnimHornBroken, kAnimGlowing, 1, 2, 3, 4, 99, 255 };
    const bool composeFront[] = { false, true };
    for (const bool busy : composeBusy)
    {
        for (const int32 anim : composeAnims)
        {
            for (const bool front : composeFront)
            {
                const bool got = CanBreakHorn(busy, anim, front);
                const bool want =
                    !busy && (anim == kAnimNormal || anim == kAnimGlowing) && front;
                ok = expect(got == want, "compose !busy&&(NORMAL||GLOWING)&&front") && ok;
                ok = expect(got == inlineCanBreakHorn(busy, anim, front), "compose inline") && ok;
                ok = expect(got == pinCanBreakHorn(busy, anim, front), "compose pin positive if/else") && ok;
            }
        }
    }

    // Production checkHornBreak path semantics:
    // Eligible → host may roll 5% and changeHornState(mob, 2).
    // Blocked → no roll / no state change.
    ok = expect(CanBreakHorn(false, kAnimNormal, true), "checkHornBreak eligible normal → continue") && ok;
    ok = expect(CanBreakHorn(false, kAnimGlowing, true), "checkHornBreak eligible glowing → continue") && ok;
    ok = expect(!CanBreakHorn(true, kAnimNormal, true), "checkHornBreak busy → blocked") && ok;
    ok = expect(!CanBreakHorn(false, kAnimHornBroken, true), "checkHornBreak broken → blocked") && ok;
    ok = expect(!CanBreakHorn(false, kAnimNormal, false), "checkHornBreak not front → blocked") && ok;

    return ok;
}
