#include "test_assault_issue_tag_2867.h"

#include "map/assault_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "assault issue tag 2867 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua onRytaalEventFinish option==1 gate for dual-wire checks:
//   option == 1 and not hasKeyItem(IMPERIAL_ARMY_ID_TAG)
auto inlineShouldIssueNewTag(const int32 option, const bool hasImperialArmyIDTag) -> bool
{
    return option == 1 && !hasImperialArmyIDTag;
}

} // namespace

// Pure dual-wire expansion for assaulthelpers::ShouldIssueNewTag
// (Lua onRytaalEventFinish obtain Imperial Army ID tag gate).
auto runAssaultIssueTag2867SelfTests() -> bool
{
    using assaulthelpers::ShouldIssueNewTag;
    using assaulthelpers::kKeyItemImperialArmyIDTag;
    using assaulthelpers::kRytaalOptionObtainTag;

    bool ok = true;

    ok = expect(kRytaalOptionObtainTag == 1, "kRytaalOptionObtainTag pin") && ok;
    ok = expect(kKeyItemImperialArmyIDTag == 787, "kKeyItemImperialArmyIDTag pin") && ok;

    // Residual 1100 pins.
    ok = expect(ShouldIssueNewTag(1, false), "option1 no KI issue") && ok;
    ok = expect(!ShouldIssueNewTag(1, true), "option1 has KI blocked") && ok;
    ok = expect(!ShouldIssueNewTag(2, false), "option2 not issue") && ok;

    // --- ShouldIssueNewTag table ---
    const struct
    {
        int32       option;
        bool        hasKI;
        bool        want;
        const char* label;
    } cases[] = {
        { 1, false, true, "option1 no KI" },
        { 1, true, false, "option1 has KI" },
        { 2, false, false, "option2 no KI" },
        { 2, true, false, "option2 has KI" },
        { 0, false, false, "option0 no KI" },
        { 0, true, false, "option0 has KI" },
        { 3, false, false, "option3 no KI" },
        { -1, false, false, "negative option no KI" },
        { kRytaalOptionObtainTag, false, true, "pin constant option" },
        { kRytaalOptionObtainTag, true, false, "pin constant option has KI" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldIssueNewTag(c.option, c.hasKI);
        const bool pure    = c.option == kRytaalOptionObtainTag && !c.hasKI;
        const bool inlineF = inlineShouldIssueNewTag(c.option, c.hasKI);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == pure, "dual-wire free == option pin && !hasKI") && ok;
        ok = expect(got == inlineF, "dual-wire free == inline Lua formula") && ok;
    }

    return ok;
}
