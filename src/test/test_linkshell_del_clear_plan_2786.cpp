#include "test_linkshell_del_clear_plan_2786.h"

#include "map/linkshell_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "linkshell del clear plan 2786 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runLinkshellDelClearPlan2786SelfTests() -> bool
{
    using namespace linkshellhelpers;

    bool ok = true;

    // Happy paths: exclusive LS1 / LS2 attachment.
    ok = expect(PlanLinkshellDelMemberClear(true, false) == LinkshellClearAttachment::ClearLS1, "clear LS1 only") && ok;
    ok = expect(PlanLinkshellDelMemberClear(false, true) == LinkshellClearAttachment::ClearLS2, "clear LS2 only") && ok;

    // Neither pointer matches this shell (defensive; should not clear session).
    ok = expect(PlanLinkshellDelMemberClear(false, false) == LinkshellClearAttachment::None, "clear none") && ok;

    // Production uses if / else if: LS1 wins when both are somehow true.
    ok = expect(PlanLinkshellDelMemberClear(true, true) == LinkshellClearAttachment::ClearLS1, "LS1 wins when both") && ok;

    return ok;
}
