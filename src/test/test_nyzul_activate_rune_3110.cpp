#include "test_nyzul_activate_rune_3110.h"

#include "map/nyzul_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "nyzul activate rune 3110 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua activateRuneOfTransfer status gate for dual-wire checks:
//   GetNPCByID(runeID, instance):getStatus() == xi.status.NORMAL
auto inlineShouldActivateRuneOfTransfer(const uint8 status) -> bool
{
    return status == nyzulhelpers::kStatusNormal;
}

} // namespace

// Pure dual-wire expansion for nyzulhelpers::ShouldActivateRuneOfTransfer
// (Lua activateRuneOfTransfer status == NORMAL gate; slice 3110).
// Required poles:
//   - StatusNormal → true
//   - other statuses → false
//   - free == inline
auto runNyzulActivateRune3110SelfTests() -> bool
{
    using nyzulhelpers::ShouldActivateRuneOfTransfer;
    using nyzulhelpers::kStatusDisappear;
    using nyzulhelpers::kStatusNormal;

    bool ok = true;

    // Status catalog pins (match Go StatusNormal / StatusDisappear).
    ok = expect(kStatusNormal == 0, "StatusNormal pin") && ok;
    ok = expect(kStatusDisappear == 2, "StatusDisappear pin") && ok;

    // Residual 1088 / prior dual-wire 2913 pins still hold under 3110.
    ok = expect(ShouldActivateRuneOfTransfer(kStatusNormal), "NORMAL should activate") && ok;
    ok = expect(!ShouldActivateRuneOfTransfer(kStatusDisappear), "DISAPPEAR should not activate") && ok;
    ok = expect(ShouldActivateRuneOfTransfer(0), "literal NORMAL 0 should activate") && ok;
    ok = expect(!ShouldActivateRuneOfTransfer(1), "status 1 should not activate") && ok;
    ok = expect(!ShouldActivateRuneOfTransfer(2), "literal DISAPPEAR 2 should not activate") && ok;

    // StatusNormal → true.
    ok = expect(ShouldActivateRuneOfTransfer(kStatusNormal), "StatusNormal → true") && ok;
    ok = expect(inlineShouldActivateRuneOfTransfer(kStatusNormal), "inline StatusNormal → true") && ok;
    ok = expect(ShouldActivateRuneOfTransfer(0), "literal NORMAL 0 → true") && ok;

    // Other statuses → false.
    const uint8 otherStatuses[] = { 1, 2, kStatusDisappear, 3, 99, 255 };
    for (const uint8 status : otherStatuses)
    {
        if (status == kStatusNormal)
        {
            continue;
        }
        ok = expect(!ShouldActivateRuneOfTransfer(status), "other status → false") && ok;
        ok = expect(!inlineShouldActivateRuneOfTransfer(status), "inline other status → false") && ok;
    }

    // --- ShouldActivateRuneOfTransfer table ---
    const struct
    {
        uint8       status;
        bool        want;
        const char* label;
    } cases[] = {
        // Required dual-wire poles.
        { kStatusNormal, true, "StatusNormal → true" },
        { 0, true, "literal NORMAL 0 → true" },
        { kStatusDisappear, false, "StatusDisappear → false" },
        { 2, false, "literal DISAPPEAR 2 → false" },
        { 1, false, "status 1 → false" },
        { 3, false, "status 3 → false" },
        { 99, false, "status 99 → false" },
        { 255, false, "status max uint8 → false" },

        // Residual 1088 / 2913 re-pins.
        { kStatusNormal, true, "residual NORMAL activates" },
        { kStatusDisappear, false, "residual DISAPPEAR blocks" },
        { 1, false, "residual status 1 blocks" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldActivateRuneOfTransfer(c.status);
        const bool inlineC = inlineShouldActivateRuneOfTransfer(c.status);
        const bool compose = c.status == kStatusNormal;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineC, "dual-wire free == inline Lua formula") && ok;
        ok = expect(got == compose, "dual-wire free == status == StatusNormal") && ok;
    }

    // Host compose: dense free == inline == status == StatusNormal.
    const uint8 composeStatuses[] = { 0, 1, 2, kStatusNormal, kStatusDisappear, 3, 99, 255 };
    for (const uint8 status : composeStatuses)
    {
        const bool got     = ShouldActivateRuneOfTransfer(status);
        const bool inlineC = inlineShouldActivateRuneOfTransfer(status);
        const bool want    = status == kStatusNormal;

        ok = expect(got == want, "compose free == status == StatusNormal") && ok;
        ok = expect(got == inlineC, "compose dual-wire free == inline") && ok;
    }

    return ok;
}
