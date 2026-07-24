#include "test_state_entry_time_6330.h"

#include "map/ai/states/state_entry_time.h"
#include "map/ai/states/state_target_id.h"

#include <chrono>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "state EntryTime 6330 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for CState::GetEntryTime / ResetEntryTime (slice 6330).
auto runStateEntryTime6330SelfTests() -> bool
{
    using statehelpers::EntryTime;
    using statehelpers::ResetEntryTime;
    using statehelpers::TargetID;

    bool ok = true;

    using Clock = std::chrono::steady_clock;
    const auto t0 = Clock::time_point{};
    const auto t1 = Clock::time_point{ std::chrono::seconds{ 42 } };

    ok = expect(EntryTime(t0) == t0, "EntryTime zero") && ok;
    ok = expect(EntryTime(t1) == t1, "EntryTime non-zero") && ok;
    ok = expect(ResetEntryTime(t1) == t1, "ResetEntryTime identity") && ok;
    ok = expect(TargetID(7) == 7, "6329 residual TargetID") && ok;

    return ok;
}
