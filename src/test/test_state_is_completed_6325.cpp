#include "test_state_is_completed_6325.h"

#include "map/ai/states/state_error_msg.h"
#include "map/ai/states/state_is_completed.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "state IsCompleted 6325 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for CState::IsCompleted (slice 6325).
auto runStateIsCompleted6325SelfTests() -> bool
{
    using statehelpers::HasErrorMsg;
    using statehelpers::IsCompleted;

    bool ok = true;

    ok = expect(IsCompleted(true) && !IsCompleted(false), "identity inject") && ok;
    for (const bool completed : { false, true })
    {
        ok = expect(IsCompleted(completed) == completed, "free==identity") && ok;
    }

    ok = expect(!HasErrorMsg(false), "6324 residual: absent error") && ok;
    ok = expect(HasErrorMsg(true), "6324 residual: present error") && ok;

    return ok;
}
