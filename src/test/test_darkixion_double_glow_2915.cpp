#include "test_darkixion_double_glow_2915.h"

#include "map/darkixion_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "darkixion ShouldDoubleGlowSkill 2915 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline DI_GLOW double-up pure gate for dual-wire cross-check:
//   animSub == GLOWING
auto inlineShouldDoubleGlowSkill(const int32 animSub) -> bool
{
    return animSub == darkixionhelpers::kAnimGlowing;
}

} // namespace

// Pure dual-wire expansion for darkixionhelpers::ShouldDoubleGlowSkill
// (Lua DI_GLOW double-up gate; slice 2915).
auto runDarkixionDoubleGlow2915SelfTests() -> bool
{
    using darkixionhelpers::kAnimGlowing;
    using darkixionhelpers::kAnimHornBroken;
    using darkixionhelpers::kAnimNormal;
    using darkixionhelpers::kAnimTrample;
    using darkixionhelpers::ShouldDoubleGlowSkill;

    bool ok = true;

    // Animation-sub catalog pins (match Go AnimNormal / AnimGlowing / …).
    ok = expect(kAnimNormal == 0, "AnimNormal pin") && ok;
    ok = expect(kAnimTrample == 1, "AnimTrample pin") && ok;
    ok = expect(kAnimHornBroken == 2, "AnimHornBroken pin") && ok;
    ok = expect(kAnimGlowing == 3, "AnimGlowing pin") && ok;

    // Residual 0985 ShouldDoubleGlowSkill pins.
    ok = expect(ShouldDoubleGlowSkill(kAnimGlowing), "glowing should pass") && ok;
    ok = expect(!ShouldDoubleGlowSkill(kAnimNormal), "normal should fail") && ok;

    // Dual-wire matches inline formula across a table.
    const struct
    {
        int32       animSub;
        bool        want;
        const char* label;
    } cases[] = {
        { kAnimGlowing, true, "table glowing" },
        { kAnimNormal, false, "table normal" },
        { kAnimTrample, false, "table trample" },
        { kAnimHornBroken, false, "table horn broken" },
        { 99, false, "table unknown anim" },
        { -1, false, "table negative anim" },
        { 0, false, "table zero explicit" },
        { kAnimGlowing, true, "table residual glowing pin" },
        { kAnimNormal, false, "table residual normal pin" },
    };

    for (const auto& c : cases)
    {
        const bool got       = ShouldDoubleGlowSkill(c.animSub);
        const bool inlineGot = inlineShouldDoubleGlowSkill(c.animSub);
        const bool wantPin   = c.animSub == kAnimGlowing;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineGot, "dual-wire free == inline Lua formula") && ok;
        ok = expect(got == wantPin, "dual-wire free == pin formula") && ok;
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
    };

    for (const auto& c : composeCases)
    {
        const bool got       = ShouldDoubleGlowSkill(c.animSub);
        const bool inlineGot = inlineShouldDoubleGlowSkill(c.animSub);
        const int  queueCount = got ? 2 : 1;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineGot, "compose dual-wire free == inline") && ok;
        ok = expect(queueCount == c.wantQueueCount, "compose queue count") && ok;
    }

    return ok;
}
