#include "test_battlefield_clear_level_3102.h"

#include "map/battlefield_capacity.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "battlefield ShouldClearLevelRestriction 3102 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline ApplyLevelRestrictions clear LevelRestriction gate formula for
// dual-wire cross-check (slice 3102):
//   levelCap == 0
auto inlineShouldClearLevelRestriction(const uint8 levelCap) -> bool
{
    return levelCap == 0;
}

} // namespace

// Pure dual-wire expansion for battlefieldhelpers::ShouldClearLevelRestriction
// (levelCap == 0 on ApplyLevelRestrictions clear path; slice 3102).
// Pins levelCap 0/1/75/99/255; free == inline levelCap == 0.
auto runBattlefieldClearLevel3102SelfTests() -> bool
{
    using battlefieldhelpers::ResolveLevelCap;
    using battlefieldhelpers::ShouldAcceptPCUnderCapacity;
    using battlefieldhelpers::ShouldAddSjRestriction;
    using battlefieldhelpers::ShouldApplyLevelCap;
    using battlefieldhelpers::ShouldClearLevelRestriction;
    using battlefieldhelpers::ShouldEnterPC;
    using battlefieldhelpers::ShouldRegisterPC;
    using battlefieldhelpers::ShouldRejectAlreadyInBattlefield;
    using battlefieldhelpers::ShouldRejectNullInsert;
    using battlefieldhelpers::RulesAllowSubjobs;

    bool ok = true;

    // Residual 1361 pins still hold under dual-wire.
    ok = expect(ShouldClearLevelRestriction(0), "residual: levelCap 0 clears restriction") && ok;
    ok = expect(!ShouldClearLevelRestriction(75), "residual: levelCap 75 does not clear") && ok;

    const struct
    {
        uint8       levelCap;
        bool        want;
        const char* label;
    } cases[] = {
        // Required dual-wire poles (task: 0/1/75/99/255).
        { 0, true, "levelCap 0 uncapped clears restriction" },
        { 1, false, "levelCap 1 does not clear (applies cap)" },
        { 75, false, "levelCap 75 classic BCNM does not clear" },
        { 99, false, "levelCap 99 max era does not clear" },
        { 255, false, "levelCap 255 uint8 max does not clear" },

        // Residual 1361 pin.
        { 0, true, "residual ShouldClearLevelRestriction(0)" },

        // Additional polarity / boundary pins.
        { 2, false, "levelCap 2 does not clear" },
        { 50, false, "levelCap 50 does not clear" },
        { 100, false, "levelCap 100 does not clear" },
        { 254, false, "levelCap 254 does not clear" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldClearLevelRestriction(c.levelCap);
        const bool inlineF = inlineShouldClearLevelRestriction(c.levelCap);
        const bool wantPin = c.levelCap == 0;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldClearLevelRestriction dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldClearLevelRestriction == pin formula levelCap == 0") && ok;
    }

    // Pin composition: clear iff levelCap == 0.
    ok = expect(ShouldClearLevelRestriction(0), "levelCap 0 must clear") && ok;
    ok = expect(!ShouldClearLevelRestriction(1), "levelCap 1 must not clear") && ok;
    ok = expect(!ShouldClearLevelRestriction(75), "levelCap 75 must not clear") && ok;
    ok = expect(!ShouldClearLevelRestriction(99), "levelCap 99 must not clear") && ok;
    ok = expect(!ShouldClearLevelRestriction(255), "levelCap 255 must not clear") && ok;

    // Dense compose over required poles + small grid: free == inline == pin.
    const uint8 poles[] = { 0, 1, 75, 99, 255 };
    for (const uint8 cap : poles)
    {
        const bool got  = ShouldClearLevelRestriction(cap);
        const bool want = cap == 0;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldClearLevelRestriction(cap), "compose free == inline") && ok;
    }
    for (uint8 cap = 0; cap <= 8; ++cap)
    {
        const bool got  = ShouldClearLevelRestriction(cap);
        const bool want = cap == 0;
        ok              = expect(got == want, "compose grid free == pin formula") && ok;
        ok              = expect(got == inlineShouldClearLevelRestriction(cap), "compose grid free == inline") && ok;
    }

    // Explicit polarity: always clear when zero; never clear when nonzero.
    for (const uint8 cap : poles)
    {
        const bool got = ShouldClearLevelRestriction(cap);
        ok             = expect(!(cap == 0 && !got), "polarity: levelCap 0 must clear") && ok;
        ok             = expect(!(cap > 0 && got), "polarity: nonzero levelCap must not clear") && ok;
    }

    // Host-style compose: ApplyLevelRestrictions injects GetLevelCap().
    const struct
    {
        uint8       levelCap;
        bool        want;
        const char* label;
    } hostCases[] = {
        { 0, true, "uncapped battlefield → clear LevelRestriction path" },
        { 1, false, "unit cap → apply path (not clear-only)" },
        { 75, false, "classic BCNM 75 → apply path" },
        { 99, false, "era max 99 → apply path" },
        { 255, false, "uint8 max → apply path" },
    };

    for (const auto& c : hostCases)
    {
        const bool got = ShouldClearLevelRestriction(c.levelCap);
        ok             = expect(got == c.want, c.label) && ok;
        ok             = expect(got == inlineShouldClearLevelRestriction(c.levelCap), "host free == inline") && ok;
        ok             = expect(got == (c.levelCap == 0), "host free == pin levelCap == 0") && ok;
    }

    // Production ApplyLevelRestrictions path semantics.
    ok = expect(ShouldClearLevelRestriction(0), "ApplyLevelRestrictions uncapped → clear path") && ok;
    ok = expect(!ShouldClearLevelRestriction(75), "ApplyLevelRestrictions capped → not clear-only") && ok;

    // Logical inverse of sibling dual-wire ShouldApplyLevelCap (3059; left alone).
    ok = expect(ShouldApplyLevelCap(75) && !ShouldApplyLevelCap(0),
                "sibling ShouldApplyLevelCap residual still holds") &&
         ok;
    for (const uint8 cap : poles)
    {
        const bool clear = ShouldClearLevelRestriction(cap);
        const bool apply = ShouldApplyLevelCap(cap);
        ok               = expect(clear != apply, "compose: clear and apply must be inverse") && ok;
        ok               = expect(clear == !apply, "compose: clear == !apply") && ok;
        ok               = expect(clear == (cap == 0), "compose: clear == (levelCap==0)") && ok;
        ok               = expect(apply == (cap > 0), "compose: apply == (levelCap>0)") && ok;
    }

    // Sibling dual-wire ShouldAddSjRestriction (3087; left alone) is orthogonal.
    ok = expect(ShouldAddSjRestriction(0x00) && !ShouldAddSjRestriction(RulesAllowSubjobs),
                "sibling ShouldAddSjRestriction residual still holds") &&
         ok;
    ok = expect(ShouldClearLevelRestriction(0) && ShouldAddSjRestriction(0x00),
                "compose: uncapped + no ALLOW_SUBJOBS clear and SJ gates") &&
         ok;
    ok = expect(ShouldClearLevelRestriction(0) && !ShouldAddSjRestriction(RulesAllowSubjobs),
                "compose: uncapped + ALLOW_SUBJOBS only clear gate") &&
         ok;
    ok = expect(!ShouldClearLevelRestriction(75) && ShouldAddSjRestriction(0x00),
                "compose: capped + no ALLOW_SUBJOBS only SJ gate") &&
         ok;
    ok = expect(!ShouldClearLevelRestriction(75) && !ShouldAddSjRestriction(RulesAllowSubjobs),
                "compose: capped + ALLOW_SUBJOBS neither clear nor SJ") &&
         ok;

    // Sibling residual ResolveLevelCap (1361) is orthogonal.
    ok = expect(ResolveLevelCap(75, 0, true, false, 99) == 75, "residual ResolveLevelCap plain") && ok;
    ok = expect(ResolveLevelCap(75, 5, true, false, 99) == 80, "residual ResolveLevelCap tweak") && ok;
    ok = expect(ResolveLevelCap(75, 0, false, true, 99) == 99, "residual ResolveLevelCap mission uncap") && ok;
    ok = expect(ResolveLevelCap(0, 0, true, false, 99) == 0, "residual ResolveLevelCap(0) returns 0") && ok;
    ok = expect(ShouldClearLevelRestriction(0), "compose: zero gate takes clear path before ResolveLevelCap") && ok;

    for (const uint8 cap : { uint8{ 1 }, uint8{ 75 }, uint8{ 99 }, uint8{ 255 } })
    {
        ok = expect(!ShouldClearLevelRestriction(cap), "compose: nonzero opens ResolveLevelCap path") && ok;
        ok = expect(ResolveLevelCap(cap, 0, true, false, 99) == cap,
                    "compose: ResolveLevelCap plain returns raw") &&
             ok;
    }

    // Explicit dual-wire poles: free == inline == levelCap == 0 for required set.
    for (const uint8 cap : poles)
    {
        const bool got = ShouldClearLevelRestriction(cap);
        ok             = expect(got == (cap == 0), "host inject dual-wire identity") && ok;
        ok             = expect(got == inlineShouldClearLevelRestriction(cap), "host inject free == inline") && ok;
    }

    // Residual independence: clear-level gate is distinct from insert gates.
    ok = expect(ShouldRejectNullInsert(true) && !ShouldRejectNullInsert(false),
                "null-insert residual still holds under dual-wire") &&
         ok;
    ok = expect(ShouldRejectAlreadyInBattlefield(true) && !ShouldRejectAlreadyInBattlefield(false),
                "already-in residual still holds under dual-wire") &&
         ok;
    ok = expect(ShouldAcceptPCUnderCapacity(2, 6) && !ShouldAcceptPCUnderCapacity(6, 6),
                "PC under-capacity residual still holds under dual-wire") &&
         ok;
    ok = expect(ShouldEnterPC(true) && !ShouldEnterPC(false),
                "enter residual still holds under dual-wire") &&
         ok;
    ok = expect(ShouldRegisterPC(false, false) && !ShouldRegisterPC(false, true),
                "register residual still holds under dual-wire") &&
         ok;

    return ok;
}
