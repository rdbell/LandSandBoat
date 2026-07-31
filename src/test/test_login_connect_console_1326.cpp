#include "test_login_connect_console_1326.h"

#include "common/timer.h"
#include "login/connect_cleanup.h"

#include <array>
#include <chrono>
#include <iostream>
#include <limits>
#include <string>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "login connect console 1326 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runLoginConnectConsole1326SelfTests() -> bool
{
    using namespace std::chrono_literals;
    bool ok = true;

    // clear command: peers null + now > authorizedTime (NO 15min offset).
    ok = expect(loginHelpers::ShouldEraseOnClearCommand(false, false, true), "clear erase") && ok;
    ok = expect(!loginHelpers::ShouldEraseOnClearCommand(true, false, true), "clear data keeps") && ok;
    ok = expect(!loginHelpers::ShouldEraseOnClearCommand(false, true, true), "clear view keeps") && ok;
    ok = expect(!loginHelpers::ShouldEraseOnClearCommand(false, false, false), "clear not yet") && ok;

    // Contrast with periodic: expired-after-authorized alone is not enough for periodic
    // (needs nowAfterExpiry which includes +15min). Document polarity only.
    ok = expect(!loginHelpers::ShouldEraseIdleSession(false, false, false) &&
                    loginHelpers::ShouldEraseOnClearCommand(false, false, true),
                "clear vs periodic polarity") &&
         ok;

    const auto base = timer::time_point{};
    ok              = expect(!loginHelpers::IsAuthorizedTimePassed(base, base), "authorized equal") && ok;
    ok              = expect(loginHelpers::IsAuthorizedTimePassed(base + 1ms, base), "authorized past") && ok;
    ok              = expect(!loginHelpers::IsAuthorizedTimePassed(base, base + 1ms), "authorized future") && ok;

    const std::array<std::size_t, 3> counts{ 2, 0, 5 };
    ok = expect(loginHelpers::SumAuthenticatedAccountSessions(counts.data(), counts.size()) == 7, "sum accounts") && ok;
    ok = expect(loginHelpers::SumAuthenticatedAccountSessions(nullptr, 0) == 0, "sum empty") && ok;

    // Both the element values and accumulator are native unsigned sizes.
    // Keep a wrapped addition here so a signed Go port cannot silently diverge.
    const std::array<std::size_t, 2> wrappedCounts{ std::numeric_limits<std::size_t>::max(), 1 };
    ok = expect(loginHelpers::SumAuthenticatedAccountSessions(wrappedCounts.data(), wrappedCounts.size()) == 0,
                                                              "sum native wrap") &&
         ok;

    ok = expect(loginHelpers::FormatConnectStats(0, 0) == "Serving 0 IP addresses with 0 accounts", "stats zero") && ok;
    ok = expect(loginHelpers::FormatConnectStats(3, 7) == "Serving 3 IP addresses with 7 accounts", "stats sample") && ok;
    ok = expect(loginHelpers::FormatConnectStats(std::numeric_limits<std::size_t>::max(), 0) ==
                    ("Serving " + std::to_string(std::numeric_limits<std::size_t>::max()) + " IP addresses with 0 accounts"),
                "stats native size") &&
         ok;
    ok = expect(loginHelpers::FormatConnectStats(0, std::numeric_limits<std::size_t>::max()) ==
                    ("Serving 0 IP addresses with " + std::to_string(std::numeric_limits<std::size_t>::max()) + " accounts"),
                "stats native account size") &&
         ok;

    return ok;
}
