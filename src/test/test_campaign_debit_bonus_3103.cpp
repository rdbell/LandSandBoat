#include "test_campaign_debit_bonus_3103.h"

#include "map/campaign_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "campaign debit bonus 3103 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua sigilOnEventFinish apply delCurrency gate for dual-wire checks:
//   if bonusCost > 0 then player:delCurrency('allied_notes', bonusCost) end
auto inlineShouldDebitBonusCost(const int32 bonusCost) -> bool
{
    return bonusCost > 0;
}

// Compact dual-wire pin matching Go pinShouldDebitBonusCost3103:
//   bonusCost > 0
auto pinShouldDebitBonusCost(const int32 bonusCost) -> bool
{
    return bonusCost > 0;
}

} // namespace

// Pure dual-wire expansion for campaignhelpers::ShouldDebitBonusCost
// (Lua sigilOnEventFinish apply delCurrency: bonusCost > 0;
// OmegaXI internal/campaign; slice 3103).
//
// Coverage:
//   - zero → no debit
//   - positive → debit
//   - negative → no debit
//   - large values
//   - free == inline == pin
//   - residual 1115 / 2858 pins still hold
auto runCampaignDebitBonus3103SelfTests() -> bool
{
    using campaignhelpers::ShouldDebitBonusCost;

    bool ok = true;

    // Residual 1115 / 2858 pins still hold under dual-wire.
    ok = expect(!ShouldDebitBonusCost(0), "residual: zero cost no debit") && ok;
    ok = expect(ShouldDebitBonusCost(50), "residual: one effect (50) debit") && ok;
    ok = expect(ShouldDebitBonusCost(1), "residual: one cost debit") && ok;
    ok = expect(!ShouldDebitBonusCost(-1), "residual: negative no debit") && ok;
    ok = expect(ShouldDebitBonusCost(200), "residual: four effects (200) debit") && ok;

    // --- zero → no debit ---
    ok = expect(!ShouldDebitBonusCost(0), "zero no debit") && ok;

    // --- positive → debit ---
    ok = expect(ShouldDebitBonusCost(1), "one cost debit") && ok;
    ok = expect(ShouldDebitBonusCost(49), "49 cost debit") && ok;
    ok = expect(ShouldDebitBonusCost(50), "one effect (50) debit") && ok;
    ok = expect(ShouldDebitBonusCost(100), "two effects (100) debit") && ok;
    ok = expect(ShouldDebitBonusCost(150), "three effects (150) debit") && ok;
    ok = expect(ShouldDebitBonusCost(200), "four effects (200) debit") && ok;

    // --- negative → no debit ---
    ok = expect(!ShouldDebitBonusCost(-1), "negative one no debit") && ok;
    ok = expect(!ShouldDebitBonusCost(-50), "negative 50 no debit") && ok;

    // --- large values ---
    ok = expect(ShouldDebitBonusCost(0x7FFFFFFF), "large max int32 debit") && ok;
    ok = expect(!ShouldDebitBonusCost(static_cast<int32>(0x80000000)), "large min int32 no debit") && ok;

    // --- Composition table: free == inline == pin ---
    const struct
    {
        int32       bonusCost;
        bool        want;
        const char* label;
    } cases[] = {
        { 0, false, "table zero" },
        { 1, true, "table one" },
        { 49, true, "table 49" },
        { 50, true, "table 50" },
        { 100, true, "table 100" },
        { 150, true, "table 150" },
        { 200, true, "table 200" },
        { -1, false, "table -1" },
        { -50, false, "table -50" },
        { 0x7FFFFFFF, true, "table max int32" },
        { static_cast<int32>(0x80000000), false, "table min int32" },
        { 2, true, "table two" },
    };

    for (const auto& c : cases)
    {
        const bool got       = ShouldDebitBonusCost(c.bonusCost);
        const bool inlineGot = inlineShouldDebitBonusCost(c.bonusCost);
        const bool pinGot    = pinShouldDebitBonusCost(c.bonusCost);
        ok                   = expect(got == c.want, c.label) && ok;
        ok                   = expect(got == inlineGot, "dual-wire free==inline") && ok;
        ok                   = expect(got == pinGot, "dual-wire free==pin") && ok;
    }

    // --- Production sigil apply path semantics ---
    // Debit → may continue delCurrency path.
    // Zero / non-positive → skip delCurrency.
    ok = expect(ShouldDebitBonusCost(50), "apply debit → delCurrency path") && ok;
    ok = expect(!ShouldDebitBonusCost(0), "apply zero → skip delCurrency") && ok;
    ok = expect(!ShouldDebitBonusCost(-1), "apply negative → skip delCurrency") && ok;

    return ok;
}
