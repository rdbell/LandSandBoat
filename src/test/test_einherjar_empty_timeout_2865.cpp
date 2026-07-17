#include "test_einherjar_empty_timeout_2865.h"

#include "map/einherjar_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "einherjar empty timeout 2865 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua emptyChamberCheck formula for dual-wire checks:
//   playersCount(chamberData.players) == 0
auto inlineShouldEmptyChamberTimeout(const int32 playerCount) -> bool
{
    return playerCount == 0;
}

} // namespace

// Pure dual-wire expansion for einherjarhelpers::ShouldEmptyChamberTimeout
// (Lua system.lua emptyChamberCheck expel gate).
auto runEinherjarEmptyTimeout2865SelfTests() -> bool
{
    using einherjarhelpers::ShouldEmptyChamberTimeout;

    bool ok = true;

    // Core emptyChamberCheck: expel only when empty.
    ok = expect(ShouldEmptyChamberTimeout(0), "empty timeouts") && ok;
    ok = expect(!ShouldEmptyChamberTimeout(1), "one player no timeout") && ok;
    ok = expect(!ShouldEmptyChamberTimeout(2), "two players no timeout") && ok;
    ok = expect(!ShouldEmptyChamberTimeout(36), "max players pin no timeout") && ok;

    // Defensive / boundary counts hosts may inject.
    ok = expect(!ShouldEmptyChamberTimeout(-1), "negative not empty") && ok;
    ok = expect(!ShouldEmptyChamberTimeout(100), "large count no timeout") && ok;

    // Dual-wire matches inline formula across a small table.
    const struct
    {
        int32       count;
        bool        want;
        const char* label;
    } cases[] = {
        { 0, true, "table empty" },
        { 1, false, "table one" },
        { 2, false, "table two" },
        { 36, false, "table max pin" },
        { -1, false, "table negative" },
        { 100, false, "table large" },
    };

    for (const auto& c : cases)
    {
        const bool got       = ShouldEmptyChamberTimeout(c.count);
        const bool inlineGot = inlineShouldEmptyChamberTimeout(c.count);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineGot, "dual-wire free == inline Lua formula") && ok;
        ok = expect(got == (c.count == 0), "dual-wire free == compose(count==0)") && ok;
    }

    return ok;
}
