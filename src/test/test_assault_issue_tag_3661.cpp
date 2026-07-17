#include "test_assault_issue_tag_3661.h"

#include "map/assault_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "assault issue tag 3661 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua onRytaalEventFinish tagStock gate for dual-wire cross-check
// (dedicated 3661 expand residual 2867 / prior 3616 stock / 3571 / 3524 / 3388 / 3440 sibling):
//   tagStock > 0
//   // ≡ not (tagStock == 0)  // inverse of Lua early return
auto inlineCanIssueTagFromStock(const int32 tagStock) -> bool
{
    return tagStock > 0;
}

// Compact dual-wire pin matching Go pinCanIssueTagFromStock3661:
//   tagStock > 0
auto pinCanIssueTagFromStock(const int32 tagStock) -> bool
{
    return tagStock > 0;
}

// Prior dedicated 3616 pin (independence cross-check):
//   tagStock > 0
auto pinCanIssueTagFromStock3616(const int32 tagStock) -> bool
{
    return tagStock > 0;
}

// Prior dedicated 3571 pin (independence cross-check):
//   tagStock > 0
auto pinCanIssueTagFromStock3571(const int32 tagStock) -> bool
{
    return tagStock > 0;
}

// Prior dedicated 3524 pin (independence cross-check):
//   tagStock > 0
auto pinCanIssueTagFromStock3524(const int32 tagStock) -> bool
{
    return tagStock > 0;
}

// Prior dedicated 3388 pin (independence cross-check):
//   tagStock > 0
auto pinCanIssueTagFromStock3388(const int32 tagStock) -> bool
{
    return tagStock > 0;
}

} // namespace

// Pure dual-wire expansion for assaulthelpers::CanIssueTagFromStock
// (Lua onRytaalEventFinish tagStock gate after ShouldIssueNewTag; OmegaXI
// internal/assault; dedicated slice 3661 expand residual 2867 / prior 3616
// stock / 3571 / 3524 / 3388 / 3440 ShouldIssueNewTag sibling / pure 1100 —
// formula unchanged).
//
// Coverage:
//   - free == inline == pin == pin3616 == tagStock > 0
//   - residual poles: stock 0 / 1
//   - dense tagStock space
//   - residual 1100 / 2867 / 3145 / 3258 / 3388 / 3524 / 3571 / 3616 stock pins still hold
//   - host-owned currentAssault / giveKeyItem left outside pure surface
//
// Sibling suites RETAINED: test_assault_issue_tag_2867,
// test_assault_issue_new_tag_3145, test_assault_issue_tag_3258,
// test_assault_issue_tag_3388 (prior stock), test_assault_issue_tag_3440
// (ShouldIssueNewTag), test_assault_issue_tag_3524 (prior stock),
// test_assault_issue_tag_3571 (prior stock), test_assault_issue_tag_3616
// (prior stock). Mirrors 3616 expand-residual suite layout (header +
// self-test runner; 3616 CanIssueTagFromStock content; no CMake/main).
auto runAssaultIssueTag3661SelfTests() -> bool
{
    using assaulthelpers::CanIssueTagFromStock;
    using assaulthelpers::ShouldIssueNewTag;
    using assaulthelpers::kRytaalOptionObtainTag;

    bool ok = true;

    // Residual 1100 / 2867 / 3145 / 3258 / 3388 / 3524 / 3571 / 3616 stock poles still hold.
    ok = expect(CanIssueTagFromStock(1), "residual: stock 1 should issue") && ok;
    ok = expect(!CanIssueTagFromStock(0), "residual: stock 0 should block") && ok;
    ok = expect(CanIssueTagFromStock(3), "residual: default max stock still positive") && ok;
    ok = expect(CanIssueTagFromStock(4), "residual: full max stock still positive") && ok;

    // --- Eligible stock path ---
    ok = expect(CanIssueTagFromStock(1), "eligible stock 1") && ok;
    ok = expect(CanIssueTagFromStock(2), "eligible stock 2") && ok;
    ok = expect(CanIssueTagFromStock(3), "eligible stock 3") && ok;
    ok = expect(CanIssueTagFromStock(4), "eligible stock 4") && ok;
    ok = expect(CanIssueTagFromStock(99), "eligible stock 99") && ok;

    // --- Blocked paths ---
    ok = expect(!CanIssueTagFromStock(0), "empty stock blocks") && ok;
    ok = expect(!CanIssueTagFromStock(-1), "negative stock blocks") && ok;
    ok = expect(!CanIssueTagFromStock(-2), "negative stock -2 blocks") && ok;

    // --- Composition table: free == inline == pin == pin3616 == compose ---
    const struct
    {
        int32       tagStock;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic residual poles.
        { 0, false, "table stock 0 empty blocks" },
        { 1, true, "table stock 1 residual issue" },
        // Residual 2867 re-pins.
        { 0, false, "residual 2867 stock 0" },
        { 1, true, "residual 2867 stock 1" },
        // Residual 1100 re-pins.
        { 0, false, "residual 1100 stock 0" },
        { 1, true, "residual 1100 stock 1" },
        // Prior dedicated 3145 / expand 3258 residual stock poles.
        { 0, false, "prior 3145/3258 stock 0" },
        { 1, true, "prior 3145/3258 stock 1" },
        // Prior dedicated stock expand residual 3388 re-pins.
        { 0, false, "prior 3388 stock 0" },
        { 1, true, "prior 3388 stock 1" },
        { 3, true, "prior 3388 MaxTagStockDefault" },
        { 4, true, "prior 3388 MaxTagStockFull" },
        // Prior dedicated stock expand residual 3524 re-pins.
        { 0, false, "prior 3524 stock 0" },
        { 1, true, "prior 3524 stock 1" },
        { 3, true, "prior 3524 MaxTagStockDefault" },
        { 4, true, "prior 3524 MaxTagStockFull" },
        // Prior dedicated stock expand residual 3571 re-pins.
        { 0, false, "prior 3571 stock 0" },
        { 1, true, "prior 3571 stock 1" },
        { 3, true, "prior 3571 MaxTagStockDefault" },
        { 4, true, "prior 3571 MaxTagStockFull" },
        // Prior dedicated stock expand residual 3616 re-pins.
        { 0, false, "prior 3616 stock 0" },
        { 1, true, "prior 3616 stock 1" },
        { 3, true, "prior 3616 MaxTagStockDefault" },
        { 4, true, "prior 3616 MaxTagStockFull" },
        // Max stock catalog pins (1100).
        { 3, true, "MaxTagStockDefault (3)" },
        { 4, true, "MaxTagStockFull (4)" },
        // Defensive / dense neighbors.
        { -1, false, "negative stock blocks" },
        { -2, false, "negative stock -2 blocks" },
        { 2, true, "stock 2 issues" },
        { 99, true, "stock 99 issues" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanIssueTagFromStock(c.tagStock);
        const bool inlineC = inlineCanIssueTagFromStock(c.tagStock);
        const bool pinGot  = pinCanIssueTagFromStock(c.tagStock);
        const bool pin3616 = pinCanIssueTagFromStock3616(c.tagStock);
        const bool compose = c.tagStock > 0;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineC, "dual-wire free==inline") && ok;
        ok = expect(got == pinGot, "dual-wire free==pin") && ok;
        ok = expect(got == pin3616, "dual-wire free==pin3616") && ok;
        ok = expect(got == compose, "formula free==tagStock>0") && ok;
    }

    // Pin composition: free function is tagStock > 0.
    ok = expect(CanIssueTagFromStock(1), "eligible stock must issue") && ok;
    ok = expect(!CanIssueTagFromStock(0), "empty stock must block") && ok;
    ok = expect(CanIssueTagFromStock(1) == pinCanIssueTagFromStock(1), "free==pin stock 1") && ok;
    ok = expect(CanIssueTagFromStock(0) == pinCanIssueTagFromStock(0), "free==pin stock 0") && ok;
    ok = expect(CanIssueTagFromStock(1) == inlineCanIssueTagFromStock(1), "free==inline stock 1") && ok;
    ok = expect(CanIssueTagFromStock(0) == inlineCanIssueTagFromStock(0), "free==inline stock 0") && ok;

    // Prior dedicated 3616 independence: free still matches prior pin.
    ok = expect(CanIssueTagFromStock(1) == pinCanIssueTagFromStock3616(1), "prior 3616 independence stock 1") && ok;
    ok = expect(CanIssueTagFromStock(0) == pinCanIssueTagFromStock3616(0), "prior 3616 independence stock 0") && ok;

    // Prior dedicated 3571 independence: free still matches prior pin.
    ok = expect(CanIssueTagFromStock(1) == pinCanIssueTagFromStock3571(1), "prior 3571 independence stock 1") && ok;
    ok = expect(CanIssueTagFromStock(0) == pinCanIssueTagFromStock3571(0), "prior 3571 independence stock 0") && ok;

    // Prior dedicated 3524 independence: free still matches prior pin.
    ok = expect(CanIssueTagFromStock(1) == pinCanIssueTagFromStock3524(1), "prior 3524 independence stock 1") && ok;
    ok = expect(CanIssueTagFromStock(0) == pinCanIssueTagFromStock3524(0), "prior 3524 independence stock 0") && ok;

    // Prior dedicated 3388 independence: free still matches prior pin.
    ok = expect(CanIssueTagFromStock(1) == pinCanIssueTagFromStock3388(1), "prior 3388 independence stock 1") && ok;
    ok = expect(CanIssueTagFromStock(0) == pinCanIssueTagFromStock3388(0), "prior 3388 independence stock 0") && ok;

    // Explicit residual poles free == inline == pin == pin3616 for stock 0 / 1.
    for (const int32 stock : { 0, 1 })
    {
        const bool got     = CanIssueTagFromStock(stock);
        const bool inlineC = inlineCanIssueTagFromStock(stock);
        const bool pinGot  = pinCanIssueTagFromStock(stock);
        const bool pin3616 = pinCanIssueTagFromStock3616(stock);
        const bool want    = stock > 0;
        ok                 = expect(got == want, "pole free == stock>0") && ok;
        ok                 = expect(got == inlineC, "pole free == inline") && ok;
        ok                 = expect(got == pinGot, "pole free == pin") && ok;
        ok                 = expect(got == pin3616, "pole free == pin3616") && ok;
    }

    // Dense tagStock: free == pin == inline == pin3616 == compose.
    const int32 denseStocks[] = { -2, -1, 0, 1, 2, 3, 4, 5, 99 };
    for (const int32 stock : denseStocks)
    {
        const bool got  = CanIssueTagFromStock(stock);
        const bool want = stock > 0;
        ok              = expect(got == want, "dense free==compose") && ok;
        ok              = expect(got == inlineCanIssueTagFromStock(stock), "dense free==inline") && ok;
        ok              = expect(got == pinCanIssueTagFromStock(stock), "dense free==pin") && ok;
        ok              = expect(got == pinCanIssueTagFromStock3616(stock), "dense free==prior 3616 pin") && ok;
        ok              = expect(got == pinCanIssueTagFromStock3571(stock), "dense free==prior 3571 pin") && ok;
        ok              = expect(got == pinCanIssueTagFromStock3524(stock), "dense free==prior 3524 pin") && ok;
        ok              = expect(got == pinCanIssueTagFromStock3388(stock), "dense free==prior 3388 pin") && ok;
    }

    // Residual 2867 independence: free still matches residual stock poles.
    ok = expect(CanIssueTagFromStock(1), "residual 2867 independence stock 1") && ok;
    ok = expect(!CanIssueTagFromStock(0), "residual 2867 independence stock 0") && ok;

    // Host-style inject poles: currency id_tags / tagStock after
    // ShouldIssueNewTag (live onRytaalEventFinish residual).
    const struct
    {
        int32       tagStock;
        bool        want;
        const char* label;
    } hostPoles[] = {
        { 0, false, "host empty stock → block" },
        { 1, true, "host stock 1 → continue" },
        { 3, true, "host default max → continue" },
        { 4, true, "host full max → continue" },
        { -1, false, "host negative stock → block" },
    };
    for (const auto& h : hostPoles)
    {
        const bool got     = CanIssueTagFromStock(h.tagStock);
        const bool inlineC = inlineCanIssueTagFromStock(h.tagStock);
        const bool pinGot  = pinCanIssueTagFromStock(h.tagStock);
        const bool pin3616 = pinCanIssueTagFromStock3616(h.tagStock);
        const bool inject  = h.tagStock > 0;
        ok                 = expect(got == inject, h.label) && ok;
        ok                 = expect(got == inlineC, "host free==inline") && ok;
        ok                 = expect(got == pinGot, "host free==pin") && ok;
        ok                 = expect(got == pin3616, "host free==pin3616") && ok;
        ok                 = expect(got == h.want, "host free==want") && ok;
    }

    // --- Production onRytaalEventFinish path semantics (after ShouldIssueNewTag) ---
    // Eligible stock → may continue currentAssault / giveKeyItem path.
    // Empty stock → early return (no writeback).
    ok = expect(CanIssueTagFromStock(1), "onRytaalEventFinish stock>0 → continue path") && ok;
    ok = expect(!CanIssueTagFromStock(0), "onRytaalEventFinish stock==0 → blocked") && ok;

    // Sibling pure gates remain outside this dual-wire surface.
    // ShouldIssueNewTag (3440 / prior 3258) residual still holds.
    ok = expect(ShouldIssueNewTag(kRytaalOptionObtainTag, false), "sibling ShouldIssueNewTag residual eligible") && ok;
    ok = expect(!ShouldIssueNewTag(kRytaalOptionObtainTag, true), "sibling ShouldIssueNewTag residual has-KI blocks") && ok;

    return ok;
}
