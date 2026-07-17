#include "test_ranged_damage_multiplier_3023.h"

#include "map/ranged_ammo_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "ranged ShouldApplyRangedDamageMultiplier 3023 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline OnRangedAttack ranged damage-multiplier gate for dual-wire
// cross-check (slice 3023):
//   return isChar && slotIsRanged
auto inlineShouldApplyRangedDamageMultiplier(const bool isChar, const bool slotIsRanged) -> bool
{
    return isChar && slotIsRanged;
}

} // namespace

// Pure dual-wire expansion for rangedammohelpers::ShouldApplyRangedDamageMultiplier
// (OnRangedAttack ranged damage-multiplier gate; slice 3023).
auto runRangedDamageMultiplier3023SelfTests() -> bool
{
    using rangedammohelpers::ShouldApplyRangedDamageMultiplier;

    bool ok = true;

    const struct
    {
        bool        isChar;
        bool        slotIsRanged;
        bool        want;
        const char* label;
    } cases[] = {
        // Char + ranged slot: apply damage multiplier.
        { true, true, true, "char ranged → apply mult" },
        // Char + non-ranged slot: skip (ammo/throwing path).
        { true, false, false, "char non-ranged → skip mult" },
        // Non-char: never apply.
        { false, true, false, "non-char ranged → skip" },
        { false, false, false, "non-char non-ranged → skip" },

        // Residual 1390 pins.
        { true, true, true, "residual char ranged" },
        { true, false, false, "residual char non-ranged" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldApplyRangedDamageMultiplier(c.isChar, c.slotIsRanged);
        const bool inlineF = inlineShouldApplyRangedDamageMultiplier(c.isChar, c.slotIsRanged);
        const bool wantPin = c.isChar && c.slotIsRanged;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldApplyRangedDamageMultiplier dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldApplyRangedDamageMultiplier == pin formula isChar && slotIsRanged") && ok;
    }

    // Pin composition: isChar / slotIsRanged poles.
    ok = expect(ShouldApplyRangedDamageMultiplier(true, true), "char ranged must apply damage multiplier") && ok;
    ok = expect(!ShouldApplyRangedDamageMultiplier(true, false), "char non-ranged must not apply damage multiplier") && ok;
    ok = expect(!ShouldApplyRangedDamageMultiplier(false, true), "non-char must not apply damage multiplier") && ok;
    ok = expect(!ShouldApplyRangedDamageMultiplier(false, false), "non-char non-ranged must not apply") && ok;

    // Dense compose: isChar × slotIsRanged poles.
    for (const bool isChar : { false, true })
    {
        for (const bool slotIsRanged : { false, true })
        {
            const bool got  = ShouldApplyRangedDamageMultiplier(isChar, slotIsRanged);
            const bool want = isChar && slotIsRanged;
            ok              = expect(got == want, "compose free == pin formula") && ok;
            ok              = expect(got == inlineShouldApplyRangedDamageMultiplier(isChar, slotIsRanged),
                        "compose free == inline") &&
                 ok;
        }
    }

    // Host-style compose poles: OnRangedAttack injects
    // isChar = entity is character,
    // slotIsRanged = (slot == SLOT_RANGED).
    // On true, host CheckForDamageMultiplier with
    // ResolveRangedPhysicalAttackType(isRapidShot).
    const struct
    {
        bool        isChar;
        bool        slotIsRanged;
        bool        wantApply;
        const char* label;
    } composeCases[] = {
        { true, true, true, "char SLOT_RANGED: apply damage multiplier" },
        { true, false, false, "char non-ranged slot: skip multiplier" },
        { false, true, false, "non-char SLOT_RANGED: skip" },
        { false, false, false, "non-char non-ranged: skip" },
    };

    for (const auto& c : composeCases)
    {
        const bool got = ShouldApplyRangedDamageMultiplier(c.isChar, c.slotIsRanged);
        ok             = expect(got == c.wantApply, c.label) && ok;
        ok             = expect(got == (c.isChar && c.slotIsRanged), "compose free == pin formula") && ok;
        ok             = expect(got == inlineShouldApplyRangedDamageMultiplier(c.isChar, c.slotIsRanged),
                    "compose free == inline") &&
             ok;
    }

    // Production inject pin: OnRangedAttack (~3352) injects
    // isChar and slot == SLOT_RANGED (slotIsRanged).
    // SLOT_RANGED = 0x02 typically; host compares slot equality before inject.
    constexpr uint8 slotRanged = 0x02;
    constexpr uint8 slotAmmo   = 0x03;
    const auto      slotIsRangedFromSlot = [](const uint8 slot) -> bool {
        return slot == slotRanged;
    };
    ok = expect(ShouldApplyRangedDamageMultiplier(true, slotIsRangedFromSlot(slotRanged)),
                "slot == SLOT_RANGED pin must apply multiplier on char") &&
         ok;
    ok = expect(!ShouldApplyRangedDamageMultiplier(true, slotIsRangedFromSlot(slotAmmo)),
                "slot == SLOT_AMMO pin must skip multiplier") &&
         ok;

    return ok;
}
