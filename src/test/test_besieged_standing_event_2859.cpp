#include "test_besieged_standing_event_2859.h"

#include "map/besieged_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "besieged standing event 2859 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua onEventUpdate gate for dual-wire checks:
//   if option < 0x40000000 then player:updateEvent(...) end
auto inlineShouldUpdateStandingEvent(const uint32 option) -> bool
{
    return option < 0x40000000u;
}

} // namespace

// Pure dual-wire expansion for besiegedhelpers::ShouldUpdateStandingEvent
// (Lua if option < 0x40000000 updateEvent gate).
auto runBesiegedStandingEvent2859SelfTests() -> bool
{
    using besiegedhelpers::IsItemOptionPath;
    using besiegedhelpers::ItemOptionCeiling;
    using besiegedhelpers::ShouldUpdateStandingEvent;

    bool ok = true;

    // Ceiling pin.
    ok = expect(ItemOptionCeiling == 0x40000000u, "ItemOptionCeiling pin") && ok;

    // Boundary: below ceiling updates; ceiling and above do not.
    ok = expect(ShouldUpdateStandingEvent(0), "zero updates") && ok;
    ok = expect(ShouldUpdateStandingEvent(1), "one updates") && ok;
    ok = expect(ShouldUpdateStandingEvent(49), "mamoolbane-like catalog updates") && ok;
    ok = expect(ShouldUpdateStandingEvent(ItemOptionCeiling - 1), "just below ceiling updates") && ok;
    ok = expect(!ShouldUpdateStandingEvent(ItemOptionCeiling), "exact ceiling no update") && ok;
    ok = expect(!ShouldUpdateStandingEvent(ItemOptionCeiling + 1), "above ceiling no update") && ok;
    ok = expect(!ShouldUpdateStandingEvent(0x80000000u), "high bit no update") && ok;
    ok = expect(!ShouldUpdateStandingEvent(0xFFFFFFFFu), "max uint32 no update") && ok;

    // Dual-wire matches IsItemOptionPath and inline formula across a table.
    const struct
    {
        uint32      option;
        bool        want;
        const char* label;
    } cases[] = {
        { 0, true, "table zero" },
        { 1, true, "table one" },
        { 16, true, "table sanction regen" },
        { 17, true, "table map low byte" },
        { 49, true, "table mamoolbane" },
        { ItemOptionCeiling - 1, true, "table ceiling-1" },
        { ItemOptionCeiling, false, "table ceiling" },
        { ItemOptionCeiling + 1, false, "table ceiling+1" },
        { 0x40000000u, false, "table literal ceiling" },
        { 0x80000000u, false, "table high bit" },
        { 0xFFFFFFFFu, false, "table max" },
    };

    for (const auto& c : cases)
    {
        const bool got       = ShouldUpdateStandingEvent(c.option);
        const bool pathGot   = IsItemOptionPath(c.option);
        const bool inlineGot = inlineShouldUpdateStandingEvent(c.option);
        ok                   = expect(got == c.want, c.label) && ok;
        ok                   = expect(got == pathGot, "dual-wire matches IsItemOptionPath") && ok;
        ok                   = expect(got == inlineGot, "dual-wire matches inline formula") && ok;
    }

    return ok;
}
