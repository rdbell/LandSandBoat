#include "test_attack_skip_anticipate_daken_3004.h"

#include "map/attack_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "attack ShouldSkipAnticipateForDaken 3004 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CAttack::CheckAnticipated Daken early-out for dual-wire cross-check
// (slice 3004):
//   attackType == PHYSICAL_ATTACK_TYPE::DAKEN
auto inlineShouldSkipAnticipateForDaken(const uint8 attackType) -> bool
{
    return attackType == attackhelpers::AttackTypeDaken;
}

} // namespace

// Pure dual-wire expansion for attackhelpers::ShouldSkipAnticipateForDaken
// (IsDakenAttack / attackType == DAKEN; slice 3004).
auto runAttackSkipAnticipateDaken3004SelfTests() -> bool
{
    using namespace attackhelpers;

    bool ok = true;

    // Residual 1376 truth-table pins.
    ok = expect(ShouldSkipAnticipateForDaken(AttackTypeDaken), "residual: Daken skips anticipate") && ok;
    ok = expect(!ShouldSkipAnticipateForDaken(AttackTypeNormal), "residual: Normal does not skip anticipate") && ok;
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
        { AttackTypeDaken, true, "DAKEN skips anticipate" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldSkipAnticipateForDaken(c.attackType);
        const bool inlineF = inlineShouldSkipAnticipateForDaken(c.attackType);
        const bool shared  = IsDakenAttack(c.attackType);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldSkipAnticipateForDaken dual-wire == inline LSB formula") && ok;
        ok = expect(got == shared, "free == IsDakenAttack") && ok;
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

        ok = expect(got == c.wantSkip, c.label) && ok;
        ok = expect(got == inlineF, "host compose dual-wire free == inline") && ok;
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
        const bool got  = ShouldSkipAnticipateForDaken(attackType);
        const bool want = attackType == AttackTypeDaken;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldSkipAnticipateForDaken(attackType),
                    "compose free == inline") &&
             ok;
        ok = expect(got == IsDakenAttack(attackType), "compose free == IsDakenAttack") && ok;
    }

    // Residual independence (1376 / 2996 / 3003): anticipate skip is distinct
    // from parry / counter sibling free functions (same formula, different hosts).
    ok = expect(ShouldSkipAnticipateForDaken(AttackTypeDaken),
                "Daken must skip anticipate via ShouldSkipAnticipateForDaken") &&
         ok;
    ok = expect(ShouldSkipParryForDaken(AttackTypeDaken),
                "Daken still skips parry under dual-wire sibling 2996") &&
         ok;
    ok = expect(ShouldSkipCounterForDaken(AttackTypeDaken),
                "Daken still skips counter under dual-wire sibling 3003") &&
         ok;
    ok = expect(!ShouldSkipAnticipateForDaken(AttackTypeNormal), "Normal must not skip anticipate") && ok;
    ok = expect(!ShouldSkipParryForDaken(AttackTypeNormal),
                "Normal must not skip parry via dual-wire sibling") &&
         ok;
    ok = expect(!ShouldSkipCounterForDaken(AttackTypeNormal),
                "Normal must not skip counter via dual-wire sibling") &&
         ok;

    return ok;
}
