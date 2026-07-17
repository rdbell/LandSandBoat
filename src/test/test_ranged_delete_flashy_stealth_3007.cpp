#include "test_ranged_delete_flashy_stealth_3007.h"

#include "map/ranged_ammo_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "ranged ShouldDeleteFlashyAndStealthShot 3007 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline OnRangedAttack FlashyShot/StealthShot delete gate for dual-wire
// cross-check (slice 3007):
//   return isChar
auto inlineShouldDeleteFlashyAndStealthShot(const bool isChar) -> bool
{
    return isChar;
}

} // namespace

// Pure dual-wire expansion for rangedammohelpers::ShouldDeleteFlashyAndStealthShot
// (OnRangedAttack FlashyShot/StealthShot status-delete gate; slice 3007).
auto runRangedDeleteFlashyStealth3007SelfTests() -> bool
{
    using rangedammohelpers::ShouldDeleteFlashyAndStealthShot;

    bool ok = true;

    const struct
    {
        bool        isChar;
        bool        want;
        const char* label;
    } cases[] = {
        // Identity poles.
        { true, true, "char → delete Flashy/Stealth" },
        { false, false, "non-char → skip delete" },

        // Residual 1390 / production inject pin: char ammo path always true.
        { true, true, "residual char path pin" },
        { false, false, "residual non-char path pin" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldDeleteFlashyAndStealthShot(c.isChar);
        const bool inlineF = inlineShouldDeleteFlashyAndStealthShot(c.isChar);
        const bool wantPin = c.isChar;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldDeleteFlashyAndStealthShot dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldDeleteFlashyAndStealthShot == pin formula isChar") && ok;
    }

    // Pin composition: identity poles.
    ok = expect(ShouldDeleteFlashyAndStealthShot(true), "char path must delete FlashyShot/StealthShot") && ok;
    ok = expect(!ShouldDeleteFlashyAndStealthShot(false), "non-char path must not delete FlashyShot/StealthShot") && ok;

    // Dense compose: isChar poles only (identity).
    for (const bool isChar : { false, true })
    {
        const bool got = ShouldDeleteFlashyAndStealthShot(isChar);
        ok             = expect(got == isChar, "compose free == pin formula isChar") && ok;
        ok             = expect(got == inlineShouldDeleteFlashyAndStealthShot(isChar), "compose free == inline") && ok;
    }

    // Host-style compose poles: OnRangedAttack injects isChar = true on the
    // PC ammo path (literal true; non-char OnRangedAttack branches skip this
    // block). On true, host DelStatusEffect(FlashyShot) and
    // DelStatusEffect(StealthShot).
    const struct
    {
        bool        isChar;
        bool        wantDelete;
        const char* label;
    } composeCases[] = {
        { true, true, "char path: delete FlashyShot + StealthShot" },
        { false, false, "non-char: skip Flashy/Stealth delete" },
    };

    for (const auto& c : composeCases)
    {
        const bool got = ShouldDeleteFlashyAndStealthShot(c.isChar);
        ok             = expect(got == c.wantDelete, c.label) && ok;
        ok             = expect(got == c.isChar, "compose free == pin formula isChar") && ok;
        ok             = expect(got == inlineShouldDeleteFlashyAndStealthShot(c.isChar), "compose free == inline") && ok;
    }

    // Production inject pin: OnRangedAttack (~3285) calls
    // ShouldDeleteFlashyAndStealthShot(true) on the char ammo path.
    ok = expect(ShouldDeleteFlashyAndStealthShot(true), "production inject true must delete") && ok;

    return ok;
}
