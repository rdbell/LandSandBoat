#include "test_einherjar_empty_chamber_timeout_3212.h"

#include "map/einherjar_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "einherjar ShouldEmptyChamberTimeout 3212 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua emptyChamberCheck gate for dual-wire cross-check
// (dedicated slice 3212):
//   playersCount(chamberData.players) == 0
auto inlineShouldEmptyChamberTimeout(const int32 playerCount) -> bool
{
    return playerCount == 0;
}

// Compact dual-wire pin matching Go pinShouldEmptyChamberTimeout3212 / C++ capacity:
//   playerCount == 0
auto pinShouldEmptyChamberTimeout(const int32 playerCount) -> bool
{
    return playerCount == 0;
}

} // namespace

// Pure dual-wire expansion for einherjarhelpers::ShouldEmptyChamberTimeout
// (Lua emptyChamberCheck expel gate; dedicated slice 3212 expand residual
// 2865 / pure 1069).
//
// Coverage:
//   - free == inline == pin == (playerCount == 0)
//   - residual 1069 / 2865 pins still hold
//   - poles: playerCount -1 / 0 / 1
auto runEinherjarEmptyChamberTimeout3212SelfTests() -> bool
{
    using einherjarhelpers::ShouldEmptyChamberTimeout;

    bool ok = true;

    // Residual 1069 / 2865 pins still hold under dual-wire.
    ok = expect(ShouldEmptyChamberTimeout(0), "residual empty timeouts") && ok;
    ok = expect(!ShouldEmptyChamberTimeout(1), "residual occupied does not timeout") && ok;

    // --- Composition table: free == inline == pin ---
    // Required poles: playerCount -1 / 0 / 1.
    const struct
    {
        int32       count;
        bool        want;
        const char* label;
    } cases[] = {
        // Required poles: -1 / 0 / 1.
        { 0, true, "pole playerCount=0 timeouts" },
        { -1, false, "pole playerCount=-1 not empty" },
        { 1, false, "pole playerCount=1 occupied" },

        // Residual 2865 happy / multi player.
        { 2, false, "two players occupied" },
        { 36, false, "max players pin occupied" },
        { 100, false, "large count occupied" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldEmptyChamberTimeout(c.count);
        const bool inlineF = inlineShouldEmptyChamberTimeout(c.count);
        const bool pin     = pinShouldEmptyChamberTimeout(c.count);
        const bool wantPin = c.count == 0;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF && got == pin, "ShouldEmptyChamberTimeout dual-wire free == inline == pin") && ok;
        ok = expect(got == wantPin, "ShouldEmptyChamberTimeout == pin formula") && ok;
    }

    // Pin composition at exact playerCount==0 boundary poles.
    ok = expect(ShouldEmptyChamberTimeout(0), "playerCount==0 must timeout") && ok;
    ok = expect(!ShouldEmptyChamberTimeout(-1), "playerCount==-1 must not timeout") && ok;
    ok = expect(!ShouldEmptyChamberTimeout(1), "playerCount==1 must not timeout") && ok;

    // Dense compose: playerCount poles free == inline == pin.
    for (int32 count = -5; count <= 20; ++count)
    {
        const bool got     = ShouldEmptyChamberTimeout(count);
        const bool inlineF = inlineShouldEmptyChamberTimeout(count);
        const bool pin     = pinShouldEmptyChamberTimeout(count);
        const bool want    = count == 0;
        ok                 = expect(got == want, "compose free == pin formula") && ok;
        ok                 = expect(got == inlineF && got == pin, "compose free == inline == pin") && ok;
    }

    // --- Production emptyChamberCheck path semantics ---
    // Host injects playerCount = playersCount(chamberData.players);
    // if ShouldEmptyChamberTimeout → log + expelAllFromChamber
    ok = expect(ShouldEmptyChamberTimeout(0), "host inject empty timeouts") && ok;
    ok = expect(!ShouldEmptyChamberTimeout(-1), "host inject negative not empty") && ok;
    ok = expect(!ShouldEmptyChamberTimeout(1), "host inject one player does not timeout") && ok;
    ok = expect(!ShouldEmptyChamberTimeout(2), "host inject multi player does not timeout") && ok;
    ok = expect(!ShouldEmptyChamberTimeout(36), "host inject max players pin does not timeout") && ok;
    ok = expect(!ShouldEmptyChamberTimeout(100), "host inject large count does not timeout") && ok;

    // Explicit dual-wire: free == inline == pin for host poles.
    const int32 hostPoles[] = { -100, -1, 0, 1, 2, 36, 100 };
    for (const int32 count : hostPoles)
    {
        const bool got     = ShouldEmptyChamberTimeout(count);
        const bool inlineF = inlineShouldEmptyChamberTimeout(count);
        const bool pin     = pinShouldEmptyChamberTimeout(count);
        const bool want    = count == 0;
        ok                 = expect(got == want, "host inject free == pin") && ok;
        ok                 = expect(got == inlineF && got == pin, "host inject free == inline == pin") && ok;
    }

    return ok;
}
