#include "test_zone_apply_level_restriction_3177.h"

#include "map/zone_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zone ShouldApplyZoneLevelRestriction 3177 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline updateCharLevelRestriction apply formula for dual-wire
// cross-check (dedicated slice 3177):
//   zoneLevelRestriction != 0
auto inlineShouldApplyZoneLevelRestriction(const uint8 zoneLevelRestriction) -> bool
{
    return zoneLevelRestriction != 0;
}

// Compact dual-wire pin matching Go pinShouldApplyZoneLevelRestriction3177 /
// C++ capacity:
//   zoneLevelRestriction != 0
auto pinShouldApplyZoneLevelRestriction(const uint8 zoneLevelRestriction) -> bool
{
    return zoneLevelRestriction != 0;
}

} // namespace

// Pure dual-wire expansion for zonehelpers::ShouldApplyZoneLevelRestriction
// (zoneLevelRestriction != 0 on updateCharLevelRestriction; dedicated slice
// 3177 expand residual 3032 / pure 1363).
//
// Coverage:
//   - free == inline == pin == (zoneLevelRestriction != 0)
//   - residual 1363 / 3032 pins still hold
//   - poles: 0, 1, 75, 255
auto runZoneApplyLevelRestriction3177SelfTests() -> bool
{
    using zonehelpers::ShouldApplyZoneLevelRestriction;

    bool ok = true;

    // Residual 1363 / 3032 pins still hold under dual-wire.
    ok = expect(ShouldApplyZoneLevelRestriction(50), "residual 3032 non-zero cap → apply") && ok;
    ok = expect(!ShouldApplyZoneLevelRestriction(0), "residual 3032 zero cap → no apply") && ok;
    ok = expect(ShouldApplyZoneLevelRestriction(1), "residual 3032 edge 1 → apply") && ok;
    ok = expect(ShouldApplyZoneLevelRestriction(75), "residual 3032 edge 75 → apply") && ok;
    ok = expect(ShouldApplyZoneLevelRestriction(255), "residual 3032 edge 255 → apply") && ok;

    const struct
    {
        uint8       zoneLevelRestriction;
        bool        want;
        const char* label;
    } cases[] = {
        // Required dual-wire poles (0, 1, 75, 255).
        { 0, false, "pole 0 → no apply" },
        { 1, true, "pole 1 → apply" },
        { 75, true, "pole 75 → apply" },
        { 255, true, "pole 255 → apply" },

        // Residual 3032 / 1363 pins.
        { 50, true, "residual 3032 apply cap" },
        { 0, false, "residual 3032 no cap" },

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
        const bool pinF    = pinShouldApplyZoneLevelRestriction(c.zoneLevelRestriction);
        const bool wantPin = c.zoneLevelRestriction != 0;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldApplyZoneLevelRestriction dual-wire == inline LSB formula") && ok;
        ok = expect(got == pinF, "ShouldApplyZoneLevelRestriction dual-wire == pin formula") && ok;
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
        ok = expect(apply == pinShouldApplyZoneLevelRestriction(restriction), "host inject free == pin") && ok;
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

    // Dense edge compose: free == inline == pin for required poles 0, 1, 75, 255.
    for (const uint8 restriction : { uint8{ 0 }, uint8{ 1 }, uint8{ 75 }, uint8{ 255 } })
    {
        const bool got  = ShouldApplyZoneLevelRestriction(restriction);
        const bool want = restriction != 0;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldApplyZoneLevelRestriction(restriction),
                    "compose free == inline") &&
             ok;
        ok = expect(got == pinShouldApplyZoneLevelRestriction(restriction), "compose free == pin") && ok;
    }

    return ok;
}
