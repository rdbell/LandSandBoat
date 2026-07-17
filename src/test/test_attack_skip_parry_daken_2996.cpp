#include "test_attack_skip_parry_daken_2996.h"

#include "map/attack_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "attack ShouldSkipParryForDaken 2996 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CAttack::CheckParried Daken early-out for dual-wire cross-check
// (slice 2996):
//   attackType == PHYSICAL_ATTACK_TYPE::DAKEN
auto inlineShouldSkipParryForDaken(const uint8 attackType) -> bool
{
    return attackType == attackhelpers::AttackTypeDaken;
}

} // namespace

// Pure dual-wire expansion for attackhelpers::ShouldSkipParryForDaken
// (IsDakenAttack / attackType == DAKEN; slice 2996).
auto runAttackSkipParryDaken2996SelfTests() -> bool
{
    using namespace attackhelpers;

    bool ok = true;

    // Residual 1376 truth-table pins.
    ok = expect(ShouldSkipParryForDaken(AttackTypeDaken), "residual: Daken skips parry") && ok;
    ok = expect(!ShouldSkipParryForDaken(AttackTypeNormal), "residual: Normal does not skip parry") && ok;
    ok = expect(IsDakenAttack(AttackTypeDaken) && !IsDakenAttack(AttackTypeNormal),
                "residual: IsDakenAttack shared predicate") &&
         ok;

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
        const bool shared  = IsDakenAttack(c.attackType);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldSkipParryForDaken dual-wire == inline LSB formula") && ok;
        ok = expect(got == shared, "free == IsDakenAttack") && ok;
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

        ok = expect(got == c.wantSkip, c.label) && ok;
        ok = expect(got == inlineF, "host compose dual-wire free == inline") && ok;
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

    // Dense compose: free == inline == IsDakenAttack for known type pins.
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

    // Residual independence (1376): parry skip is distinct from counter /
    // anticipate sibling free functions (same formula, different hosts).
    ok = expect(ShouldSkipParryForDaken(AttackTypeDaken),
                "Daken must skip parry via ShouldSkipParryForDaken") &&
         ok;
    ok = expect(ShouldSkipCounterForDaken(AttackTypeDaken),
                "Daken still skips counter under dual-wire residual") &&
         ok;
    ok = expect(ShouldSkipAnticipateForDaken(AttackTypeDaken),
                "Daken still skips anticipate under dual-wire residual") &&
         ok;
    ok = expect(!ShouldSkipParryForDaken(AttackTypeNormal), "Normal must not skip parry") && ok;
    ok = expect(!ShouldSkipCounterForDaken(AttackTypeNormal),
                "Normal must not skip counter via residual sibling") &&
         ok;
    ok = expect(!ShouldSkipAnticipateForDaken(AttackTypeNormal),
                "Normal must not skip anticipate via residual sibling") &&
         ok;

    return ok;
}
