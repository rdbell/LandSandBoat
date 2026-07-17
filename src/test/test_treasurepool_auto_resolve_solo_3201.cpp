#include "test_treasurepool_auto_resolve_solo_3201.h"

#include "map/treasure_pool_capacity.h"

#include <cstddef>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "treasurepool ShouldAutoResolveSolo 3201 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline addItem solo auto-resolve formula for dual-wire cross-check (dedicated 3201):
//   memberCount == 1
auto inlineShouldAutoResolveSolo(const std::size_t memberCount) -> bool
{
    return memberCount == 1;
}

// Compact dual-wire pin matching Go pinShouldAutoResolveSolo3201 / C++ capacity:
//   memberCount == 1
auto pinShouldAutoResolveSolo(const std::size_t memberCount) -> bool
{
    return memberCount == 1;
}

} // namespace

// Pure dual-wire expansion for treasurepoolhelpers::ShouldAutoResolveSolo
// (memberCount() == 1 after insert; OmegaXI internal/treasurepool;
// dedicated slice 3201 expand residual 2938).
//
// Coverage:
//   - free == inline == pin == (memberCount == 1)
//   - residual 1367 / 2938 pins still hold
//   - poles: 0, 1, 2, 6
auto runTreasurepoolAutoResolveSolo3201SelfTests() -> bool
{
    using treasurepoolhelpers::ShouldAutoResolveSolo;

    bool ok = true;

    // Residual 1367 / 2938 pins still hold under dual-wire.
    ok = expect(ShouldAutoResolveSolo(1), "residual solo auto-resolves") && ok;
    ok = expect(!ShouldAutoResolveSolo(0), "residual empty does not auto-resolve") && ok;
    ok = expect(!ShouldAutoResolveSolo(2), "residual party pair defers") && ok;
    ok = expect(!ShouldAutoResolveSolo(6), "residual full party defers") && ok;
    ok = expect(!ShouldAutoResolveSolo(18), "residual alliance full defers") && ok;

    // --- Composition table: free == inline == pin ---
    const struct
    {
        std::size_t memberCount;
        bool        want;
        const char* label;
    } cases[] = {
        // Residual 1367 / 2938 poles.
        { 1, true, "residual solo auto-resolves" },
        { 2, false, "residual party pair defers" },
        { 0, false, "residual empty does not auto-resolve" },
        { 3, false, "residual party three defers" },
        { 6, false, "residual full party defers" },
        { 18, false, "residual alliance full defers" },

        // Classic dual poles.
        { 1, true, "solo auto-resolves" },
        { 2, false, "party pair defers" },

        // Dedicated poles: 0, 1, 2, 6.
        { 0, false, "pole empty (0)" },
        { 1, true, "pole solo (1)" },
        { 2, false, "pole party pair (2)" },
        { 6, false, "pole full party (6)" },

        // Boundary neighbors around solo.
        { 0, false, "zero boundary" },
        { 1, true, "exact solo" },
        { 2, false, "one over solo" },

        // Multi-member extremes.
        { 3, false, "party three defers" },
        { 4, false, "party four defers" },
        { 5, false, "party five defers" },
        { 18, false, "alliance full defers" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldAutoResolveSolo(c.memberCount);
        const bool inlineF = inlineShouldAutoResolveSolo(c.memberCount);
        const bool pin     = pinShouldAutoResolveSolo(c.memberCount);
        const bool wantPin = c.memberCount == 1;

        ok = expect(got == c.want, c.label) && ok;
        // Positive form: free == inline == pin.
        ok = expect(got == inlineF && got == pin, "dual-wire free == inline == pin") && ok;
        ok = expect(got == wantPin, "free == pin formula memberCount == 1") && ok;
    }

    // Explicit poles free == inline == pin for 0 / 1 / 2 / 6.
    for (const std::size_t n : { std::size_t{ 0 }, std::size_t{ 1 }, std::size_t{ 2 }, std::size_t{ 6 } })
    {
        const bool got     = ShouldAutoResolveSolo(n);
        const bool inlineF = inlineShouldAutoResolveSolo(n);
        const bool pin     = pinShouldAutoResolveSolo(n);
        const bool want    = n == 1;
        ok                 = expect(got == want, "pole free == pin formula") && ok;
        ok                 = expect(got == inlineF && got == pin, "pole free == inline == pin") && ok;
    }

    // Dense compose over solo/party/alliance-relevant member counts
    // free == inline == pin.
    for (std::size_t memberCount = 0; memberCount <= 18; ++memberCount)
    {
        const bool got     = ShouldAutoResolveSolo(memberCount);
        const bool inlineF = inlineShouldAutoResolveSolo(memberCount);
        const bool pin     = pinShouldAutoResolveSolo(memberCount);
        const bool want    = memberCount == 1;
        ok                 = expect(got == want, "compose free == pin formula") && ok;
        ok                 = expect(got == inlineF && got == pin, "compose free == inline == pin") && ok;
    }

    // Edge poles free == inline == pin.
    for (const std::size_t n : { std::size_t{ 0 }, std::size_t{ 1 }, std::size_t{ 2 }, std::size_t{ 3 },
                                 std::size_t{ 6 }, std::size_t{ 18 }, std::size_t{ 32 }, std::size_t{ 255 } })
    {
        const bool got     = ShouldAutoResolveSolo(n);
        const bool inlineF = inlineShouldAutoResolveSolo(n);
        const bool pin     = pinShouldAutoResolveSolo(n);
        const bool want    = n == 1;
        ok                 = expect(got == want, "edge free == pin formula") && ok;
        ok                 = expect(got == inlineF && got == pin, "edge free == inline == pin") && ok;
    }

    // Host-style inject poles: free == inline == pin for addItem memberCount().
    for (const std::size_t n : { std::size_t{ 0 }, std::size_t{ 1 }, std::size_t{ 2 }, std::size_t{ 6 }, std::size_t{ 18 } })
    {
        const bool inject  = n == 1; // mirrors memberCount() == 1 host inject
        const bool got     = ShouldAutoResolveSolo(n);
        const bool inlineF = inlineShouldAutoResolveSolo(n);
        const bool pin     = pinShouldAutoResolveSolo(n);
        ok                 = expect(got == inject, "host inject dual-wire identity") && ok;
        ok                 = expect(got == inlineF && got == pin, "host inject free == inline == pin") && ok;
    }

    // Production addItem path semantics:
    // solo (1) → checkTreasureItem immediately
    // multi (2+) / empty (0) → leave for lot/pass/timeout
    ok = expect(ShouldAutoResolveSolo(1), "addItem solo → auto-resolve path") && ok;
    ok = expect(!ShouldAutoResolveSolo(0), "addItem empty → no auto-resolve") && ok;
    ok = expect(!ShouldAutoResolveSolo(2), "addItem party → defer lot/pass") && ok;
    ok = expect(!ShouldAutoResolveSolo(6), "addItem full party → defer") && ok;
    ok = expect(!ShouldAutoResolveSolo(18), "addItem alliance → defer") && ok;

    return ok;
}
