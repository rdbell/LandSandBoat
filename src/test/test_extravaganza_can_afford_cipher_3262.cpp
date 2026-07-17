#include "test_extravaganza_can_afford_cipher_3262.h"

#include "map/extravaganza_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "extravaganza CanAffordCipher 3262 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua shadowEraFinish notes gate for dual-wire cross-check
// (dedicated 3262 expand residual 2881):
//   notes >= 1000
auto inlineCanAffordCipher(const int32 notes) -> bool
{
    return notes >= 1000;
}

// Compact dual-wire pin matching Go pinCanAffordCipher3262:
//   notes >= CipherPurchaseNotes
auto pinCanAffordCipher(const int32 notes) -> bool
{
    return notes >= extravaganzahelpers::CipherPurchaseNotes;
}

} // namespace

// Pure dual-wire expansion for extravaganzahelpers::CanAffordCipher
// (Lua shadowEraFinish allied_notes >= 1000 gate; OmegaXI
// internal/extravaganza; dedicated slice 3262 expand residual 2881;
// prior dedicated 3153 retained).
//
// Coverage:
//   - free == inline == pin == notes >= CipherPurchaseNotes
//   - CipherPurchaseNotes == 1000 pin
//   - residual 2881 / 3153 / 0949 pins still hold
//   - residual poles: short / exact / surplus
//   - dense note range compose around the cost pin
auto runExtravaganzaCanAffordCipher3262SelfTests() -> bool
{
    using extravaganzahelpers::CanAffordCipher;
    using extravaganzahelpers::CipherPurchaseNotes;

    bool ok = true;

    // CipherPurchaseNotes pin (same debit cost).
    ok = expect(CipherPurchaseNotes == 1000, "CipherPurchaseNotes pin 1000") && ok;

    // Residual 2881 / 3153 / 0949 CanAffordCipher pins still hold under dual-wire.
    ok = expect(!CanAffordCipher(999), "residual: 999 notes cannot afford") && ok;
    ok = expect(CanAffordCipher(1000), "residual: 1000 notes exact boundary") && ok;
    ok = expect(CanAffordCipher(1001), "residual: 1001 notes must afford") && ok;
    ok = expect(!CanAffordCipher(0), "residual: zero notes cannot afford") && ok;
    ok = expect(!CanAffordCipher(-1), "residual: negative notes cannot afford") && ok;
    ok = expect(!CanAffordCipher(1), "residual: 1 note cannot afford") && ok;
    ok = expect(!CanAffordCipher(500), "residual: half cost cannot afford") && ok;
    ok = expect(CanAffordCipher(2000), "residual: double cost must afford") && ok;
    ok = expect(CanAffordCipher(999999), "residual: very large notes must afford") && ok;
    ok = expect(!CanAffordCipher(CipherPurchaseNotes - 1), "residual: pin-1 cannot afford") && ok;
    ok = expect(CanAffordCipher(CipherPurchaseNotes), "residual: pin exact can afford") && ok;
    ok = expect(CanAffordCipher(CipherPurchaseNotes + 1), "residual: pin+1 can afford") && ok;

    // Residual poles (short / exact / surplus) + free==inline==pin table.
    const struct
    {
        int32       notes;
        bool        want;
        const char* label;
    } cases[] = {
        // Residual poles: short / exact / surplus.
        { 0, false, "pole short zero notes" },
        { 1, false, "pole short 1 note" },
        { 500, false, "pole short half cost" },
        { 998, false, "pole short 998" },
        { 999, false, "pole short 999 under cost" },
        { CipherPurchaseNotes - 1, false, "pole short pin-1" },
        { 1000, true, "pole exact 1000 boundary" },
        { CipherPurchaseNotes, true, "pole exact pin" },
        { 1001, true, "pole surplus 1001" },
        { CipherPurchaseNotes + 1, true, "pole surplus pin+1" },
        { 2000, true, "pole surplus double cost" },
        { 999999, true, "pole surplus very large" },

        // Negative / edge injects.
        { -1, false, "negative notes" },
        { -1000, false, "large negative notes" },
        { -999999, false, "very large negative notes" },

        // Dense host poles around pin.
        { 0, false, "dense 0" },
        { 1, false, "dense 1" },
        { 999, false, "dense 999" },
        { 1000, true, "dense 1000" },
        { 1001, true, "dense 1001" },
        { 1500, true, "dense 1500" },
        { 2000, true, "dense 2000" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanAffordCipher(c.notes);
        const bool inlineF = inlineCanAffordCipher(c.notes);
        const bool pinGot  = pinCanAffordCipher(c.notes);
        const bool compose = c.notes >= CipherPurchaseNotes;

        ok = expect(got == c.want, c.label) && ok;
        // Positive form: free == inline == pin.
        ok = expect(got == inlineF && got == pinGot, "dual-wire free==inline==pin") && ok;
        ok = expect(got == compose, "formula free==notes>=CipherPurchaseNotes") && ok;
    }

    // Explicit residual poles free == inline == pin (short / exact / surplus).
    const struct
    {
        int32       notes;
        const char* label;
    } poles[] = {
        { 999, "short" },
        { CipherPurchaseNotes - 1, "short pin-1" },
        { 0, "short zero" },
        { 500, "short half" },
        { 1000, "exact" },
        { CipherPurchaseNotes, "exact pin" },
        { 1001, "surplus" },
        { CipherPurchaseNotes + 1, "surplus pin+1" },
        { 2000, "surplus double" },
        { 999999, "surplus large" },
        { -1, "short negative" },
    };
    for (const auto& p : poles)
    {
        const bool got     = CanAffordCipher(p.notes);
        const bool inlineF = inlineCanAffordCipher(p.notes);
        const bool pinGot  = pinCanAffordCipher(p.notes);
        const bool want    = p.notes >= CipherPurchaseNotes;
        ok                 = expect(got == want, p.label) && ok;
        ok                 = expect(got == inlineF && got == pinGot, "pole free==inline==pin") && ok;
    }

    // Dense compose free == inline == pin over representative note values.
    const int32 dense[] = {
        -1000,
        -1,
        0,
        1,
        500,
        998,
        999,
        1000,
        1001,
        1500,
        2000,
        999999,
        CipherPurchaseNotes - 1,
        CipherPurchaseNotes,
        CipherPurchaseNotes + 1,
    };
    for (const int32 notes : dense)
    {
        const bool got     = CanAffordCipher(notes);
        const bool inlineF = inlineCanAffordCipher(notes);
        const bool pinGot  = pinCanAffordCipher(notes);
        const bool want    = notes >= CipherPurchaseNotes;
        ok                 = expect(got == want, "dense free==pin formula") && ok;
        ok                 = expect(got == inlineF && got == pinGot, "dense free==inline==pin") && ok;
    }

    // Dense neighborhood scan around the cost pin.
    for (int32 notes = CipherPurchaseNotes - 5; notes <= CipherPurchaseNotes + 5; ++notes)
    {
        const bool got     = CanAffordCipher(notes);
        const bool inlineF = inlineCanAffordCipher(notes);
        const bool pinGot  = pinCanAffordCipher(notes);
        const bool want    = notes >= CipherPurchaseNotes;
        ok                 = expect(got == want, "neighborhood free==pin formula") && ok;
        ok                 = expect(got == inlineF && got == pinGot, "neighborhood free==inline==pin") && ok;
    }

    // Production shadowEraFinish path semantics:
    // Eligible → giveItem / delCurrency; blocked → messageSpecial.
    ok = expect(CanAffordCipher(CipherPurchaseNotes), "shadowEraFinish eligible exact") && ok;
    ok = expect(!CanAffordCipher(CipherPurchaseNotes - 1), "shadowEraFinish short blocked") && ok;
    ok = expect(CanAffordCipher(CipherPurchaseNotes + 1), "shadowEraFinish surplus continue") && ok;

    return ok;
}
