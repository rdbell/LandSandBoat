#include "test_campaign_afford_notes_2946.h"

#include "map/campaign_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "campaign afford notes 2946 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua sigilOnEventFinish shop afford gate for dual-wire checks:
//   if player:getCurrency('allied_notes') < itemPrice then return end
// Positive form: notes >= price.
auto inlineCanAffordAlliedNotes(const int32 notes, const int32 price) -> bool
{
    return notes >= price;
}

} // namespace

// Pure dual-wire expansion for campaignhelpers::CanAffordAlliedNotes
// (Lua sigilOnEventFinish shop purchase afford: notes >= itemPrice).
auto runCampaignAffordNotes2946SelfTests() -> bool
{
    using campaignhelpers::CanAffordAlliedNotes;

    bool ok = true;

    // --- Affordable: notes cover price ---
    ok = expect(CanAffordAlliedNotes(1000, 1000), "exact notes") && ok;
    ok = expect(CanAffordAlliedNotes(1500, 1000), "over notes") && ok;
    ok = expect(CanAffordAlliedNotes(0, 0), "free item") && ok;
    ok = expect(CanAffordAlliedNotes(1, 0), "free with notes") && ok;
    ok = expect(CanAffordAlliedNotes(1, 1), "one exact") && ok;

    // --- Insufficient notes ---
    ok = expect(!CanAffordAlliedNotes(999, 1000), "short by one") && ok;
    ok = expect(!CanAffordAlliedNotes(0, 1), "empty wallet") && ok;

    // --- Composition table: dual-wire pure gate from host-injected scalars ---
    const struct
    {
        int32       notes;
        int32       price;
        bool        want;
        const char* label;
    } cases[] = {
        { 1000, 1000, true, "table exact" },
        { 1500, 1000, true, "table over" },
        { 999, 1000, false, "table short" },
        { 0, 0, true, "table free" },
        { 0, 1, false, "table empty" },
        { 1, 0, true, "table free with notes" },
        { 1, 1, true, "table one exact" },
        { -1, 0, false, "table negative notes free" },
        { -1, -1, true, "table negative exact" },
        { 0, -1, true, "table zero notes negative price" },
        { 0x7FFFFFFF, 0x7FFFFFFF, true, "table max int32 exact" },
        { 0x7FFFFFFE, 0x7FFFFFFF, false, "table max int32 short" },
        { 1470, 1470, true, "table off-nation 980*1.5 exact" },
        { 1469, 1470, false, "table off-nation short" },
    };

    for (const auto& c : cases)
    {
        const bool got       = CanAffordAlliedNotes(c.notes, c.price);
        const bool inlineGot = inlineCanAffordAlliedNotes(c.notes, c.price);
        ok                   = expect(got == c.want, c.label) && ok;
        ok                   = expect(got == inlineGot, "dual-wire matches inline formula") && ok;
    }

    // --- Production sigil shop path semantics ---
    // Afford → may continue giveItem / delCurrency path.
    // Short notes → early return (cannot buy).
    ok = expect(CanAffordAlliedNotes(1000, 1000), "shop afford → continue path") && ok;
    ok = expect(!CanAffordAlliedNotes(999, 1000), "shop short → early return") && ok;
    ok = expect(CanAffordAlliedNotes(0, 0), "shop free → continue path") && ok;

    return ok;
}
