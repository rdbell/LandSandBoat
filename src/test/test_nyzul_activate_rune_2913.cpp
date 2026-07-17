#include "test_nyzul_activate_rune_2913.h"

#include "map/nyzul_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "nyzul activate rune 2913 self-test failed: " << label << '\n';
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
// (Lua activateRuneOfTransfer status == NORMAL gate; slice 2913).
auto runNyzulActivateRune2913SelfTests() -> bool
{
    using nyzulhelpers::ShouldActivateRuneOfTransfer;
    using nyzulhelpers::kStatusDisappear;
    using nyzulhelpers::kStatusNormal;

    bool ok = true;

    // Status catalog pins (match Go StatusNormal / StatusDisappear).
    ok = expect(kStatusNormal == 0, "StatusNormal pin") && ok;
    ok = expect(kStatusDisappear == 2, "StatusDisappear pin") && ok;

    // Residual 1088 ShouldActivateRuneOfTransfer pins.
    ok = expect(ShouldActivateRuneOfTransfer(kStatusNormal), "NORMAL should activate") && ok;
    ok = expect(!ShouldActivateRuneOfTransfer(kStatusDisappear), "DISAPPEAR should not activate") && ok;
    ok = expect(ShouldActivateRuneOfTransfer(0), "literal NORMAL 0 should activate") && ok;
    ok = expect(!ShouldActivateRuneOfTransfer(1), "status 1 should not activate") && ok;
    ok = expect(!ShouldActivateRuneOfTransfer(2), "literal DISAPPEAR 2 should not activate") && ok;

    // --- ShouldActivateRuneOfTransfer table ---
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
        const bool got     = ShouldActivateRuneOfTransfer(c.status);
        const bool inlineC = inlineShouldActivateRuneOfTransfer(c.status);
        const bool compose = c.status == kStatusNormal;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineC, "dual-wire free == inline Lua formula") && ok;
        ok = expect(got == compose, "dual-wire free == status == StatusNormal") && ok;
    }

    // Host compose: only status == NORMAL activates; DISAPPEAR and other
    // statuses skip (loop continues / no animationSub write).
    const struct
    {
        uint8       status;
        bool        want;
        const char* label;
    } composeCases[] = {
        { kStatusNormal, true, "compose NORMAL activates" },
        { kStatusDisappear, false, "compose DISAPPEAR blocks" },
        { 1, false, "compose status 1 blocks" },
        { 0, true, "compose literal 0 activates" },
        { 2, false, "compose literal 2 blocks" },
        { 99, false, "compose status 99 blocks" },
        { 255, false, "compose status 255 blocks" },
    };

    for (const auto& c : composeCases)
    {
        const bool got     = ShouldActivateRuneOfTransfer(c.status);
        const bool inlineC = inlineShouldActivateRuneOfTransfer(c.status);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineC, "compose dual-wire free == inline") && ok;
    }

    return ok;
}
