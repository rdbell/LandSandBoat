#include "test_besieged_standing_event_3142.h"

#include "map/besieged_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "besieged standing event 3142 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua onEventUpdate gate for dual-wire cross-check (slice 3142):
//   if option < 0x40000000 then player:updateEvent(...) end
auto inlineShouldUpdateStandingEvent(const uint32 option) -> bool
{
    return option < 0x40000000u;
}

// Compact dual-wire pin matching C++ capacity formula:
//   IsItemOptionPath(option)
auto pinShouldUpdateStandingEvent(const uint32 option) -> bool
{
    return besiegedhelpers::IsItemOptionPath(option);
}

} // namespace

// Pure dual-wire expansion for besiegedhelpers::ShouldUpdateStandingEvent
// (Lua onEventUpdate option < 0x40000000 updateEvent gate; slice 3142).
// Option below / at / above 0x40000000; free == inline.
auto runBesiegedStandingEvent3142SelfTests() -> bool
{
    using besiegedhelpers::IsItemOptionPath;
    using besiegedhelpers::ItemOptionCeiling;
    using besiegedhelpers::ShouldUpdateStandingEvent;

    bool ok = true;

    // Residual constant / 1120 / 2859 pins still hold under dual-wire.
    ok = expect(ItemOptionCeiling == 0x40000000u, "ItemOptionCeiling pin") && ok;
    ok = expect(ShouldUpdateStandingEvent(0), "residual zero updates") && ok;
    ok = expect(ShouldUpdateStandingEvent(1), "residual one updates") && ok;
    ok = expect(ShouldUpdateStandingEvent(49), "residual mamoolbane-like catalog updates") && ok;
    ok = expect(ShouldUpdateStandingEvent(ItemOptionCeiling - 1), "residual just below ceiling updates") && ok;
    ok = expect(!ShouldUpdateStandingEvent(ItemOptionCeiling), "residual exact ceiling no update") && ok;
    ok = expect(!ShouldUpdateStandingEvent(ItemOptionCeiling + 1), "residual above ceiling no update") && ok;
    ok = expect(!ShouldUpdateStandingEvent(0x80000000u), "residual high bit no update") && ok;
    ok = expect(!ShouldUpdateStandingEvent(0xFFFFFFFFu), "residual max uint32 no update") && ok;

    // Option below / at / above 0x40000000.
    const struct
    {
        uint32      option;
        bool        want;
        const char* label;
    } cases[] = {
        // Below ceiling → update.
        { 0, true, "zero (below)" },
        { 1, true, "one (below)" },
        { 16, true, "sanction regen also below" },
        { 17, true, "map low byte also below" },
        { 49, true, "mamoolbane-like catalog (below)" },
        { 0x3FFFFFFFu, true, "literal just below ceiling" },
        { ItemOptionCeiling - 1, true, "ItemOptionCeiling-1 (below)" },
        { 0x20000000u, true, "mid below ceiling" },

        // At ceiling → no update.
        { ItemOptionCeiling, false, "ItemOptionCeiling (at)" },
        { 0x40000000u, false, "literal 0x40000000 (at)" },

        // Above ceiling → no update.
        { ItemOptionCeiling + 1, false, "ItemOptionCeiling+1 (above)" },
        { 0x40000001u, false, "literal ceiling+1 (above)" },
        { 0x80000000u, false, "high bit (above)" },
        { 0xFFFFFFFFu, false, "max uint32 (above)" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldUpdateStandingEvent(c.option);
        const bool pathGot = IsItemOptionPath(c.option);
        const bool inlineF = inlineShouldUpdateStandingEvent(c.option);
        const bool pinGot  = pinShouldUpdateStandingEvent(c.option);
        const bool wantPin = c.option < ItemOptionCeiling;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == pathGot, "ShouldUpdateStandingEvent dual-wire == IsItemOptionPath") && ok;
        ok = expect(got == inlineF, "ShouldUpdateStandingEvent dual-wire == inline Lua formula") && ok;
        ok = expect(got == pinGot, "ShouldUpdateStandingEvent dual-wire == C++ pin formula") && ok;
        ok = expect(got == wantPin, "ShouldUpdateStandingEvent == option < ItemOptionCeiling") && ok;
    }

    // --- Production onEventUpdate path semantics (host inject compose) ---
    // Host looks up catalog entry first; missing rows return early. Then
    // injects option into ShouldUpdateStandingEvent. Below → updateEvent;
    // at/above → skip.
    ok = expect(ShouldUpdateStandingEvent(0), "update zero → proceed updateEvent") && ok;
    ok = expect(ShouldUpdateStandingEvent(49), "update mamoolbane → proceed updateEvent") && ok;
    ok = expect(ShouldUpdateStandingEvent(ItemOptionCeiling - 1), "update ceiling-1 → proceed") && ok;
    ok = expect(!ShouldUpdateStandingEvent(ItemOptionCeiling), "update ceiling → skip") && ok;
    ok = expect(!ShouldUpdateStandingEvent(ItemOptionCeiling + 1), "update ceiling+1 → skip") && ok;
    ok = expect(!ShouldUpdateStandingEvent(0x80000000u), "update high bit → skip") && ok;

    // Compose: free == inline == pin for below / at / above poles.
    const uint32 poles[] = {
        0,
        1,
        16,
        17,
        49,
        0x1000u,
        ItemOptionCeiling - 1,
        ItemOptionCeiling,
        ItemOptionCeiling + 1,
        0x40000000u,
        0x40000001u,
        0x80000000u,
        0xFFFFFFFFu,
    };
    for (const uint32 opt : poles)
    {
        const bool got     = ShouldUpdateStandingEvent(opt);
        const bool want    = opt < ItemOptionCeiling;
        const bool inlineF = inlineShouldUpdateStandingEvent(opt);
        const bool pinGot  = pinShouldUpdateStandingEvent(opt);
        const bool pathGot = IsItemOptionPath(opt);
        ok                 = expect(got == want, "compose free == (option < ceiling)") && ok;
        ok                 = expect(got == inlineF, "compose free == inline") && ok;
        ok                 = expect(got == pinGot, "compose free == pin") && ok;
        ok                 = expect(got == pathGot, "compose free == IsItemOptionPath") && ok;
    }

    return ok;
}
