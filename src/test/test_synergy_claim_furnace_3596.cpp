#include "test_synergy_claim_furnace_3596.h"

#include "map/synergy_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "synergy claim furnace 3596 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua AVAILABLE gate for dual-wire checks
// (slice 3596 dedicated expand residual 2877; prior dedicated expand 3551):
// furnaceState == furnaceStates.AVAILABLE  -- AVAILABLE = 0
// Direct return form matching production free function / capacity.
auto inlineCanClaimFurnace(const uint8 state) -> bool
{
    return state == synergyhelpers::FurnaceAvailable;
}

// Compact dual-wire pin matching free function / capacity body (slice 3596).
// Direct return only — same formula as production CanClaimFurnace.
auto pinCanClaimFurnace3596(const uint8 state) -> bool
{
    return state == synergyhelpers::FurnaceAvailable;
}

// Prior dedicated expand 3551 pin (retained for independence check).
auto pinCanClaimFurnace3551(const uint8 state) -> bool
{
    return state == synergyhelpers::FurnaceAvailable;
}

// Prior dedicated expand 3494 pin (retained for independence check).
auto pinCanClaimFurnace3494(const uint8 state) -> bool
{
    return state == synergyhelpers::FurnaceAvailable;
}

// Prior dedicated expand 3315 pin (retained for independence check).
auto pinCanClaimFurnace3315(const uint8 state) -> bool
{
    return state == synergyhelpers::FurnaceAvailable;
}

// Prior dedicated expand 3285 pin (retained for independence check).
auto pinCanClaimFurnace3285(const uint8 state) -> bool
{
    return state == synergyhelpers::FurnaceAvailable;
}

// Prior dedicated expand 3239 pin (retained for independence check).
auto pinCanClaimFurnace3239(const uint8 state) -> bool
{
    return state == synergyhelpers::FurnaceAvailable;
}

// Prior dedicated 3065 pin (retained for independence check).
auto pinCanClaimFurnace3065(const uint8 state) -> bool
{
    return state == synergyhelpers::FurnaceAvailable;
}

} // namespace

// Pure dual-wire expansion for synergyhelpers::CanClaimFurnace
// (Lua synergyFurnaceOnTrigger AVAILABLE claim gate; slice 3596 dedicated
// expand residual 2877; prior dedicated expand 3551). Formula unchanged:
//
//   CanClaimFurnace(state) = state == FurnaceAvailable
//
// Coverage:
//   - free == inline == pin (direct return formula)
//   - residual 2877 / 1149 / prior dedicated 3065 / prior expand 3239 /
//     prior expand 3285 / prior expand 3315 / prior expand 3494 /
//     prior expand 3551 poles
//   - residual poles for all furnaceStates + dense 0..8 out-of-range
//   - prior 3551 pin independence
auto runSynergyClaimFurnace3596SelfTests() -> bool
{
    using synergyhelpers::CanClaimFurnace;
    using synergyhelpers::FurnaceAvailable;

    bool ok = true;

    // FurnaceAvailable pin matches Go/Lua enum value 0.
    ok = expect(FurnaceAvailable == 0, "FurnaceAvailable == 0") && ok;

    // Residual 1149 / 2877 / prior dedicated 3065 / prior expand 3239 /
    // prior expand 3285 / prior expand 3315 / prior expand 3494 / prior
    // expand 3551 pins.
    ok = expect(CanClaimFurnace(FurnaceAvailable), "residual AVAILABLE allows claim") && ok;
    ok = expect(!CanClaimFurnace(1), "residual CLAIMED blocks claim") && ok;
    ok = expect(!CanClaimFurnace(2), "residual ACTIVE blocks claim") && ok;
    ok = expect(!CanClaimFurnace(3), "residual COMPLETED blocks claim") && ok;
    ok = expect(!CanClaimFurnace(4), "residual unknown high state blocks claim") && ok;
    ok = expect(!CanClaimFurnace(255), "residual max uint8 state blocks claim") && ok;

    // Residual poles for all furnaceStates + out-of-range:
    // free == inline == pin (direct return).
    const struct
    {
        uint8       state;
        bool        want;
        const char* label;
    } poles[] = {
        // All named furnaceStates (residual 2877 / prior 3065 / 3239 / 3285 / 3315 / 3494 / 3551).
        { 0, true, "residual AVAILABLE allows claim" },
        { 1, false, "residual CLAIMED blocks claim" },
        { 2, false, "residual ACTIVE blocks claim" },
        { 3, false, "residual COMPLETED blocks claim" },

        // Prior dedicated / expand out-of-range poles.
        { 4, false, "prior unknown high state blocks claim" },
        { 255, false, "prior max uint8 state blocks claim" },

        // Host inject path poles (synergyFurnaceOnTrigger AVAILABLE branch).
        { 0, true, "host AVAILABLE → CLAIM_SET + attach" },
        { 1, false, "host CLAIMED → other handleFurnaceState" },
        { 2, false, "host ACTIVE → other handleFurnaceState" },
        { 3, false, "host COMPLETED → other handleFurnaceState" },
    };

    for (const auto& p : poles)
    {
        const bool got     = CanClaimFurnace(p.state);
        const bool inlineF = inlineCanClaimFurnace(p.state);
        const bool pin     = pinCanClaimFurnace3596(p.state);
        const bool pure    = p.state == FurnaceAvailable;

        ok = expect(got == p.want, p.label) && ok;
        ok = expect(got == pure, "dual-wire free == state == AVAILABLE") && ok;
        ok = expect(got == inlineF, "dual-wire free == inline Lua formula") && ok;
        ok = expect(got == pin, "dual-wire free == C++ pin formula") && ok;
        ok = expect(got == pinCanClaimFurnace3065(p.state), "free == residual pinCanClaimFurnace3065") && ok;
        ok = expect(got == pinCanClaimFurnace3239(p.state), "free == prior pinCanClaimFurnace3239") && ok;
        ok = expect(got == pinCanClaimFurnace3285(p.state), "free == prior pinCanClaimFurnace3285") && ok;
        ok = expect(got == pinCanClaimFurnace3315(p.state), "free == prior pinCanClaimFurnace3315") && ok;
        ok = expect(got == pinCanClaimFurnace3494(p.state), "free == prior pinCanClaimFurnace3494") && ok;
        ok = expect(got == pinCanClaimFurnace3551(p.state), "free == prior pinCanClaimFurnace3551") && ok;
    }

    // Dense furnaceStates 0..3 + out-of-range poles:
    // free == inline == pin == (state == AVAILABLE).
    for (uint16 state = 0; state <= 8; ++state)
    {
        const uint8 s       = static_cast<uint8>(state);
        const bool  got     = CanClaimFurnace(s);
        const bool  want    = s == FurnaceAvailable;
        const bool  inlineF = inlineCanClaimFurnace(s);
        const bool  pin     = pinCanClaimFurnace3596(s);

        ok = expect(got == want, "dense free == state==AVAILABLE") && ok;
        ok = expect(got == inlineF, "dense free == inline") && ok;
        ok = expect(got == pin, "dense free == pin") && ok;
        ok = expect(got == pinCanClaimFurnace3065(s), "dense free == residual pin3065") && ok;
        ok = expect(got == pinCanClaimFurnace3239(s), "dense free == prior pin3239") && ok;
        ok = expect(got == pinCanClaimFurnace3285(s), "dense free == prior pin3285") && ok;
        ok = expect(got == pinCanClaimFurnace3315(s), "dense free == prior pin3315") && ok;
        ok = expect(got == pinCanClaimFurnace3494(s), "dense free == prior pin3494") && ok;
        ok = expect(got == pinCanClaimFurnace3551(s), "dense free == prior pin3551") && ok;
    }

    // Explicit free-path poles matching capacity body (direct return).
    ok = expect(CanClaimFurnace(0) == true, "AVAILABLE must allow claim") && ok;
    ok = expect(CanClaimFurnace(1) == false, "CLAIMED must block claim") && ok;
    ok = expect(CanClaimFurnace(2) == false, "ACTIVE must block claim") && ok;
    ok = expect(CanClaimFurnace(3) == false, "COMPLETED must block claim") && ok;
    ok = expect(CanClaimFurnace(0) == true, "state 0 (AVAILABLE) must allow claim") && ok;
    ok = expect(CanClaimFurnace(1) == false, "state 1 must block claim") && ok;

    // Host synergyFurnaceOnTrigger AVAILABLE branch path semantics.
    ok = expect(CanClaimFurnace(0), "host AVAILABLE → CLAIM_SET + attach path") && ok;
    ok = expect(!CanClaimFurnace(1), "host CLAIMED → other handleFurnaceState") && ok;
    ok = expect(!CanClaimFurnace(2), "host ACTIVE → other handleFurnaceState") && ok;
    ok = expect(!CanClaimFurnace(3), "host COMPLETED → other handleFurnaceState") && ok;

    return ok;
}
