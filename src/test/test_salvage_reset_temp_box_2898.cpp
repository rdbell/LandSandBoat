#include "test_salvage_reset_temp_box_2898.h"

#include "map/salvage_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "salvage reset temp box 2898 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua resetTempBoxes pure status gate for dual-wire cross-check:
//   casket:getStatus() == xi.status.NORMAL
auto inlineShouldResetTempBox(const uint8 status) -> bool
{
    return status == salvagehelpers::kStatusNormal;
}

} // namespace

// Pure dual-wire expansion for salvagehelpers::ShouldResetTempBox
// (Lua resetTempBoxes status == NORMAL gate; slice 2898).
auto runSalvageResetTempBox2898SelfTests() -> bool
{
    using salvagehelpers::ShouldResetTempBox;
    using salvagehelpers::kStatusDisappear;
    using salvagehelpers::kStatusNormal;

    bool ok = true;

    // Status catalog pins (match Go StatusNormal / StatusDisappear).
    ok = expect(kStatusNormal == 0, "StatusNormal pin") && ok;
    ok = expect(kStatusDisappear == 2, "StatusDisappear pin") && ok;

    // Residual 1083 ShouldResetTempBox pins.
    ok = expect(ShouldResetTempBox(kStatusNormal), "NORMAL should reset") && ok;
    ok = expect(!ShouldResetTempBox(kStatusDisappear), "DISAPPEAR should not reset") && ok;
    ok = expect(!ShouldResetTempBox(1), "status 1 should not reset") && ok;
    ok = expect(ShouldResetTempBox(0), "literal NORMAL 0 should reset") && ok;

    // --- ShouldResetTempBox table ---
    const struct
    {
        uint8       status;
        bool        want;
        const char* label;
    } cases[] = {
        { kStatusNormal, true, "StatusNormal pin" },
        { 0, true, "literal NORMAL 0" },
        { kStatusDisappear, false, "StatusDisappear pin" },
        { 2, false, "literal DISAPPEAR 2" },
        { 1, false, "status 1" },
        { 3, false, "status 3" },
        { 255, false, "status max uint8" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldResetTempBox(c.status);
        const bool inlineC = inlineShouldResetTempBox(c.status);
        const bool compose = c.status == kStatusNormal;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineC, "dual-wire ShouldResetTempBox == inline Lua") && ok;
        ok = expect(got == compose, "ShouldResetTempBox == status == StatusNormal") && ok;
    }

    // Host compose: only status == NORMAL succeeds; DISAPPEAR and other
    // statuses leave the casket untouched.
    const struct
    {
        uint8       status;
        bool        want;
        const char* label;
    } composeCases[] = {
        { kStatusNormal, true, "compose NORMAL eligible" },
        { kStatusDisappear, false, "compose DISAPPEAR blocks" },
        { 1, false, "compose status 1 blocks" },
        { 0, true, "compose literal 0 eligible" },
        { 99, false, "compose status 99 blocks" },
    };

    for (const auto& c : composeCases)
    {
        const bool got     = ShouldResetTempBox(c.status);
        const bool inlineC = inlineShouldResetTempBox(c.status);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineC, "compose dual-wire free == inline") && ok;
    }

    return ok;
}
