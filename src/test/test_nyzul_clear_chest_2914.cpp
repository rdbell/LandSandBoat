#include "test_nyzul_clear_chest_2914.h"

#include "map/nyzul_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "nyzul clear chest 2914 self-test failed: " << label << '\n';
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
// (Lua clearChests present + not-DISAPPEAR gate; slice 2914).
auto runNyzulClearChest2914SelfTests() -> bool
{
    using nyzulhelpers::ShouldClearChestNPC;
    using nyzulhelpers::kStatusDisappear;
    using nyzulhelpers::kStatusNormal;

    bool ok = true;

    // Status catalog pins (match Go StatusNormal / StatusDisappear).
    ok = expect(kStatusNormal == 0, "StatusNormal pin") && ok;
    ok = expect(kStatusDisappear == 2, "StatusDisappear pin") && ok;

    // Residual 1088 ShouldClearChestNPC pins.
    ok = expect(ShouldClearChestNPC(true, kStatusNormal), "present NORMAL should clear") && ok;
    ok = expect(!ShouldClearChestNPC(true, kStatusDisappear), "present DISAPPEAR should not clear") && ok;
    ok = expect(!ShouldClearChestNPC(false, kStatusNormal), "absent NORMAL should not clear") && ok;
    ok = expect(!ShouldClearChestNPC(false, kStatusDisappear), "absent DISAPPEAR should not clear") && ok;
    ok = expect(ShouldClearChestNPC(true, 0), "present literal NORMAL 0 should clear") && ok;
    ok = expect(!ShouldClearChestNPC(true, 2), "present literal DISAPPEAR 2 should not clear") && ok;

    // --- ShouldClearChestNPC table ---
    const struct
    {
        bool        present;
        uint8       status;
        bool        want;
        const char* label;
    } cases[] = {
        { true, kStatusNormal, true, "present StatusNormal pin" },
        { true, 0, true, "present literal NORMAL 0" },
        { true, kStatusDisappear, false, "present StatusDisappear pin" },
        { true, 2, false, "present literal DISAPPEAR 2" },
        { true, 1, true, "present status 1" },
        { true, 3, true, "present status 3" },
        { true, 255, true, "present status max uint8" },
        { false, kStatusNormal, false, "absent StatusNormal" },
        { false, kStatusDisappear, false, "absent StatusDisappear" },
        { false, 1, false, "absent status 1" },
        { false, 0, false, "absent literal 0" },
        { false, 255, false, "absent status max" },
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

    // Host compose: present && status != DISAPPEAR clears; absent or
    // DISAPPEAR skip (no setStatus / setAnimationSub / resetLocalVars).
    const struct
    {
        bool        present;
        uint8       status;
        bool        want;
        const char* label;
    } composeCases[] = {
        { true, kStatusNormal, true, "compose present NORMAL clears" },
        { true, kStatusDisappear, false, "compose present DISAPPEAR blocks" },
        { false, kStatusNormal, false, "compose absent NORMAL blocks" },
        { false, kStatusDisappear, false, "compose absent DISAPPEAR blocks" },
        { true, 1, true, "compose present status 1 clears" },
        { true, 0, true, "compose present literal 0 clears" },
        { true, 2, false, "compose present literal 2 blocks" },
        { true, 99, true, "compose present status 99 clears" },
        { true, 255, true, "compose present status 255 clears" },
        { false, 255, false, "compose absent status 255 blocks" },
    };

    for (const auto& c : composeCases)
    {
        const bool got     = ShouldClearChestNPC(c.present, c.status);
        const bool inlineC = inlineShouldClearChestNPC(c.present, c.status);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineC, "compose dual-wire free == inline") && ok;
    }

    return ok;
}
