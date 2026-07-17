#include "test_login_erase_idle_2963.h"

#include "login/connect_cleanup.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "login ShouldEraseIdleSession 2963 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline periodicCleanup per-session erase formula for dual-wire cross-check
// (slice 2963):
//   !hasDataSession && !hasViewSession && nowAfterExpiry
auto inlineShouldEraseIdleSession(
    const bool hasDataSession,
    const bool hasViewSession,
    const bool nowAfterExpiry) -> bool
{
    return !hasDataSession && !hasViewSession && nowAfterExpiry;
}

} // namespace

// Pure dual-wire expansion for loginHelpers::ShouldEraseIdleSession
// (!hasDataSession && !hasViewSession && nowAfterExpiry; slice 2963).
auto runLoginEraseIdle2963SelfTests() -> bool
{
    using loginHelpers::IsSessionExpired;
    using loginHelpers::SessionCleanInterval;
    using loginHelpers::ShouldEraseIdleSession;
    using loginHelpers::ShouldEraseOnClearCommand;

    using namespace std::chrono_literals;

    bool ok = true;

    // Residual 1325 pins still hold under dual-wire.
    ok = expect(ShouldEraseIdleSession(false, false, true), "residual idle expired erases") && ok;
    ok = expect(!ShouldEraseIdleSession(true, false, true), "residual data peer keeps") && ok;
    ok = expect(!ShouldEraseIdleSession(false, true, true), "residual view peer keeps") && ok;
    ok = expect(!ShouldEraseIdleSession(true, true, true), "residual both peers keep") && ok;
    ok = expect(!ShouldEraseIdleSession(false, false, false), "residual not expired keeps") && ok;

    const struct
    {
        bool        hasDataSession;
        bool        hasViewSession;
        bool        nowAfterExpiry;
        bool        want;
        const char* label;
    } cases[] = {
        // Both peers null + expired → erase
        { false, false, true, true, "idle expired erases" },

        // Any live peer keeps even when expired
        { true, false, true, false, "data peer keeps" },
        { false, true, true, false, "view peer keeps" },
        { true, true, true, false, "both peers keep" },

        // Not expired keeps even when both peers null
        { false, false, false, false, "not expired keeps" },

        // Peers present + not expired → keep
        { true, false, false, false, "data peer not expired keeps" },
        { false, true, false, false, "view peer not expired keeps" },
        { true, true, false, false, "both peers not expired keep" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldEraseIdleSession(c.hasDataSession, c.hasViewSession, c.nowAfterExpiry);
        const bool inlineF = inlineShouldEraseIdleSession(c.hasDataSession, c.hasViewSession, c.nowAfterExpiry);
        const bool wantPin = !c.hasDataSession && !c.hasViewSession && c.nowAfterExpiry;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldEraseIdleSession dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldEraseIdleSession == pin formula") && ok;
    }

    // Pin composition: AND of !data, !view, and expired.
    ok = expect(ShouldEraseIdleSession(false, false, true), "idle expired must erase") && ok;
    ok = expect(!ShouldEraseIdleSession(true, false, true), "data peer must keep") && ok;
    ok = expect(!ShouldEraseIdleSession(false, true, true), "view peer must keep") && ok;
    ok = expect(!ShouldEraseIdleSession(false, false, false), "not expired must keep") && ok;

    // Dense compose: full 2^3 boolean space.
    for (const bool hasDataSession : { false, true })
    {
        for (const bool hasViewSession : { false, true })
        {
            for (const bool nowAfterExpiry : { false, true })
            {
                const bool got  = ShouldEraseIdleSession(hasDataSession, hasViewSession, nowAfterExpiry);
                const bool want = !hasDataSession && !hasViewSession && nowAfterExpiry;
                ok              = expect(got == want, "compose free == pin formula") && ok;
                ok              = expect(got == inlineShouldEraseIdleSession(hasDataSession, hasViewSession, nowAfterExpiry),
                            "compose free == inline") &&
                     ok;
            }
        }
    }

    // --- Production periodicCleanup path semantics ---
    // Host injects:
    //   hasDataSession = session.data_session != nullptr
    //   hasViewSession = session.view_session != nullptr
    //   nowAfterExpiry = IsSessionExpired(timer::now(), authorizedTime)
    //                  = now > authorizedTime + SessionCleanInterval
    const auto base       = timer::time_point{};
    const bool notExpired = IsSessionExpired(base + SessionCleanInterval, base);
    const bool expired    = IsSessionExpired(base + SessionCleanInterval + 1ms, base);
    ok                    = expect(!notExpired, "exact 15min not expired (strict >)") && ok;
    ok                    = expect(expired, "past 15min expired") && ok;

    ok = expect(!ShouldEraseIdleSession(false, false, notExpired), "host inject not expired must keep") && ok;
    ok = expect(ShouldEraseIdleSession(false, false, expired), "host inject expired must erase") && ok;
    ok = expect(!ShouldEraseIdleSession(true, false, expired), "host inject data peer must keep") && ok;
    ok = expect(!ShouldEraseIdleSession(false, true, expired), "host inject view peer must keep") && ok;
    ok = expect(!ShouldEraseIdleSession(true, true, expired), "host inject both peers must keep") && ok;

    // Explicit dual-wire: free == !data && !view && expired for host poles.
    for (const bool hasDataSession : { false, true })
    {
        for (const bool hasViewSession : { false, true })
        {
            for (const bool nowAfterExpiry : { false, true })
            {
                const bool got  = ShouldEraseIdleSession(hasDataSession, hasViewSession, nowAfterExpiry);
                const bool want = !hasDataSession && !hasViewSession && nowAfterExpiry;
                ok              = expect(got == want, "host inject free == pin") && ok;
                ok              = expect(got == inlineShouldEraseIdleSession(hasDataSession, hasViewSession, nowAfterExpiry),
                            "host inject free == inline") &&
                     ok;
            }
        }
    }

    // Sibling clear-console polarity remains orthogonal (no +15min offset).
    ok = expect(!ShouldEraseIdleSession(false, false, false) && ShouldEraseOnClearCommand(false, false, true),
                "clear vs periodic polarity residual") &&
         ok;
    ok = expect(SessionCleanInterval == 15min, "SessionCleanInterval residual") && ok;

    return ok;
}
