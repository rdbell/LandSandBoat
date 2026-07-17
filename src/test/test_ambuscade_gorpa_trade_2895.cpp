#include "test_ambuscade_gorpa_trade_2895.h"

#include "map/ambuscade_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "ambuscade gorpa-trade 2895 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua onTradeGorpaMasorpa formula for dual-wire checks:
//   if player:getEminenceCompleted(499) then -- TODO end
// eminenceCompleted499 is the host inject for getEminenceCompleted(499).
auto inlineShouldProcessGorpaTrade(const bool eminenceCompleted499) -> bool
{
    return eminenceCompleted499;
}

} // namespace

// Pure dual-wire expansion for ambuscadehelpers::ShouldProcessGorpaTrade
// (Lua ambuscade onTradeGorpaMasorpa eminence-completed gate).
auto runAmbuscadeGorpaTrade2895SelfTests() -> bool
{
    using ambuscadehelpers::RoERecordSteppingIntoAnAmbuscade;
    using ambuscadehelpers::ShouldProcessGorpaTrade;

    bool ok = true;

    // Constant pin.
    ok = expect(RoERecordSteppingIntoAnAmbuscade == 499,
                "RoERecordSteppingIntoAnAmbuscade == 499") &&
         ok;

    // Truth table for ShouldProcessGorpaTrade (identity on completed flag).
    ok = expect(ShouldProcessGorpaTrade(true), "completed 499 → process") && ok;
    ok = expect(!ShouldProcessGorpaTrade(false), "incomplete 499 → skip") && ok;

    // Dual-wire matches inline formula across a small table.
    const struct
    {
        bool        eminenceCompleted499;
        bool        want;
        const char* label;
    } cases[] = {
        { true, true, "table completed" },
        { false, false, "table incomplete" },
    };

    for (const auto& c : cases)
    {
        const bool got       = ShouldProcessGorpaTrade(c.eminenceCompleted499);
        const bool inlineGot = inlineShouldProcessGorpaTrade(c.eminenceCompleted499);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineGot, "dual-wire free == inline Lua formula") && ok;
    }

    // Host compose: inject getEminenceCompleted(499) then pure gate.
    // When gate is true, host enters the (TODO) trade body; when false, skips.
    const struct
    {
        bool        eminenceCompleted499;
        bool        wantProcess;
        const char* label;
    } composeCases[] = {
        { true, true, "compose completed → enter body" },
        { false, false, "compose incomplete → skip body" },
    };

    for (const auto& c : composeCases)
    {
        const bool completed = c.eminenceCompleted499;
        const bool got       = ShouldProcessGorpaTrade(completed);
        const bool inlineGot = inlineShouldProcessGorpaTrade(completed);

        ok = expect(got == c.wantProcess, c.label) && ok;
        ok = expect(got == inlineGot, "compose dual-wire free == inline") && ok;
    }

    return ok;
}
