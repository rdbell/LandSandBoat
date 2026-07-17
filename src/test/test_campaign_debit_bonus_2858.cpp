#include "test_campaign_debit_bonus_2858.h"

#include "map/campaign_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "campaign debit bonus 2858 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua sigilOnEventFinish gate for dual-wire checks:
//   if bonusCost > 0 then player:delCurrency('allied_notes', bonusCost) end
auto inlineShouldDebitBonusCost(const int32 bonusCost) -> bool
{
    return bonusCost > 0;
}

} // namespace

// Pure dual-wire expansion for campaignhelpers::ShouldDebitBonusCost
// (Lua if bonusCost > 0 delCurrency gate).
auto runCampaignDebitBonus2858SelfTests() -> bool
{
    using campaignhelpers::ShouldDebitBonusCost;

    bool ok = true;

    // Boundary: zero and negative do not debit; positive does.
    ok = expect(!ShouldDebitBonusCost(0), "zero cost no debit") && ok;
    ok = expect(!ShouldDebitBonusCost(-1), "negative cost no debit") && ok;
    ok = expect(!ShouldDebitBonusCost(-50), "negative 50 no debit") && ok;
    ok = expect(ShouldDebitBonusCost(1), "one cost debit") && ok;
    ok = expect(ShouldDebitBonusCost(50), "one effect (50) debit") && ok;
    ok = expect(ShouldDebitBonusCost(100), "two effects (100) debit") && ok;
    ok = expect(ShouldDebitBonusCost(150), "three effects (150) debit") && ok;
    ok = expect(ShouldDebitBonusCost(200), "four effects (200) debit") && ok;

    // Dual-wire matches inline formula across a small table.
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
        { 200, true, "table 200" },
        { -1, false, "table -1" },
        { -50, false, "table -50" },
        { 0x7FFFFFFF, true, "table max int32" },
        { static_cast<int32>(0x80000000), false, "table min int32" },
    };

    for (const auto& c : cases)
    {
        const bool got       = ShouldDebitBonusCost(c.bonusCost);
        const bool inlineGot = inlineShouldDebitBonusCost(c.bonusCost);
        ok                   = expect(got == c.want, c.label) && ok;
        ok                   = expect(got == inlineGot, "dual-wire matches inline formula") && ok;
    }

    return ok;
}
