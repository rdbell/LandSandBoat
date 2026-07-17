#include "test_auction_refresh_history_2935.h"

#include "map/utils/auction_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "auction refresh history 2935 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline production OpenListOfSales cooldown formula for dual-wire cross-checks:
//   nowMs > lastMs + HistoryCooldownMs  (strict greater-than; 5s = 5000 ms)
auto inlineCanRefreshHistory(const int64 nowMs, const int64 lastMs) -> bool
{
    return nowMs > lastMs + auctionutilshelpers::HistoryCooldownMs;
}

} // namespace

// Pure dual-wire expansion for auctionutilshelpers::CanRefreshHistory
// (OpenListOfSales cooldown gate in auctionutils.cpp).
// Refresh only when now is strictly after last + 5s; boundary still busy (246).
auto runAuctionRefreshHistory2935SelfTests() -> bool
{
    using auctionutilshelpers::CanRefreshHistory;
    using auctionutilshelpers::HistoryCooldownMs;

    bool ok = true;

    // --- Constant pin ---
    ok = expect(HistoryCooldownMs == 5000, "HistoryCooldownMs is 5000") && ok;

    constexpr int64 lastMs = 1'700'000'000'000LL; // fixed epoch ms

    // --- Still busy (within / at cooldown) ---
    ok = expect(!CanRefreshHistory(lastMs, lastMs), "same instant busy") && ok;
    ok = expect(!CanRefreshHistory(lastMs + 4000, lastMs), "4s before cooldown busy") && ok;
    ok = expect(!CanRefreshHistory(lastMs + 4999, lastMs), "1ms before boundary busy") && ok;
    ok = expect(!CanRefreshHistory(lastMs + HistoryCooldownMs, lastMs), "exactly 5s boundary busy") && ok;
    ok = expect(!CanRefreshHistory(lastMs - 1000, lastMs), "now in past relative to last busy") && ok;

    // --- Refresh allowed (strictly after 5s) ---
    ok = expect(CanRefreshHistory(lastMs + HistoryCooldownMs + 1, lastMs), "1ms after boundary refresh") && ok;
    ok = expect(CanRefreshHistory(lastMs + 6000, lastMs), "6s after refresh") && ok;
    ok = expect(CanRefreshHistory(lastMs + 10000, lastMs), "10s after refresh") && ok;

    // --- Composition table: dual-wire pure gate from host-injected millis ---
    const struct
    {
        int64       nowMs;
        int64       lastMs;
        bool        want;
        const char* label;
    } cases[] = {
        { lastMs, lastMs, false, "table same instant" },
        { lastMs + 4000, lastMs, false, "table 4s busy" },
        { lastMs + 4999, lastMs, false, "table 4999ms busy" },
        { lastMs + 5000, lastMs, false, "table 5000ms boundary busy" },
        { lastMs + 5001, lastMs, true, "table 5001ms refresh" },
        { lastMs + 6000, lastMs, true, "table 6s refresh" },
        { lastMs + 10000, lastMs, true, "table 10s refresh" },
        { lastMs - 1, lastMs, false, "table now before last" },
        { 0, 0, false, "table zeros busy" },
        { 5000, 0, false, "table zero last at boundary" },
        { 5001, 0, true, "table zero last just after" },
    };

    for (const auto& c : cases)
    {
        const bool got       = CanRefreshHistory(c.nowMs, c.lastMs);
        const bool inlineGot = inlineCanRefreshHistory(c.nowMs, c.lastMs);
        ok                   = expect(got == c.want, c.label) && ok;
        ok                   = expect(got == inlineGot, "dual-wire matches inline formula") && ok;
    }

    // --- Production OpenListOfSales path semantics ---
    // Refresh → clear history, stamp timestamp, re-query LIMIT 7.
    // Busy → Info result 246 ("try again in a little while").
    ok = expect(CanRefreshHistory(lastMs + 6000, lastMs), "OpenListOfSales refresh → clear/re-query path") && ok;
    ok = expect(!CanRefreshHistory(lastMs + 5000, lastMs), "OpenListOfSales boundary → 246 busy path") && ok;
    ok = expect(!CanRefreshHistory(lastMs + 1000, lastMs), "OpenListOfSales early → 246 busy path") && ok;

    return ok;
}
