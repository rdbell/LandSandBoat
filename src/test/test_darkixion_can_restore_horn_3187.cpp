#include "test_darkixion_can_restore_horn_3187.h"

#include "map/darkixion_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "darkixion CanRestoreHorn 3187 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Damsel Memento pure gate (before 25% roll) for dual-wire cross-check
// (slice 3187; residual 2893):
//   animSub == HORN_BROKEN
auto inlineCanRestoreHorn(const int32 animSub) -> bool
{
    return animSub == darkixionhelpers::kAnimHornBroken;
}

// Positive if/else pin matching free function / capacity body (slice 3187).
// Avoid De Morgan rewrites of the equality gate (QF1001).
auto pinCanRestoreHorn(const int32 animSub) -> bool
{
    if (animSub == darkixionhelpers::kAnimHornBroken)
    {
        return true;
    }
    return false;
}

} // namespace

// Pure dual-wire expansion for darkixionhelpers::CanRestoreHorn
// (Lua Damsel Memento gate before 25% roll; OmegaXI internal/darkixion;
// dedicated slice 3187).
//
// Coverage:
//   - HORN_BROKEN → restore true
//   - NORMAL / TRAMPLE / GLOWING / unknown → restore false
//   - free == inline == pin (positive if/else)
//   - residual 0985 / 2893 pins still hold
//   - poles normal / trample / broken / glowing
auto runDarkixionCanRestoreHorn3187SelfTests() -> bool
{
    using darkixionhelpers::CanRestoreHorn;
    using darkixionhelpers::kAnimGlowing;
    using darkixionhelpers::kAnimHornBroken;
    using darkixionhelpers::kAnimNormal;
    using darkixionhelpers::kAnimTrample;

    bool ok = true;

    // Animation-sub catalog pins (match Go AnimHornBroken / residual set).
    ok = expect(kAnimNormal == 0, "AnimNormal pin") && ok;
    ok = expect(kAnimTrample == 1, "AnimTrample pin") && ok;
    ok = expect(kAnimHornBroken == 2, "AnimHornBroken pin") && ok;
    ok = expect(kAnimGlowing == 3, "AnimGlowing pin") && ok;

    // Residual 0985 / 2893 CanRestoreHorn pins still hold under dual-wire.
    ok = expect(CanRestoreHorn(kAnimHornBroken), "residual broken should pass") && ok;
    ok = expect(!CanRestoreHorn(kAnimNormal), "residual normal should fail") && ok;
    ok = expect(!CanRestoreHorn(kAnimTrample), "residual trample should fail") && ok;
    ok = expect(!CanRestoreHorn(kAnimGlowing), "residual glowing should fail") && ok;

    // --- Eligible restore path (HORN_BROKEN only) ---
    ok = expect(CanRestoreHorn(kAnimHornBroken), "eligible AnimHornBroken") && ok;
    ok = expect(CanRestoreHorn(2), "eligible literal HORN_BROKEN 2") && ok;

    // --- Blocked paths (normal / trample / glowing / unknown) ---
    ok = expect(!CanRestoreHorn(kAnimNormal), "blocked normal") && ok;
    ok = expect(!CanRestoreHorn(kAnimTrample), "blocked trample") && ok;
    ok = expect(!CanRestoreHorn(kAnimGlowing), "blocked glowing") && ok;
    ok = expect(!CanRestoreHorn(0), "blocked literal NORMAL 0") && ok;
    ok = expect(!CanRestoreHorn(1), "blocked literal TRAMPLE 1") && ok;
    ok = expect(!CanRestoreHorn(3), "blocked literal GLOWING 3") && ok;
    ok = expect(!CanRestoreHorn(99), "blocked unknown anim") && ok;
    ok = expect(!CanRestoreHorn(-1), "blocked negative anim") && ok;
    ok = expect(!CanRestoreHorn(4), "blocked anim 4") && ok;
    ok = expect(!CanRestoreHorn(255), "blocked anim 255") && ok;

    // --- Composition table: free == inline == pin (positive if/else) ---
    // Poles: normal / trample / broken / glowing (+ unknowns).
    const struct
    {
        int32       animSub;
        bool        want;
        const char* label;
    } cases[] = {
        // eligible
        { kAnimHornBroken, true, "table horn broken" },
        { 2, true, "table literal HORN_BROKEN 2" },
        // wrong anim poles
        { kAnimNormal, false, "table normal blocks" },
        { kAnimTrample, false, "table trample blocks" },
        { kAnimGlowing, false, "table glowing blocks" },
        { 0, false, "table literal NORMAL 0 blocks" },
        { 1, false, "table literal TRAMPLE 1 blocks" },
        { 3, false, "table literal GLOWING 3 blocks" },
        // unknown / edge
        { 99, false, "table unknown anim blocks" },
        { -1, false, "table negative anim blocks" },
        { 4, false, "table anim 4 blocks" },
        { 255, false, "table anim 255 blocks" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanRestoreHorn(c.animSub);
        const bool inlineC = inlineCanRestoreHorn(c.animSub);
        const bool pinGot  = pinCanRestoreHorn(c.animSub);
        const bool compose = c.animSub == kAnimHornBroken;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineC, "dual-wire free==inline") && ok;
        ok = expect(got == pinGot, "dual-wire free==pin (positive if/else)") && ok;
        ok = expect(got == compose, "formula free==animSub==HORN_BROKEN") && ok;
    }

    // Free == pin across residual poles (positive if/else pin form).
    ok = expect(CanRestoreHorn(kAnimHornBroken) == pinCanRestoreHorn(kAnimHornBroken),
                "free == pin residual broken") &&
         ok;
    ok = expect(CanRestoreHorn(kAnimNormal) == pinCanRestoreHorn(kAnimNormal),
                "free == pin residual normal") &&
         ok;
    ok = expect(CanRestoreHorn(kAnimTrample) == pinCanRestoreHorn(kAnimTrample),
                "free == pin residual trample") &&
         ok;
    ok = expect(CanRestoreHorn(kAnimGlowing) == pinCanRestoreHorn(kAnimGlowing),
                "free == pin residual glowing") &&
         ok;

    // Dense compose identity over representative anim poles.
    const int32 composeAnims[] = { -1, 0, kAnimNormal, kAnimTrample, kAnimHornBroken, kAnimGlowing, 1, 2, 3, 4, 99, 255 };
    for (const int32 anim : composeAnims)
    {
        const bool got  = CanRestoreHorn(anim);
        const bool want = anim == kAnimHornBroken;
        ok              = expect(got == want, "compose animSub==HORN_BROKEN") && ok;
        ok              = expect(got == inlineCanRestoreHorn(anim), "compose inline") && ok;
        ok              = expect(got == pinCanRestoreHorn(anim), "compose pin positive if/else") && ok;
    }

    // Production Damsel Memento path semantics:
    // Eligible (HORN_BROKEN) → host may roll 25% and restore.
    // Blocked → no roll / no state change.
    ok = expect(CanRestoreHorn(kAnimHornBroken), "Damsel Memento eligible broken → continue") && ok;
    ok = expect(!CanRestoreHorn(kAnimNormal), "Damsel Memento normal → blocked") && ok;
    ok = expect(!CanRestoreHorn(kAnimTrample), "Damsel Memento trample → blocked") && ok;
    ok = expect(!CanRestoreHorn(kAnimGlowing), "Damsel Memento glowing → blocked") && ok;

    return ok;
}
