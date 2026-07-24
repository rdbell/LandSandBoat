#include "test_state_was_exit_delayed_6326.h"

#include "map/ai/states/state_is_completed.h"
#include "map/ai/states/state_was_exit_delayed.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "state WasExitDelayed 6326 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for CState::WasExitDelayed (slice 6326).
auto runStateWasExitDelayed6326SelfTests() -> bool
{
    using statehelpers::IsCompleted;
    using statehelpers::WasExitDelayed;

    bool ok = true;

    ok = expect(WasExitDelayed(true) && !WasExitDelayed(false), "identity inject") && ok;
    for (const bool delayed : { false, true })
    {
        ok = expect(WasExitDelayed(delayed) == delayed, "free==identity") && ok;
    }

    ok = expect(!IsCompleted(false), "6325 residual: incomplete") && ok;
    ok = expect(IsCompleted(true), "6325 residual: completed") && ok;

    return ok;
}
