#include "test_campaign_debit_selected_3141.h"

#include "map/campaign_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "campaign debit selected 3141 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua sigilOnEventFinish apply delCurrency compose for dual-wire checks:
//   SigilBonusCost(selectedEffects) > 0
auto inlineShouldDebitSelectedEffects(const int32 selectedEffects) -> bool
{
    return campaignhelpers::SigilBonusCost(selectedEffects) > 0;
}

// Compact dual-wire pin matching Go pinShouldDebitSelectedEffects3141:
//   ShouldDebitBonusCost(SigilBonusCost(selectedEffects))
auto pinShouldDebitSelectedEffects(const int32 selectedEffects) -> bool
{
    return campaignhelpers::ShouldDebitBonusCost(
        campaignhelpers::SigilBonusCost(selectedEffects));
}

} // namespace

// Pure dual-wire expansion for campaignhelpers::ShouldDebitSelectedEffects
// (Lua sigilOnEventFinish apply delCurrency compose via SigilBonusCost then
// ShouldDebitBonusCost; OmegaXI internal/campaign; slice 3141).
//
// Coverage:
//   - zero-cost selectedEffects → no debit
//   - positive-cost selectedEffects → debit
//   - free == inline via SigilBonusCost
//   - free == pin == ShouldDebitBonusCost(SigilBonusCost(...))
//   - residual 1115 / 2858 compose pins still hold
//   - sibling 3103 ShouldDebitBonusCost not reimplemented
auto runCampaignDebitSelected3141SelfTests() -> bool
{
    using campaignhelpers::ShouldDebitSelectedEffects;
    using campaignhelpers::ShouldDebitBonusCost;
    using campaignhelpers::SigilBonusCost;

    bool ok = true;

    // Residual 1115 / 2858 compose pins still hold under dual-wire.
    ok = expect(!ShouldDebitSelectedEffects(0), "residual: no effects no debit") && ok;
    ok = expect(!ShouldDebitSelectedEffects(1 << 0), "residual: regen-only no debit") && ok;
    ok = expect(ShouldDebitSelectedEffects(1 << 1), "residual: refresh debit") && ok;
    ok = expect(ShouldDebitSelectedEffects(0x1E), "residual: bits 1-4 debit") && ok;

    // --- zero cost selectedEffects → no debit ---
    ok = expect(!ShouldDebitSelectedEffects(0), "empty mask no debit") && ok;
    ok = expect(!ShouldDebitSelectedEffects(1 << 0), "bit0 unbilled no debit") && ok;
    ok = expect(!ShouldDebitSelectedEffects(1), "effects=1 (bit0) no debit") && ok;

    // --- positive cost selectedEffects → debit ---
    ok = expect(ShouldDebitSelectedEffects(1 << 1), "bit1 (refresh) debit") && ok;
    ok = expect(ShouldDebitSelectedEffects(1 << 2), "bit2 debit") && ok;
    ok = expect(ShouldDebitSelectedEffects(1 << 3), "bit3 debit") && ok;
    ok = expect(ShouldDebitSelectedEffects(1 << 4), "bit4 debit") && ok;
    ok = expect(ShouldDebitSelectedEffects(6), "bits 1+2 debit") && ok;
    ok = expect(ShouldDebitSelectedEffects(0x1E), "bits 1-4 debit") && ok;
    ok = expect(ShouldDebitSelectedEffects(0x1F), "bits 0-4 debit (bit0 free)") && ok;

    // --- Composition table: free == inline == pin via SigilBonusCost ---
    const struct
    {
        int32       selectedEffects;
        bool        want;
        const char* label;
    } cases[] = {
        { 0, false, "table empty" },
        { 1, false, "table bit0" },
        { 2, true, "table bit1" },
        { 4, true, "table bit2" },
        { 6, true, "table bits1+2" },
        { 8, true, "table bit3" },
        { 16, true, "table bit4" },
        { 0x1E, true, "table 0x1E" },
        { 0x1F, true, "table 0x1F" },
        { 0x1A, true, "table 0x1A" },
        { 0x15, true, "table 0x15" },
        { 0x0A, true, "table 0x0A" },
    };

    for (const auto& c : cases)
    {
        const bool got       = ShouldDebitSelectedEffects(c.selectedEffects);
        const bool inlineGot = inlineShouldDebitSelectedEffects(c.selectedEffects);
        const bool pinGot    = pinShouldDebitSelectedEffects(c.selectedEffects);
        const bool compose   = ShouldDebitBonusCost(SigilBonusCost(c.selectedEffects));
        ok                   = expect(got == c.want, c.label) && ok;
        ok                   = expect(got == inlineGot, "dual-wire free==inline") && ok;
        ok                   = expect(got == pinGot, "dual-wire free==pin") && ok;
        ok                   = expect(got == compose, "dual-wire free==compose") && ok;
        // free == inline via SigilBonusCost(selectedEffects) > 0
        const bool costPositive = SigilBonusCost(c.selectedEffects) > 0;
        ok                      = expect(got == costPositive, "free==inline via SigilBonusCost") && ok;
    }

    // --- Production sigil apply path semantics ---
    // Debit → may continue delCurrency path.
    // Zero-cost masks → skip delCurrency.
    ok = expect(ShouldDebitSelectedEffects(1 << 1), "apply debit → delCurrency path") && ok;
    ok = expect(!ShouldDebitSelectedEffects(0), "apply zero → skip delCurrency") && ok;
    ok = expect(!ShouldDebitSelectedEffects(1 << 0), "apply regen-only → skip delCurrency") && ok;

    // Sibling 3103 gate remains the cost>0 authority (not reimplemented).
    ok = expect(ShouldDebitBonusCost(50), "sibling 3103 positive debit") && ok;
    ok = expect(!ShouldDebitBonusCost(0), "sibling 3103 zero no debit") && ok;

    return ok;
}
