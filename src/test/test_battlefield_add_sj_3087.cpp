#include "test_battlefield_add_sj_3087.h"

#include "map/battlefield_capacity.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "battlefield ShouldAddSjRestriction 3087 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline ApplyLevelRestrictions SJ restriction gate formula for dual-wire
// cross-check (slice 3087):
//   (rules & RulesAllowSubjobs) == 0
auto inlineShouldAddSjRestriction(const uint16 rules) -> bool
{
    return (rules & battlefieldhelpers::RulesAllowSubjobs) == 0;
}

} // namespace

// Pure dual-wire expansion for battlefieldhelpers::ShouldAddSjRestriction
// ((rules & ALLOW_SUBJOBS) == 0 on ApplyLevelRestrictions; slice 3087).
// Pins rules 0 / ALLOW_SUBJOBS / LOSE_EXP / combined; free == inline pin.
auto runBattlefieldAddSj3087SelfTests() -> bool
{
    using battlefieldhelpers::RulesAllowSubjobs;
    using battlefieldhelpers::ResolveLevelCap;
    using battlefieldhelpers::ShouldAcceptPCUnderCapacity;
    using battlefieldhelpers::ShouldAddSjRestriction;
    using battlefieldhelpers::ShouldApplyLevelCap;
    using battlefieldhelpers::ShouldEnterPC;
    using battlefieldhelpers::ShouldRegisterPC;
    using battlefieldhelpers::ShouldRejectAlreadyInBattlefield;
    using battlefieldhelpers::ShouldRejectNullInsert;

    bool ok = true;

    // Residual 1361 pins still hold under dual-wire.
    ok = expect(ShouldAddSjRestriction(0x00), "residual: rules 0 adds SJ restriction") && ok;
    ok = expect(!ShouldAddSjRestriction(RulesAllowSubjobs), "residual: ALLOW_SUBJOBS does not restrict") && ok;

    constexpr uint16 loseExp = 0x02;

    const struct
    {
        uint16      rules;
        bool        want;
        const char* label;
    } cases[] = {
        // Required dual-wire poles.
        { 0x00, true, "rules 0 no ALLOW_SUBJOBS → restrict SJ" },
        { RulesAllowSubjobs, false, "ALLOW_SUBJOBS alone → no SJ restriction" },
        { loseExp, true, "LOSE_EXP alone (no ALLOW_SUBJOBS) → restrict SJ" },
        { static_cast<uint16>(RulesAllowSubjobs | loseExp), false, "ALLOW_SUBJOBS|LOSE_EXP → no SJ restriction" },

        // Residual 1361 pins.
        { 0x00, true, "residual ShouldAddSjRestriction(0)" },
        { RulesAllowSubjobs, false, "residual ShouldAddSjRestriction(ALLOW_SUBJOBS)" },

        // Additional polarity / boundary pins.
        { 0x00, true, "explicit zero bitset restricts" },
        { 0x01, false, "explicit 0x01 ALLOW_SUBJOBS clears" },
        { 0x02, true, "explicit 0x02 LOSE_EXP only restricts" },
        { 0x03, false, "explicit 0x03 both bits clear restriction" },
        { 0xFE, true, "high bits without ALLOW_SUBJOBS restrict" },
        { 0xFF, false, "all bits including ALLOW_SUBJOBS clear" },
        { 0x80, true, "high bit alone restricts" },
        { static_cast<uint16>(RulesAllowSubjobs | 0x80), false, "ALLOW_SUBJOBS with high bit clears" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldAddSjRestriction(c.rules);
        const bool inlineF = inlineShouldAddSjRestriction(c.rules);
        const bool wantPin = (c.rules & RulesAllowSubjobs) == 0;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldAddSjRestriction dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldAddSjRestriction == pin formula (rules & ALLOW_SUBJOBS) == 0") && ok;
    }

    // Pin composition: restrict iff ALLOW_SUBJOBS clear.
    ok = expect(ShouldAddSjRestriction(0x00), "rules 0 must restrict SJ") && ok;
    ok = expect(!ShouldAddSjRestriction(RulesAllowSubjobs), "ALLOW_SUBJOBS must not restrict SJ") && ok;
    ok = expect(ShouldAddSjRestriction(loseExp), "LOSE_EXP alone must restrict SJ") && ok;
    ok = expect(!ShouldAddSjRestriction(static_cast<uint16>(RulesAllowSubjobs | loseExp)),
                "ALLOW_SUBJOBS|LOSE_EXP must not restrict SJ") &&
         ok;

    // Dense compose over required poles + small grid: free == inline == pin.
    const uint16 poles[] = {
        0x00,
        RulesAllowSubjobs,
        loseExp,
        static_cast<uint16>(RulesAllowSubjobs | loseExp),
        0xFE,
        0xFF,
    };
    for (const uint16 rules : poles)
    {
        const bool got  = ShouldAddSjRestriction(rules);
        const bool want = (rules & RulesAllowSubjobs) == 0;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldAddSjRestriction(rules), "compose free == inline") && ok;
    }
    for (uint16 rules = 0; rules <= 0x0F; ++rules)
    {
        const bool got  = ShouldAddSjRestriction(rules);
        const bool want = (rules & RulesAllowSubjobs) == 0;
        ok              = expect(got == want, "compose grid free == pin formula") && ok;
        ok              = expect(got == inlineShouldAddSjRestriction(rules), "compose grid free == inline") && ok;
    }

    // Explicit polarity: restrict when bit clear; never restrict when bit set.
    for (const uint16 rules : poles)
    {
        const bool got = ShouldAddSjRestriction(rules);
        ok             = expect(!((rules & RulesAllowSubjobs) == 0 && !got), "polarity: bit clear must restrict") && ok;
        ok             = expect(!((rules & RulesAllowSubjobs) != 0 && got), "polarity: bit set must not restrict") && ok;
    }

    // Constant pin: RulesAllowSubjobs is 0x01 (RULES_ALLOW_SUBJOBS).
    ok = expect(RulesAllowSubjobs == 0x01, "RulesAllowSubjobs == 0x01") && ok;

    // Host-style compose: ApplyLevelRestrictions injects m_Rules.
    const struct
    {
        uint16      rules;
        bool        want;
        const char* label;
    } hostCases[] = {
        { 0x00, true, "no ALLOW_SUBJOBS → add SjRestriction path" },
        { RulesAllowSubjobs, false, "ALLOW_SUBJOBS → skip SjRestriction" },
        { loseExp, true, "LOSE_EXP only → add SjRestriction path" },
        { static_cast<uint16>(RulesAllowSubjobs | loseExp), false, "ALLOW_SUBJOBS with LOSE_EXP → skip" },
        { 0xFE, true, "all bits but ALLOW_SUBJOBS → add path" },
        { 0xFF, false, "all bits set → skip path" },
    };

    for (const auto& c : hostCases)
    {
        const bool got = ShouldAddSjRestriction(c.rules);
        ok             = expect(got == c.want, c.label) && ok;
        ok             = expect(got == inlineShouldAddSjRestriction(c.rules), "host free == inline") && ok;
        ok             = expect(got == ((c.rules & RulesAllowSubjobs) == 0), "host free == pin") && ok;
    }

    // Production ApplyLevelRestrictions path semantics.
    ok = expect(ShouldAddSjRestriction(0x00), "ApplyLevelRestrictions no ALLOW_SUBJOBS → add SjRestriction") && ok;
    ok = expect(!ShouldAddSjRestriction(RulesAllowSubjobs), "ApplyLevelRestrictions ALLOW_SUBJOBS → skip") && ok;

    // Sibling dual-wire ShouldApplyLevelCap (3059) is orthogonal: SJ gate does
    // not consult levelCap; level-cap gate does not consult rules.
    ok = expect(ShouldApplyLevelCap(75) && !ShouldApplyLevelCap(0),
                "sibling ShouldApplyLevelCap residual still holds") &&
         ok;
    // Both gates can fire independently in host.
    ok = expect(ShouldApplyLevelCap(75) && ShouldAddSjRestriction(0x00),
                "compose: capped + no ALLOW_SUBJOBS both gates") &&
         ok;
    ok = expect(ShouldApplyLevelCap(75) && !ShouldAddSjRestriction(RulesAllowSubjobs),
                "compose: capped + ALLOW_SUBJOBS only level gate") &&
         ok;
    ok = expect(!ShouldApplyLevelCap(0) && ShouldAddSjRestriction(0x00),
                "compose: uncapped + no ALLOW_SUBJOBS only SJ gate") &&
         ok;
    ok = expect(!ShouldApplyLevelCap(0) && !ShouldAddSjRestriction(RulesAllowSubjobs),
                "compose: uncapped + ALLOW_SUBJOBS neither gate") &&
         ok;

    // Sibling residual ResolveLevelCap (1361) still holds under dual-wire.
    ok = expect(ResolveLevelCap(75, 0, true, false, 99) == 75, "residual ResolveLevelCap plain") && ok;
    ok = expect(ResolveLevelCap(75, 5, true, false, 99) == 80, "residual ResolveLevelCap tweak") && ok;

    // Explicit dual-wire poles: free == inline == pin for required set.
    for (const uint16 rules : { uint16{ 0x00 }, RulesAllowSubjobs, loseExp, static_cast<uint16>(RulesAllowSubjobs | loseExp) })
    {
        const bool got = ShouldAddSjRestriction(rules);
        ok             = expect(got == ((rules & RulesAllowSubjobs) == 0), "host inject dual-wire identity") && ok;
        ok             = expect(got == inlineShouldAddSjRestriction(rules), "host inject free == inline") && ok;
    }

    // Residual independence: SJ gate is distinct from insert gates.
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
