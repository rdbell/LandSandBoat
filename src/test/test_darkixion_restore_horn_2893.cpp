#include "test_darkixion_restore_horn_2893.h"

#include "map/darkixion_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "darkixion CanRestoreHorn 2893 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Damsel Memento pure gate (before 25% roll) for dual-wire cross-check:
//   animSub == HORN_BROKEN
auto inlineCanRestoreHorn(const int32 animSub) -> bool
{
    return animSub == darkixionhelpers::kAnimHornBroken;
}

} // namespace

// Pure dual-wire expansion for darkixionhelpers::CanRestoreHorn
// (Lua Damsel Memento gate before 25% roll; slice 2893).
auto runDarkixionRestoreHorn2893SelfTests() -> bool
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

    // Primary residual 0985 pins for CanRestoreHorn.
    ok = expect(CanRestoreHorn(kAnimHornBroken), "broken should pass") && ok;
    ok = expect(!CanRestoreHorn(kAnimNormal), "normal should fail") && ok;

    // Dual-wire matches inline formula across a table.
    const struct
    {
        int32       animSub;
        bool        want;
        const char* label;
    } cases[] = {
        { kAnimHornBroken, true, "table horn broken" },
        { kAnimNormal, false, "table normal" },
        { kAnimTrample, false, "table trample" },
        { kAnimGlowing, false, "table glowing" },
        { 99, false, "table unknown anim" },
        { -1, false, "table negative anim" },
        { 0, false, "table zero explicit" },
        { kAnimHornBroken, true, "table residual broken pin" },
    };

    for (const auto& c : cases)
    {
        const bool got       = CanRestoreHorn(c.animSub);
        const bool inlineGot = inlineCanRestoreHorn(c.animSub);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineGot, "dual-wire free == inline Lua formula") && ok;
    }

    // Host compose: animSub is the only inject; only HORN_BROKEN succeeds
    // (25% roll / stun / changeHornState still host-owned).
    const struct
    {
        int32       animSub;
        bool        want;
        const char* label;
    } composeCases[] = {
        { kAnimHornBroken, true, "compose eligible broken" },
        { kAnimNormal, false, "compose normal blocks" },
        { kAnimTrample, false, "compose trample blocks" },
        { kAnimGlowing, false, "compose glowing blocks" },
    };

    for (const auto& c : composeCases)
    {
        const bool got       = CanRestoreHorn(c.animSub);
        const bool inlineGot = inlineCanRestoreHorn(c.animSub);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineGot, "compose dual-wire free == inline") && ok;
    }

    return ok;
}
