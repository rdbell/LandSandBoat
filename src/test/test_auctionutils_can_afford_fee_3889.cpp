#include "test_auctionutils_can_afford_fee_3889.h"

#include "map/utils/auction_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "auctionutils CanAffordFee 3889 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline production ProofOfPurchase fee formula for dual-wire cross-checks
// (dedicated slice 3889; residual expand 2924; prior dedicated 3844 / 3799 / 3754 / 3709 / 3664 / 3619 / 3574 / 3503 / 3310 / 3280):
//   gilQuantity >= fee && gilReserve == 0
// (negation of quantity < fee || reserve > 0).
auto inlineCanAffordFee(const uint32 gilQuantity, const uint32 gilReserve, const uint32 fee) -> bool
{
    return gilQuantity >= fee && gilReserve == 0;
}

// Positive multi-branch if/else pin matching free function / capacity body
// (slice 3889). Multi-branch: positive if/else only (avoid QF1001 De Morgan
// rewrites of qty < fee || reserve > 0).
auto pinCanAffordFee(const uint32 gilQuantity, const uint32 gilReserve, const uint32 fee) -> bool
{
    if (gilQuantity < fee)
    {
        return false;
    }
    if (gilReserve != 0)
    {
        return false;
    }
    return true;
}

// Prior dedicated 3844 pin (positive multi-branch if/else) for
// free == pin3844 cross-check.
auto pinCanAffordFee3844(const uint32 gilQuantity, const uint32 gilReserve, const uint32 fee) -> bool
{
    if (gilQuantity < fee)
    {
        return false;
    }
    if (gilReserve != 0)
    {
        return false;
    }
    return true;
}

} // namespace

// Pure dual-wire expansion for auctionutilshelpers::CanAffordFee
// (ProofOfPurchase gil fee gate; OmegaXI internal/auctionutils;
// dedicated slice 3889 expand residual 2924 — formula unchanged;
// prior dedicated ~3844 / 3799 / 3754 / 3709 / 3664 / 3619 / 3574 / 3503 / 3310 / 3280).
//
// Coverage:
//   - free == inline == pin == pin3844 (multi-branch positive if/else)
//   - residual poles: exact, short, surplus, zero fee, reserve block
//   - residual 1135 / 2924 / prior dedicated 3280 / 3310 / 3503 / 3574 / 3619 / 3664 / 3709 / 3754 / 3799 / 3844 pins still hold
//   - dense neighbors around fee boundaries and uint32 edges
//
// NOTE: intentionally NOT registered in CMake/main (LSB self-tests not registered).
auto runAuctionutilsCanAffordFee3889SelfTests() -> bool
{
    using auctionutilshelpers::CanAffordFee;

    bool ok = true;

    // Residual 1135 / 2924 / prior dedicated 3280 / 3310 / 3503 / 3574 / 3619 / 3664 / 3709 / 3754 / 3799 / 3844 pins still hold under dual-wire.
    ok = expect(CanAffordFee(100, 0, 50), "residual afford mid accepts") && ok;
    ok = expect(CanAffordFee(50, 0, 50), "residual exact gil accepts") && ok;
    ok = expect(!CanAffordFee(49, 0, 50), "residual short by one rejects") && ok;
    ok = expect(!CanAffordFee(100, 1, 50), "residual reserve blocks afford") && ok;
    ok = expect(CanAffordFee(0, 0, 0), "residual zero fee free accepts") && ok;

    // --- Affordable: quantity covers fee, reserve zero ---
    ok = expect(CanAffordFee(100, 0, 50), "eligible afford mid") && ok;
    ok = expect(CanAffordFee(50, 0, 50), "eligible exact gil") && ok;
    ok = expect(CanAffordFee(0, 0, 0), "eligible zero fee free") && ok;
    ok = expect(CanAffordFee(1, 0, 0), "eligible zero fee with gil") && ok;
    ok = expect(CanAffordFee(1, 0, 1), "eligible qty one fee one") && ok;
    ok = expect(CanAffordFee(0xFFFFFFFFu, 0, 0xFFFFFFFFu), "eligible max uint32 exact") && ok;

    // --- Blocked: short gil / non-zero reserve ---
    ok = expect(!CanAffordFee(49, 0, 50), "blocked short by one") && ok;
    ok = expect(!CanAffordFee(0, 0, 1), "blocked empty gil non-zero fee") && ok;
    ok = expect(!CanAffordFee(100, 1, 50), "blocked reserve blocks afford") && ok;
    ok = expect(!CanAffordFee(50, 1, 50), "blocked reserve blocks exact") && ok;
    ok = expect(!CanAffordFee(0, 1, 0), "blocked reserve blocks zero fee") && ok;
    ok = expect(!CanAffordFee(1000, 100, 1), "blocked reserve large with gil") && ok;
    ok = expect(!CanAffordFee(0xFFFFFFFEu, 0, 0xFFFFFFFFu), "blocked max uint32 short") && ok;
    ok = expect(!CanAffordFee(0xFFFFFFFFu, 1, 0), "blocked max qty reserve blocks") && ok;

    // --- Composition table: free == inline == pin == pin3844 (multi-branch if/else) ---
    // Residual poles: exact, short, surplus, zero fee, reserve block, uint32 edges.
    const struct
    {
        uint32      gilQuantity;
        uint32      gilReserve;
        uint32      fee;
        bool        want;
        const char* label;
    } cases[] = {
        // residual poles (exact / short / surplus / zero fee / reserve)
        { 100, 0, 50, true, "table pole afford mid" },
        { 50, 0, 50, true, "table pole exact gil" },
        { 51, 0, 50, true, "table pole surplus" },
        { 49, 0, 50, false, "table pole short by one" },
        { 0, 0, 0, true, "table pole zero fee free" },
        { 1, 0, 0, true, "table pole zero fee with gil" },
        { 0, 0, 1, false, "table pole empty gil non-zero fee" },
        { 100, 1, 50, false, "table pole reserve blocks afford" },
        { 50, 1, 50, false, "table pole reserve blocks exact" },
        { 0, 1, 0, false, "table pole reserve blocks zero fee" },
        { 1000, 100, 1, false, "table pole reserve large with gil" },
        { 1, 0, 1, true, "table pole qty one fee one" },
        // uint32 edges
        { 0xFFFFFFFFu, 0, 0xFFFFFFFFu, true, "table pole max uint32 exact" },
        { 0xFFFFFFFEu, 0, 0xFFFFFFFFu, false, "table pole max uint32 short" },
        { 0xFFFFFFFFu, 1, 0, false, "table pole max qty reserve blocks" },
        { 0xFFFFFFFFu, 0, 0, true, "table pole max qty zero fee" },
        // residual 2924 / prior dedicated 3280 / 3310 / 3503 / 3574 / 3619 / 3664 / 3709 / 3754 / 3799 / 3844 re-pins
        { 100, 0, 50, true, "table residual afford mid" },
        { 49, 0, 50, false, "table residual short gil" },
        { 100, 1, 50, false, "table residual reserve blocks" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanAffordFee(c.gilQuantity, c.gilReserve, c.fee);
        const bool inlineC = inlineCanAffordFee(c.gilQuantity, c.gilReserve, c.fee);
        const bool pinGot  = pinCanAffordFee(c.gilQuantity, c.gilReserve, c.fee);
        const bool pin3844 = pinCanAffordFee3844(c.gilQuantity, c.gilReserve, c.fee);

        ok = expect(got == c.want, c.label) && ok;
        // Positive form: free == inline == pin == pin3844.
        ok = expect(got == inlineC && got == pinGot && got == pin3844, "dual-wire free==inline==pin==pin3844") && ok;
    }

    // Free == pin across residual poles (multi-branch positive if/else).
    const struct
    {
        uint32      gilQuantity;
        uint32      gilReserve;
        uint32      fee;
        const char* label;
    } poles[] = {
        { 50, 0, 50, "exact" },
        { 49, 0, 50, "short" },
        { 51, 0, 50, "surplus" },
        { 0, 0, 0, "zero fee free" },
        { 0, 0, 1, "empty non-zero fee" },
        { 100, 1, 50, "reserve block" },
        { 0, 1, 0, "reserve zero fee" },
        { 0xFFFFFFFFu, 0, 0xFFFFFFFFu, "max exact" },
        { 0xFFFFFFFEu, 0, 0xFFFFFFFFu, "max short" },
    };
    for (const auto& pole : poles)
    {
        const bool got     = CanAffordFee(pole.gilQuantity, pole.gilReserve, pole.fee);
        const bool inlineC = inlineCanAffordFee(pole.gilQuantity, pole.gilReserve, pole.fee);
        const bool pinGot  = pinCanAffordFee(pole.gilQuantity, pole.gilReserve, pole.fee);
        const bool pin3844 = pinCanAffordFee3844(pole.gilQuantity, pole.gilReserve, pole.fee);
        ok                 = expect(got == inlineC && got == pinGot && got == pin3844, "pole free==inline==pin==pin3844") && ok;
    }

    // Dense compose: free == inline == pin == pin3844 over required poles + neighbors.
    const struct
    {
        uint32 gilQuantity;
        uint32 gilReserve;
        uint32 fee;
    } dense[] = {
        // zero fee neighbors
        { 0, 0, 0 },
        { 1, 0, 0 },
        { 0, 1, 0 },
        { 1, 1, 0 },
        // fee=1 boundary
        { 0, 0, 1 },
        { 1, 0, 1 },
        { 2, 0, 1 },
        { 1, 1, 1 },
        // fee=50 boundary
        { 49, 0, 50 },
        { 50, 0, 50 },
        { 51, 0, 50 },
        { 100, 0, 50 },
        { 49, 1, 50 },
        { 50, 1, 50 },
        { 100, 1, 50 },
        // reserve poles
        { 1000, 0, 1 },
        { 1000, 1, 1 },
        { 1000, 100, 1 },
        // uint32 edges
        { 0xFFFFFFFFu, 0, 0xFFFFFFFFu },
        { 0xFFFFFFFEu, 0, 0xFFFFFFFFu },
        { 0xFFFFFFFFu, 1, 0 },
        { 0xFFFFFFFFu, 0, 0 },
        { 0, 0, 0xFFFFFFFFu },
        { 1, 0, 0xFFFFFFFFu },
    };
    for (const auto& p : dense)
    {
        const bool got     = CanAffordFee(p.gilQuantity, p.gilReserve, p.fee);
        const bool want    = p.gilQuantity >= p.fee && p.gilReserve == 0;
        const bool pin3844 = pinCanAffordFee3844(p.gilQuantity, p.gilReserve, p.fee);
        ok                 = expect(got == want, "dense free == formula") && ok;
        ok                 = expect(got == inlineCanAffordFee(p.gilQuantity, p.gilReserve, p.fee), "dense free == inline") && ok;
        ok                 = expect(got == pinCanAffordFee(p.gilQuantity, p.gilReserve, p.fee), "dense free == pin multi-branch") && ok;
        ok                 = expect(got == pin3844, "dense free == pin3844") && ok;
    }

    // Production ProofOfPurchase path semantics:
    // Afford → may continue listing-limit / DB insert path.
    // Short gil or reserved gil → LotIn result 197 ("Not enough gil to pay fee").
    ok = expect(CanAffordFee(100, 0, 50), "ProofOfPurchase afford → continue path") && ok;
    ok = expect(CanAffordFee(50, 0, 50), "ProofOfPurchase exact → continue path") && ok;
    ok = expect(!CanAffordFee(49, 0, 50), "ProofOfPurchase short gil → 197 path") && ok;
    ok = expect(!CanAffordFee(100, 1, 50), "ProofOfPurchase reserve → 197 path") && ok;
    ok = expect(CanAffordFee(0, 0, 0), "ProofOfPurchase zero fee free → continue path") && ok;
    ok = expect(!CanAffordFee(0, 1, 0), "ProofOfPurchase reserve zero fee → 197 path") && ok;

    return ok;
}
