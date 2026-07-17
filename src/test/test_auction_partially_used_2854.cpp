#include "test_auction_partially_used_2854.h"

#include "map/utils/auction_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "auction partially used 2854 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline production lambda formula for dual-wire cross-checks:
//   if charged: currentCharges < maxCharges; else false.
auto inlineIsPartiallyUsed(const bool isCharged, const uint8 currentCharges, const uint8 maxCharges) -> bool
{
    if (isCharged)
    {
        return currentCharges < maxCharges;
    }
    return false;
}

} // namespace

// Pure dual-wire expansion for auctionutilshelpers::IsPartiallyUsed
// (anonymous isPartiallyUsed lambda in auctionutils.cpp).
// Charged + current < max → true; non-charged / full charges → false.
auto runAuctionPartiallyUsed2854SelfTests() -> bool
{
    using auctionutilshelpers::IsPartiallyUsed;

    bool ok = true;

    // --- Non-charged: charges ignored ---
    ok = expect(!IsPartiallyUsed(false, 0, 5), "non-charged zero current") && ok;
    ok = expect(!IsPartiallyUsed(false, 2, 5), "non-charged partial charges ignored") && ok;
    ok = expect(!IsPartiallyUsed(false, 5, 5), "non-charged full charges") && ok;
    ok = expect(!IsPartiallyUsed(false, 0, 0), "non-charged zeros") && ok;

    // --- Charged full charges → not partially used ---
    ok = expect(!IsPartiallyUsed(true, 5, 5), "charged full") && ok;
    ok = expect(!IsPartiallyUsed(true, 0, 0), "charged zero max equal") && ok;
    ok = expect(!IsPartiallyUsed(true, 1, 1), "charged single full") && ok;
    ok = expect(!IsPartiallyUsed(true, 6, 5), "charged current above max defensive") && ok;

    // --- Charged partial → partially used ---
    ok = expect(IsPartiallyUsed(true, 2, 5), "charged partial mid") && ok;
    ok = expect(IsPartiallyUsed(true, 0, 3), "charged empty") && ok;
    ok = expect(IsPartiallyUsed(true, 0, 1), "charged empty single") && ok;
    ok = expect(IsPartiallyUsed(true, 4, 5), "charged one below max") && ok;

    // --- Composition table: dual-wire pure gate from host-injected scalars ---
    const struct
    {
        bool        isCharged;
        uint8       currentCharges;
        uint8       maxCharges;
        bool        want;
        const char* label;
    } cases[] = {
        { false, 0, 5, false, "table non-charged" },
        { false, 2, 5, false, "table non-charged partial ignored" },
        { true, 5, 5, false, "table charged full" },
        { true, 0, 0, false, "table charged zero max" },
        { true, 2, 5, true, "table charged partial" },
        { true, 0, 3, true, "table charged empty" },
        { true, 0, 1, true, "table charged empty single" },
        { true, 6, 5, false, "table charged over max" },
        { true, 255, 255, false, "table max uint8 equal" },
        { true, 254, 255, true, "table max uint8 partial" },
    };

    for (const auto& c : cases)
    {
        const bool got       = IsPartiallyUsed(c.isCharged, c.currentCharges, c.maxCharges);
        const bool inlineGot = inlineIsPartiallyUsed(c.isCharged, c.currentCharges, c.maxCharges);
        ok                   = expect(got == c.want, c.label) && ok;
        ok                   = expect(got == inlineGot, "dual-wire matches inline formula") && ok;
    }

    // --- Production SellingItems / ProofOfPurchase gate semantics ---
    // Partially used → result 197; full charges / non-charged → gate passes.
    ok = expect(IsPartiallyUsed(true, 1, 3), "AskCommit/LotIn partial → 197 path") && ok;
    ok = expect(!IsPartiallyUsed(true, 3, 3), "AskCommit/LotIn full charges → ok path") && ok;
    ok = expect(!IsPartiallyUsed(false, 0, 0), "AskCommit/LotIn non-charged → ok path") && ok;

    return ok;
}
