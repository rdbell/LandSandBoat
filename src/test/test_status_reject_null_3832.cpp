#include "test_status_reject_null_3832.h"

#include "map/status_effect_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "status effect ShouldRejectNullStatusEffect 3832 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline AddStatusEffect null-effect formula for dual-wire cross-check
// (dedicated slice 3832 expand residual 3080; prior dedicated 3787 / 3742 / 3697 / 3652 / 3607 / 3562 / 3526 / 3481 / 3428 / 3348):
//   isNull
auto inlineShouldRejectNullStatusEffect(const bool isNull) -> bool
{
    return isNull;
}

// Compact dual-wire pin matching Go pinShouldRejectNullStatusEffect3832 /
// C++ capacity identity form (formula unchanged from 1371 / 3080 / 3348 / 3428 / 3481 / 3526 / 3562 / 3607 / 3652 / 3697 / 3742 / 3787):
//   isNull
auto pinShouldRejectNullStatusEffect(const bool isNull) -> bool
{
    return isNull;
}

// Prior dedicated 3787 pin (retained) for free==inline==pin==pin3787.
auto pinShouldRejectNullStatusEffect3787(const bool isNull) -> bool
{
    return isNull;
}

} // namespace

// Pure dual-wire expansion for statuseffecthelpers::ShouldRejectNullStatusEffect
// (isNull identity; OmegaXI internal/statuseffect; dedicated slice 3832
// expand residual 3080 / prior dedicated 3787 / 3742 / 3697 / 3652 / 3607 / 3562 / 3526 / 3481 / 3428 / 3348 / pure 1371).
//
// Coverage:
//   - free == inline == pin == pin3787 identity form
//   - poles: null rejects; non-null proceeds
//   - residual 3080 / 1371 pins still hold
//   - prior 3348 / 3428 / 3481 / 3526 / 3562 / 3607 / 3652 / 3697 / 3742 / 3787 dedicated poles still hold
//   - dense full 2^1 boolean space
//   - host-style inject + residual independence (2932 / clamp / 3049 / 3069;
//     3113 / 3135 left alone)
// Formula unchanged — not registered in CMake/main.
auto runStatusRejectNull3832SelfTests() -> bool
{
    using statuseffecthelpers::CanGainWhenNoExisting;
    using statuseffecthelpers::MaxEffectID;
    using statuseffecthelpers::ShouldClampMinDuration;
    using statuseffecthelpers::ShouldExpireEffect;
    using statuseffecthelpers::ShouldRejectEffectIDOutOfRange;
    using statuseffecthelpers::ShouldRejectNullStatusEffect;
    using statuseffecthelpers::ShouldRejectSimpleImmunity;
    using statuseffecthelpers::ShouldTickEffect;

    bool ok = true;

    // Residual 1371 / 3080 pins still hold under dedicated dual-wire.
    ok = expect(ShouldRejectNullStatusEffect(true), "residual: null effect rejects") && ok;
    ok = expect(!ShouldRejectNullStatusEffect(false), "residual: non-null effect proceeds") && ok;

    // --- Core poles: free == inline == pin == pin3787 identity form ---
    const struct
    {
        bool        isNull;
        bool        want;
        const char* label;
    } cases[] = {
        // Pole: null PStatusEffectPtr rejects (warning + false before other gates).
        { true, true, "null effect rejects" },

        // Pole: non-null PStatusEffectPtr proceeds to ID-range / CanGain.
        { false, false, "non-null effect proceeds" },

        // Residual 3080 / 1371 re-pins.
        { true, true, "residual 3080 null rejects" },
        { false, false, "residual 3080 non-null proceeds" },
        { true, true, "residual 1371 null rejects" },
        { false, false, "residual 1371 non-null proceeds" },

        // Prior dedicated 3348 poles still hold.
        { true, true, "prior 3348 null rejects" },
        { false, false, "prior 3348 non-null proceeds" },

        // Prior dedicated 3428 poles still hold.
        { true, true, "prior 3428 null rejects" },
        { false, false, "prior 3428 non-null proceeds" },

        // Prior dedicated 3481 poles still hold.
        { true, true, "prior 3481 null rejects" },
        { false, false, "prior 3481 non-null proceeds" },

        // Prior dedicated 3526 poles still hold.
        { true, true, "prior 3526 null rejects" },
        { false, false, "prior 3526 non-null proceeds" },

        // Prior dedicated 3562 poles still hold.
        { true, true, "prior 3562 null rejects" },
        { false, false, "prior 3562 non-null proceeds" },

        // Prior dedicated 3607 poles still hold.
        { true, true, "prior 3607 null rejects" },
        { false, false, "prior 3607 non-null proceeds" },

        // Prior dedicated 3652 poles still hold.
        { true, true, "prior 3652 null rejects" },
        { false, false, "prior 3652 non-null proceeds" },

        // Prior dedicated 3697 poles still hold.
        { true, true, "prior 3697 null rejects" },
        { false, false, "prior 3697 non-null proceeds" },

        // Prior dedicated 3742 poles still hold.
        { true, true, "prior 3742 null rejects" },
        { false, false, "prior 3742 non-null proceeds" },

        // Prior dedicated 3787 poles still hold.
        { true, true, "prior 3787 null rejects" },
        { false, false, "prior 3787 non-null proceeds" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRejectNullStatusEffect(c.isNull);
        const bool inlineF = inlineShouldRejectNullStatusEffect(c.isNull);
        const bool pinGot  = pinShouldRejectNullStatusEffect(c.isNull);
        const bool pin3787 = pinShouldRejectNullStatusEffect3787(c.isNull);
        const bool wantPin = c.isNull;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "dual-wire free==inline") && ok;
        ok = expect(got == pinGot, "dual-wire free==pin") && ok;
        ok = expect(got == pin3787, "dual-wire free==pin3787") && ok;
        ok = expect(got == wantPin, "dual-wire free==identity pin form") && ok;
    }

    // Free == pin across residual poles.
    ok = expect(ShouldRejectNullStatusEffect(true) == pinShouldRejectNullStatusEffect(true),
                "free==pin null effect") &&
         ok;
    ok = expect(ShouldRejectNullStatusEffect(false) == pinShouldRejectNullStatusEffect(false),
                "free==pin non-null effect") &&
         ok;

    // Free == pin3787 across residual poles (prior dedicated retained).
    ok = expect(ShouldRejectNullStatusEffect(true) == pinShouldRejectNullStatusEffect3787(true),
                "free==pin3787 null effect") &&
         ok;
    ok = expect(ShouldRejectNullStatusEffect(false) == pinShouldRejectNullStatusEffect3787(false),
                "free==pin3787 non-null effect") &&
         ok;

    // Dense compose: full 2^1 boolean space free == inline == pin == pin3787.
    for (const bool isNull : { false, true })
    {
        const bool got     = ShouldRejectNullStatusEffect(isNull);
        const bool inlineF = inlineShouldRejectNullStatusEffect(isNull);
        const bool pinGot  = pinShouldRejectNullStatusEffect(isNull);
        const bool pin3787 = pinShouldRejectNullStatusEffect3787(isNull);
        ok                 = expect(got == inlineF, "compose free==inline") && ok;
        ok                 = expect(got == pinGot, "compose free==pin") && ok;
        ok                 = expect(got == pin3787, "compose free==pin3787") && ok;
        ok                 = expect(got == isNull, "compose free==identity") && ok;
    }

    // Host-style inject poles: CStatusEffectContainer::AddStatusEffect injects
    // PStatusEffectPtr == nullptr.
    const struct
    {
        bool        isNull;
        bool        wantReject;
        const char* label;
    } hostCases[] = {
        { true, true, "PStatusEffectPtr == nullptr → warning + false before other gates" },
        { false, false, "PStatusEffectPtr non-null → proceed to ID-range / CanGain" },
    };

    for (const auto& c : hostCases)
    {
        const bool got     = ShouldRejectNullStatusEffect(c.isNull);
        const bool inlineF = inlineShouldRejectNullStatusEffect(c.isNull);
        const bool pinGot  = pinShouldRejectNullStatusEffect(c.isNull);
        const bool pin3787 = pinShouldRejectNullStatusEffect3787(c.isNull);

        ok = expect(got == c.wantReject, c.label) && ok;
        ok = expect(got == inlineF, "host compose free==inline") && ok;
        ok = expect(got == pinGot, "host compose free==pin") && ok;
        ok = expect(got == pin3787, "host compose free==pin3787") && ok;
    }

    // Production CStatusEffectContainer::AddStatusEffect path semantics:
    // null → reject before other gates; non-null → proceed.
    ok = expect(ShouldRejectNullStatusEffect(true) && pinShouldRejectNullStatusEffect(true),
                "AddStatusEffect null → reject path") &&
         ok;
    ok = expect(!ShouldRejectNullStatusEffect(false) && !pinShouldRejectNullStatusEffect(false),
                "AddStatusEffect non-null → proceed path") &&
         ok;

    // Residual independence (1371 / 2932 / 3080 / 3348 / 3428 / 3481 / 3526 /
    // 3562 / 3607 / 3652 / 3697 / 3742 / 3787; 3049 / 3069 / 3113 / 3135 left alone): null reject is distinct from
    // ID-range / clamp / expire / tick / immunity / can-gain-no-existing poles.
    ok = expect(ShouldRejectNullStatusEffect(true),
                "null effect must reject via free gate") &&
         ok;
    ok = expect(ShouldRejectEffectIDOutOfRange(MaxEffectID, MaxEffectID),
                "ID at MaxEffectID still rejects under dual-wire residual (2932 left alone)") &&
         ok;
    ok = expect(!ShouldRejectEffectIDOutOfRange(0, MaxEffectID),
                "valid ID must not reject via ID-range gate") &&
         ok;
    ok = expect(ShouldClampMinDuration(5, 10) && !ShouldClampMinDuration(10, 10),
                "min-duration residual still holds under dual-wire") &&
         ok;
    ok = expect(!ShouldRejectNullStatusEffect(false),
                "non-null must proceed even if later ID-range / clamp fails") &&
         ok;
    ok = expect(ShouldExpireEffect(true, 100, 100),
                "expire residual still holds (orthogonal sibling 3049/3225 left alone)") &&
         ok;
    ok = expect(ShouldTickEffect(true, 0, 1),
                "tick residual still holds (orthogonal sibling 3069 left alone)") &&
         ok;
    // 3113 / 3135 left alone: light residual independence pins only (not thrash).
    ok = expect(ShouldRejectSimpleImmunity(true) && !ShouldRejectSimpleImmunity(false),
                "simple-immunity residual still holds (3113 left alone)") &&
         ok;
    ok = expect(CanGainWhenNoExisting(),
                "can-gain-no-existing residual still holds (3135 left alone)") &&
         ok;

    // Explicit dual-wire poles across dense 2^1 space; orthogonal siblings
    // unchanged under null-add compose.
    for (const bool isNull : { false, true })
    {
        const bool got     = ShouldRejectNullStatusEffect(isNull);
        const bool inlineF = inlineShouldRejectNullStatusEffect(isNull);
        const bool pinGot  = pinShouldRejectNullStatusEffect(isNull);
        const bool pin3787 = pinShouldRejectNullStatusEffect3787(isNull);
        ok                 = expect(got == isNull, "dense free==identity") && ok;
        ok                 = expect(got == inlineF && got == pinGot && got == pin3787,
                    "dense free==inline==pin==pin3787") &&
             ok;
        ok = expect(ShouldExpireEffect(true, 100, 100),
                    "expire residual flipped under null-add compose") &&
             ok;
        ok = expect(ShouldTickEffect(true, 0, 1),
                    "tick residual flipped under null-add compose") &&
             ok;
    }

    return ok;
}
