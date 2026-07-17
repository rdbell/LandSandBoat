#include "test_campaign_debit_selected_3397.h"

#include "map/campaign_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "campaign debit selected 3397 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua sigilOnEventFinish apply delCurrency compose for dual-wire checks
// (dedicated 3397 expand residual 3141; prior dedicated 3141 retained):
//   SigilBonusCost(selectedEffects) > 0
auto inlineShouldDebitSelectedEffects(const int32 selectedEffects) -> bool
{
    return campaignhelpers::SigilBonusCost(selectedEffects) > 0;
}

// Compact dual-wire pin matching Go pinShouldDebitSelectedEffects3397:
//   ShouldDebitBonusCost(SigilBonusCost(selectedEffects))
auto pinShouldDebitSelectedEffects(const int32 selectedEffects) -> bool
{
    return campaignhelpers::ShouldDebitBonusCost(
        campaignhelpers::SigilBonusCost(selectedEffects));
}

} // namespace

// Pure dual-wire expansion for campaignhelpers::ShouldDebitSelectedEffects
// (Lua sigilOnEventFinish apply delCurrency compose via SigilBonusCost then
// ShouldDebitBonusCost; OmegaXI internal/campaign; dedicated slice 3397 expand
// residual 3141).
//
// Coverage:
//   - free == inline == pin == ShouldDebitBonusCost(SigilBonusCost(...))
//   - residual 3141 / 2858 / 1115 pins still hold
//   - residual poles: zero-cost no debit, positive-cost debit
//   - dense selectedEffects masks
//   - sibling ShouldDebitBonusCost called, not reimplemented
auto runCampaignDebitSelected3397SelfTests() -> bool
{
    using campaignhelpers::ShouldDebitSelectedEffects;
    using campaignhelpers::ShouldDebitBonusCost;
    using campaignhelpers::SigilBonusCost;

    bool ok = true;

    // Residual 1115 / 2858 / 3141 pins still hold under dual-wire.
    ok = expect(!ShouldDebitSelectedEffects(0), "residual: no effects no debit") && ok;
    ok = expect(!ShouldDebitSelectedEffects(1 << 0), "residual: regen-only no debit") && ok;
    ok = expect(ShouldDebitSelectedEffects(1 << 1), "residual: refresh debit") && ok;
    ok = expect(ShouldDebitSelectedEffects(0x1E), "residual: bits 1-4 debit") && ok;
    ok = expect(ShouldDebitSelectedEffects(1 << 2), "residual: bit2 debit") && ok;
    ok = expect(ShouldDebitSelectedEffects(1 << 3), "residual: bit3 debit") && ok;
    ok = expect(ShouldDebitSelectedEffects(1 << 4), "residual: bit4 debit") && ok;
    ok = expect(ShouldDebitSelectedEffects(0x1F), "residual: bits 0-4 debit") && ok;

    // --- Composition table: free == inline == pin via SigilBonusCost ---
    const struct
    {
        int32       selectedEffects;
        bool        want;
        const char* label;
    } cases[] = {
        // Residual 3141 poles.
        { 0, false, "residual empty" },
        { 1, false, "residual bit0" },
        { 1 << 0, false, "residual regen-only" },
        { 2, true, "residual bit1" },
        { 4, true, "residual bit2" },
        { 6, true, "residual bits1+2" },
        { 8, true, "residual bit3" },
        { 16, true, "residual bit4" },
        { 0x1E, true, "residual 0x1E" },
        { 0x1F, true, "residual 0x1F" },

        // Residual poles: zero-cost no debit, positive-cost debit.
        { 0, false, "pole zero no debit" },
        { 1 << 0, false, "pole regen-only no debit" },
        { 1 << 1, true, "pole refresh debit" },
        { 0x1E, true, "pole bits1-4 debit" },

        // Dense host poles (required).
        { 0, false, "dense 0" },
        { 1, false, "dense 1" },
        { 2, true, "dense 2" },
        { 4, true, "dense 4" },
        { 6, true, "dense 6" },
        { 8, true, "dense 8" },
        { 16, true, "dense 16" },
        { 0x0A, true, "dense 0x0A" },
        { 0x15, true, "dense 0x15" },
        { 0x1A, true, "dense 0x1A" },
        { 0x1E, true, "dense 0x1E" },
        { 0x1F, true, "dense 0x1F" },
        // bit5+ not billed by SigilBonusCost (loop 1..4 only)
        { 1 << 5, false, "dense bit5 unbilled no debit" },
        { (1 << 5) | (1 << 0), false, "dense bit5+bit0 unbilled no debit" },
        { (1 << 5) | (1 << 1), true, "dense bit5+bit1 debit" },
    };

    for (const auto& c : cases)
    {
        const bool got       = ShouldDebitSelectedEffects(c.selectedEffects);
        const bool inlineGot = inlineShouldDebitSelectedEffects(c.selectedEffects);
        const bool pinGot    = pinShouldDebitSelectedEffects(c.selectedEffects);
        const bool compose   = ShouldDebitBonusCost(SigilBonusCost(c.selectedEffects));
        ok                   = expect(got == c.want, c.label) && ok;
        // Positive form: free == inline == pin.
        ok = expect(got == inlineGot && got == pinGot, "dual-wire free == inline == pin") && ok;
        ok = expect(got == compose, "dual-wire free==compose") && ok;
        // free == inline via SigilBonusCost(selectedEffects) > 0
        const bool costPositive = SigilBonusCost(c.selectedEffects) > 0;
        ok                      = expect(got == costPositive, "free==inline via SigilBonusCost") && ok;
    }

    // Explicit residual poles free == inline == pin.
    const struct
    {
        int32       selectedEffects;
        const char* label;
    } poles[] = {
        { 0, "zero no debit" },
        { 1 << 0, "regen-only no debit" },
        { 1 << 1, "refresh debit" },
        { 1 << 2, "bit2 debit" },
        { 1 << 3, "bit3 debit" },
        { 1 << 4, "bit4 debit" },
        { 6, "bits1+2 debit" },
        { 0x1E, "bits1-4 debit" },
        { 0x1F, "bits0-4 debit" },
        { 1 << 5, "bit5 unbilled no debit" },
    };

    for (const auto& pole : poles)
    {
        const bool got     = ShouldDebitSelectedEffects(pole.selectedEffects);
        const bool inlineF = inlineShouldDebitSelectedEffects(pole.selectedEffects);
        const bool pin     = pinShouldDebitSelectedEffects(pole.selectedEffects);
        const bool want    = ShouldDebitBonusCost(SigilBonusCost(pole.selectedEffects));

        ok = expect(got == want, pole.label) && ok;
        ok = expect(got == inlineF && got == pin, "pole free == inline == pin") && ok;
    }

    // Dense compose free == inline == pin over required masks + neighbors.
    const int32 dense[] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 14, 15, 16,
        0x0A, 0x15, 0x1A, 0x1C, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x3F,
    };

    for (const int32 effects : dense)
    {
        const bool got     = ShouldDebitSelectedEffects(effects);
        const bool inlineF = inlineShouldDebitSelectedEffects(effects);
        const bool pin     = pinShouldDebitSelectedEffects(effects);
        const bool want    = ShouldDebitBonusCost(SigilBonusCost(effects));

        ok = expect(got == want, "dense free == formula") && ok;
        ok = expect(got == inlineF && got == pin, "dense free == inline == pin") && ok;
        ok = expect(got == (SigilBonusCost(effects) > 0), "dense free==inline via SigilBonusCost") && ok;
    }

    // Host-style sigil apply inject poles (selectedEffects from option>>11).
    const int32 inject[] = { 0, 1, 2, 4, 6, 0x1E, 0x1F, 8, 16 };
    for (const int32 effects : inject)
    {
        const bool got     = ShouldDebitSelectedEffects(effects);
        const bool inlineF = inlineShouldDebitSelectedEffects(effects);
        const bool pin     = pinShouldDebitSelectedEffects(effects);
        const bool want    = ShouldDebitBonusCost(SigilBonusCost(effects));

        ok = expect(got == want, "inject free == formula") && ok;
        ok = expect(got == inlineF && got == pin, "inject free == inline == pin") && ok;
    }

    // --- Production sigil apply path semantics ---
    // Debit → may continue delCurrency path.
    // Zero-cost masks → skip delCurrency.
    ok = expect(ShouldDebitSelectedEffects(1 << 1), "apply debit → delCurrency path") && ok;
    ok = expect(!ShouldDebitSelectedEffects(0), "apply zero → skip delCurrency") && ok;
    ok = expect(!ShouldDebitSelectedEffects(1 << 0), "apply regen-only → skip delCurrency") && ok;
    ok = expect(ShouldDebitSelectedEffects(0x1E), "apply bits1-4 → delCurrency path") && ok;

    // Sibling gate remains the cost>0 authority (not reimplemented).
    ok = expect(ShouldDebitBonusCost(50), "sibling 3357/3103 positive debit") && ok;
    ok = expect(!ShouldDebitBonusCost(0), "sibling 3357/3103 zero no debit") && ok;
    ok = expect(ShouldDebitBonusCost(1), "sibling poles positive") && ok;
    ok = expect(!ShouldDebitBonusCost(-1), "sibling poles negative") && ok;

    return ok;
}
