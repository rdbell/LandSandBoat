#include "test_nyzul_clear_chest_npc_3061.h"

#include "map/nyzul_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "nyzul clear chest NPC 3061 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua clearChests status gate for dual-wire checks:
//   npc and npc:getStatus() ~= xi.status.DISAPPEAR
auto inlineShouldClearChestNPC(const bool present, const uint8 status) -> bool
{
    return present && status != nyzulhelpers::kStatusDisappear;
}

} // namespace

// Pure dual-wire expansion for nyzulhelpers::ShouldClearChestNPC
// (Lua clearChests present + not-DISAPPEAR gate; slice 3061).
// Required poles:
//   - present false → false for any status
//   - present true + StatusDisappear → false
//   - present true + other status → true
//   - free == inline
auto runNyzulClearChestNPC3061SelfTests() -> bool
{
    using nyzulhelpers::ShouldClearChestNPC;
    using nyzulhelpers::kStatusDisappear;
    using nyzulhelpers::kStatusNormal;

    bool ok = true;

    // Status catalog pins (match Go StatusNormal / StatusDisappear).
    ok = expect(kStatusNormal == 0, "StatusNormal pin") && ok;
    ok = expect(kStatusDisappear == 2, "StatusDisappear pin") && ok;

    // Residual 1088 / prior dual-wire 2914 pins still hold under 3061.
    ok = expect(ShouldClearChestNPC(true, kStatusNormal), "present NORMAL should clear") && ok;
    ok = expect(!ShouldClearChestNPC(true, kStatusDisappear), "present DISAPPEAR should not clear") && ok;
    ok = expect(!ShouldClearChestNPC(false, kStatusNormal), "absent NORMAL should not clear") && ok;
    ok = expect(!ShouldClearChestNPC(false, kStatusDisappear), "absent DISAPPEAR should not clear") && ok;
    ok = expect(ShouldClearChestNPC(true, 0), "present literal NORMAL 0 should clear") && ok;
    ok = expect(!ShouldClearChestNPC(true, 2), "present literal DISAPPEAR 2 should not clear") && ok;

    // present false → false any status.
    const uint8 anyStatuses[] = { 0, 1, 2, kStatusNormal, kStatusDisappear, 99, 255 };
    for (const uint8 status : anyStatuses)
    {
        ok = expect(!ShouldClearChestNPC(false, status), "present false → false any status") && ok;
        ok = expect(!inlineShouldClearChestNPC(false, status), "inline present false → false") && ok;
    }

    // present true + StatusDisappear → false.
    ok = expect(!ShouldClearChestNPC(true, kStatusDisappear), "present true + StatusDisappear → false") && ok;
    ok = expect(!ShouldClearChestNPC(true, 2), "present true + literal 2 → false") && ok;

    // present true + other status → true.
    const uint8 otherStatuses[] = { 0, 1, 3, kStatusNormal, 99, 255 };
    for (const uint8 status : otherStatuses)
    {
        if (status == kStatusDisappear)
        {
            continue;
        }
        ok = expect(ShouldClearChestNPC(true, status), "present true + other status → true") && ok;
    }

    // --- ShouldClearChestNPC table ---
    const struct
    {
        bool        present;
        uint8       status;
        bool        want;
        const char* label;
    } cases[] = {
        // Required dual-wire poles.
        { false, kStatusNormal, false, "present false + StatusNormal → false" },
        { false, kStatusDisappear, false, "present false + StatusDisappear → false" },
        { false, 0, false, "present false + 0 → false" },
        { false, 1, false, "present false + 1 → false" },
        { false, 255, false, "present false + 255 → false" },
        { true, kStatusDisappear, false, "present true + StatusDisappear → false" },
        { true, 2, false, "present true + literal DISAPPEAR 2 → false" },
        { true, kStatusNormal, true, "present true + StatusNormal → true" },
        { true, 0, true, "present true + literal NORMAL 0 → true" },
        { true, 1, true, "present true + status 1 → true" },
        { true, 3, true, "present true + status 3 → true" },
        { true, 255, true, "present true + status max → true" },

        // Residual 1088 re-pins.
        { true, kStatusNormal, true, "residual present NORMAL clears" },
        { true, kStatusDisappear, false, "residual present DISAPPEAR blocks" },
        { false, kStatusNormal, false, "residual absent NORMAL blocks" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldClearChestNPC(c.present, c.status);
        const bool inlineC = inlineShouldClearChestNPC(c.present, c.status);
        const bool compose = c.present && c.status != kStatusDisappear;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineC, "dual-wire free == inline Lua formula") && ok;
        ok = expect(got == compose, "dual-wire free == present && status != StatusDisappear") && ok;
    }

    // Host compose: dense free == inline == present && status != DISAPPEAR.
    const uint8 composeStatuses[] = { 0, 1, 2, kStatusNormal, kStatusDisappear, 3, 99, 255 };
    for (const bool present : { true, false })
    {
        for (const uint8 status : composeStatuses)
        {
            const bool got     = ShouldClearChestNPC(present, status);
            const bool inlineC = inlineShouldClearChestNPC(present, status);
            const bool want    = present && status != kStatusDisappear;

            ok = expect(got == want, "compose free == present && status != StatusDisappear") && ok;
            ok = expect(got == inlineC, "compose dual-wire free == inline") && ok;
        }
    }

    return ok;
}
