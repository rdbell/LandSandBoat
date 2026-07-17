#include "test_mapwire_mark_current_key_3336.h"

#include "map/map_networking_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "mapwire ShouldMarkCurrentKeyDecryption 3336 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline recv_parse current-key mark formula for dual-wire cross-check
// (residual 2995 / dedicated 3336):
//   decryptCount == 0
auto inlineShouldMarkCurrentKeyDecryption(const int decryptCount) -> bool
{
    return decryptCount == 0;
}

// Compact dual-wire pin matching Go pinShouldMarkCurrentKeyDecryption3336:
//   decryptCount == 0
// Direct return (not if/else).
auto pinShouldMarkCurrentKeyDecryption(const int decryptCount) -> bool
{
    return decryptCount == 0;
}

} // namespace

// Pure dual-wire expansion for mapnetworkinghelpers::ShouldMarkCurrentKeyDecryption
// (decryptCount == 0; OmegaXI internal/mapwire; dedicated slice 3336;
// expand residual 2995 / pure 2711).
//
// Coverage:
//   - free == inline == pin == (decryptCount == 0)
//   - poles decryptCount -1 / 0 / 1 / 2
//   - residual 2711 / 2995 pins still hold
//   - dense compose required edges
//   - MapNetworking::recv_parse host inject path
auto runMapwireMarkCurrentKey3336SelfTests() -> bool
{
    using mapnetworkinghelpers::ShouldMarkCurrentKeyDecryption;

    bool ok = true;

    // Residual 2711 / 2995 pins still hold under dual-wire.
    ok = expect(ShouldMarkCurrentKeyDecryption(0), "residual zero must mark current key") && ok;
    ok = expect(!ShouldMarkCurrentKeyDecryption(1), "residual one must not mark current key") && ok;
    ok = expect(!ShouldMarkCurrentKeyDecryption(-1), "residual reject must not mark current key") && ok;

    // --- Eligible / blocked paths ---
    ok = expect(ShouldMarkCurrentKeyDecryption(0), "current-key success marks") && ok;
    ok = expect(!ShouldMarkCurrentKeyDecryption(1), "previous-key fallback does not mark") && ok;

    const struct
    {
        int         decryptCount;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles.
        { 0, true, "current-key success marks" },
        { 1, false, "previous-key zone fallback does not mark" },

        // Required dual-wire poles (-1, 0, 1, 2).
        { -1, false, "edge decryptCount -1" },
        { 0, true, "edge decryptCount 0" },
        { 1, false, "edge decryptCount 1" },
        { 2, false, "edge decryptCount 2" },

        // Residual 2995 re-pins.
        { 0, true, "residual 2995 current-key success" },
        { 1, false, "residual 2995 previous-key fallback" },
        { -1, false, "residual 2995 reject" },
        { 2, false, "residual 2995 out-of-band" },

        // Residual 2711 re-pins.
        { 0, true, "residual 2711 zero marks" },
        { 1, false, "residual 2711 one does not mark" },
        { -1, false, "residual 2711 reject does not mark" },

        // Identity / polarity repeats for dual-wire stability.
        { 0, true, "identity current key" },
        { 1, false, "identity previous key" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldMarkCurrentKeyDecryption(c.decryptCount);
        const bool inlineF = inlineShouldMarkCurrentKeyDecryption(c.decryptCount);
        const bool pinGot  = pinShouldMarkCurrentKeyDecryption(c.decryptCount);
        const bool wantPin = c.decryptCount == 0;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldMarkCurrentKeyDecryption dual-wire == inline LSB formula") && ok;
        ok = expect(got == pinGot, "ShouldMarkCurrentKeyDecryption dual-wire == pin") && ok;
        ok = expect(got == wantPin, "ShouldMarkCurrentKeyDecryption == pin formula decryptCount==0") && ok;
    }

    // Pin composition: only zero marks current key.
    ok = expect(ShouldMarkCurrentKeyDecryption(0), "decryptCount 0 must mark current key") && ok;
    ok = expect(!ShouldMarkCurrentKeyDecryption(1), "decryptCount 1 must not mark current key") && ok;
    ok = expect(!ShouldMarkCurrentKeyDecryption(-1), "decryptCount -1 must not mark current key") && ok;
    ok = expect(!ShouldMarkCurrentKeyDecryption(2), "decryptCount 2 must not mark current key") && ok;
    ok = expect(ShouldMarkCurrentKeyDecryption(0) == pinShouldMarkCurrentKeyDecryption(0), "free==pin 0") && ok;
    ok = expect(ShouldMarkCurrentKeyDecryption(1) == pinShouldMarkCurrentKeyDecryption(1), "free==pin 1") && ok;
    ok = expect(ShouldMarkCurrentKeyDecryption(-1) == pinShouldMarkCurrentKeyDecryption(-1), "free==pin -1") && ok;
    ok = expect(ShouldMarkCurrentKeyDecryption(2) == pinShouldMarkCurrentKeyDecryption(2), "free==pin 2") && ok;

    // Dense compose: required edges -1, 0, 1, 2 — free == pin == inline.
    for (const int decryptCount : { -1, 0, 1, 2 })
    {
        const bool got  = ShouldMarkCurrentKeyDecryption(decryptCount);
        const bool want = decryptCount == 0;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldMarkCurrentKeyDecryption(decryptCount), "compose free == inline") && ok;
        ok              = expect(got == pinShouldMarkCurrentKeyDecryption(decryptCount), "compose free == pin") && ok;
    }

    // Residual 2711 / 2995 still hold under dedicated suite.
    ok = expect(ShouldMarkCurrentKeyDecryption(0), "residual zero marks under 3336") && ok;
    ok = expect(!ShouldMarkCurrentKeyDecryption(1), "residual one does not mark under 3336") && ok;
    ok = expect(!ShouldMarkCurrentKeyDecryption(-1), "residual reject does not mark under 3336") && ok;
    ok = expect(!ShouldMarkCurrentKeyDecryption(2), "residual count-2 does not mark under 3336") && ok;

    // --- Production recv_parse path semantics ---
    // Host injects:
    //   decryptCount starts 0; UsePrevious increments to 1
    // if (ShouldMarkCurrentKeyDecryption(decryptCount)) hasDecryptedPacket = true
    ok = expect(ShouldMarkCurrentKeyDecryption(0), "recv_parse primary decrypt → mark hasDecryptedPacket path") && ok;
    ok = expect(!ShouldMarkCurrentKeyDecryption(1), "recv_parse previous-key fallback → skip mark path") && ok;

    // Host-style inject polarity: mark only on current-key success (count 0).
    const bool markPrimary  = ShouldMarkCurrentKeyDecryption(0);
    const bool skipPrevious = !ShouldMarkCurrentKeyDecryption(1);
    ok                      = expect(markPrimary && skipPrevious, "recv_parse mark/skip polarity") && ok;
    ok                      = expect(markPrimary == pinShouldMarkCurrentKeyDecryption(0),
                "primary free == pin") &&
         ok;
    ok = expect(ShouldMarkCurrentKeyDecryption(1) == pinShouldMarkCurrentKeyDecryption(1),
                "previous free == pin") &&
         ok;
    ok = expect(ShouldMarkCurrentKeyDecryption(-1) == pinShouldMarkCurrentKeyDecryption(-1),
                "reject free == pin") &&
         ok;
    ok = expect(ShouldMarkCurrentKeyDecryption(2) == pinShouldMarkCurrentKeyDecryption(2),
                "count-2 free == pin") &&
         ok;

    // Residual independence: 2995 inline pin still matches free.
    ok = expect(ShouldMarkCurrentKeyDecryption(0) == inlineShouldMarkCurrentKeyDecryption(0),
                "prior 2995 inline current-key still holds under 3336") &&
         ok;
    ok = expect(ShouldMarkCurrentKeyDecryption(1) == inlineShouldMarkCurrentKeyDecryption(1),
                "prior 2995 inline previous-key still holds under 3336") &&
         ok;

    return ok;
}
