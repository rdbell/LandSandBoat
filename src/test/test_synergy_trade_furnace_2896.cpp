#include "test_synergy_trade_furnace_2896.h"

#include "map/synergy_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "synergy trade furnace 2896 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua CLAIMED+claimedByYou gate for dual-wire checks:
// furnaceState == furnaceStates.CLAIMED  -- CLAIMED = 1
// and furnacePlayerID ~= 0 and furnacePlayerID == playerID
auto inlineCanTradeIntoFurnace(const uint8 state, const uint32 furnacePlayerID, const uint32 playerID) -> bool
{
    return state == synergyhelpers::FurnaceClaimed && synergyhelpers::IsClaimedBy(furnacePlayerID, playerID);
}

} // namespace

// Pure dual-wire expansion for synergyhelpers::CanTradeIntoFurnace
// (Lua synergyFurnaceOnTrade CLAIMED+owner gate).
auto runSynergyTradeFurnace2896SelfTests() -> bool
{
    using synergyhelpers::CanTradeIntoFurnace;
    using synergyhelpers::FurnaceClaimed;
    using synergyhelpers::IsClaimedBy;

    bool ok = true;

    // FurnaceClaimed pin matches Go/Lua enum value 1.
    ok = expect(FurnaceClaimed == 1, "FurnaceClaimed == 1") && ok;

    // IsClaimedBy residual 1149 pins.
    ok = expect(IsClaimedBy(42, 42), "IsClaimedBy matching non-zero") && ok;
    ok = expect(!IsClaimedBy(0, 0), "IsClaimedBy zero/zero false") && ok;
    ok = expect(!IsClaimedBy(0, 1), "IsClaimedBy zero furnace false") && ok;
    ok = expect(!IsClaimedBy(1, 2), "IsClaimedBy mismatch false") && ok;

    const struct
    {
        uint8       state;
        uint32      furnacePlayerID;
        uint32      playerID;
        bool        want;
        const char* label;
    } cases[] = {
        { 1, 1, 1, true, "CLAIMED + owner allows trade" },
        { 1, 9, 9, true, "CLAIMED + matching non-zero IDs allow trade" },
        { 0, 1, 1, false, "AVAILABLE blocks trade" },
        { 2, 1, 1, false, "ACTIVE blocks trade" },
        { 3, 1, 1, false, "COMPLETED blocks trade" },
        { 1, 1, 2, false, "CLAIMED + non-owner blocks trade" },
        { 1, 0, 0, false, "CLAIMED + zero IDs block trade" },
        { 1, 0, 1, false, "CLAIMED + zero furnace player blocks trade" },
        { 4, 1, 1, false, "unknown high state blocks trade" },
        { 255, 1, 1, false, "max uint8 state blocks trade" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanTradeIntoFurnace(c.state, c.furnacePlayerID, c.playerID);
        const bool inlineF = inlineCanTradeIntoFurnace(c.state, c.furnacePlayerID, c.playerID);
        const bool pure    = c.state == FurnaceClaimed && IsClaimedBy(c.furnacePlayerID, c.playerID);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == pure, "dual-wire free == formula") && ok;
        ok = expect(got == inlineF, "dual-wire free == inline Lua formula") && ok;
    }

    // Residual 1149 pins.
    ok = expect(CanTradeIntoFurnace(FurnaceClaimed, 9, 9), "residual CLAIMED+owner allows trade") && ok;
    ok = expect(!CanTradeIntoFurnace(0, 9, 9), "residual AVAILABLE blocks trade") && ok;
    ok = expect(!CanTradeIntoFurnace(FurnaceClaimed, 9, 8), "residual non-owner blocks trade") && ok;
    ok = expect(!CanTradeIntoFurnace(2, 9, 9), "residual ACTIVE blocks trade") && ok;

    // Dense compose range identity (furnaceStates 0..3 + out-of-range) × ownership.
    const struct
    {
        uint32 furnacePlayerID;
        uint32 playerID;
    } pairs[] = {
        { 0, 0 },
        { 0, 1 },
        { 1, 1 },
        { 1, 2 },
        { 9, 9 },
        { 42, 7 },
    };
    for (uint16 state = 0; state <= 8; ++state)
    {
        const uint8 s = static_cast<uint8>(state);
        for (const auto& p : pairs)
        {
            const bool got  = CanTradeIntoFurnace(s, p.furnacePlayerID, p.playerID);
            const bool want = s == FurnaceClaimed && IsClaimedBy(p.furnacePlayerID, p.playerID);
            ok = expect(got == want, "compose range free == formula") && ok;
            ok = expect(got == inlineCanTradeIntoFurnace(s, p.furnacePlayerID, p.playerID),
                        "compose range free == inline") &&
                 ok;
        }
    }

    return ok;
}
