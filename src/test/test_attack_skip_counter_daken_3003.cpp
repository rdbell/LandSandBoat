#include "test_attack_skip_counter_daken_3003.h"

#include "map/attack_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "attack ShouldSkipCounterForDaken 3003 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CAttack::CheckCounter Daken early-out for dual-wire cross-check
// (slice 3003):
//   attackType == PHYSICAL_ATTACK_TYPE::DAKEN
auto inlineShouldSkipCounterForDaken(const uint8 attackType) -> bool
{
    return attackType == attackhelpers::AttackTypeDaken;
}

} // namespace

// Pure dual-wire expansion for attackhelpers::ShouldSkipCounterForDaken
// (IsDakenAttack / attackType == DAKEN; slice 3003).
auto runAttackSkipCounterDaken3003SelfTests() -> bool
{
    using namespace attackhelpers;

    bool ok = true;

    // Residual 1376 truth-table pins.
    ok = expect(ShouldSkipCounterForDaken(AttackTypeDaken), "residual: Daken skips counter") && ok;
    ok = expect(!ShouldSkipCounterForDaken(AttackTypeNormal), "residual: Normal does not skip counter") && ok;
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
        { AttackTypeDaken, true, "DAKEN skips counter" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldSkipCounterForDaken(c.attackType);
        const bool inlineF = inlineShouldSkipCounterForDaken(c.attackType);
        const bool shared  = IsDakenAttack(c.attackType);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldSkipCounterForDaken dual-wire == inline LSB formula") && ok;
        ok = expect(got == shared, "free == IsDakenAttack") && ok;
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

        ok = expect(got == c.wantSkip, c.label) && ok;
        ok = expect(got == inlineF, "host compose dual-wire free == inline") && ok;
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
        const bool got  = ShouldSkipCounterForDaken(attackType);
        const bool want = attackType == AttackTypeDaken;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldSkipCounterForDaken(attackType),
                    "compose free == inline") &&
             ok;
        ok = expect(got == IsDakenAttack(attackType), "compose free == IsDakenAttack") && ok;
    }

    // Residual independence (1376 / 2996): counter skip is distinct from
    // parry / anticipate sibling free functions (same formula, different hosts).
    ok = expect(ShouldSkipCounterForDaken(AttackTypeDaken),
                "Daken must skip counter via ShouldSkipCounterForDaken") &&
         ok;
    ok = expect(ShouldSkipParryForDaken(AttackTypeDaken),
                "Daken still skips parry under dual-wire sibling 2996") &&
         ok;
    ok = expect(ShouldSkipAnticipateForDaken(AttackTypeDaken),
                "Daken still skips anticipate under dual-wire residual") &&
         ok;
    ok = expect(!ShouldSkipCounterForDaken(AttackTypeNormal), "Normal must not skip counter") && ok;
    ok = expect(!ShouldSkipParryForDaken(AttackTypeNormal),
                "Normal must not skip parry via dual-wire sibling") &&
         ok;
    ok = expect(!ShouldSkipAnticipateForDaken(AttackTypeNormal),
                "Normal must not skip anticipate via residual sibling") &&
         ok;

    return ok;
}
