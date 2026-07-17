#include "test_attack_skip_anticipate_daken_3346.h"

#include "map/attack_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "attack ShouldSkipAnticipateForDaken 3346 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CAttack::CheckAnticipated Daken early-out for dual-wire cross-check
// (dedicated slice 3346):
//   attackType == PHYSICAL_ATTACK_TYPE::DAKEN
auto inlineShouldSkipAnticipateForDaken(const uint8 attackType) -> bool
{
    return attackType == attackhelpers::AttackTypeDaken;
}

// Compact dual-wire pin matching Go pinShouldSkipAnticipateForDaken3346 / C++ capacity:
//   IsDakenAttack(attackType)
auto pinShouldSkipAnticipateForDaken(const uint8 attackType) -> bool
{
    return attackhelpers::IsDakenAttack(attackType);
}

} // namespace

// Pure dual-wire expansion for attackhelpers::ShouldSkipAnticipateForDaken
// (IsDakenAttack / attackType == DAKEN; dedicated slice 3346;
// residual expand 3004 / pure 1376).
//
// Coverage:
//   - free == inline == pin == IsDakenAttack(attackType) == (type == DAKEN)
//   - residual 1376 / 3004 pins still hold
//   - siblings ShouldSkipParryForDaken (3166) /
//     ShouldSkipCounterForDaken (3204) residual only
auto runAttackSkipAnticipateDaken3346SelfTests() -> bool
{
    using namespace attackhelpers;

    bool ok = true;

    // Residual 1376 / 3004 pins still hold under dual-wire.
    ok = expect(ShouldSkipAnticipateForDaken(AttackTypeDaken), "residual: Daken skips anticipate") && ok;
    ok = expect(!ShouldSkipAnticipateForDaken(AttackTypeNormal), "residual: Normal does not skip anticipate") && ok;
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
        { AttackTypeDaken, true, "DAKEN skips anticipate" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldSkipAnticipateForDaken(c.attackType);
        const bool inlineF = inlineShouldSkipAnticipateForDaken(c.attackType);
        const bool pin     = pinShouldSkipAnticipateForDaken(c.attackType);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF && got == pin, "ShouldSkipAnticipateForDaken dual-wire free == inline == pin") &&
             ok;
        ok = expect(got == IsDakenAttack(c.attackType), "free == IsDakenAttack") && ok;
        ok = expect(got == (c.attackType == AttackTypeDaken), "free == attackType == DAKEN") && ok;
    }

    // Pin composition: free == IsDakenAttack == (type == DAKEN).
    ok = expect(ShouldSkipAnticipateForDaken(AttackTypeDaken), "DAKEN must skip") && ok;
    ok = expect(!ShouldSkipAnticipateForDaken(AttackTypeNormal), "NORMAL must not skip") && ok;

    // Host path: CAttack::CheckAnticipated injects attackType and early-returns
    // false when skip is true (no Third Eye / Seigan / retention evaluation).
    const struct
    {
        uint8       attackType;
        bool        wantSkip;
        const char* label;
    } hostCases[] = {
        { AttackTypeDaken, true, "DAKEN → CheckAnticipated returns false early" },
        { AttackTypeNormal, false, "NORMAL → host continues anticipate path" },
        { AttackTypeKick, false, "KICK → host continues anticipate path" },
        { AttackTypeZanshin, false, "ZANSHIN → host continues anticipate path" },
    };

    for (const auto& c : hostCases)
    {
        const bool got     = ShouldSkipAnticipateForDaken(c.attackType);
        const bool inlineF = inlineShouldSkipAnticipateForDaken(c.attackType);
        const bool pin     = pinShouldSkipAnticipateForDaken(c.attackType);

        ok = expect(got == c.wantSkip, c.label) && ok;
        ok = expect(got == inlineF && got == pin, "host compose dual-wire free == inline == pin") && ok;
        ok = expect(got == IsDakenAttack(c.attackType), "host compose free == IsDakenAttack") && ok;

        // Host compose: if (skip) return false; else continue path.
        const bool anticipateAttemptAllowed = !got;
        if (c.wantSkip)
        {
            ok = expect(!anticipateAttemptAllowed, "skip must force early return (no anticipate attempt)") && ok;
        }
        else
        {
            ok = expect(anticipateAttemptAllowed, "non-skip must allow anticipate path") && ok;
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
        const bool got  = ShouldSkipAnticipateForDaken(attackType);
        const bool want = attackType == AttackTypeDaken;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldSkipAnticipateForDaken(attackType),
                    "compose free == inline") &&
             ok;
        ok = expect(got == pinShouldSkipAnticipateForDaken(attackType), "compose free == pin") && ok;
        ok = expect(got == IsDakenAttack(attackType), "compose free == IsDakenAttack") && ok;
    }

    // Sibling residual 3166 / 3204 still hold (not re-expanded under 3346).
    ok = expect(ShouldSkipParryForDaken(AttackTypeDaken),
                "sibling residual: Daken skips parry (3166)") &&
         ok;
    ok = expect(!ShouldSkipParryForDaken(AttackTypeNormal),
                "sibling residual: Normal does not skip parry (3166)") &&
         ok;
    ok = expect(ShouldSkipCounterForDaken(AttackTypeDaken),
                "sibling residual: Daken skips counter (3204)") &&
         ok;
    ok = expect(!ShouldSkipCounterForDaken(AttackTypeNormal),
                "sibling residual: Normal does not skip counter (3204)") &&
         ok;

    return ok;
}
