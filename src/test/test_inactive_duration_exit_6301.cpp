#include "test_inactive_duration_exit_6301.h"

#include "map/ai/states/inactive_duration_exit.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "inactive duration exit 6301 self-test failed: " << label << '\n';
    }

    return condition;
}
} // namespace

auto runInactiveDurationExit6301SelfTests() -> bool
{
    using inactivedurationexit::shouldExit;

    // Truth table for durationPositive && tickAfterEntryPlusDuration.
    const bool tableOK =
        expect(!shouldExit(false, false), "zero duration before deadline holds") &&
        expect(!shouldExit(false, true), "zero duration after deadline holds") &&
        expect(!shouldExit(true, false), "positive duration at/before deadline holds") &&
        expect(shouldExit(true, true), "positive duration after deadline exits");

    // Production inject shape (inactive_state.cpp positive-duration tail):
    //   return shouldExit(m_duration > 0ms, tick > GetEntryTime() + m_duration);
    // Host precomputes both bools; equality is not after (strict >).
    const auto inject = [](const bool durationPositive, const bool tickAfter) -> bool {
        return shouldExit(durationPositive, tickAfter);
    };

    const bool injectOK =
        expect(!inject(false, true), "inject zero-duration never wall-clock exits") &&
        expect(!inject(true, false), "inject exact deadline does not exit (strict >)") &&
        expect(inject(true, true), "inject after deadline exits") &&
        expect(!inject(false, false), "inject zero+before holds");

    // Negative duration is not positive → same as zero (never expire via this gate).
    const bool negDuration = false; // host: m_duration > 0ms is false for <= 0
    const bool negOK =
        expect(!shouldExit(negDuration, true), "non-positive duration ignores wall clock");

    return tableOK && injectOK && negOK;
}
