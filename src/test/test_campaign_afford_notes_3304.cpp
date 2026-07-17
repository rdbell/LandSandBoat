#include "test_campaign_afford_notes_3304.h"

#include "map/campaign_capacity.h"

#include <cstdint>
#include <iostream>
#include <limits>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "campaign afford notes 3304 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua sigilOnEventFinish shop afford gate for dual-wire checks
// (dedicated 3304 expand residual 2946; prior dedicated 3273 retained):
//   if player:getCurrency('allied_notes') < itemPrice then return end
// Positive form: notes >= price.
auto inlineCanAffordAlliedNotes(const int32 notes, const int32 price) -> bool
{
    return notes >= price;
}

// Compact dual-wire pin matching Go pinCanAffordAlliedNotes3304 / C++ capacity:
//   notes >= price
auto pinCanAffordAlliedNotes(const int32 notes, const int32 price) -> bool
{
    return notes >= price;
}

} // namespace

// Pure dual-wire expansion for campaignhelpers::CanAffordAlliedNotes
// (Lua sigilOnEventFinish shop purchase afford: notes >= itemPrice;
// OmegaXI internal/campaign; dedicated slice 3304 expand residual 2946).
//
// Coverage:
//   - free == inline == pin == (notes >= price)
//   - residual 2946 / 3072 / 3226 / 3273 / 1115 pins still hold
//   - poles: equal, short, surplus, zero price, negative edges
//   - dense: 0/0, 0/1, 1/0, 1/1, equal, short, surplus, large ints
auto runCampaignAffordNotes3304SelfTests() -> bool
{
    using campaignhelpers::CanAffordAlliedNotes;

    bool ok = true;

    // Residual 1115 / 2946 / 3072 / 3226 / 3273 pins still hold under dual-wire.
    ok = expect(CanAffordAlliedNotes(1000, 1000), "residual exact notes") && ok;
    ok = expect(CanAffordAlliedNotes(1500, 1000), "residual over notes") && ok;
    ok = expect(!CanAffordAlliedNotes(999, 1000), "residual short by one") && ok;
    ok = expect(!CanAffordAlliedNotes(0, 1), "residual empty wallet") && ok;
    ok = expect(CanAffordAlliedNotes(0, 0), "residual free item") && ok;
    ok = expect(CanAffordAlliedNotes(1, 0), "residual free with notes") && ok;
    ok = expect(CanAffordAlliedNotes(1470, 1470), "residual off-nation exact") && ok;
    ok = expect(!CanAffordAlliedNotes(1469, 1470), "residual off-nation short") && ok;

    constexpr int32 kMaxInt = std::numeric_limits<int32>::max();
    constexpr int32 kMinInt = std::numeric_limits<int32>::min();

    // --- Composition table: free == inline == pin ---
    const struct
    {
        int32       notes;
        int32       price;
        bool        want;
        const char* label;
    } cases[] = {
        // Residual 2946 / 3072 / 3226 / 3273 poles.
        { 1000, 1000, true, "residual exact equality" },
        { 1500, 1000, true, "residual surplus" },
        { 999, 1000, false, "residual short" },
        { 0, 0, true, "residual zero/zero" },
        { 0, 1, false, "residual empty non-zero price" },
        { 1, 0, true, "residual zero price" },
        { 1, 1, true, "residual one exact" },
        { 1470, 1470, true, "residual off-nation 980*1.5 exact" },
        { 1469, 1470, false, "residual off-nation short" },
        { 980, 980, true, "residual unadjusted base exact" },
        { 979, 980, false, "residual unadjusted base short" },

        // Residual poles: equal, short, surplus, zero price.
        { 50, 50, true, "pole equal" },
        { 49, 50, false, "pole short" },
        { 51, 50, true, "pole surplus" },
        { 999, 0, true, "pole zero price surplus notes" },
        { 0, 0, true, "pole zero price zero notes" },

        // Negative edges (int32 allows).
        { -1, 0, false, "negative notes vs zero price" },
        { 0, -1, true, "zero notes vs negative price" },
        { -1, -1, true, "negative exact equality" },
        { -2, -1, false, "negative short" },
        { -1, -2, true, "negative surplus" },
        { 1, -1, true, "positive vs negative price" },
        { -100, 1, false, "large negative notes" },

        // Dense host poles (required).
        { 0, 0, true, "dense 0/0" },
        { 0, 1, false, "dense 0/1" },
        { 1, 0, true, "dense 1/0" },
        { 1, 1, true, "dense 1/1" },
        { 1000, 999, true, "dense 1000/999 surplus" },
        { 1000, 1000, true, "dense 1000/1000 equal" },
        { 1000, 1001, false, "dense 1000/1001 short" },
        { 999, 1000, false, "dense 999/1000 short" },
        { 1500, 1000, true, "dense 1500/1000 surplus" },

        // Large ints / MaxInt-ish values.
        { kMaxInt, kMaxInt, true, "MaxInt exact" },
        { kMaxInt - 1, kMaxInt, false, "MaxInt short" },
        { kMaxInt, kMaxInt - 1, true, "MaxInt surplus" },
        { kMinInt, kMinInt, true, "MinInt exact" },
        { kMinInt, kMinInt + 1, false, "MinInt short" },
        { kMinInt + 1, kMinInt, true, "MinInt surplus" },
        { 0x7FFFFFFF, 0x7FFFFFFF, true, "0x7FFFFFFF exact" },
        { 0x7FFFFFFE, 0x7FFFFFFF, false, "0x7FFFFFFF short" },
        { 0x7FFFFFFF, 0x7FFFFFFE, true, "0x7FFFFFFF surplus" },
        { 0x7FFFFFFF, 0, true, "large notes zero price" },
        { 0x7FFFFFFF, 1, true, "large notes small price" },
        { 1, 0x7FFFFFFF, false, "small notes large price" },
        { 1 << 30, 1 << 30, true, "1<<30 exact" },
        { (1 << 30) - 1, 1 << 30, false, "1<<30 short" },
        { 1 << 30, (1 << 30) - 1, true, "1<<30 surplus" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanAffordAlliedNotes(c.notes, c.price);
        const bool inlineF = inlineCanAffordAlliedNotes(c.notes, c.price);
        const bool pin     = pinCanAffordAlliedNotes(c.notes, c.price);
        const bool wantPin = c.notes >= c.price;

        ok = expect(got == c.want, c.label) && ok;
        // Positive form: free == inline == pin.
        ok = expect(got == inlineF && got == pin, "dual-wire free == inline == pin") && ok;
        ok = expect(got == wantPin, "free == pin formula notes >= price") && ok;
    }

    // Explicit residual poles free == inline == pin.
    const struct
    {
        int32       notes;
        int32       price;
        const char* label;
    } poles[] = {
        { 1000, 1000, "equal" },
        { 999, 1000, "short" },
        { 1500, 1000, "surplus" },
        { 5, 0, "zero price" },
        { 0, 0, "notes 0 price 0" },
        { 0, 1, "notes 0 price 1" },
        { -1, 0, "notes -1 price 0" },
        { kMaxInt, kMaxInt, "MaxInt equal" },
        { kMaxInt - 1, kMaxInt, "MaxInt short" },
        { 1470, 1470, "off-nation equal" },
        { 1469, 1470, "off-nation short" },
    };
    for (const auto& p : poles)
    {
        const bool got     = CanAffordAlliedNotes(p.notes, p.price);
        const bool inlineF = inlineCanAffordAlliedNotes(p.notes, p.price);
        const bool pin     = pinCanAffordAlliedNotes(p.notes, p.price);
        const bool want    = p.notes >= p.price;
        ok                 = expect(got == want, p.label) && ok;
        ok                 = expect(got == inlineF && got == pin, "pole free == inline == pin") && ok;
    }

    // Dense compose free == inline == pin over required pairs + neighbors.
    const int32 dense[][2] = {
        { 0, 0 },
        { 0, 1 },
        { 1, 0 },
        { 1, 1 },
        { 1000, 999 },
        { 1000, 1000 },
        { 1000, 1001 },
        { 999, 1000 },
        { 1500, 1000 },
        { -1, 0 },
        { 0, -1 },
        { -1, -1 },
        { kMaxInt, kMaxInt },
        { kMaxInt - 1, kMaxInt },
        { kMaxInt, kMaxInt - 1 },
        { 0x7FFFFFFF, 0x7FFFFFFF },
        { 0x7FFFFFFE, 0x7FFFFFFF },
        { 1 << 30, 1 << 30 },
        { (1 << 30) - 1, 1 << 30 },
        { 1470, 1470 },
        { 1469, 1470 },
    };
    for (const auto& p : dense)
    {
        const int32 notes = p[0];
        const int32 price = p[1];
        const bool  got     = CanAffordAlliedNotes(notes, price);
        const bool  inlineF = inlineCanAffordAlliedNotes(notes, price);
        const bool  pin     = pinCanAffordAlliedNotes(notes, price);
        const bool  want    = notes >= price;
        ok                  = expect(got == want, "dense free == pin formula") && ok;
        ok                  = expect(got == inlineF && got == pin, "dense free == inline == pin") && ok;
    }

    // Host-style sigil shop purchase inject poles (notes / itemPrice).
    const int32 inject[][2] = {
        { 1000, 1000 },
        { 999, 1000 },
        { 1500, 1000 },
        { 1470, 1470 },
        { 1469, 1470 },
        { 980, 980 },
        { 979, 980 },
        { 0, 0 },
        { 0, 1 },
        { 1, 0 },
        { 1, 1 },
    };
    for (const auto& p : inject)
    {
        const int32 notes = p[0];
        const int32 price = p[1];
        const bool  got     = CanAffordAlliedNotes(notes, price);
        const bool  inlineF = inlineCanAffordAlliedNotes(notes, price);
        const bool  pin     = pinCanAffordAlliedNotes(notes, price);
        const bool  want    = notes >= price;
        ok                  = expect(got == want, "inject free == pin formula") && ok;
        ok                  = expect(got == inlineF && got == pin, "inject free == inline == pin") && ok;
    }

    // Production path semantics still hold.
    // Afford → may continue giveItem / delCurrency path.
    // Short notes → early return (cannot buy).
    ok = expect(CanAffordAlliedNotes(0, 0), "0>=0 must afford") && ok;
    ok = expect(!CanAffordAlliedNotes(0, 1), "0>=1 must reject") && ok;
    ok = expect(CanAffordAlliedNotes(1, 1), "1>=1 must afford") && ok;
    ok = expect(CanAffordAlliedNotes(1, 0), "1>=0 must afford") && ok;
    ok = expect(!CanAffordAlliedNotes(999, 1000), "999>=1000 must reject") && ok;
    ok = expect(CanAffordAlliedNotes(1000, 1000), "1000>=1000 must afford") && ok;
    ok = expect(CanAffordAlliedNotes(1500, 1000), "1500>=1000 must afford") && ok;
    ok = expect(CanAffordAlliedNotes(1000, 1000), "shop afford → continue path") && ok;
    ok = expect(!CanAffordAlliedNotes(999, 1000), "shop short → early return") && ok;
    ok = expect(CanAffordAlliedNotes(0, 0), "shop free → continue path") && ok;

    return ok;
}
