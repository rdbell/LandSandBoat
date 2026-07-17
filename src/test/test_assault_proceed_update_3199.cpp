#include "test_assault_proceed_update_3199.h"

#include "map/assault_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "assault proceed update 3199 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua onAssaultUpdate party-size gate for dual-wire checks:
// getGMLevel() == 0 and getPartySize() < ASSAULT_MINIMUM
auto inlinePartyTooSmall(const int32 gmLevel, const int32 partySize, const int32 assaultMinimum) -> bool
{
    return gmLevel == 0 && partySize < assaultMinimum;
}

// Inline Lua alliance gate: checkSoloPartyAlliance() == 2
auto inlineAllianceBlocked(const int32 checkSoloPartyAlliance) -> bool
{
    return checkSoloPartyAlliance == 2;
}

// Inline inverted early-return shape (proceed when neither gate fires).
auto inlineShouldProceed(const int32 gmLevel,
                         const int32 partySize,
                         const int32 assaultMinimum,
                         const int32 checkSoloPartyAlliance) -> bool
{
    if (inlinePartyTooSmall(gmLevel, partySize, assaultMinimum))
    {
        return false;
    }
    if (inlineAllianceBlocked(checkSoloPartyAlliance))
    {
        return false;
    }
    return true;
}

// Compact dual-wire pin matching Go pinShouldProceedAssaultUpdate3199:
//   !PartyTooSmall && !IsAllianceBlocked  (positive form)
auto pinShouldProceed(const int32 gmLevel,
                      const int32 partySize,
                      const int32 assaultMinimum,
                      const int32 checkSoloPartyAlliance) -> bool
{
    return !inlinePartyTooSmall(gmLevel, partySize, assaultMinimum) &&
           !inlineAllianceBlocked(checkSoloPartyAlliance);
}

} // namespace

// Pure dual-wire expansion for assaulthelpers::ShouldProceedAssaultUpdate
// (Lua onAssaultUpdate party/alliance proceed gate; OmegaXI
// internal/assault; slice 3199).
//
// Coverage:
//   - free == inline == pin positive form (!PartyTooSmall && !AllianceBlocked)
//   - too-small / alliance blocked early returns
//   - GM size bypass still alliance-blocked
//   - residual 1100 / 2863 pins still hold
auto runAssaultProceedUpdate3199SelfTests() -> bool
{
    using assaulthelpers::IsAllianceBlocked;
    using assaulthelpers::PartyTooSmallForAssault;
    using assaulthelpers::ShouldProceedAssaultUpdate;
    using assaulthelpers::kAllianceBlocked;

    bool ok = true;

    // Catalog pins (match Go AllianceBlocked / residual hosts).
    ok = expect(kAllianceBlocked == 2, "kAllianceBlocked pin") && ok;

    // Residual 1100 / 2863 ShouldProceedAssaultUpdate pins still hold under dual-wire.
    ok = expect(ShouldProceedAssaultUpdate(0, 3, 1, 0), "residual: normal party proceed") && ok;
    ok = expect(!ShouldProceedAssaultUpdate(0, 0, 1, 0), "residual: empty party too small") && ok;
    ok = expect(!ShouldProceedAssaultUpdate(0, 3, 1, 2), "residual: alliance blocked") && ok;
    ok = expect(ShouldProceedAssaultUpdate(5, 0, 3, 0), "residual: GM small party ok") && ok;
    ok = expect(!ShouldProceedAssaultUpdate(5, 0, 3, 2), "residual: GM still alliance-blocked") && ok;

    // Residual component gates.
    ok = expect(PartyTooSmallForAssault(0, 0, 1), "too small default min") && ok;
    ok = expect(!PartyTooSmallForAssault(0, 1, 1), "meets default min") && ok;
    ok = expect(!PartyTooSmallForAssault(1, 0, 3), "GM bypass") && ok;
    ok = expect(PartyTooSmallForAssault(0, 2, 3), "TOAU min undersized") && ok;
    ok = expect(IsAllianceBlocked(2), "spa alliance") && ok;
    ok = expect(!IsAllianceBlocked(0), "spa solo") && ok;
    ok = expect(!IsAllianceBlocked(1), "spa party") && ok;

    // --- Eligible proceed path ---
    ok = expect(ShouldProceedAssaultUpdate(0, 1, 1, 0), "eligible solo default min") && ok;
    ok = expect(ShouldProceedAssaultUpdate(0, 3, 1, 0), "eligible party default min") && ok;
    ok = expect(ShouldProceedAssaultUpdate(0, 3, 3, 0), "eligible TOAU min3 exact") && ok;
    ok = expect(ShouldProceedAssaultUpdate(5, 0, 3, 0), "eligible GM empty party") && ok;

    // --- Blocked paths ---
    ok = expect(!ShouldProceedAssaultUpdate(0, 0, 1, 0), "empty party blocks") && ok;
    ok = expect(!ShouldProceedAssaultUpdate(0, 2, 3, 0), "TOAU undersized blocks") && ok;
    ok = expect(!ShouldProceedAssaultUpdate(0, 6, 1, 2), "full alliance blocks") && ok;
    ok = expect(!ShouldProceedAssaultUpdate(5, 0, 3, 2), "GM alliance blocks") && ok;

    // --- Composition table: free == inline == pin == compose ---
    const struct
    {
        int32       gm;
        int32       size;
        int32       min;
        int32       spa;
        bool        want;
        const char* label;
    } cases[] = {
        { 0, 1, 1, 0, true, "table solo meets default min" },
        { 0, 3, 1, 0, true, "table party meets default min" },
        { 0, 0, 1, 0, false, "table empty party too small" },
        { 0, 3, 3, 0, true, "table TOAU min3 exact" },
        { 0, 2, 3, 0, false, "table TOAU min3 undersized" },
        { 1, 0, 3, 0, true, "table GM empty party ok" },
        { 5, 0, 3, 0, true, "table GM5 empty party ok" },
        { 0, 6, 1, 2, false, "table full alliance blocked" },
        { 5, 0, 3, 2, false, "table GM still alliance blocked" },
        { 0, 2, 1, 1, true, "table party spa=1 ok" },
        { 0, 1, 1, 3, true, "table spa!=2 not blocked" },
        { 0, 0, 0, 0, true, "table min0 size0 ok" },
        { 0, -1, 0, 0, false, "table negative size < min0" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldProceedAssaultUpdate(c.gm, c.size, c.min, c.spa);
        const bool pure    = !PartyTooSmallForAssault(c.gm, c.size, c.min) && !IsAllianceBlocked(c.spa);
        const bool inlineF = inlineShouldProceed(c.gm, c.size, c.min, c.spa);
        const bool pinGot  = pinShouldProceed(c.gm, c.size, c.min, c.spa);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == pure, "dual-wire free == !tooSmall && !blocked") && ok;
        ok = expect(got == inlineF, "dual-wire free == inline Lua formula") && ok;
        ok = expect(got == pinGot, "dual-wire free == pin positive form") && ok;
        ok = expect(PartyTooSmallForAssault(c.gm, c.size, c.min) == inlinePartyTooSmall(c.gm, c.size, c.min),
                    "PartyTooSmall dual-wire") &&
             ok;
        ok = expect(IsAllianceBlocked(c.spa) == inlineAllianceBlocked(c.spa), "IsAllianceBlocked dual-wire") && ok;
    }

    // --- Production onAssaultUpdate path semantics ---
    // Eligible → may continue xi.instance.onEventUpdate.
    // Blocked → message / instanceEntry early return.
    ok = expect(ShouldProceedAssaultUpdate(0, 3, 1, 0), "onAssaultUpdate eligible → continue path") && ok;
    ok = expect(!ShouldProceedAssaultUpdate(0, 0, 1, 0), "onAssaultUpdate too small → blocked") && ok;
    ok = expect(!ShouldProceedAssaultUpdate(0, 3, 1, 2), "onAssaultUpdate alliance → blocked") && ok;
    ok = expect(ShouldProceedAssaultUpdate(5, 0, 3, 0), "onAssaultUpdate GM size bypass → continue") && ok;
    ok = expect(!ShouldProceedAssaultUpdate(5, 0, 3, 2), "onAssaultUpdate GM alliance → blocked") && ok;

    // Dense compose identity over representative tuples.
    const struct
    {
        int32 gm;
        int32 size;
        int32 min;
        int32 spa;
    } composeArgs[] = {
        { 0, 3, 1, 0 },
        { 0, 0, 1, 0 },
        { 0, 3, 1, 2 },
        { 5, 0, 3, 0 },
        { 5, 0, 3, 2 },
        { 0, 2, 3, 0 },
        { 0, 3, 3, 0 },
        { 1, 0, 3, 2 },
        { 0, 1, 1, 0 },
        { 0, 1, 1, 1 },
        { 0, 1, 1, 3 },
        { 0, 0, 0, 0 },
        { 0, -1, 0, 0 },
    };
    for (const auto& a : composeArgs)
    {
        const bool got  = ShouldProceedAssaultUpdate(a.gm, a.size, a.min, a.spa);
        const bool want = !PartyTooSmallForAssault(a.gm, a.size, a.min) && !IsAllianceBlocked(a.spa);
        ok              = expect(got == want, "compose !tooSmall&&!blocked") && ok;
        ok              = expect(got == inlineShouldProceed(a.gm, a.size, a.min, a.spa), "compose inline") && ok;
        ok              = expect(got == pinShouldProceed(a.gm, a.size, a.min, a.spa), "compose pin") && ok;
    }

    // Free == pin positive form at residual poles.
    ok = expect(ShouldProceedAssaultUpdate(0, 3, 1, 0) == pinShouldProceed(0, 3, 1, 0),
                "free==pin normal party") &&
         ok;
    ok = expect(ShouldProceedAssaultUpdate(0, 0, 1, 0) == pinShouldProceed(0, 0, 1, 0),
                "free==pin too small") &&
         ok;
    ok = expect(ShouldProceedAssaultUpdate(0, 3, 1, 2) == pinShouldProceed(0, 3, 1, 2),
                "free==pin alliance") &&
         ok;

    return ok;
}
