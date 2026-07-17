#include "test_campaign_afford_notes_3072.h"

#include "map/campaign_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "campaign afford notes 3072 self-test failed: " << label << '\n';
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

// Compact dual-wire pin matching Go pinCanAffordAlliedNotes3072:
//   notes >= price
auto pinCanAffordAlliedNotes(const int32 notes, const int32 price) -> bool
{
    return notes >= price;
}

} // namespace

// Pure dual-wire expansion for campaignhelpers::CanAffordAlliedNotes
// (Lua sigilOnEventFinish shop purchase afford: notes >= itemPrice;
// OmegaXI internal/campaign; slice 3072).
//
// Coverage:
//   - notes == price → true
//   - notes < price → false
//   - zero price
//   - large values
//   - free == inline == pin
//   - residual 1115 / 2946 pins still hold
auto runCampaignAffordNotes3072SelfTests() -> bool
{
    using campaignhelpers::CanAffordAlliedNotes;

    bool ok = true;

    // Residual 1115 / 2946 pins still hold under dual-wire.
    ok = expect(CanAffordAlliedNotes(1000, 1000), "residual: exact notes") && ok;
    ok = expect(CanAffordAlliedNotes(1500, 1000), "residual: over notes") && ok;
    ok = expect(!CanAffordAlliedNotes(999, 1000), "residual: short by one") && ok;
    ok = expect(!CanAffordAlliedNotes(0, 1), "residual: empty wallet") && ok;
    ok = expect(CanAffordAlliedNotes(0, 0), "residual: free item") && ok;

    // --- notes == price → true ---
    ok = expect(CanAffordAlliedNotes(1000, 1000), "notes==price exact") && ok;
    ok = expect(CanAffordAlliedNotes(0, 0), "notes==price zero") && ok;
    ok = expect(CanAffordAlliedNotes(1, 1), "notes==price one") && ok;
    ok = expect(CanAffordAlliedNotes(1470, 1470), "notes==price off-nation") && ok;

    // --- notes < price → false ---
    ok = expect(!CanAffordAlliedNotes(999, 1000), "notes<price short by one") && ok;
    ok = expect(!CanAffordAlliedNotes(0, 1), "notes<price empty wallet") && ok;
    ok = expect(!CanAffordAlliedNotes(1469, 1470), "notes<price off-nation short") && ok;

    // --- zero price ---
    ok = expect(CanAffordAlliedNotes(0, 0), "zero price free exact") && ok;
    ok = expect(CanAffordAlliedNotes(1, 0), "zero price with notes") && ok;
    ok = expect(CanAffordAlliedNotes(1000, 0), "zero price large notes") && ok;

    // --- large values ---
    ok = expect(CanAffordAlliedNotes(0x7FFFFFFF, 0x7FFFFFFF), "large max int32 exact") && ok;
    ok = expect(!CanAffordAlliedNotes(0x7FFFFFFE, 0x7FFFFFFF), "large max int32 short") && ok;
    ok = expect(CanAffordAlliedNotes(0x7FFFFFFF, 1), "large notes small price") && ok;
    ok = expect(CanAffordAlliedNotes(0x7FFFFFFF, 0), "large notes zero price") && ok;
    ok = expect(!CanAffordAlliedNotes(1, 0x7FFFFFFF), "small notes large price") && ok;

    // --- Composition table: free == inline == pin ---
    const struct
    {
        int32       notes;
        int32       price;
        bool        want;
        const char* label;
    } cases[] = {
        { 1000, 1000, true, "table notes==price exact" },
        { 1500, 1000, true, "table over" },
        { 999, 1000, false, "table notes<price short" },
        { 0, 0, true, "table zero price free" },
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
        const bool pinGot    = pinCanAffordAlliedNotes(c.notes, c.price);
        ok                   = expect(got == c.want, c.label) && ok;
        ok                   = expect(got == inlineGot, "dual-wire free==inline") && ok;
        ok                   = expect(got == pinGot, "dual-wire free==pin") && ok;
    }

    // --- Production sigil shop path semantics ---
    // Afford → may continue giveItem / delCurrency path.
    // Short notes → early return (cannot buy).
    ok = expect(CanAffordAlliedNotes(1000, 1000), "shop afford → continue path") && ok;
    ok = expect(!CanAffordAlliedNotes(999, 1000), "shop short → early return") && ok;
    ok = expect(CanAffordAlliedNotes(0, 0), "shop free → continue path") && ok;

    return ok;
}
