#include "test_attack_skip_parry_daken_3166.h"

#include "map/attack_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "attack ShouldSkipParryForDaken 3166 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CAttack::CheckParried Daken early-out for dual-wire cross-check
// (dedicated slice 3166):
//   attackType == PHYSICAL_ATTACK_TYPE::DAKEN
auto inlineShouldSkipParryForDaken(const uint8 attackType) -> bool
{
    return attackType == attackhelpers::AttackTypeDaken;
}

// Compact dual-wire pin matching Go pinShouldSkipParryForDaken3166 / C++ capacity:
//   IsDakenAttack(attackType)
auto pinShouldSkipParryForDaken(const uint8 attackType) -> bool
{
    return attackhelpers::IsDakenAttack(attackType);
}

} // namespace

// Pure dual-wire expansion for attackhelpers::ShouldSkipParryForDaken
// (IsDakenAttack / attackType == DAKEN; dedicated slice 3166;
// residual expand 2996 / pure 1376).
//
// Coverage:
//   - free == inline == pin == IsDakenAttack(attackType) == (type == DAKEN)
//   - residual 1376 / 2996 pins still hold
//   - siblings ShouldSkipCounterForDaken (3003) /
//     ShouldSkipAnticipateForDaken (3004) residual only
auto runAttackSkipParryDaken3166SelfTests() -> bool
{
    using namespace attackhelpers;

    bool ok = true;

    // Residual 1376 / 2996 pins still hold under dual-wire.
    ok = expect(ShouldSkipParryForDaken(AttackTypeDaken), "residual: Daken skips parry") && ok;
    ok = expect(!ShouldSkipParryForDaken(AttackTypeNormal), "residual: Normal does not skip parry") && ok;
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
        { AttackTypeDaken, true, "DAKEN skips parry" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldSkipParryForDaken(c.attackType);
        const bool inlineF = inlineShouldSkipParryForDaken(c.attackType);
        const bool pin     = pinShouldSkipParryForDaken(c.attackType);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF && got == pin, "ShouldSkipParryForDaken dual-wire free == inline == pin") &&
             ok;
        ok = expect(got == IsDakenAttack(c.attackType), "free == IsDakenAttack") && ok;
        ok = expect(got == (c.attackType == AttackTypeDaken), "free == attackType == DAKEN") && ok;
    }

    // Pin composition: free == IsDakenAttack == (type == DAKEN).
    ok = expect(ShouldSkipParryForDaken(AttackTypeDaken), "DAKEN must skip") && ok;
    ok = expect(!ShouldSkipParryForDaken(AttackTypeNormal), "NORMAL must not skip") && ok;

    // Host path: CAttack::CheckParried injects attackType before IsParried.
    const struct
    {
        uint8       attackType;
        bool        wantSkip;
        const char* label;
    } hostCases[] = {
        { AttackTypeDaken, true, "DAKEN → skip IsParried lookup; preserve state" },
        { AttackTypeNormal, false, "NORMAL → host may call IsParried" },
        { AttackTypeKick, false, "KICK → host may call IsParried" },
        { AttackTypeZanshin, false, "ZANSHIN → host may call IsParried" },
    };

    for (const auto& c : hostCases)
    {
        const bool got     = ShouldSkipParryForDaken(c.attackType);
        const bool inlineF = inlineShouldSkipParryForDaken(c.attackType);
        const bool pin     = pinShouldSkipParryForDaken(c.attackType);

        ok = expect(got == c.wantSkip, c.label) && ok;
        ok = expect(got == inlineF && got == pin, "host compose dual-wire free == inline == pin") && ok;
        ok = expect(got == IsDakenAttack(c.attackType), "host compose free == IsDakenAttack") && ok;

        // Host compose: parryProcs = !skip && IsParried(...); skip forces false.
        const bool parryProcs = !got && true; // inject true lookup only when not skipped
        if (c.wantSkip)
        {
            ok = expect(!parryProcs, "skip must force parryProcs false") && ok;
        }
        else
        {
            ok = expect(parryProcs, "non-skip with true lookup must allow parryProcs") && ok;
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
        const bool got  = ShouldSkipParryForDaken(attackType);
        const bool want = attackType == AttackTypeDaken;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldSkipParryForDaken(attackType),
                    "compose free == inline") &&
             ok;
        ok = expect(got == pinShouldSkipParryForDaken(attackType), "compose free == pin") && ok;
        ok = expect(got == IsDakenAttack(attackType), "compose free == IsDakenAttack") && ok;
    }

    // ResolveParryCheck compose: Daken preserves state; normal OR-procs.
    ok = expect(!ResolveParryCheck(false, AttackTypeDaken, true).parried,
                "ResolveParryCheck Daken must skip proc") &&
         ok;
    ok = expect(ResolveParryCheck(true, AttackTypeDaken, false).parried,
                "ResolveParryCheck Daken must preserve existing parry") &&
         ok;
    ok = expect(ResolveParryCheck(false, AttackTypeNormal, true).parried,
                "ResolveParryCheck Normal must accept proc when not skipped") &&
         ok;

    // Sibling residual 3003 / 3004 still hold (not re-expanded under 3166).
    ok = expect(ShouldSkipCounterForDaken(AttackTypeDaken),
                "sibling residual: Daken skips counter (3003)") &&
         ok;
    ok = expect(!ShouldSkipCounterForDaken(AttackTypeNormal),
                "sibling residual: Normal does not skip counter (3003)") &&
         ok;
    ok = expect(ShouldSkipAnticipateForDaken(AttackTypeDaken),
                "sibling residual: Daken skips anticipate (3004)") &&
         ok;
    ok = expect(!ShouldSkipAnticipateForDaken(AttackTypeNormal),
                "sibling residual: Normal does not skip anticipate (3004)") &&
         ok;

    return ok;
}
