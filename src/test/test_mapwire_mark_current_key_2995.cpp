#include "test_mapwire_mark_current_key_2995.h"

#include "map/map_networking_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "mapwire ShouldMarkCurrentKeyDecryption 2995 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline recv_parse current-key mark formula for dual-wire cross-check
// (slice 2995):
//   decryptCount == 0
auto inlineShouldMarkCurrentKeyDecryption(const int decryptCount) -> bool
{
    return decryptCount == 0;
}

} // namespace

// Pure dual-wire expansion for mapnetworkinghelpers::ShouldMarkCurrentKeyDecryption
// (decryptCount == 0; slice 2995).
auto runMapwireMarkCurrentKey2995SelfTests() -> bool
{
    using mapnetworkinghelpers::ShouldMarkCurrentKeyDecryption;

    bool ok = true;

    const struct
    {
        int         decryptCount;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles.
        { 0, true, "current-key success marks" },
        { 1, false, "previous-key zone fallback does not mark" },

        // Residual 2711 pins.
        { 0, true, "residual zero marks" },
        { 1, false, "residual one does not mark" },
        { -1, false, "residual reject does not mark" },

        // Required dual-wire edges (-1, 0, 1, 2).
        { -1, false, "edge decryptCount -1" },
        { 0, true, "edge decryptCount 0" },
        { 1, false, "edge decryptCount 1" },
        { 2, false, "edge decryptCount 2" },

        // Identity / polarity repeats for dual-wire stability.
        { 0, true, "identity current key" },
        { 1, false, "identity previous key" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldMarkCurrentKeyDecryption(c.decryptCount);
        const bool inlineF = inlineShouldMarkCurrentKeyDecryption(c.decryptCount);
        const bool wantPin = c.decryptCount == 0;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldMarkCurrentKeyDecryption dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldMarkCurrentKeyDecryption == pin formula decryptCount==0") && ok;
    }

    // Pin composition: only zero marks current key.
    ok = expect(ShouldMarkCurrentKeyDecryption(0), "decryptCount 0 must mark current key") && ok;
    ok = expect(!ShouldMarkCurrentKeyDecryption(1), "decryptCount 1 must not mark current key") && ok;
    ok = expect(!ShouldMarkCurrentKeyDecryption(-1), "decryptCount -1 must not mark current key") && ok;
    ok = expect(!ShouldMarkCurrentKeyDecryption(2), "decryptCount 2 must not mark current key") && ok;

    // Dense compose: required edges -1, 0, 1, 2.
    for (const int decryptCount : { -1, 0, 1, 2 })
    {
        const bool got  = ShouldMarkCurrentKeyDecryption(decryptCount);
        const bool want = decryptCount == 0;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldMarkCurrentKeyDecryption(decryptCount),
                    "compose free == inline") &&
             ok;
    }

    // --- Production recv_parse path semantics ---
    // Host injects:
    //   decryptCount starts 0; UsePrevious increments to 1
    // if (ShouldMarkCurrentKeyDecryption(decryptCount)) hasDecryptedPacket = true
    ok = expect(ShouldMarkCurrentKeyDecryption(0), "recv_parse primary decrypt → mark hasDecryptedPacket path") && ok;
    ok = expect(!ShouldMarkCurrentKeyDecryption(1), "recv_parse previous-key fallback → skip mark path") && ok;

    // Residual 2711 pins still hold under dual-wire.
    ok = expect(ShouldMarkCurrentKeyDecryption(0), "residual zero marks") && ok;
    ok = expect(!ShouldMarkCurrentKeyDecryption(1), "residual one does not mark") && ok;
    ok = expect(!ShouldMarkCurrentKeyDecryption(-1), "residual reject does not mark") && ok;

    // Host-style inject polarity: mark only on current-key success (count 0).
    const bool markPrimary  = ShouldMarkCurrentKeyDecryption(0);
    const bool skipPrevious = !ShouldMarkCurrentKeyDecryption(1);
    ok                      = expect(markPrimary && skipPrevious, "recv_parse mark/skip polarity") && ok;
    ok                      = expect(markPrimary == inlineShouldMarkCurrentKeyDecryption(0),
                "primary free == inline") &&
         ok;
    ok = expect(ShouldMarkCurrentKeyDecryption(1) == inlineShouldMarkCurrentKeyDecryption(1),
                "previous free == inline") &&
         ok;
    ok = expect(ShouldMarkCurrentKeyDecryption(-1) == inlineShouldMarkCurrentKeyDecryption(-1),
                "reject free == inline") &&
         ok;
    ok = expect(ShouldMarkCurrentKeyDecryption(2) == inlineShouldMarkCurrentKeyDecryption(2),
                "count-2 free == inline") &&
         ok;

    return ok;
}
