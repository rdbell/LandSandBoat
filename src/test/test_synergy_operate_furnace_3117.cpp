#include "test_synergy_operate_furnace_3117.h"

#include "map/synergy_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "synergy operate furnace 3117 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua claimedByYou gate for dual-wire checks (slice 3117):
// furnacePlayerID ~= 0 and furnacePlayerID == playerID
auto inlineCanOperateFurnace(const uint32 furnacePlayerID, const uint32 playerID) -> bool
{
    return synergyhelpers::IsClaimedBy(furnacePlayerID, playerID);
}

// Compact dual-wire pin matching C++ capacity formula:
//   IsClaimedBy(furnacePlayerID, playerID)
auto pinCanOperateFurnace(const uint32 furnacePlayerID, const uint32 playerID) -> bool
{
    return synergyhelpers::IsClaimedBy(furnacePlayerID, playerID);
}

} // namespace

// Pure dual-wire expansion for synergyhelpers::CanOperateFurnace
// (Lua synergyFurnaceOnTrigger claimedByYou operate gate; slice 3117).
auto runSynergyOperateFurnace3117SelfTests() -> bool
{
    using synergyhelpers::CanOperateFurnace;
    using synergyhelpers::IsClaimedBy;

    bool ok = true;

    // Residual 1149 / 2899 pins still hold under dual-wire.
    ok = expect(IsClaimedBy(42, 42), "residual IsClaimedBy matching non-zero") && ok;
    ok = expect(!IsClaimedBy(0, 0), "residual IsClaimedBy zero/zero false") && ok;
    ok = expect(!IsClaimedBy(0, 1), "residual IsClaimedBy zero furnace false") && ok;
    ok = expect(!IsClaimedBy(1, 2), "residual IsClaimedBy mismatch false") && ok;

    ok = expect(CanOperateFurnace(5, 5), "residual matching allows operate") && ok;
    ok = expect(!CanOperateFurnace(5, 6), "residual mismatch blocks operate") && ok;
    ok = expect(!CanOperateFurnace(0, 0), "residual zero/zero blocks operate") && ok;

    // Owner match true; other poles false — free == inline == pin == IsClaimedBy.
    const struct
    {
        uint32      furnacePlayerID;
        uint32      playerID;
        bool        want;
        const char* label;
    } cases[] = {
        { 1, 1, true, "matching non-zero IDs allow operate" },
        { 9, 9, true, "matching non-zero pair allows operate" },
        { 42, 42, true, "matching 42 allows operate" },
        { 1, 2, false, "non-matching IDs block operate" },
        { 9, 8, false, "non-owner blocks operate" },
        { 0, 0, false, "zero/zero blocks operate" },
        { 0, 1, false, "zero furnace player blocks operate" },
        { 1, 0, false, "zero player vs non-zero furnace blocks operate" },
        { 5, 6, false, "adjacent mismatch blocks operate" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanOperateFurnace(c.furnacePlayerID, c.playerID);
        const bool inlineF = inlineCanOperateFurnace(c.furnacePlayerID, c.playerID);
        const bool pinGot  = pinCanOperateFurnace(c.furnacePlayerID, c.playerID);
        const bool pure    = IsClaimedBy(c.furnacePlayerID, c.playerID);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == pure, "dual-wire free == IsClaimedBy") && ok;
        ok = expect(got == inlineF, "dual-wire free == inline Lua formula") && ok;
        ok = expect(got == pinGot, "dual-wire free == C++ pin formula") && ok;
    }

    // Dense compose identity across ownership pairs.
    const struct
    {
        uint32 furnacePlayerID;
        uint32 playerID;
    } pairs[] = {
        { 0, 0 },
        { 0, 1 },
        { 1, 0 },
        { 1, 1 },
        { 1, 2 },
        { 9, 9 },
        { 9, 8 },
        { 42, 7 },
        { 42, 42 },
        { 0xFFFFFFFFu, 0xFFFFFFFFu },
        { 0xFFFFFFFFu, 1 },
    };
    for (const auto& p : pairs)
    {
        const bool got     = CanOperateFurnace(p.furnacePlayerID, p.playerID);
        const bool want    = p.furnacePlayerID != 0 && p.furnacePlayerID == p.playerID;
        const bool inlineF = inlineCanOperateFurnace(p.furnacePlayerID, p.playerID);
        const bool pinGot  = pinCanOperateFurnace(p.furnacePlayerID, p.playerID);

        ok = expect(got == want, "compose free == formula") && ok;
        ok = expect(got == IsClaimedBy(p.furnacePlayerID, p.playerID), "compose free == IsClaimedBy") && ok;
        ok = expect(got == inlineF, "compose free == inline") && ok;
        ok = expect(got == pinGot, "compose free == pin") && ok;
    }

    // Explicit composition pins matching C++ capacity.
    ok = expect(CanOperateFurnace(1, 1) == true, "compose matching non-zero") && ok;
    ok = expect(CanOperateFurnace(1, 2) == false, "compose non-owner") && ok;
    ok = expect(CanOperateFurnace(0, 0) == false, "compose zero/zero") && ok;
    ok = expect(CanOperateFurnace(0, 1) == false, "compose zero furnace") && ok;
    ok = expect(CanOperateFurnace(1, 0) == false, "compose zero player") && ok;

    return ok;
}
