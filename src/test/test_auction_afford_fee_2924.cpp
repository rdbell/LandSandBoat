#include "test_auction_afford_fee_2924.h"

#include "map/utils/auction_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "auction afford fee 2924 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline production ProofOfPurchase fee formula for dual-wire cross-checks:
//   gilQuantity >= fee && gilReserve == 0
// (negation of quantity < fee || reserve > 0).
auto inlineCanAffordFee(const uint32 gilQuantity, const uint32 gilReserve, const uint32 fee) -> bool
{
    return gilQuantity >= fee && gilReserve == 0;
}

} // namespace

// Pure dual-wire expansion for auctionutilshelpers::CanAffordFee
// (ProofOfPurchase gil fee check in auctionutils.cpp).
// Valid when gil quantity covers fee and gil reserve is zero.
auto runAuctionAffordFee2924SelfTests() -> bool
{
    using auctionutilshelpers::CanAffordFee;

    bool ok = true;

    // --- Affordable: quantity covers fee, reserve zero ---
    ok = expect(CanAffordFee(100, 0, 50), "afford mid") && ok;
    ok = expect(CanAffordFee(50, 0, 50), "exact gil") && ok;
    ok = expect(CanAffordFee(0, 0, 0), "zero fee free") && ok;
    ok = expect(CanAffordFee(1, 0, 0), "zero fee with gil") && ok;
    ok = expect(CanAffordFee(1, 0, 1), "qty one fee one") && ok;

    // --- Insufficient gil ---
    ok = expect(!CanAffordFee(49, 0, 50), "short by one") && ok;
    ok = expect(!CanAffordFee(0, 0, 1), "empty gil non-zero fee") && ok;

    // --- Reserved gil blocks fee ---
    ok = expect(!CanAffordFee(100, 1, 50), "reserve blocks afford") && ok;
    ok = expect(!CanAffordFee(50, 1, 50), "reserve blocks exact") && ok;
    ok = expect(!CanAffordFee(0, 1, 0), "reserve blocks zero fee") && ok;
    ok = expect(!CanAffordFee(1000, 100, 1), "reserve large with gil") && ok;

    // --- Composition table: dual-wire pure gate from host-injected scalars ---
    const struct
    {
        uint32      gilQuantity;
        uint32      gilReserve;
        uint32      fee;
        bool        want;
        const char* label;
    } cases[] = {
        { 100, 0, 50, true, "table afford mid" },
        { 50, 0, 50, true, "table exact gil" },
        { 0, 0, 0, true, "table zero fee" },
        { 49, 0, 50, false, "table short gil" },
        { 0, 0, 1, false, "table empty gil" },
        { 100, 1, 50, false, "table reserve blocks" },
        { 50, 1, 50, false, "table reserve exact" },
        { 0, 1, 0, false, "table reserve zero fee" },
        { 0xFFFFFFFFu, 0, 0xFFFFFFFFu, true, "table max uint32 exact" },
        { 0xFFFFFFFEu, 0, 0xFFFFFFFFu, false, "table max uint32 short" },
        { 0xFFFFFFFFu, 1, 0, false, "table max qty reserve blocks" },
    };

    for (const auto& c : cases)
    {
        const bool got       = CanAffordFee(c.gilQuantity, c.gilReserve, c.fee);
        const bool inlineGot = inlineCanAffordFee(c.gilQuantity, c.gilReserve, c.fee);
        ok                   = expect(got == c.want, c.label) && ok;
        ok                   = expect(got == inlineGot, "dual-wire matches inline formula") && ok;
    }

    // --- Production ProofOfPurchase path semantics ---
    // Afford → may continue listing-limit / DB insert path.
    // Short gil or reserved gil → LotIn result 197 ("Not enough gil to pay fee").
    ok = expect(CanAffordFee(100, 0, 50), "ProofOfPurchase afford → continue path") && ok;
    ok = expect(!CanAffordFee(49, 0, 50), "ProofOfPurchase short gil → 197 path") && ok;
    ok = expect(!CanAffordFee(100, 1, 50), "ProofOfPurchase reserve → 197 path") && ok;

    return ok;
}
