#include "test_assault_issue_new_tag_3145.h"

#include "map/assault_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "assault issue new tag 3145 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua onRytaalEventFinish option==1 gate for dual-wire cross-check:
//   option == 1 and not hasKeyItem(IMPERIAL_ARMY_ID_TAG)
auto inlineShouldIssueNewTag(const int32 option, const bool hasImperialArmyIDTag) -> bool
{
    return option == 1 && !hasImperialArmyIDTag;
}

// Compact dual-wire pin matching Go pinShouldIssueNewTag3145:
//   option == kRytaalOptionObtainTag && !hasImperialArmyIDTag
auto pinShouldIssueNewTag(const int32 option, const bool hasImperialArmyIDTag) -> bool
{
    return option == assaulthelpers::kRytaalOptionObtainTag && !hasImperialArmyIDTag;
}

} // namespace

// Pure dual-wire expansion for assaulthelpers::ShouldIssueNewTag
// (Lua onRytaalEventFinish obtain Imperial Army ID tag gate; OmegaXI
// internal/assault; slice 3145).
//
// Coverage:
//   - option==1 && !hasKI → issue true
//   - has KI / wrong option → issue false
//   - free == inline == pin == option==kRytaalOptionObtainTag&&!hasKI
//   - residual 1100 / 2867 pins still hold
auto runAssaultIssueNewTag3145SelfTests() -> bool
{
    using assaulthelpers::ShouldIssueNewTag;
    using assaulthelpers::kKeyItemImperialArmyIDTag;
    using assaulthelpers::kRytaalOptionObtainTag;

    bool ok = true;

    // Catalog pins (match Go RytaalOptionObtainTag / KeyItemImperialArmyIDTag).
    ok = expect(kRytaalOptionObtainTag == 1, "kRytaalOptionObtainTag pin") && ok;
    ok = expect(kKeyItemImperialArmyIDTag == 787, "kKeyItemImperialArmyIDTag pin") && ok;

    // Residual 1100 / 2867 ShouldIssueNewTag pins still hold under dual-wire.
    ok = expect(ShouldIssueNewTag(1, false), "residual: option1 no KI should issue") && ok;
    ok = expect(!ShouldIssueNewTag(1, true), "residual: option1 has KI should block") && ok;
    ok = expect(!ShouldIssueNewTag(2, false), "residual: option2 should not issue") && ok;
    ok = expect(!ShouldIssueNewTag(0, false), "residual: option0 should not issue") && ok;
    ok = expect(ShouldIssueNewTag(kRytaalOptionObtainTag, false), "residual: pin option no KI issues") && ok;
    ok = expect(!ShouldIssueNewTag(kRytaalOptionObtainTag, true), "residual: pin option has KI blocks") && ok;

    // --- Eligible issue path ---
    ok = expect(ShouldIssueNewTag(kRytaalOptionObtainTag, false), "eligible pin option no KI") && ok;
    ok = expect(ShouldIssueNewTag(1, false), "eligible option1 no KI") && ok;

    // --- Blocked paths ---
    ok = expect(!ShouldIssueNewTag(kRytaalOptionObtainTag, true), "has KI blocks") && ok;
    ok = expect(!ShouldIssueNewTag(2, false), "option2 no KI blocks") && ok;
    ok = expect(!ShouldIssueNewTag(2, true), "option2 has KI blocks") && ok;
    ok = expect(!ShouldIssueNewTag(0, false), "option0 no KI blocks") && ok;
    ok = expect(!ShouldIssueNewTag(3, false), "option3 no KI blocks") && ok;
    ok = expect(!ShouldIssueNewTag(-1, false), "negative option no KI blocks") && ok;
    ok = expect(!ShouldIssueNewTag(99, false), "option99 no KI blocks") && ok;

    // --- Composition table: free == inline == pin == compose ---
    const struct
    {
        int32       option;
        bool        hasKI;
        bool        want;
        const char* label;
    } cases[] = {
        { 1, false, true, "table option1 no KI" },
        { kRytaalOptionObtainTag, false, true, "table pin option no KI" },
        { 1, true, false, "table option1 has KI" },
        { kRytaalOptionObtainTag, true, false, "table pin option has KI" },
        { 2, false, false, "table option2 no KI" },
        { 2, true, false, "table option2 has KI" },
        { 0, false, false, "table option0 no KI" },
        { 0, true, false, "table option0 has KI" },
        { 3, false, false, "table option3 no KI" },
        { 3, true, false, "table option3 has KI" },
        { -1, false, false, "table negative option no KI" },
        { -1, true, false, "table negative option has KI" },
        { 99, false, false, "table option99 no KI" },
        { 99, true, false, "table option99 has KI" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldIssueNewTag(c.option, c.hasKI);
        const bool inlineC = inlineShouldIssueNewTag(c.option, c.hasKI);
        const bool pinGot  = pinShouldIssueNewTag(c.option, c.hasKI);
        const bool compose = c.option == kRytaalOptionObtainTag && !c.hasKI;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineC, "dual-wire free==inline") && ok;
        ok = expect(got == pinGot, "dual-wire free==pin") && ok;
        ok = expect(got == compose, "formula free==option==kRytaalOptionObtainTag&&!hasKI") && ok;
    }

    // --- Production onRytaalEventFinish path semantics ---
    // Eligible → may continue stock / currentAssault / giveKeyItem path.
    // Blocked → no issue writeback.
    ok = expect(ShouldIssueNewTag(kRytaalOptionObtainTag, false), "onRytaalEventFinish eligible → continue path") && ok;
    ok = expect(!ShouldIssueNewTag(kRytaalOptionObtainTag, true), "onRytaalEventFinish has KI → blocked") && ok;
    ok = expect(!ShouldIssueNewTag(2, false), "onRytaalEventFinish option2 → blocked") && ok;
    ok = expect(!ShouldIssueNewTag(0, false), "onRytaalEventFinish option0 → blocked") && ok;

    // Dense compose identity over representative option × hasKI pairs.
    const int32 composeOptions[] = { -2, -1, 0, 1, 2, 3, kRytaalOptionObtainTag, 99 };
    const bool  composeHasKI[]   = { false, true };
    for (const int32 option : composeOptions)
    {
        for (const bool hasKI : composeHasKI)
        {
            const bool got = ShouldIssueNewTag(option, hasKI);
            const bool want =
                option == kRytaalOptionObtainTag && !hasKI;
            ok = expect(got == want, "compose option==kRytaalOptionObtainTag&&!hasKI") && ok;
            ok = expect(got == inlineShouldIssueNewTag(option, hasKI), "compose inline") && ok;
            ok = expect(got == pinShouldIssueNewTag(option, hasKI), "compose pin") && ok;
        }
    }

    return ok;
}
