#include "test_auction_cancel_sale_2920.h"

#include "map/utils/auction_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "auction cancel sale 2920 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline production CancelSale index formula for dual-wire cross-checks:
//   if aucWorkIndex < 0: false; else aucWorkIndex < historyLen
auto inlineCanCancelSale(const int aucWorkIndex, const int historyLen) -> bool
{
    if (aucWorkIndex < 0)
    {
        return false;
    }
    return aucWorkIndex < historyLen;
}

} // namespace

// Pure dual-wire expansion for auctionutilshelpers::CanCancelSale
// (CancelSale index check in auctionutils.cpp).
// Valid when 0 <= aucWorkIndex < historyLen; negatives and OOB fail.
auto runAuctionCancelSale2920SelfTests() -> bool
{
    using auctionutilshelpers::CanCancelSale;

    bool ok = true;

    // --- Negative index (packet handler should reject -1; pure gate fails) ---
    ok = expect(!CanCancelSale(-1, 0), "negative index empty history") && ok;
    ok = expect(!CanCancelSale(-1, 3), "negative index non-empty history") && ok;
    ok = expect(!CanCancelSale(-2, 7), "more negative index") && ok;
    ok = expect(!CanCancelSale(-128, 7), "int8 min defensive") && ok;

    // --- Empty history ---
    ok = expect(!CanCancelSale(0, 0), "index 0 empty history") && ok;
    ok = expect(!CanCancelSale(1, 0), "index 1 empty history") && ok;

    // --- Valid slots ---
    ok = expect(CanCancelSale(0, 1), "first slot len 1") && ok;
    ok = expect(CanCancelSale(0, 3), "first slot len 3") && ok;
    ok = expect(CanCancelSale(2, 3), "last valid slot") && ok;
    ok = expect(CanCancelSale(6, 7), "HistoryLimit last valid") && ok;

    // --- Out of range ---
    ok = expect(!CanCancelSale(1, 1), "index == len") && ok;
    ok = expect(!CanCancelSale(3, 3), "index == len 3") && ok;
    ok = expect(!CanCancelSale(7, 7), "HistoryLimit OOB") && ok;
    ok = expect(!CanCancelSale(100, 7), "far OOB") && ok;

    // --- Composition table: dual-wire pure gate from host-injected scalars ---
    const struct
    {
        int         aucWorkIndex;
        int         historyLen;
        bool        want;
        const char* label;
    } cases[] = {
        { -1, 3, false, "table negative" },
        { -1, 0, false, "table negative empty" },
        { 0, 0, false, "table empty history" },
        { 0, 1, true, "table first slot" },
        { 0, 7, true, "table first of HistoryLimit" },
        { 2, 3, true, "table last valid" },
        { 3, 3, false, "table index equals len" },
        { 6, 7, true, "table HistoryLimit last" },
        { 7, 7, false, "table HistoryLimit OOB" },
        { 127, 7, false, "table int8 max OOB" },
    };

    for (const auto& c : cases)
    {
        const bool got       = CanCancelSale(c.aucWorkIndex, c.historyLen);
        const bool inlineGot = inlineCanCancelSale(c.aucWorkIndex, c.historyLen);
        ok                   = expect(got == c.want, c.label) && ok;
        ok                   = expect(got == inlineGot, "dual-wire matches inline formula") && ok;
    }

    // --- Production CancelSale path semantics ---
    // Valid index → may enter DELETE/return transaction path.
    // Invalid index → inventory-full / failure packet path (0xE5).
    ok = expect(CanCancelSale(0, 1), "CancelSale valid → transaction path") && ok;
    ok = expect(!CanCancelSale(-1, 1), "CancelSale negative → fail packet path") && ok;
    ok = expect(!CanCancelSale(1, 1), "CancelSale OOB → fail packet path") && ok;

    return ok;
}
