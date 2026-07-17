#include "test_auctionutils_cancel_sale_3234.h"

#include "map/utils/auction_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "auctionutils CanCancelSale 3234 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline production CancelSale index formula for dual-wire cross-checks
// (dedicated slice 3234; residual expand 2920):
//   if aucWorkIndex < 0: return false
//   return aucWorkIndex < historyLen
auto inlineCanCancelSale(const int aucWorkIndex, const int historyLen) -> bool
{
    if (aucWorkIndex < 0)
    {
        return false;
    }
    return aucWorkIndex < historyLen;
}

// Positive multi-branch if/else pin matching free function / capacity body
// (slice 3234). Multi-branch: positive if/else only (avoid QF1001 De Morgan
// rewrites of the range gate).
auto pinCanCancelSale(const int aucWorkIndex, const int historyLen) -> bool
{
    if (aucWorkIndex < 0)
    {
        return false;
    }
    if (aucWorkIndex < historyLen)
    {
        return true;
    }
    return false;
}

// HistoryLimit pin (SQL LIMIT 7 / m_ah_history capacity).
constexpr int kHistoryLimit = 7;

} // namespace

// Pure dual-wire expansion for auctionutilshelpers::CanCancelSale
// (CancelSale history index gate; OmegaXI internal/auctionutils;
// dedicated slice 3234 expand residual 2920).
//
// Coverage:
//   - free == inline == pin (multi-branch positive if/else)
//   - residual poles: negative index, 0, len-1, len, large
//   - residual 1135 / 2920 pins still hold
//   - dense neighbors around HistoryLimit and empty history
auto runAuctionutilsCancelSale3234SelfTests() -> bool
{
    using auctionutilshelpers::CanCancelSale;

    bool ok = true;

    // Residual 1135 / 2920 pins still hold under dual-wire.
    ok = expect(!CanCancelSale(-1, 3), "residual negative index rejects") && ok;
    ok = expect(!CanCancelSale(0, 0), "residual empty history rejects") && ok;
    ok = expect(CanCancelSale(0, 1), "residual first slot accepts") && ok;
    ok = expect(CanCancelSale(2, 3), "residual last valid accepts") && ok;
    ok = expect(!CanCancelSale(3, 3), "residual index == len rejects") && ok;

    // --- Eligible cancel indexes (0 <= index < len) ---
    ok = expect(CanCancelSale(0, 1), "eligible first slot len 1") && ok;
    ok = expect(CanCancelSale(0, 3), "eligible first slot len 3") && ok;
    ok = expect(CanCancelSale(2, 3), "eligible last valid len 3") && ok;
    ok = expect(CanCancelSale(6, kHistoryLimit), "eligible HistoryLimit last valid") && ok;
    ok = expect(CanCancelSale(0, kHistoryLimit), "eligible first of HistoryLimit") && ok;

    // --- Blocked indexes (negative / empty / OOB) ---
    ok = expect(!CanCancelSale(-1, 0), "blocked negative empty") && ok;
    ok = expect(!CanCancelSale(-1, 3), "blocked negative non-empty") && ok;
    ok = expect(!CanCancelSale(-2, 7), "blocked more negative") && ok;
    ok = expect(!CanCancelSale(-128, 7), "blocked int8 min") && ok;
    ok = expect(!CanCancelSale(0, 0), "blocked empty index 0") && ok;
    ok = expect(!CanCancelSale(1, 0), "blocked empty index 1") && ok;
    ok = expect(!CanCancelSale(1, 1), "blocked index == len") && ok;
    ok = expect(!CanCancelSale(3, 3), "blocked index == len 3") && ok;
    ok = expect(!CanCancelSale(kHistoryLimit, kHistoryLimit), "blocked HistoryLimit OOB") && ok;
    ok = expect(!CanCancelSale(100, kHistoryLimit), "blocked far OOB large") && ok;
    ok = expect(!CanCancelSale(127, kHistoryLimit), "blocked int8 max OOB") && ok;

    // --- Composition table: free == inline == pin (multi-branch if/else) ---
    // Required residual poles: negative index, 0, len-1, len, large.
    const struct
    {
        int         aucWorkIndex;
        int         historyLen;
        bool        want;
        const char* label;
    } cases[] = {
        // residual poles (negative / 0 / len-1 / len / large)
        { -1, 3, false, "table pole negative index" },
        { -1, 0, false, "table pole negative empty" },
        { -2, 7, false, "table pole more negative" },
        { -128, 7, false, "table pole int8 min" },
        { 0, 0, false, "table pole index 0 empty" },
        { 0, 1, true, "table pole index 0 len 1" },
        { 0, 3, true, "table pole index 0 len 3" },
        { 0, kHistoryLimit, true, "table pole index 0 HistoryLimit" },
        { 2, 3, true, "table pole len-1 (2 of 3)" },
        { 6, kHistoryLimit, true, "table pole HistoryLimit len-1" },
        { 1, 1, false, "table pole index == len" },
        { 3, 3, false, "table pole index == len 3" },
        { kHistoryLimit, kHistoryLimit, false, "table pole HistoryLimit == len" },
        { 100, kHistoryLimit, false, "table pole large OOB" },
        { 127, kHistoryLimit, false, "table pole int8 max OOB" },
        // residual 2920 re-pins
        { 1, 0, false, "table residual empty history index 1" },
        { 2, 3, true, "table residual last valid" },
        { 3, 3, false, "table residual out of range" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanCancelSale(c.aucWorkIndex, c.historyLen);
        const bool inlineC = inlineCanCancelSale(c.aucWorkIndex, c.historyLen);
        const bool pinGot  = pinCanCancelSale(c.aucWorkIndex, c.historyLen);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineC, "dual-wire free==inline") && ok;
        ok = expect(got == pinGot, "dual-wire free==pin (multi-branch if/else)") && ok;
    }

    // Free == pin across residual poles (positive multi-branch if/else).
    const struct
    {
        int         aucWorkIndex;
        int         historyLen;
        const char* label;
    } poles[] = {
        { -1, 3, "negative" },
        { 0, 0, "empty" },
        { 0, 1, "first slot" },
        { 2, 3, "len-1" },
        { 3, 3, "len" },
        { 100, 7, "large" },
        { 6, 7, "HistoryLimit last valid" },
        { 7, 7, "HistoryLimit OOB" },
    };
    for (const auto& pole : poles)
    {
        const bool got     = CanCancelSale(pole.aucWorkIndex, pole.historyLen);
        const bool inlineC = inlineCanCancelSale(pole.aucWorkIndex, pole.historyLen);
        const bool pinGot  = pinCanCancelSale(pole.aucWorkIndex, pole.historyLen);
        ok                 = expect(got == inlineC && got == pinGot, "pole free==inline==pin") && ok;
    }

    // Dense compose: free == inline == pin over required poles + neighbors.
    const struct
    {
        int aucWorkIndex;
        int historyLen;
    } dense[] = {
        // negative
        { -1, 0 },
        { -1, 1 },
        { -1, 3 },
        { -1, kHistoryLimit },
        { -2, 7 },
        { -128, 7 },
        // index 0
        { 0, 0 },
        { 0, 1 },
        { 0, 3 },
        { 0, kHistoryLimit },
        // len-1 / len / large for len=1
        { 0, 1 },
        { 1, 1 },
        { 2, 1 },
        { 100, 1 },
        // len-1 / len / large for len=3
        { 2, 3 },
        { 3, 3 },
        { 4, 3 },
        { 100, 3 },
        // HistoryLimit neighbors
        { 5, kHistoryLimit },
        { 6, kHistoryLimit },
        { 7, kHistoryLimit },
        { 8, kHistoryLimit },
        { 100, kHistoryLimit },
        { 127, kHistoryLimit },
        // empty / far
        { 1, 0 },
        { 7, 0 },
        { 100, 0 },
    };
    for (const auto& p : dense)
    {
        const bool got  = CanCancelSale(p.aucWorkIndex, p.historyLen);
        bool       want = false;
        if (p.aucWorkIndex >= 0)
        {
            want = p.aucWorkIndex < p.historyLen;
        }
        ok = expect(got == want, "dense free == formula") && ok;
        ok = expect(got == inlineCanCancelSale(p.aucWorkIndex, p.historyLen), "dense free == inline") && ok;
        ok = expect(got == pinCanCancelSale(p.aucWorkIndex, p.historyLen), "dense free == pin multi-branch") && ok;
    }

    // Production CancelSale path semantics:
    // Valid index → may enter DELETE/return transaction path.
    // Invalid index → inventory-full / failure packet path (0xE5).
    ok = expect(CanCancelSale(0, 1), "CancelSale valid → transaction path") && ok;
    ok = expect(!CanCancelSale(-1, 1), "CancelSale negative → fail packet path") && ok;
    ok = expect(!CanCancelSale(1, 1), "CancelSale OOB → fail packet path") && ok;
    ok = expect(CanCancelSale(6, kHistoryLimit), "CancelSale HistoryLimit last valid") && ok;
    ok = expect(!CanCancelSale(kHistoryLimit, kHistoryLimit), "CancelSale HistoryLimit OOB") && ok;

    return ok;
}
