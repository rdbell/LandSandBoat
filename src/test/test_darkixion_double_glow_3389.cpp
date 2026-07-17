#include "test_darkixion_double_glow_3389.h"

#include "map/darkixion_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "darkixion ShouldDoubleGlowSkill 3389 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline DI_GLOW double-up pure gate for dual-wire cross-check
// (dedicated 3389 expand residual 2915):
//   animSub == GLOWING
auto inlineShouldDoubleGlowSkill(const int32 animSub) -> bool
{
    return animSub == darkixionhelpers::kAnimGlowing;
}

// Compact dual-wire pin matching Go pinShouldDoubleGlowSkill3389 /
// free function / capacity body (slice 3389). Equality form only — avoid
// De Morgan rewrites of the gate.
auto pinShouldDoubleGlowSkill(const int32 animSub) -> bool
{
    return animSub == darkixionhelpers::kAnimGlowing;
}

} // namespace

// Pure dual-wire expansion for darkixionhelpers::ShouldDoubleGlowSkill
// (Lua DI_GLOW double-up gate; OmegaXI internal/darkixion; dedicated slice
// 3389 expand residual 2915 / pure 0985 — formula unchanged).
//
// Coverage:
//   - free == inline == pin == animSub == GLOWING
//   - residual poles: glowing / normal / trample / horn broken
//   - dense animSub space
//   - residual 0985 / 2915 pins still hold
//   - host-owned skill pick / useMobAbility left outside pure surface
//
// Sibling suite RETAINED: test_darkixion_double_glow_2915.
auto runDarkixionDoubleGlow3389SelfTests() -> bool
{
    using darkixionhelpers::kAnimGlowing;
    using darkixionhelpers::kAnimHornBroken;
    using darkixionhelpers::kAnimNormal;
    using darkixionhelpers::kAnimTrample;
    using darkixionhelpers::ShouldDoubleGlowSkill;

    bool ok = true;

    // Animation-sub catalog pins (match Go AnimGlowing / residual set).
    ok = expect(kAnimNormal == 0, "AnimNormal pin") && ok;
    ok = expect(kAnimTrample == 1, "AnimTrample pin") && ok;
    ok = expect(kAnimHornBroken == 2, "AnimHornBroken pin") && ok;
    ok = expect(kAnimGlowing == 3, "AnimGlowing pin") && ok;

    // Residual 0985 / 2915 ShouldDoubleGlowSkill pins still hold under dual-wire.
    ok = expect(ShouldDoubleGlowSkill(kAnimGlowing), "residual glowing should pass") && ok;
    ok = expect(!ShouldDoubleGlowSkill(kAnimNormal), "residual normal should fail") && ok;
    ok = expect(!ShouldDoubleGlowSkill(kAnimTrample), "residual trample should fail") && ok;
    ok = expect(!ShouldDoubleGlowSkill(kAnimHornBroken), "residual horn broken should fail") && ok;

    // --- Eligible double-up path (GLOWING only) ---
    ok = expect(ShouldDoubleGlowSkill(kAnimGlowing), "eligible AnimGlowing") && ok;
    ok = expect(ShouldDoubleGlowSkill(3), "eligible literal GLOWING 3") && ok;

    // --- Blocked paths (normal / trample / horn broken / unknown) ---
    ok = expect(!ShouldDoubleGlowSkill(kAnimNormal), "blocked normal") && ok;
    ok = expect(!ShouldDoubleGlowSkill(kAnimTrample), "blocked trample") && ok;
    ok = expect(!ShouldDoubleGlowSkill(kAnimHornBroken), "blocked horn broken") && ok;
    ok = expect(!ShouldDoubleGlowSkill(0), "blocked literal NORMAL 0") && ok;
    ok = expect(!ShouldDoubleGlowSkill(1), "blocked literal TRAMPLE 1") && ok;
    ok = expect(!ShouldDoubleGlowSkill(2), "blocked literal HORN_BROKEN 2") && ok;
    ok = expect(!ShouldDoubleGlowSkill(99), "blocked unknown anim") && ok;
    ok = expect(!ShouldDoubleGlowSkill(-1), "blocked negative anim") && ok;
    ok = expect(!ShouldDoubleGlowSkill(4), "blocked anim 4") && ok;
    ok = expect(!ShouldDoubleGlowSkill(255), "blocked anim 255") && ok;

    // --- Composition table: free == inline == pin (positive equality) ---
    // Poles: glowing / normal / trample / horn broken (+ unknowns).
    const struct
    {
        int32       animSub;
        bool        want;
        const char* label;
    } cases[] = {
        // eligible
        { kAnimGlowing, true, "table glowing" },
        { 3, true, "table literal GLOWING 3" },
        // wrong anim poles
        { kAnimNormal, false, "table normal blocks" },
        { kAnimTrample, false, "table trample blocks" },
        { kAnimHornBroken, false, "table horn broken blocks" },
        { 0, false, "table literal NORMAL 0 blocks" },
        { 1, false, "table literal TRAMPLE 1 blocks" },
        { 2, false, "table literal HORN_BROKEN 2 blocks" },
        // residual 2915 re-pins
        { kAnimGlowing, true, "residual 2915 glowing" },
        { kAnimNormal, false, "residual 2915 normal" },
        // residual 0985 re-pins
        { kAnimGlowing, true, "residual 0985 glowing" },
        { kAnimNormal, false, "residual 0985 normal" },
        // unknown / edge
        { 99, false, "table unknown anim blocks" },
        { -1, false, "table negative anim blocks" },
        { 4, false, "table anim 4 blocks" },
        { 255, false, "table anim 255 blocks" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldDoubleGlowSkill(c.animSub);
        const bool inlineC = inlineShouldDoubleGlowSkill(c.animSub);
        const bool pinGot  = pinShouldDoubleGlowSkill(c.animSub);
        const bool compose = c.animSub == kAnimGlowing;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineC, "dual-wire free==inline") && ok;
        ok = expect(got == pinGot, "dual-wire free==pin") && ok;
        ok = expect(got == compose, "formula free==animSub==GLOWING") && ok;
    }

    // Free == pin across residual poles (positive equality pin form).
    ok = expect(ShouldDoubleGlowSkill(kAnimGlowing) == pinShouldDoubleGlowSkill(kAnimGlowing),
                "free == pin residual glowing") &&
         ok;
    ok = expect(ShouldDoubleGlowSkill(kAnimNormal) == pinShouldDoubleGlowSkill(kAnimNormal),
                "free == pin residual normal") &&
         ok;
    ok = expect(ShouldDoubleGlowSkill(kAnimTrample) == pinShouldDoubleGlowSkill(kAnimTrample),
                "free == pin residual trample") &&
         ok;
    ok = expect(ShouldDoubleGlowSkill(kAnimHornBroken) == pinShouldDoubleGlowSkill(kAnimHornBroken),
                "free == pin residual horn broken") &&
         ok;

    // Explicit residual poles free == inline == pin for glowing / normal.
    for (const int32 anim : { kAnimGlowing, kAnimNormal })
    {
        const bool got     = ShouldDoubleGlowSkill(anim);
        const bool inlineC = inlineShouldDoubleGlowSkill(anim);
        const bool pinGot  = pinShouldDoubleGlowSkill(anim);
        const bool want    = anim == kAnimGlowing;
        ok                 = expect(got == want, "pole free == animSub==GLOWING") && ok;
        ok                 = expect(got == inlineC, "pole free == inline") && ok;
        ok                 = expect(got == pinGot, "pole free == pin") && ok;
    }

    // Dense compose identity over representative anim poles.
    const int32 composeAnims[] = { -1, 0, kAnimNormal, kAnimTrample, kAnimHornBroken, kAnimGlowing, 1, 2, 3, 4, 99, 255 };
    for (const int32 anim : composeAnims)
    {
        const bool got  = ShouldDoubleGlowSkill(anim);
        const bool want = anim == kAnimGlowing;
        ok              = expect(got == want, "compose animSub==GLOWING") && ok;
        ok              = expect(got == inlineShouldDoubleGlowSkill(anim), "compose inline") && ok;
        ok              = expect(got == pinShouldDoubleGlowSkill(anim), "compose pin") && ok;
    }

    // Host compose: animSub inject after DI_GLOW first follow-up; when true,
    // host queues the same chosen skill a second time (useMobAbility still
    // host-owned).
    const struct
    {
        int32       animSub;
        bool        want;
        int         wantQueueCount;
        const char* label;
    } composeCases[] = {
        { kAnimGlowing, true, 2, "compose eligible glowing" },
        { kAnimNormal, false, 1, "compose normal blocks" },
        { kAnimTrample, false, 1, "compose trample blocks" },
        { kAnimHornBroken, false, 1, "compose horn broken blocks" },
        { 99, false, 1, "compose unknown blocks" },
        { -1, false, 1, "compose negative blocks" },
        { 3, true, 2, "compose literal GLOWING 3" },
    };

    for (const auto& c : composeCases)
    {
        const bool got        = ShouldDoubleGlowSkill(c.animSub);
        const bool inlineGot  = inlineShouldDoubleGlowSkill(c.animSub);
        const bool pinGot     = pinShouldDoubleGlowSkill(c.animSub);
        const int  queueCount = got ? 2 : 1;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineGot, "compose dual-wire free == inline") && ok;
        ok = expect(got == pinGot, "compose dual-wire free == pin") && ok;
        ok = expect(queueCount == c.wantQueueCount, "compose queue count") && ok;
    }

    // Production DI_GLOW path semantics:
    // Eligible (GLOWING) → host may queue second useMobAbility(chosenSkill).
    // Blocked → single follow-up only.
    ok = expect(ShouldDoubleGlowSkill(kAnimGlowing), "DI_GLOW eligible glowing → continue") && ok;
    ok = expect(!ShouldDoubleGlowSkill(kAnimNormal), "DI_GLOW normal → blocked") && ok;
    ok = expect(!ShouldDoubleGlowSkill(kAnimTrample), "DI_GLOW trample → blocked") && ok;
    ok = expect(!ShouldDoubleGlowSkill(kAnimHornBroken), "DI_GLOW horn broken → blocked") && ok;

    // Residual 2915 independence: free still matches residual poles.
    ok = expect(ShouldDoubleGlowSkill(kAnimGlowing) && !ShouldDoubleGlowSkill(kAnimNormal),
                "residual 2915 independence poles") &&
         ok;

    return ok;
}
