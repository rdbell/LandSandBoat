#include "test_state_target_id_6329.h"

#include "map/ai/states/state_complete.h"
#include "map/ai/states/state_target_id.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "state TargetID 6329 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for CState::GetTargetID (slice 6329).
auto runStateTargetID6329SelfTests() -> bool
{
    using statehelpers::MarkCompleted;
    using statehelpers::TargetID;

    bool ok = true;

    for (const uint16_t id : { uint16_t{ 0 }, uint16_t{ 1 }, uint16_t{ 42 }, uint16_t{ 65535 } })
    {
        ok = expect(TargetID(id) == id, "identity inject") && ok;
    }

    ok = expect(MarkCompleted(), "6328 residual MarkCompleted") && ok;

    return ok;
}
