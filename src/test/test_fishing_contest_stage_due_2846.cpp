#include "test_fishing_contest_stage_due_2846.h"

#include "map/fishing_contest_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "fishing contest stage due 2846 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire expansion for fishingcontesthelpers::IsStageDue.
// Strict greater-than: equality is not due (not >=).
auto runFishingContestStageDue2846SelfTests() -> bool
{
    using fishingcontesthelpers::IsStageDue;

    bool ok = true;

    // Boundary: equality is not due.
    ok = expect(!IsStageDue(0, 0), "zero equality") && ok;
    ok = expect(!IsStageDue(100, 100), "equality is not due") && ok;
    ok = expect(!IsStageDue(0xFFFFFFFF, 0xFFFFFFFF), "max equality") && ok;

    // Strictly after changeTime is due.
    ok = expect(IsStageDue(1, 0), "one after zero") && ok;
    ok = expect(IsStageDue(101, 100), "strictly after is due") && ok;
    ok = expect(IsStageDue(0xFFFFFFFF, 0xFFFFFFFE), "max after max-1") && ok;

    // Before changeTime is not due.
    ok = expect(!IsStageDue(0, 1), "before zero") && ok;
    ok = expect(!IsStageDue(99, 100), "strictly before") && ok;
    ok = expect(!IsStageDue(0, 0xFFFFFFFF), "zero before max") && ok;

    // Dual-wire matches inline formula across a small table.
    const struct
    {
        uint32      currentTime;
        uint32      changeTime;
        bool        want;
        const char* label;
    } cases[] = {
        { 0, 0, false, "table zero eq" },
        { 1, 1, false, "table one eq" },
        { 1, 0, true, "table one after zero" },
        { 0, 1, false, "table zero before one" },
        { 100, 100, false, "table 100 eq" },
        { 101, 100, true, "table 101 after 100" },
        { 99, 100, false, "table 99 before 100" },
        { 0x80000000, 0x7FFFFFFF, true, "table high half after" },
        { 0x7FFFFFFF, 0x80000000, false, "table high half before" },
    };

    for (const auto& c : cases)
    {
        const bool got       = IsStageDue(c.currentTime, c.changeTime);
        const bool inlineGot = c.currentTime > c.changeTime;
        ok                   = expect(got == c.want, c.label) && ok;
        ok                   = expect(got == inlineGot, "dual-wire matches inline formula") && ok;
    }

    // Production ProgressContest dual-wires this pure gate after earth_time inject.
    ok = expect(!IsStageDue(100, 100), "production equality path") && ok;
    ok = expect(IsStageDue(101, 100), "production advance path") && ok;

    return ok;
}
