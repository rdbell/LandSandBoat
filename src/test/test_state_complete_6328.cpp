#include "test_state_complete_6328.h"

#include "map/ai/states/state_complete.h"
#include "map/ai/states/state_is_completed.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "state Complete 6328 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for CState::Complete (slice 6328).
auto runStateComplete6328SelfTests() -> bool
{
    using statehelpers::IsCompleted;
    using statehelpers::MarkCompleted;

    bool ok = true;

    ok = expect(MarkCompleted(), "MarkCompleted true") && ok;
    ok = expect(IsCompleted(MarkCompleted()), "compose with IsCompleted") && ok;
    ok = expect(!IsCompleted(false), "6325 residual incomplete") && ok;

    return ok;
}
