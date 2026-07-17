#include "test_status_is_asleep_2825.h"

#include "map/status_effect_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "status is asleep 2825 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runStatusIsAsleep2825SelfTests() -> bool
{
    using statuseffecthelpers::IsAsleepEffectID;
    using statuseffecthelpers::IsAsleepStatusID;
    using statuseffecthelpers::StatusIDLullaby;
    using statuseffecthelpers::StatusIDSleepI;
    using statuseffecthelpers::StatusIDSleepIi;

    bool ok = true;
    ok = expect(IsAsleepStatusID(StatusIDSleepI), "sleep I") && ok;
    ok = expect(IsAsleepStatusID(StatusIDSleepIi), "sleep II") && ok;
    ok = expect(IsAsleepStatusID(StatusIDLullaby), "lullaby") && ok;
    ok = expect(!IsAsleepStatusID(0), "none") && ok;
    ok = expect(!IsAsleepStatusID(StatusIDSleepI + 1) || StatusIDSleepI + 1 == StatusIDSleepIi, "near miss") && ok;
    // Alias parity with IsAsleepEffectID.
    ok = expect(IsAsleepStatusID(StatusIDSleepI) == IsAsleepEffectID(StatusIDSleepI), "alias sleepI") && ok;
    ok = expect(IsAsleepStatusID(3) == IsAsleepEffectID(3), "alias non-sleep") && ok;
    return ok;
}
