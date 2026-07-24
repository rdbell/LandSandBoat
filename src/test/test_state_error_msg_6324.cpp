#include "test_state_error_msg_6324.h"

#include "map/ai/states/state_error_msg.h"
#include "map/ai/states/state_set_target.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "state HasErrorMsg 6324 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for CState::HasErrorMsg (slice 6324).
auto runStateErrorMsg6324SelfTests() -> bool
{
    using statehelpers::HasErrorMsg;
    using statehelpers::ShouldUpdateTarget;

    bool ok = true;

    ok = expect(HasErrorMsg(true) && !HasErrorMsg(false), "identity inject") && ok;
    for (const bool has : { false, true })
    {
        ok = expect(HasErrorMsg(has) == has, "free==identity") && ok;
    }

    ok = expect(!ShouldUpdateTarget(true, 1, 1, 1), "6317 residual: stable target") && ok;
    ok = expect(ShouldUpdateTarget(false, 0, 0, 0), "6317 residual: no target") && ok;

    return ok;
}
