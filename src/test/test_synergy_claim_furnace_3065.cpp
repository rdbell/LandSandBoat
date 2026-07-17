#include "test_synergy_claim_furnace_3065.h"

#include "map/synergy_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "synergy claim furnace 3065 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua AVAILABLE gate for dual-wire checks (slice 3065):
// furnaceState == furnaceStates.AVAILABLE  -- AVAILABLE = 0
auto inlineCanClaimFurnace(const uint8 state) -> bool
{
    return state == synergyhelpers::FurnaceAvailable;
}

// Compact dual-wire pin matching C++ capacity formula:
//   state == FurnaceAvailable
auto pinCanClaimFurnace(const uint8 state) -> bool
{
    return state == synergyhelpers::FurnaceAvailable;
}

} // namespace

// Pure dual-wire expansion for synergyhelpers::CanClaimFurnace
// (Lua synergyFurnaceOnTrigger AVAILABLE claim gate; slice 3065).
auto runSynergyClaimFurnace3065SelfTests() -> bool
{
    using synergyhelpers::CanClaimFurnace;
    using synergyhelpers::FurnaceAvailable;

    bool ok = true;

    // FurnaceAvailable pin matches Go/Lua enum value 0.
    ok = expect(FurnaceAvailable == 0, "FurnaceAvailable == 0") && ok;

    // Residual 1149 / 2877 pins still hold under dual-wire.
    ok = expect(CanClaimFurnace(FurnaceAvailable), "residual AVAILABLE allows claim") && ok;
    ok = expect(!CanClaimFurnace(1), "residual CLAIMED blocks claim") && ok;
    ok = expect(!CanClaimFurnace(2), "residual ACTIVE blocks claim") && ok;
    ok = expect(!CanClaimFurnace(3), "residual COMPLETED blocks claim") && ok;

    // AVAILABLE true; other states false — free == inline == pin.
    const struct
    {
        uint8       state;
        bool        want;
        const char* label;
    } cases[] = {
        { 0, true, "AVAILABLE allows claim" },
        { 1, false, "CLAIMED blocks claim" },
        { 2, false, "ACTIVE blocks claim" },
        { 3, false, "COMPLETED blocks claim" },
        { 4, false, "unknown high state blocks claim" },
        { 255, false, "max uint8 state blocks claim" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanClaimFurnace(c.state);
        const bool inlineF = inlineCanClaimFurnace(c.state);
        const bool pinGot  = pinCanClaimFurnace(c.state);
        const bool pure    = c.state == FurnaceAvailable;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == pure, "dual-wire free == state == AVAILABLE") && ok;
        ok = expect(got == inlineF, "dual-wire free == inline Lua formula") && ok;
        ok = expect(got == pinGot, "dual-wire free == C++ pin formula") && ok;
    }

    // Dense compose range identity (furnaceStates 0..3 + out-of-range).
    for (uint16 state = 0; state <= 8; ++state)
    {
        const uint8 s       = static_cast<uint8>(state);
        const bool  got     = CanClaimFurnace(s);
        const bool  want    = s == FurnaceAvailable;
        const bool  inlineF = inlineCanClaimFurnace(s);
        const bool  pinGot  = pinCanClaimFurnace(s);

        ok = expect(got == want, "compose range free == state==AVAILABLE") && ok;
        ok = expect(got == inlineF, "compose range free == inline") && ok;
        ok = expect(got == pinGot, "compose range free == pin") && ok;
    }

    // Explicit composition pins matching C++ capacity.
    ok = expect(CanClaimFurnace(0) == true, "compose AVAILABLE") && ok;
    ok = expect(CanClaimFurnace(1) == false, "compose CLAIMED") && ok;
    ok = expect(CanClaimFurnace(2) == false, "compose ACTIVE") && ok;
    ok = expect(CanClaimFurnace(3) == false, "compose COMPLETED") && ok;

    return ok;
}
