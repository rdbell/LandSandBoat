#include "test_battlefield_apply_level_cap_3059.h"

#include "map/battlefield_capacity.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "battlefield ShouldApplyLevelCap 3059 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline ApplyLevelRestrictions level-cap gate formula for dual-wire
// cross-check (slice 3059):
//   levelCap > 0
auto inlineShouldApplyLevelCap(const uint8 levelCap) -> bool
{
    return levelCap > 0;
}

} // namespace

// Pure dual-wire expansion for battlefieldhelpers::ShouldApplyLevelCap
// (levelCap > 0 on ApplyLevelRestrictions; slice 3059).
// Pins levelCap 0/1/75/99/255; free == inline levelCap > 0.
auto runBattlefieldApplyLevelCap3059SelfTests() -> bool
{
    using battlefieldhelpers::ResolveLevelCap;
    using battlefieldhelpers::ShouldAcceptPCUnderCapacity;
    using battlefieldhelpers::ShouldApplyLevelCap;
    using battlefieldhelpers::ShouldEnterPC;
    using battlefieldhelpers::ShouldRegisterPC;
    using battlefieldhelpers::ShouldRejectAlreadyInBattlefield;
    using battlefieldhelpers::ShouldRejectNullInsert;

    bool ok = true;

    // Residual 1361 pins still hold under dual-wire.
    ok = expect(ShouldApplyLevelCap(75), "residual: levelCap 75 applies cap") && ok;
    ok = expect(!ShouldApplyLevelCap(0), "residual: levelCap 0 does not apply cap") && ok;

    const struct
    {
        uint8       levelCap;
        bool        want;
        const char* label;
    } cases[] = {
        // Required dual-wire poles (task: 0/1/75/99/255).
        { 0, false, "levelCap 0 uncapped clears restriction" },
        { 1, true, "levelCap 1 applies cap" },
        { 75, true, "levelCap 75 classic BCNM cap" },
        { 99, true, "levelCap 99 max era cap" },
        { 255, true, "levelCap 255 uint8 max applies" },

        // Residual 1361 pins.
        { 75, true, "residual ShouldApplyLevelCap(75)" },
        { 0, false, "residual ShouldApplyLevelCap(0)" },

        // Additional polarity / boundary pins.
        { 2, true, "levelCap 2 applies" },
        { 50, true, "levelCap 50 applies" },
        { 100, true, "levelCap 100 applies" },
        { 254, true, "levelCap 254 applies" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldApplyLevelCap(c.levelCap);
        const bool inlineF = inlineShouldApplyLevelCap(c.levelCap);
        const bool wantPin = c.levelCap > 0;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldApplyLevelCap dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldApplyLevelCap == pin formula levelCap > 0") && ok;
    }

    // Pin composition: apply iff levelCap > 0.
    ok = expect(!ShouldApplyLevelCap(0), "levelCap 0 must not apply") && ok;
    ok = expect(ShouldApplyLevelCap(1), "levelCap 1 must apply") && ok;
    ok = expect(ShouldApplyLevelCap(75), "levelCap 75 must apply") && ok;
    ok = expect(ShouldApplyLevelCap(99), "levelCap 99 must apply") && ok;
    ok = expect(ShouldApplyLevelCap(255), "levelCap 255 must apply") && ok;

    // Dense compose over required poles + small grid: free == inline == pin.
    const uint8 poles[] = { 0, 1, 75, 99, 255 };
    for (const uint8 cap : poles)
    {
        const bool got  = ShouldApplyLevelCap(cap);
        const bool want = cap > 0;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldApplyLevelCap(cap), "compose free == inline") && ok;
    }
    for (uint8 cap = 0; cap <= 8; ++cap)
    {
        const bool got  = ShouldApplyLevelCap(cap);
        const bool want = cap > 0;
        ok              = expect(got == want, "compose grid free == pin formula") && ok;
        ok              = expect(got == inlineShouldApplyLevelCap(cap), "compose grid free == inline") && ok;
    }

    // Explicit polarity: never apply when zero; always apply when nonzero.
    for (const uint8 cap : poles)
    {
        const bool got = ShouldApplyLevelCap(cap);
        ok             = expect(!(cap == 0 && got), "polarity: levelCap 0 must not apply") && ok;
        ok             = expect(!(cap > 0 && !got), "polarity: nonzero levelCap must apply") && ok;
    }

    // Host-style compose: ApplyLevelRestrictions injects GetLevelCap().
    const struct
    {
        uint8       levelCap;
        bool        want;
        const char* label;
    } hostCases[] = {
        { 0, false, "uncapped battlefield → clear LevelRestriction path" },
        { 1, true, "unit cap → apply LevelRestriction path" },
        { 75, true, "classic BCNM 75 → apply path" },
        { 99, true, "era max 99 → apply path" },
        { 255, true, "uint8 max → apply path" },
    };

    for (const auto& c : hostCases)
    {
        const bool got = ShouldApplyLevelCap(c.levelCap);
        ok             = expect(got == c.want, c.label) && ok;
        ok             = expect(got == inlineShouldApplyLevelCap(c.levelCap), "host free == inline") && ok;
        ok             = expect(got == (c.levelCap > 0), "host free == pin levelCap > 0") && ok;
    }

    // Production ApplyLevelRestrictions path semantics.
    ok = expect(ShouldApplyLevelCap(75), "ApplyLevelRestrictions capped → apply path") && ok;
    ok = expect(!ShouldApplyLevelCap(0), "ApplyLevelRestrictions uncapped → clear path") && ok;

    // Sibling residual ResolveLevelCap (1361) is orthogonal: only meaningful
    // when ShouldApplyLevelCap is true; free gate does not compute the cap.
    ok = expect(ResolveLevelCap(75, 0, true, false, 99) == 75, "residual ResolveLevelCap plain") && ok;
    ok = expect(ResolveLevelCap(75, 5, true, false, 99) == 80, "residual ResolveLevelCap tweak") && ok;
    ok = expect(ResolveLevelCap(75, 0, false, true, 99) == 99, "residual ResolveLevelCap mission uncap") && ok;
    ok = expect(ResolveLevelCap(0, 0, true, false, 99) == 0, "residual ResolveLevelCap(0) returns 0") && ok;
    ok = expect(!ShouldApplyLevelCap(0), "compose: zero gate declines before ResolveLevelCap") && ok;

    for (const uint8 cap : { uint8{ 1 }, uint8{ 75 }, uint8{ 99 }, uint8{ 255 } })
    {
        ok = expect(ShouldApplyLevelCap(cap), "compose: nonzero opens ResolveLevelCap path") && ok;
        ok = expect(ResolveLevelCap(cap, 0, true, false, 99) == cap,
                    "compose: ResolveLevelCap plain returns raw") &&
             ok;
    }

    // Explicit dual-wire poles: free == inline == levelCap > 0 for required set.
    for (const uint8 cap : poles)
    {
        const bool got = ShouldApplyLevelCap(cap);
        ok             = expect(got == (cap > 0), "host inject dual-wire identity") && ok;
        ok             = expect(got == inlineShouldApplyLevelCap(cap), "host inject free == inline") && ok;
    }

    // Residual independence: level-cap gate is distinct from insert gates.
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
