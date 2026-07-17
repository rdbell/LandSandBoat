#include "test_darkixion_can_break_horn_3295.h"

#include "map/darkixion_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "darkixion CanBreakHorn 3295 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline checkHornBreak pure gate (before 5% roll) for dual-wire cross-check
// (dedicated 3295 expand residual 2885; prior 3266 / 3154):
//   !busy && (animSub == NORMAL || animSub == GLOWING) && attackerInFront
auto inlineCanBreakHorn(const bool busy, const int32 animSub, const bool attackerInFront) -> bool
{
    return !busy && (animSub == darkixionhelpers::kAnimNormal || animSub == darkixionhelpers::kAnimGlowing) &&
           attackerInFront;
}

// Positive if/else pin matching free function / capacity body (slice 3295).
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
// dedicated slice 3295 expand residual 2885; prior dedicated 3266 / 3154 retained).
//
// Coverage:
//   - free == inline == pin (positive if/else)
//   - residual 2885 / 3266 / 3154 / 0985 pins still hold
//   - residual poles: busy / anim / front
//   - dense busy / anim / front compose
auto runDarkixionCanBreakHorn3295SelfTests() -> bool
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

    // Residual 0985 / 2885 / 3154 / 3266 CanBreakHorn pins still hold under dual-wire.
    ok = expect(CanBreakHorn(false, kAnimNormal, true), "residual normal+front should pass") && ok;
    ok = expect(CanBreakHorn(false, kAnimGlowing, true), "residual glowing+front should pass") && ok;
    ok = expect(!CanBreakHorn(true, kAnimNormal, true), "residual busy should fail") && ok;
    ok = expect(!CanBreakHorn(false, kAnimHornBroken, true), "residual broken should fail") && ok;
    ok = expect(!CanBreakHorn(false, kAnimTrample, true), "residual trample should fail") && ok;
    ok = expect(!CanBreakHorn(false, kAnimNormal, false), "residual not in front should fail") && ok;
    ok = expect(!CanBreakHorn(true, kAnimGlowing, true), "residual busy glowing should fail") && ok;
    ok = expect(!CanBreakHorn(false, kAnimGlowing, false), "residual glowing not front should fail") && ok;
    ok = expect(!CanBreakHorn(false, 99, true), "residual unknown anim should fail") && ok;
    ok = expect(!CanBreakHorn(false, -1, true), "residual negative anim should fail") && ok;
    ok = expect(!CanBreakHorn(true, kAnimHornBroken, false), "residual all poles fail") && ok;
    ok = expect(!CanBreakHorn(true, kAnimTrample, true), "residual busy trample should fail") && ok;
    ok = expect(!CanBreakHorn(true, kAnimNormal, false), "residual busy not front should fail") && ok;
    ok = expect(!CanBreakHorn(false, 4, true), "residual anim 4 should fail") && ok;
    ok = expect(!CanBreakHorn(false, 255, true), "residual anim 255 should fail") && ok;
    ok = expect(CanBreakHorn(false, 0, true), "residual literal NORMAL 0 + front should pass") && ok;
    ok = expect(CanBreakHorn(false, 3, true), "residual literal GLOWING 3 + front should pass") && ok;

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
    ok = expect(!CanBreakHorn(true, kAnimGlowing, false), "blocked busy glowing not front") && ok;
    ok = expect(!CanBreakHorn(false, 1, true), "blocked literal TRAMPLE 1") && ok;
    ok = expect(!CanBreakHorn(false, 2, true), "blocked literal HORN_BROKEN 2") && ok;

    // Residual poles (busy / anim / front) + free==inline==pin table.
    const struct
    {
        bool        busy;
        int32       animSub;
        bool        attackerInFront;
        bool        want;
        const char* label;
    } cases[] = {
        // Residual poles: eligible.
        { false, kAnimNormal, true, true, "pole eligible normal+front" },
        { false, kAnimGlowing, true, true, "pole eligible glowing+front" },
        { false, 0, true, true, "pole eligible literal NORMAL 0" },
        { false, 3, true, true, "pole eligible literal GLOWING 3" },
        // Residual poles: busy blocks.
        { true, kAnimNormal, true, false, "pole busy normal blocks" },
        { true, kAnimGlowing, true, false, "pole busy glowing blocks" },
        { true, kAnimNormal, false, false, "pole busy + not front blocks" },
        { true, kAnimGlowing, false, false, "pole busy glowing + not front blocks" },
        // Residual poles: wrong anim blocks.
        { false, kAnimTrample, true, false, "pole trample + front blocks" },
        { false, kAnimHornBroken, true, false, "pole broken + front blocks" },
        { false, 99, true, false, "pole unknown anim blocks" },
        { false, -1, true, false, "pole negative anim blocks" },
        { false, 1, true, false, "pole literal TRAMPLE 1 blocks" },
        { false, 2, true, false, "pole literal HORN_BROKEN 2 blocks" },
        { false, 4, true, false, "pole anim 4 blocks" },
        { false, 255, true, false, "pole anim 255 blocks" },
        // Residual poles: not in front blocks.
        { false, kAnimNormal, false, false, "pole normal not front blocks" },
        { false, kAnimGlowing, false, false, "pole glowing not front blocks" },
        { false, 0, false, false, "pole literal NORMAL not front blocks" },
        { false, 3, false, false, "pole literal GLOWING not front blocks" },
        // Combined fail poles.
        { true, kAnimTrample, false, false, "pole busy trample not-front" },
        { true, kAnimHornBroken, false, false, "pole busy broken not-front" },
        { true, 99, false, false, "pole busy unknown not-front" },
        { true, kAnimTrample, true, false, "pole busy trample front" },
        { true, kAnimHornBroken, true, false, "pole busy broken front" },
        // Dense host poles.
        { false, 4, true, false, "dense unknown anim 4" },
        { false, 255, true, false, "dense anim 255" },
        { false, kAnimNormal, true, true, "dense normal+front" },
        { false, kAnimGlowing, true, true, "dense glowing+front" },
        { true, 0, true, false, "dense busy literal NORMAL" },
        { true, 3, true, false, "dense busy literal GLOWING" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanBreakHorn(c.busy, c.animSub, c.attackerInFront);
        const bool inlineC = inlineCanBreakHorn(c.busy, c.animSub, c.attackerInFront);
        const bool pinGot  = pinCanBreakHorn(c.busy, c.animSub, c.attackerInFront);
        const bool compose =
            !c.busy && (c.animSub == kAnimNormal || c.animSub == kAnimGlowing) && c.attackerInFront;

        ok = expect(got == c.want, c.label) && ok;
        // Positive form: free == inline == pin.
        ok = expect(got == inlineC && got == pinGot, "dual-wire free==inline==pin") && ok;
        ok = expect(got == compose, "formula free==!busy&&(NORMAL||GLOWING)&&front") && ok;
    }

    // Explicit residual poles free == inline == pin (busy / anim / front).
    const struct
    {
        bool        busy;
        int32       animSub;
        bool        attackerInFront;
        const char* label;
    } poles[] = {
        { false, kAnimNormal, true, "eligible normal+front" },
        { false, kAnimGlowing, true, "eligible glowing+front" },
        { false, 0, true, "eligible literal NORMAL" },
        { false, 3, true, "eligible literal GLOWING" },
        { true, kAnimNormal, true, "busy blocks" },
        { false, kAnimHornBroken, true, "broken blocks" },
        { false, kAnimTrample, true, "trample blocks" },
        { false, kAnimNormal, false, "not front blocks" },
        { false, kAnimGlowing, false, "glowing not front blocks" },
        { true, kAnimGlowing, true, "busy glowing blocks" },
        { true, kAnimGlowing, false, "busy glowing not front" },
        { true, kAnimNormal, false, "busy normal not front" },
        { false, 99, true, "unknown anim blocks" },
        { false, -1, true, "negative anim blocks" },
        { false, 4, true, "anim 4 blocks" },
        { false, 255, true, "anim 255 blocks" },
        { true, kAnimHornBroken, false, "all poles fail" },
        { true, kAnimTrample, true, "busy trample front" },
    };
    for (const auto& p : poles)
    {
        const bool got     = CanBreakHorn(p.busy, p.animSub, p.attackerInFront);
        const bool inlineC = inlineCanBreakHorn(p.busy, p.animSub, p.attackerInFront);
        const bool pinGot  = pinCanBreakHorn(p.busy, p.animSub, p.attackerInFront);
        const bool want =
            !p.busy && (p.animSub == kAnimNormal || p.animSub == kAnimGlowing) && p.attackerInFront;
        ok = expect(got == want, p.label) && ok;
        ok = expect(got == inlineC && got == pinGot, "pole free==inline==pin") && ok;
    }

    // Dense compose free == inline == pin over representative busy / anim / front poles.
    const bool  composeBusy[]  = { false, true };
    const int32 composeAnims[] = { -1, 0, kAnimNormal, kAnimTrample, kAnimHornBroken, kAnimGlowing, 1, 2, 3, 4, 5, 99, 127, 255 };
    const bool  composeFront[] = { false, true };
    for (const bool busy : composeBusy)
    {
        for (const int32 anim : composeAnims)
        {
            for (const bool front : composeFront)
            {
                const bool got = CanBreakHorn(busy, anim, front);
                const bool want =
                    !busy && (anim == kAnimNormal || anim == kAnimGlowing) && front;
                ok = expect(got == want, "dense free==pin formula") && ok;
                ok = expect(got == inlineCanBreakHorn(busy, anim, front) && got == pinCanBreakHorn(busy, anim, front),
                            "dense free==inline==pin") &&
                     ok;
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
    ok = expect(!CanBreakHorn(true, kAnimGlowing, false), "checkHornBreak busy glowing not front → blocked") && ok;

    return ok;
}
