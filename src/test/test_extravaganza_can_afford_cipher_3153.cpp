#include "test_extravaganza_can_afford_cipher_3153.h"

#include "map/extravaganza_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "extravaganza CanAffordCipher 3153 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua shadowEraFinish notes gate for dual-wire cross-check:
//   notes >= 1000
auto inlineCanAffordCipher(const int32 notes) -> bool
{
    return notes >= 1000;
}

// Compact dual-wire pin matching Go pinCanAffordCipher3153:
//   notes >= CipherPurchaseNotes
auto pinCanAffordCipher(const int32 notes) -> bool
{
    return notes >= extravaganzahelpers::CipherPurchaseNotes;
}

} // namespace

// Pure dual-wire expansion for extravaganzahelpers::CanAffordCipher
// (Lua shadowEraFinish allied_notes >= 1000 gate; OmegaXI
// internal/extravaganza; slice 3153).
//
// Coverage:
//   - poles 0 / 999 / 1000 / 1001 / negative
//   - free == inline == pin == notes >= CipherPurchaseNotes
//   - CipherPurchaseNotes == 1000 pin
//   - residual 2881 / 0949 pins still hold
auto runExtravaganzaCanAffordCipher3153SelfTests() -> bool
{
    using extravaganzahelpers::CanAffordCipher;
    using extravaganzahelpers::CipherPurchaseNotes;

    bool ok = true;

    // CipherPurchaseNotes pin (same debit cost).
    ok = expect(CipherPurchaseNotes == 1000, "CipherPurchaseNotes pin 1000") && ok;

    // Residual 2881 / 0949 CanAffordCipher pins still hold under dual-wire.
    ok = expect(!CanAffordCipher(999), "residual: 999 notes cannot afford") && ok;
    ok = expect(CanAffordCipher(1000), "residual: 1000 notes exact boundary") && ok;
    ok = expect(CanAffordCipher(1001), "residual: 1001 notes must afford") && ok;
    ok = expect(!CanAffordCipher(0), "residual: zero notes cannot afford") && ok;
    ok = expect(!CanAffordCipher(-1), "residual: negative notes cannot afford") && ok;
    ok = expect(!CanAffordCipher(CipherPurchaseNotes - 1), "residual: pin-1 cannot afford") && ok;
    ok = expect(CanAffordCipher(CipherPurchaseNotes), "residual: pin exact can afford") && ok;
    ok = expect(CanAffordCipher(CipherPurchaseNotes + 1), "residual: pin+1 can afford") && ok;

    // Poles: 0, 999, 1000, 1001, negative (+ extras for free==inline==pin).
    const struct
    {
        int32       notes;
        bool        want;
        const char* label;
    } cases[] = {
        { 0, false, "pole zero notes" },
        { 999, false, "pole 999 under cost" },
        { 1000, true, "pole 1000 exact boundary" },
        { 1001, true, "pole 1001 above cost" },
        { -1, false, "pole negative notes" },
        { -1000, false, "large negative notes" },
        { 1, false, "1 note" },
        { 500, false, "half cost" },
        { 2000, true, "double cost" },
        { 999999, true, "very large notes" },
        { CipherPurchaseNotes - 1, false, "pin-1" },
        { CipherPurchaseNotes, true, "pin exact" },
        { CipherPurchaseNotes + 1, true, "pin+1" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanAffordCipher(c.notes);
        const bool inlineF = inlineCanAffordCipher(c.notes);
        const bool pinGot  = pinCanAffordCipher(c.notes);
        const bool compose = c.notes >= CipherPurchaseNotes;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "dual-wire free==inline") && ok;
        ok = expect(got == pinGot, "dual-wire free==pin") && ok;
        ok = expect(got == compose, "formula free==notes>=CipherPurchaseNotes") && ok;
    }

    // Production shadowEraFinish path semantics:
    // Eligible → giveItem / delCurrency; blocked → messageSpecial.
    ok = expect(CanAffordCipher(CipherPurchaseNotes), "shadowEraFinish eligible exact") && ok;
    ok = expect(!CanAffordCipher(CipherPurchaseNotes - 1), "shadowEraFinish short blocked") && ok;

    // Dense compose identity over representative note values.
    const int32 composeNotes[] = {
        -1000, -1, 0, 1, 500, 999, 1000, 1001, 2000, 999999,
        CipherPurchaseNotes - 1, CipherPurchaseNotes, CipherPurchaseNotes + 1,
    };
    for (const int32 notes : composeNotes)
    {
        const bool got = CanAffordCipher(notes);
        const bool want = notes >= CipherPurchaseNotes;
        ok = expect(got == want, "compose notes>=CipherPurchaseNotes") && ok;
        ok = expect(got == inlineCanAffordCipher(notes), "compose inline") && ok;
        ok = expect(got == pinCanAffordCipher(notes), "compose pin") && ok;
    }

    return ok;
}
