#include "test_assault_issue_tag_3440.h"

#include "map/assault_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "assault issue tag 3440 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua onRytaalEventFinish option==1 gate for dual-wire cross-check
// (dedicated 3440 expand residual 2867 / prior 3388 stock / 3258 / 3145):
//   option == 1 and not hasKeyItem(IMPERIAL_ARMY_ID_TAG)
auto inlineShouldIssueNewTag(const int32 option, const bool hasImperialArmyIDTag) -> bool
{
    return option == 1 && !hasImperialArmyIDTag;
}

// Compact dual-wire pin matching Go pinShouldIssueNewTag3440:
//   option == kRytaalOptionObtainTag && !hasImperialArmyIDTag
auto pinShouldIssueNewTag(const int32 option, const bool hasImperialArmyIDTag) -> bool
{
    return option == assaulthelpers::kRytaalOptionObtainTag && !hasImperialArmyIDTag;
}

// Prior dedicated 3258 pin (independence cross-check):
//   option == kRytaalOptionObtainTag && !hasImperialArmyIDTag
auto pinShouldIssueNewTag3258(const int32 option, const bool hasImperialArmyIDTag) -> bool
{
    return option == assaulthelpers::kRytaalOptionObtainTag && !hasImperialArmyIDTag;
}

// Prior dedicated 3145 pin (independence cross-check):
//   option == kRytaalOptionObtainTag && !hasImperialArmyIDTag
auto pinShouldIssueNewTag3145(const int32 option, const bool hasImperialArmyIDTag) -> bool
{
    return option == assaulthelpers::kRytaalOptionObtainTag && !hasImperialArmyIDTag;
}

} // namespace

// Pure dual-wire expansion for assaulthelpers::ShouldIssueNewTag
// (Lua onRytaalEventFinish obtain Imperial Army ID tag gate; OmegaXI
// internal/assault; dedicated slice 3440 expand residual 2867 / prior
// 3388 stock / 3258 / 3145 — formula unchanged).
//
// Coverage:
//   - free == inline == pin == option==kRytaalOptionObtainTag&&!hasKI
//   - residual poles: option1×hasKI / wrong option
//   - dense option × hasTag space
//   - residual 1100 / 2867 / prior 3145 / 3258 / 3388 pins still hold
//   - host-owned stock / currentAssault gates left outside pure surface
//
// Sibling suites RETAINED: test_assault_issue_tag_2867,
// test_assault_issue_new_tag_3145, test_assault_issue_tag_3258,
// test_assault_issue_tag_3388 (CanIssueTagFromStock). Mirrors 3388
// expand-residual suite layout (header + self-test runner; no CMake/main).
auto runAssaultIssueTag3440SelfTests() -> bool
{
    using assaulthelpers::CanIssueTagFromStock;
    using assaulthelpers::ShouldIssueNewTag;
    using assaulthelpers::kKeyItemImperialArmyIDTag;
    using assaulthelpers::kRytaalOptionObtainTag;

    bool ok = true;

    // Catalog pins (match Go RytaalOptionObtainTag / KeyItemImperialArmyIDTag).
    ok = expect(kRytaalOptionObtainTag == 1, "kRytaalOptionObtainTag pin") && ok;
    ok = expect(kKeyItemImperialArmyIDTag == 787, "kKeyItemImperialArmyIDTag pin") && ok;

    // Residual 1100 / 2867 / prior 3145 / 3258 ShouldIssueNewTag pins still hold.
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
        // Classic residual poles / eligible.
        { 1, false, true, "table option1 no KI" },
        { kRytaalOptionObtainTag, false, true, "table pin option no KI" },
        // Residual poles: blocked has KI.
        { 1, true, false, "table option1 has KI" },
        { kRytaalOptionObtainTag, true, false, "table pin option has KI" },
        // Residual poles: wrong option.
        { 2, false, false, "table option2 no KI" },
        { 2, true, false, "table option2 has KI" },
        { 0, false, false, "table option0 no KI" },
        { 0, true, false, "table option0 has KI" },
        // Residual 2867 re-pins.
        { 1, false, true, "residual 2867 option1 no KI" },
        { 1, true, false, "residual 2867 option1 has KI" },
        { 3, false, false, "residual 2867 option3 no KI" },
        { -1, false, false, "residual 2867 negative option no KI" },
        // Prior dedicated 3145 re-pins.
        { 1, false, true, "prior 3145 option1 no KI" },
        { 1, true, false, "prior 3145 option1 has KI" },
        { kRytaalOptionObtainTag, false, true, "prior 3145 pin option no KI" },
        { kRytaalOptionObtainTag, true, false, "prior 3145 pin option has KI" },
        { 99, false, false, "prior 3145 option99 no KI" },
        { 99, true, false, "prior 3145 option99 has KI" },
        // Prior dedicated expand residual 3258 re-pins.
        { 1, false, true, "prior 3258 option1 no KI" },
        { 1, true, false, "prior 3258 option1 has KI" },
        { kRytaalOptionObtainTag, false, true, "prior 3258 pin option no KI" },
        { kRytaalOptionObtainTag, true, false, "prior 3258 pin option has KI" },
        // Residual 1100 re-pins.
        { 1, false, true, "residual 1100 option1 no KI" },
        { 1, true, false, "residual 1100 option1 has KI" },
        { 2, false, false, "residual 1100 option2 no KI" },
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

    // Pin composition: free function is option==kRytaalOptionObtainTag && !hasKI.
    ok = expect(ShouldIssueNewTag(kRytaalOptionObtainTag, false), "eligible path must issue") && ok;
    ok = expect(!ShouldIssueNewTag(kRytaalOptionObtainTag, true), "has-KI path must block") && ok;
    ok = expect(ShouldIssueNewTag(kRytaalOptionObtainTag, false) == pinShouldIssueNewTag(kRytaalOptionObtainTag, false),
                "free==pin eligible") &&
         ok;
    ok = expect(ShouldIssueNewTag(kRytaalOptionObtainTag, true) == pinShouldIssueNewTag(kRytaalOptionObtainTag, true),
                "free==pin has KI") &&
         ok;
    ok = expect(ShouldIssueNewTag(kRytaalOptionObtainTag, false) ==
                    inlineShouldIssueNewTag(kRytaalOptionObtainTag, false),
                "free==inline eligible") &&
         ok;
    ok = expect(ShouldIssueNewTag(kRytaalOptionObtainTag, true) ==
                    inlineShouldIssueNewTag(kRytaalOptionObtainTag, true),
                "free==inline has KI") &&
         ok;

    // Explicit residual poles free == inline == pin for option1 × hasTag.
    for (const bool hasKI : { false, true })
    {
        const bool got     = ShouldIssueNewTag(1, hasKI);
        const bool inlineC = inlineShouldIssueNewTag(1, hasKI);
        const bool pinGot  = pinShouldIssueNewTag(1, hasKI);
        const bool want    = !hasKI;
        ok                 = expect(got == want, "pole option1 free == !hasKI") && ok;
        ok                 = expect(got == inlineC, "pole option1 free == inline") && ok;
        ok                 = expect(got == pinGot, "pole option1 free == pin") && ok;
    }
    // Wrong-option residual poles never issue.
    for (const int32 option : { 0, 2 })
    {
        for (const bool hasKI : { false, true })
        {
            const bool got     = ShouldIssueNewTag(option, hasKI);
            const bool inlineC = inlineShouldIssueNewTag(option, hasKI);
            const bool pinGot  = pinShouldIssueNewTag(option, hasKI);
            ok                 = expect(!got, "pole wrong-option never issues") && ok;
            ok                 = expect(got == inlineC, "pole wrong-option free == inline") && ok;
            ok                 = expect(got == pinGot, "pole wrong-option free == pin") && ok;
        }
    }

    // Dense option × hasTag: free == pin == inline == compose.
    const int32 composeOptions[] = { -2, -1, 0, 1, 2, 3, kRytaalOptionObtainTag, 99 };
    const bool  composeHasKI[]   = { false, true };
    for (const int32 option : composeOptions)
    {
        for (const bool hasKI : composeHasKI)
        {
            const bool got = ShouldIssueNewTag(option, hasKI);
            const bool want =
                option == kRytaalOptionObtainTag && !hasKI;
            ok = expect(got == want, "dense free==compose") && ok;
            ok = expect(got == inlineShouldIssueNewTag(option, hasKI), "dense free==inline") && ok;
            ok = expect(got == pinShouldIssueNewTag(option, hasKI), "dense free==pin") && ok;
        }
    }

    // Prior dedicated 3258 independence: free still matches prior pin.
    for (const int32 option : composeOptions)
    {
        for (const bool hasKI : composeHasKI)
        {
            const bool got      = ShouldIssueNewTag(option, hasKI);
            const bool priorPin = pinShouldIssueNewTag3258(option, hasKI);
            ok                  = expect(got == priorPin, "prior 3258 independence") && ok;
        }
    }

    // Prior dedicated 3145 independence: free still matches prior pin.
    for (const int32 option : composeOptions)
    {
        for (const bool hasKI : composeHasKI)
        {
            const bool got      = ShouldIssueNewTag(option, hasKI);
            const bool priorPin = pinShouldIssueNewTag3145(option, hasKI);
            ok                  = expect(got == priorPin, "prior 3145 independence") && ok;
        }
    }

    // Host-style inject poles: menu option + hasKeyItem(IMPERIAL_ARMY_ID_TAG).
    const struct
    {
        int32       option;
        bool        hasKI;
        bool        want;
        const char* label;
    } hostPoles[] = {
        { 1, false, true, "host option1 no KI → issue" },
        { 1, true, false, "host option1 has KI → block" },
        { kRytaalOptionObtainTag, false, true, "host pin option no KI → issue" },
        { kRytaalOptionObtainTag, true, false, "host pin option has KI → block" },
        { 2, false, false, "host option2 no KI → not issue" },
        { 0, false, false, "host option0 no KI → not issue" },
    };
    for (const auto& h : hostPoles)
    {
        const bool got     = ShouldIssueNewTag(h.option, h.hasKI);
        const bool inlineC = inlineShouldIssueNewTag(h.option, h.hasKI);
        const bool pinGot  = pinShouldIssueNewTag(h.option, h.hasKI);
        const bool inject  = h.option == kRytaalOptionObtainTag && !h.hasKI;
        ok                 = expect(got == inject, h.label) && ok;
        ok                 = expect(got == inlineC, "host free==inline") && ok;
        ok                 = expect(got == pinGot, "host free==pin") && ok;
        ok                 = expect(got == h.want, "host free==want") && ok;
    }

    // --- Production onRytaalEventFinish path semantics ---
    // Eligible → may continue stock / currentAssault / giveKeyItem path.
    // Blocked → no issue writeback.
    ok = expect(ShouldIssueNewTag(kRytaalOptionObtainTag, false), "onRytaalEventFinish eligible → continue path") && ok;
    ok = expect(!ShouldIssueNewTag(kRytaalOptionObtainTag, true), "onRytaalEventFinish has KI → blocked") && ok;
    ok = expect(!ShouldIssueNewTag(2, false), "onRytaalEventFinish option2 → blocked") && ok;
    ok = expect(!ShouldIssueNewTag(0, false), "onRytaalEventFinish option0 → blocked") && ok;

    // Host-owned stock gate residual (prior 3388 retained).
    ok = expect(CanIssueTagFromStock(1), "sibling CanIssueTagFromStock residual stock 1") && ok;
    ok = expect(!CanIssueTagFromStock(0), "sibling CanIssueTagFromStock residual stock 0") && ok;

    return ok;
}
