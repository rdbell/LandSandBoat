#include "test_recast_change_job_2814.h"

#include "map/recast_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "recast change job 2814 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runRecastChangeJob2814SelfTests() -> bool
{
    using namespace recasthelpers;
    bool ok = true;

    // ShouldEraseAbilityOnChangeJob: erase non-one-hours only
    ok = expect(!ShouldEraseAbilityOnChangeJob(true, false), "keep special") && ok;
    ok = expect(!ShouldEraseAbilityOnChangeJob(false, true), "keep special2") && ok;
    ok = expect(!ShouldEraseAbilityOnChangeJob(true, true), "keep both") && ok;
    ok = expect(ShouldEraseAbilityOnChangeJob(false, false), "erase normal ability") && ok;

    // Same truth table as ShouldResetAbilityRecast / !IsOneHourSpecialRecast
    const struct
    {
        bool        isSpecial;
        bool        isSpecial2;
        bool        wantErase;
        const char* label;
    } cases[] = {
        { true, false, false, "compose special" },
        { false, true, false, "compose special2" },
        { true, true, false, "compose both" },
        { false, false, true, "compose normal" },
    };
    for (const auto& c : cases)
    {
        const bool oneHour = IsOneHourSpecialRecast(c.isSpecial, c.isSpecial2);
        const bool erase   = ShouldEraseAbilityOnChangeJob(c.isSpecial, c.isSpecial2);
        const bool reset   = ShouldResetAbilityRecast(c.isSpecial, c.isSpecial2);
        ok                 = expect(erase == c.wantErase, c.label) && ok;
        ok                 = expect(erase == !oneHour, "compose inverse of one-hour") && ok;
        ok                 = expect(erase == reset, "alias of ShouldResetAbilityRecast") && ok;
    }

    return ok;
}
