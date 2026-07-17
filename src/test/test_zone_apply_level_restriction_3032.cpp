#include "test_zone_apply_level_restriction_3032.h"

#include "map/zone_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zone ShouldApplyZoneLevelRestriction 3032 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline updateCharLevelRestriction apply formula for dual-wire
// cross-check (slice 3032):
//   zoneLevelRestriction != 0
auto inlineShouldApplyZoneLevelRestriction(const uint8 zoneLevelRestriction) -> bool
{
    return zoneLevelRestriction != 0;
}

} // namespace

// Pure dual-wire expansion for zonehelpers::ShouldApplyZoneLevelRestriction
// (zoneLevelRestriction != 0 on updateCharLevelRestriction; slice 3032).
// Edges: 0, 1, 75, 255.
auto runZoneApplyLevelRestriction3032SelfTests() -> bool
{
    using zonehelpers::ShouldApplyZoneLevelRestriction;

    bool ok = true;

    // Residual 1363 pins still hold under dual-wire.
    ok = expect(ShouldApplyZoneLevelRestriction(50), "residual non-zero cap → apply") && ok;
    ok = expect(!ShouldApplyZoneLevelRestriction(0), "residual zero cap → no apply") && ok;

    const struct
    {
        uint8       zoneLevelRestriction;
        bool        want;
        const char* label;
    } cases[] = {
        // Required dual-wire edges (0, 1, 75, 255).
        { 0, false, "edge 0 → no apply" },
        { 1, true, "edge 1 → apply" },
        { 75, true, "edge 75 → apply" },
        { 255, true, "edge 255 → apply" },

        // Residual 1363 pins.
        { 50, true, "residual apply cap" },
        { 0, false, "residual no cap" },

        // Additional poles for dual-wire stability.
        { 2, true, "small non-zero → apply" },
        { 99, true, "mid cap → apply" },
        { 100, true, "round cap → apply" },
        { 127, true, "mid uint8 → apply" },
        { 128, true, "high half → apply" },
        { 254, true, "near max → apply" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldApplyZoneLevelRestriction(c.zoneLevelRestriction);
        const bool inlineF = inlineShouldApplyZoneLevelRestriction(c.zoneLevelRestriction);
        const bool wantPin = c.zoneLevelRestriction != 0;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldApplyZoneLevelRestriction dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldApplyZoneLevelRestriction == pin formula zoneLevelRestriction!=0") && ok;
    }

    // Pin composition: apply iff restriction != 0.
    ok = expect(!ShouldApplyZoneLevelRestriction(0), "zoneLevelRestriction 0 must not apply") && ok;
    ok = expect(ShouldApplyZoneLevelRestriction(1), "zoneLevelRestriction 1 must apply") && ok;
    ok = expect(ShouldApplyZoneLevelRestriction(75), "zoneLevelRestriction 75 must apply") && ok;
    ok = expect(ShouldApplyZoneLevelRestriction(255), "zoneLevelRestriction 255 must apply") && ok;

    // Explicit polarity: apply is exact != 0 of inject value.
    for (const uint8 restriction : { uint8{ 0 }, uint8{ 1 }, uint8{ 75 }, uint8{ 255 } })
    {
        const bool got  = ShouldApplyZoneLevelRestriction(restriction);
        const bool want = restriction != 0;
        ok              = expect(got == want, "polarity: apply == (restriction != 0)") && ok;
        ok              = expect(!(got && restriction == 0), "polarity: never apply when restriction 0") && ok;
        ok              = expect(!(!got && restriction != 0), "polarity: always apply when restriction != 0") && ok;
    }

    // Host-style inject poles: m_levelRestriction in updateCharLevelRestriction.
    for (const uint8 restriction : { uint8{ 0 }, uint8{ 1 }, uint8{ 50 }, uint8{ 75 }, uint8{ 99 }, uint8{ 255 } })
    {
        const bool apply = ShouldApplyZoneLevelRestriction(restriction);
        const bool want  = restriction != 0;
        ok               = expect(apply == want, "host inject dual-wire identity") && ok;
        ok               = expect(apply == inlineShouldApplyZoneLevelRestriction(restriction),
                    "host inject free == inline") &&
             ok;
        // apply only when restriction non-zero.
        ok = expect(!(apply && restriction == 0), "never apply when m_levelRestriction == 0") && ok;
        ok = expect(!(!apply && restriction != 0), "non-zero m_levelRestriction must apply") && ok;
    }

    // Production updateCharLevelRestriction path semantics (after optional
    // skip/delete of existing LevelRestriction):
    // zoneLevelRestriction!=0 → strip buffs + AddStatusEffect LevelRestriction;
    // zoneLevelRestriction==0 → no apply.
    ok = expect(!ShouldApplyZoneLevelRestriction(0), "updateCharLevelRestriction zero cap → no-apply path") && ok;
    ok = expect(ShouldApplyZoneLevelRestriction(75), "updateCharLevelRestriction non-zero cap → apply path") && ok;

    // Dense edge compose: free == inline == pin for required edges.
    for (const uint8 restriction : { uint8{ 0 }, uint8{ 1 }, uint8{ 75 }, uint8{ 255 } })
    {
        const bool got  = ShouldApplyZoneLevelRestriction(restriction);
        const bool want = restriction != 0;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldApplyZoneLevelRestriction(restriction),
                    "compose free == inline") &&
             ok;
    }

    return ok;
}
