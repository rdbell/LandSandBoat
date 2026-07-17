#include "test_assault_proceed_update_2863.h"

#include "map/assault_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "assault proceed update 2863 self-test failed: " << label << '\n';
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

} // namespace

// Pure dual-wire expansion for assaulthelpers::ShouldProceedAssaultUpdate
// (Lua onAssaultUpdate party/alliance proceed gate after AssaultCap store).
auto runAssaultProceedUpdate2863SelfTests() -> bool
{
    using assaulthelpers::IsAllianceBlocked;
    using assaulthelpers::PartyTooSmallForAssault;
    using assaulthelpers::ShouldProceedAssaultUpdate;
    using assaulthelpers::kAllianceBlocked;

    bool ok = true;

    ok = expect(kAllianceBlocked == 2, "kAllianceBlocked pin") && ok;

    // --- Component gates ---
    ok = expect(PartyTooSmallForAssault(0, 0, 1), "too small default min") && ok;
    ok = expect(!PartyTooSmallForAssault(0, 1, 1), "meets default min") && ok;
    ok = expect(!PartyTooSmallForAssault(1, 0, 3), "GM bypass") && ok;
    ok = expect(PartyTooSmallForAssault(0, 2, 3), "TOAU min undersized") && ok;

    ok = expect(IsAllianceBlocked(2), "spa alliance") && ok;
    ok = expect(!IsAllianceBlocked(0), "spa solo") && ok;
    ok = expect(!IsAllianceBlocked(1), "spa party") && ok;
    ok = expect(!IsAllianceBlocked(3), "spa other") && ok;

    // --- ShouldProceedAssaultUpdate table ---
    const struct
    {
        int32       gm;
        int32       size;
        int32       min;
        int32       spa;
        bool        want;
        const char* label;
    } cases[] = {
        { 0, 1, 1, 0, true, "solo meets default min" },
        { 0, 3, 1, 0, true, "party meets default min" },
        { 0, 0, 1, 0, false, "empty party too small" },
        { 0, 3, 3, 0, true, "TOAU min3 exact" },
        { 0, 2, 3, 0, false, "TOAU min3 undersized" },
        { 1, 0, 3, 0, true, "GM empty party ok" },
        { 5, 0, 3, 0, true, "GM5 empty party ok" },
        { 0, 6, 1, 2, false, "full alliance blocked" },
        { 5, 0, 3, 2, false, "GM still alliance blocked" },
        { 0, 2, 1, 1, true, "party spa=1 ok" },
        { 0, 1, 1, 3, true, "spa!=2 not blocked" },
        { 0, 0, 0, 0, true, "min0 size0 ok" },
        { 0, -1, 0, 0, false, "negative size < min0" },
    };

    for (const auto& c : cases)
    {
        const bool got      = ShouldProceedAssaultUpdate(c.gm, c.size, c.min, c.spa);
        const bool pure     = !PartyTooSmallForAssault(c.gm, c.size, c.min) && !IsAllianceBlocked(c.spa);
        const bool inlineF  = inlineShouldProceed(c.gm, c.size, c.min, c.spa);
        const bool tooSmall = PartyTooSmallForAssault(c.gm, c.size, c.min);
        const bool blocked  = IsAllianceBlocked(c.spa);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == pure, "dual-wire free == !tooSmall && !blocked") && ok;
        ok = expect(got == inlineF, "dual-wire free == inline Lua formula") && ok;
        ok = expect(tooSmall == inlinePartyTooSmall(c.gm, c.size, c.min), "PartyTooSmall dual-wire") && ok;
        ok = expect(blocked == inlineAllianceBlocked(c.spa), "IsAllianceBlocked dual-wire") && ok;
    }

    // Residual 1100 compose pins.
    ok = expect(ShouldProceedAssaultUpdate(0, 3, 1, 0), "residual normal party") && ok;
    ok = expect(!ShouldProceedAssaultUpdate(0, 0, 1, 0), "residual too small") && ok;
    ok = expect(!ShouldProceedAssaultUpdate(0, 3, 1, 2), "residual alliance") && ok;
    ok = expect(ShouldProceedAssaultUpdate(5, 0, 3, 0), "residual GM small party") && ok;

    return ok;
}
