#include "test_campaign_debit_bonus_3357.h"

#include "map/campaign_capacity.h"

#include <cstdint>
#include <iostream>
#include <limits>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "campaign debit bonus 3357 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua sigilOnEventFinish apply delCurrency gate for dual-wire checks
// (dedicated 3357 expand residual 2858; prior dedicated 3103 retained):
//   if bonusCost > 0 then player:delCurrency('allied_notes', bonusCost) end
auto inlineShouldDebitBonusCost(const int32 bonusCost) -> bool
{
    return bonusCost > 0;
}

// Compact dual-wire pin matching Go pinShouldDebitBonusCost3357 / C++ capacity:
//   bonusCost > 0
auto pinShouldDebitBonusCost(const int32 bonusCost) -> bool
{
    return bonusCost > 0;
}

} // namespace

// Pure dual-wire expansion for campaignhelpers::ShouldDebitBonusCost
// (Lua sigilOnEventFinish apply delCurrency: bonusCost > 0;
// OmegaXI internal/campaign; dedicated slice 3357 expand residual 2858).
//
// Coverage:
//   - free == inline == pin == (bonusCost > 0)
//   - residual 2858 / 3103 / 1115 pins still hold
//   - poles: zero no debit, positive debit, negative no debit
//   - dense: 0, 1, 49, 50, 100, 150, 200, negatives, large ints
auto runCampaignDebitBonus3357SelfTests() -> bool
{
    using campaignhelpers::ShouldDebitBonusCost;
    using campaignhelpers::ShouldDebitSelectedEffects;
    using campaignhelpers::SigilBonusCost;

    bool ok = true;

    // Residual 1115 / 2858 / 3103 pins still hold under dual-wire.
    ok = expect(!ShouldDebitBonusCost(0), "residual: zero cost no debit") && ok;
    ok = expect(ShouldDebitBonusCost(50), "residual: one effect (50) debit") && ok;
    ok = expect(ShouldDebitBonusCost(1), "residual: one cost debit") && ok;
    ok = expect(!ShouldDebitBonusCost(-1), "residual: negative no debit") && ok;
    ok = expect(ShouldDebitBonusCost(200), "residual: four effects (200) debit") && ok;
    ok = expect(!ShouldDebitBonusCost(-50), "residual: negative 50 no debit") && ok;
    ok = expect(ShouldDebitBonusCost(100), "residual: two effects (100) debit") && ok;
    ok = expect(ShouldDebitBonusCost(150), "residual: three effects (150) debit") && ok;

    constexpr int32 kMaxInt = std::numeric_limits<int32>::max();
    constexpr int32 kMinInt = std::numeric_limits<int32>::min();

    // --- Composition table: free == inline == pin ---
    const struct
    {
        int32       bonusCost;
        bool        want;
        const char* label;
    } cases[] = {
        // Residual 2858 / 3103 poles.
        { 0, false, "residual zero" },
        { 1, true, "residual one" },
        { 49, true, "residual 49" },
        { 50, true, "residual 50 one effect" },
        { 100, true, "residual 100 two effects" },
        { 150, true, "residual 150 three effects" },
        { 200, true, "residual 200 four effects" },
        { -1, false, "residual -1" },
        { -50, false, "residual -50" },
        { 2, true, "residual two" },

        // Residual poles: zero no debit, positive debit, negative no debit.
        { 0, false, "pole zero no debit" },
        { 1, true, "pole one debit" },
        { 50, true, "pole 50 debit" },
        { -1, false, "pole negative no debit" },

        // Dense host poles (required).
        { 0, false, "dense 0" },
        { 1, true, "dense 1" },
        { 49, true, "dense 49" },
        { 50, true, "dense 50" },
        { 100, true, "dense 100" },
        { 150, true, "dense 150" },
        { 200, true, "dense 200" },
        { -1, false, "dense -1" },
        { -50, false, "dense -50" },

        // Large ints / MaxInt-ish values.
        { kMaxInt, true, "MaxInt debit" },
        { kMaxInt - 1, true, "MaxInt-1 debit" },
        { kMinInt, false, "MinInt no debit" },
        { kMinInt + 1, false, "MinInt+1 no debit" },
        { 0x7FFFFFFF, true, "0x7FFFFFFF debit" },
        { static_cast<int32>(0x80000000), false, "min int32 no debit" },
        { 1 << 30, true, "1<<30 debit" },
        { -(1 << 30), false, "-(1<<30) no debit" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldDebitBonusCost(c.bonusCost);
        const bool inlineF = inlineShouldDebitBonusCost(c.bonusCost);
        const bool pin     = pinShouldDebitBonusCost(c.bonusCost);
        const bool wantPin = c.bonusCost > 0;

        ok = expect(got == c.want, c.label) && ok;
        // Positive form: free == inline == pin.
        ok = expect(got == inlineF && got == pin, "dual-wire free == inline == pin") && ok;
        ok = expect(got == wantPin, "free == pin formula bonusCost > 0") && ok;
    }

    // Explicit residual poles free == inline == pin.
    const struct
    {
        int32       bonusCost;
        const char* label;
    } poles[] = {
        { 0, "zero no debit" },
        { 1, "one debit" },
        { 50, "50 debit" },
        { 100, "100 debit" },
        { 200, "200 debit" },
        { -1, "negative no debit" },
        { -50, "negative 50 no debit" },
        { kMaxInt, "MaxInt debit" },
        { kMinInt, "MinInt no debit" },
        { 0x7FFFFFFF, "0x7FFFFFFF debit" },
    };

    for (const auto& pole : poles)
    {
        const bool got     = ShouldDebitBonusCost(pole.bonusCost);
        const bool inlineF = inlineShouldDebitBonusCost(pole.bonusCost);
        const bool pin     = pinShouldDebitBonusCost(pole.bonusCost);
        const bool want    = pole.bonusCost > 0;

        ok = expect(got == want, pole.label) && ok;
        ok = expect(got == inlineF && got == pin, "pole free == inline == pin") && ok;
    }

    // Dense compose free == inline == pin over required costs + neighbors.
    const int32 dense[] = {
        static_cast<int32>(0x80000000),
        -50,
        -1,
        0,
        1,
        2,
        49,
        50,
        51,
        100,
        150,
        200,
        kMaxInt,
        kMinInt,
        0x7FFFFFFF,
        1 << 30,
    };

    for (const int32 cost : dense)
    {
        const bool got     = ShouldDebitBonusCost(cost);
        const bool inlineF = inlineShouldDebitBonusCost(cost);
        const bool pin     = pinShouldDebitBonusCost(cost);
        const bool want    = cost > 0;

        ok = expect(got == want, "dense free == formula") && ok;
        ok = expect(got == inlineF && got == pin, "dense free == inline == pin") && ok;
    }

    // Host-style sigil apply inject poles (bonusCost from bits-1..4 × 50).
    const int32 inject[] = { 0, 50, 100, 150, 200, 1, -1 };
    for (const int32 cost : inject)
    {
        const bool got     = ShouldDebitBonusCost(cost);
        const bool inlineF = inlineShouldDebitBonusCost(cost);
        const bool pin     = pinShouldDebitBonusCost(cost);
        const bool want    = cost > 0;

        ok = expect(got == want, "inject free == formula") && ok;
        ok = expect(got == inlineF && got == pin, "inject free == inline == pin") && ok;
    }

    // Apply-path dual-wire: ShouldDebitSelectedEffects → SigilBonusCost then gate.
    // Sibling 3141 compose; call only (do not thrash debit_selected).
    ok = expect(!ShouldDebitSelectedEffects(0), "no effects should not debit") && ok;
    ok = expect(!ShouldDebitSelectedEffects(1 << 0), "regen-only should not debit") && ok;
    ok = expect(ShouldDebitSelectedEffects(1 << 1), "refresh should debit") && ok;
    ok = expect(ShouldDebitSelectedEffects(0x1E), "bits 1-4 should debit") && ok;

    const int32 effectsCases[] = { 0, 1, 2, 4, 6, 0x1E, 0x1F };
    for (const int32 effects : effectsCases)
    {
        const bool got  = ShouldDebitSelectedEffects(effects);
        const bool want = ShouldDebitBonusCost(SigilBonusCost(effects));
        ok              = expect(got == want, "compose ShouldDebitSelectedEffects") && ok;
        ok              = expect(got == inlineShouldDebitBonusCost(SigilBonusCost(effects)),
                    "compose dual-wire free==inline via SigilBonusCost") &&
             ok;
        ok = expect(got == pinShouldDebitBonusCost(SigilBonusCost(effects)),
                    "compose dual-wire free==pin via SigilBonusCost") &&
             ok;
    }

    // Production path semantics still hold.
    ok = expect(ShouldDebitBonusCost(50), "apply debit → delCurrency path") && ok;
    ok = expect(!ShouldDebitBonusCost(0), "apply zero → skip delCurrency") && ok;
    ok = expect(!ShouldDebitBonusCost(-1), "apply negative → skip delCurrency") && ok;
    ok = expect(ShouldDebitBonusCost(1), "1>0 must debit") && ok;
    ok = expect(ShouldDebitBonusCost(200), "200>0 must debit") && ok;

    return ok;
}
