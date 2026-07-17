#include "test_attack_skip_counter_daken_3204.h"

#include "map/attack_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "attack ShouldSkipCounterForDaken 3204 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CAttack::CheckCounter Daken early-out for dual-wire cross-check
// (dedicated slice 3204):
//   attackType == PHYSICAL_ATTACK_TYPE::DAKEN
auto inlineShouldSkipCounterForDaken(const uint8 attackType) -> bool
{
    return attackType == attackhelpers::AttackTypeDaken;
}

// Compact dual-wire pin matching Go pinShouldSkipCounterForDaken3204 / C++ capacity:
//   IsDakenAttack(attackType)
auto pinShouldSkipCounterForDaken(const uint8 attackType) -> bool
{
    return attackhelpers::IsDakenAttack(attackType);
}

} // namespace

// Pure dual-wire expansion for attackhelpers::ShouldSkipCounterForDaken
// (IsDakenAttack / attackType == DAKEN; dedicated slice 3204;
// residual expand 3003 / pure 1376).
//
// Coverage:
//   - free == inline == pin == IsDakenAttack(attackType) == (type == DAKEN)
//   - residual 1376 / 3003 pins still hold
//   - siblings ShouldSkipParryForDaken (3166) /
//     ShouldSkipAnticipateForDaken (3004) residual only
auto runAttackSkipCounterDaken3204SelfTests() -> bool
{
    using namespace attackhelpers;

    bool ok = true;

    // Residual 1376 / 3003 pins still hold under dual-wire.
    ok = expect(ShouldSkipCounterForDaken(AttackTypeDaken), "residual: Daken skips counter") && ok;
    ok = expect(!ShouldSkipCounterForDaken(AttackTypeNormal), "residual: Normal does not skip counter") && ok;
    ok = expect(IsDakenAttack(AttackTypeDaken) && !IsDakenAttack(AttackTypeNormal),
                "residual: IsDakenAttack shared predicate") &&
         ok;

    // --- Composition table: free == inline == pin ---
    const struct
    {
        uint8       attackType;
        bool        want;
        const char* label;
    } cases[] = {
        { AttackTypeNormal, false, "NORMAL does not skip" },
        { AttackTypeDouble, false, "DOUBLE does not skip" },
        { AttackTypeTriple, false, "TRIPLE does not skip" },
        { AttackTypeZanshin, false, "ZANSHIN does not skip" },
        { AttackTypeKick, false, "KICK does not skip" },
        { AttackTypeSamba, false, "SAMBA does not skip" },
        { AttackTypeDaken, true, "DAKEN skips counter" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldSkipCounterForDaken(c.attackType);
        const bool inlineF = inlineShouldSkipCounterForDaken(c.attackType);
        const bool pin     = pinShouldSkipCounterForDaken(c.attackType);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF && got == pin, "ShouldSkipCounterForDaken dual-wire free == inline == pin") &&
             ok;
        ok = expect(got == IsDakenAttack(c.attackType), "free == IsDakenAttack") && ok;
        ok = expect(got == (c.attackType == AttackTypeDaken), "free == attackType == DAKEN") && ok;
    }

    // Pin composition: free == IsDakenAttack == (type == DAKEN).
    ok = expect(ShouldSkipCounterForDaken(AttackTypeDaken), "DAKEN must skip") && ok;
    ok = expect(!ShouldSkipCounterForDaken(AttackTypeNormal), "NORMAL must not skip") && ok;

    // Host path: CAttack::CheckCounter injects attackType and early-returns
    // false when skip is true (no state/merit/rate/facing evaluation).
    const struct
    {
        uint8       attackType;
        bool        wantSkip;
        const char* label;
    } hostCases[] = {
        { AttackTypeDaken, true, "DAKEN → CheckCounter returns false early" },
        { AttackTypeNormal, false, "NORMAL → host continues counter path" },
        { AttackTypeKick, false, "KICK → host continues counter path" },
        { AttackTypeZanshin, false, "ZANSHIN → host continues counter path" },
    };

    for (const auto& c : hostCases)
    {
        const bool got     = ShouldSkipCounterForDaken(c.attackType);
        const bool inlineF = inlineShouldSkipCounterForDaken(c.attackType);
        const bool pin     = pinShouldSkipCounterForDaken(c.attackType);

        ok = expect(got == c.wantSkip, c.label) && ok;
        ok = expect(got == inlineF && got == pin, "host compose dual-wire free == inline == pin") && ok;
        ok = expect(got == IsDakenAttack(c.attackType), "host compose free == IsDakenAttack") && ok;

        // Host compose: if (skip) return false; else continue path.
        const bool counteredAttemptAllowed = !got;
        if (c.wantSkip)
        {
            ok = expect(!counteredAttemptAllowed, "skip must force early return (no counter attempt)") && ok;
        }
        else
        {
            ok = expect(counteredAttemptAllowed, "non-skip must allow counter path") && ok;
        }
    }

    // Dense compose: free == inline == pin == IsDakenAttack for known type pins.
    const uint8 denseTypes[] = {
        AttackTypeNormal,
        AttackTypeDouble,
        AttackTypeTriple,
        AttackTypeZanshin,
        AttackTypeKick,
        AttackTypeSamba,
        AttackTypeDaken,
    };
    for (const uint8 attackType : denseTypes)
    {
        const bool got  = ShouldSkipCounterForDaken(attackType);
        const bool want = attackType == AttackTypeDaken;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldSkipCounterForDaken(attackType),
                    "compose free == inline") &&
             ok;
        ok = expect(got == pinShouldSkipCounterForDaken(attackType), "compose free == pin") && ok;
        ok = expect(got == IsDakenAttack(attackType), "compose free == IsDakenAttack") && ok;
    }

    // Sibling residual 3166 / 3004 still hold (not re-expanded under 3204).
    ok = expect(ShouldSkipParryForDaken(AttackTypeDaken),
                "sibling residual: Daken skips parry (3166)") &&
         ok;
    ok = expect(!ShouldSkipParryForDaken(AttackTypeNormal),
                "sibling residual: Normal does not skip parry (3166)") &&
         ok;
    ok = expect(ShouldSkipAnticipateForDaken(AttackTypeDaken),
                "sibling residual: Daken skips anticipate (3004)") &&
         ok;
    ok = expect(!ShouldSkipAnticipateForDaken(AttackTypeNormal),
                "sibling residual: Normal does not skip anticipate (3004)") &&
         ok;

    return ok;
}
