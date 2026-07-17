#include "test_treasure_auto_solo_2938.h"

#include "map/treasure_pool_capacity.h"

#include <cstddef>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "treasure ShouldAutoResolveSolo 2938 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline addItem solo auto-resolve formula for dual-wire cross-check (slice 2938):
//   memberCount == 1
auto inlineShouldAutoResolveSolo(const std::size_t memberCount) -> bool
{
    return memberCount == 1;
}

} // namespace

// Pure dual-wire expansion for treasurepoolhelpers::ShouldAutoResolveSolo
// (memberCount() == 1 after insert; slice 2938).
auto runTreasureAutoSolo2938SelfTests() -> bool
{
    using treasurepoolhelpers::ShouldAutoResolveSolo;

    bool ok = true;

    const struct
    {
        std::size_t memberCount;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles.
        { 1, true, "solo auto-resolves" },
        { 2, false, "party pair defers" },

        // Empty / multi-member extremes.
        { 0, false, "empty pool does not auto-resolve" },
        { 3, false, "party three defers" },
        { 6, false, "full party defers" },
        { 18, false, "alliance full defers" },

        // Residual 1367 pins.
        { 1, true, "residual solo" },
        { 2, false, "residual multi" },

        // Boundary neighbors around solo.
        { 0, false, "zero boundary" },
        { 1, true, "exact solo" },
        { 2, false, "one over solo" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldAutoResolveSolo(c.memberCount);
        const bool inlineF = inlineShouldAutoResolveSolo(c.memberCount);
        const bool wantPin = c.memberCount == 1;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldAutoResolveSolo dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldAutoResolveSolo == pin formula memberCount == 1") && ok;
    }

    // Pin composition: equality to 1 only.
    ok = expect(ShouldAutoResolveSolo(1), "memberCount == 1 must auto-resolve") && ok;
    ok = expect(!ShouldAutoResolveSolo(0), "memberCount == 0 must not auto-resolve") && ok;
    ok = expect(!ShouldAutoResolveSolo(2), "memberCount == 2 must not auto-resolve") && ok;

    // Dense compose over solo/party/alliance-relevant member counts.
    for (std::size_t memberCount = 0; memberCount <= 18; ++memberCount)
    {
        const bool got  = ShouldAutoResolveSolo(memberCount);
        const bool want = memberCount == 1;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldAutoResolveSolo(memberCount), "compose free == inline") && ok;
    }

    // Host-style inject poles: memberCount() as size_t after addItem.
    // (Live CTreasurePool addItem checkTreasureItem is residual 1367 /
    // treasure_pool tests.)
    for (const std::size_t n : { std::size_t{ 0 }, std::size_t{ 1 }, std::size_t{ 2 }, std::size_t{ 6 }, std::size_t{ 18 } })
    {
        const bool inject = n == 1; // mirrors memberCount() == 1 host inject
        ok                = expect(ShouldAutoResolveSolo(n) == inject, "host inject dual-wire identity") && ok;
        ok                = expect(ShouldAutoResolveSolo(n) == inlineShouldAutoResolveSolo(n),
                    "host inject free == inline") &&
             ok;
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
