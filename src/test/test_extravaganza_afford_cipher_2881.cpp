#include "test_extravaganza_afford_cipher_2881.h"

#include "map/extravaganza_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "extravaganza CanAffordCipher 2881 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua shadowEraFinish notes gate for dual-wire cross-check:
//   notes >= 1000
auto inlineCanAffordCipher(const int32 notes) -> bool
{
    return notes >= 1000;
}

} // namespace

// Pure dual-wire expansion for extravaganzahelpers::CanAffordCipher
// (Lua shadowEraFinish allied_notes >= 1000 gate).
auto runExtravaganzaAffordCipher2881SelfTests() -> bool
{
    using extravaganzahelpers::CanAffordCipher;
    using extravaganzahelpers::CipherPurchaseNotes;

    bool ok = true;

    ok = expect(CipherPurchaseNotes == 1000, "CipherPurchaseNotes pin 1000") && ok;

    const struct
    {
        int32       notes;
        bool        want;
        const char* label;
    } cases[] = {
        { 999, false, "999 notes cannot afford" },
        { 1000, true, "1000 notes exact boundary" },
        { 1001, true, "1001 notes above cost" },
        { 0, false, "zero notes" },
        { -1, false, "negative notes" },
        { 1, false, "1 note" },
        { 500, false, "half cost" },
        { 2000, true, "double cost" },
        { 999999, true, "very large notes" },
        { 999, false, "just under boundary" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanAffordCipher(c.notes);
        const bool inlineF = inlineCanAffordCipher(c.notes);
        const bool wantPin = c.notes >= CipherPurchaseNotes;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "CanAffordCipher dual-wire == inline Lua formula") && ok;
        ok = expect(got == wantPin, "CanAffordCipher == notes >= CipherPurchaseNotes") && ok;
    }

    // Pin composition: free function uses CipherPurchaseNotes constant.
    ok = expect(!CanAffordCipher(CipherPurchaseNotes - 1), "pin-1 cannot afford") && ok;
    ok = expect(CanAffordCipher(CipherPurchaseNotes), "pin exact can afford") && ok;
    ok = expect(CanAffordCipher(CipherPurchaseNotes + 1), "pin+1 can afford") && ok;

    return ok;
}
