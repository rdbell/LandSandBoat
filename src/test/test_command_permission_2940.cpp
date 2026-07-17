#include "test_command_permission_2940.h"

#include "map/command_handler_capacity.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "command ShouldAllowCommandPermission 2940 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CCommandHandler::call permission gate for dual-wire cross-check (slice 2940):
//   static_cast<int>(permission) <= static_cast<int>(gmLevel)
// Equivalent production form: !(permission > gmLevel) after int promotions.
auto inlineShouldAllowCommandPermission(const uint8 gmLevel, const int8 permission) -> bool
{
    return static_cast<int>(permission) <= static_cast<int>(gmLevel);
}

} // namespace

// Pure dual-wire expansion for commandhandlerhelpers::ShouldAllowCommandPermission
// (permission <= m_GMlevel after int promotions; slice 2940).
auto runCommandPermission2940SelfTests() -> bool
{
    using commandhandlerhelpers::PlanCommandCallPostProps;
    using commandhandlerhelpers::ShouldAllowCommandPermission;

    bool ok = true;

    const struct
    {
        uint8       gmLevel;
        int8        permission;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles / equal boundary.
        { 0, 0, true, "perm 0 gm 0 allow" },
        { 5, 5, true, "perm 5 gm 5 allow" },
        { 5, 0, true, "perm 0 gm 5 allow" },
        { 5, 4, true, "perm 4 gm 5 allow" },
        { 0, 1, false, "perm 1 gm 0 reject" },
        { 4, 5, false, "perm 5 gm 4 reject" },

        // Negative int8 permission: promotes to signed int; never > any uint8 gmLevel.
        { 0, static_cast<int8>(-1), true, "perm -1 gm 0 allow" },
        { 255, static_cast<int8>(-128), true, "perm -128 gm 255 allow" },

        // High gmLevel vs mid permission after promotion to int.
        { 255, static_cast<int8>(127), true, "perm 127 gm 255 allow" },
        { 100, static_cast<int8>(127), false, "perm 127 gm 100 reject" },

        // Residual 2792 pins.
        { 5, 3, true, "residual allow mid" },
        { 2, 5, false, "residual reject high" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldAllowCommandPermission(c.gmLevel, c.permission);
        const bool inlineF = inlineShouldAllowCommandPermission(c.gmLevel, c.permission);
        const bool wantPin = static_cast<int>(c.permission) <= static_cast<int>(c.gmLevel);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldAllowCommandPermission dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldAllowCommandPermission == pin formula permission <= gmLevel") && ok;
    }

    // Pin composition: permission must not exceed gmLevel (int promotions).
    ok = expect(ShouldAllowCommandPermission(5, 5), "equal permission must allow") && ok;
    ok = expect(ShouldAllowCommandPermission(5, 0), "lower permission must allow") && ok;
    ok = expect(!ShouldAllowCommandPermission(0, 1), "higher permission must reject") && ok;
    ok = expect(ShouldAllowCommandPermission(0, static_cast<int8>(-1)), "negative permission must allow") && ok;

    // Dense compose over common GM levels and permission span.
    for (int gm = 0; gm <= 10; ++gm)
    {
        for (int perm = -2; perm <= 12; ++perm)
        {
            const auto gmLevel    = static_cast<uint8>(gm);
            const auto permission = static_cast<int8>(perm);
            const bool got        = ShouldAllowCommandPermission(gmLevel, permission);
            const bool want       = static_cast<int>(permission) <= static_cast<int>(gmLevel);
            ok                    = expect(got == want, "compose free == pin formula") && ok;
            ok                    = expect(got == inlineShouldAllowCommandPermission(gmLevel, permission),
                        "compose free == inline") &&
                 ok;
        }
    }

    // Host-style inject poles: m_GMlevel (uint8) + Lua cmdprops permission (int8).
    // Production call uses PlanCommandCallPostProps which dual-wires this free function.
    {
        const auto planAllow = PlanCommandCallPostProps(/*gmLevel=*/5, /*permission=*/3, /*auditLevel=*/0);
        ok                   = expect(!planAllow.rejectPermission, "post-props allow composes free gate") && ok;
        ok                   = expect(ShouldAllowCommandPermission(5, 3), "host inject allow dual-wire") && ok;
    }
    {
        const auto planReject = PlanCommandCallPostProps(/*gmLevel=*/2, /*permission=*/5, /*auditLevel=*/1);
        ok                    = expect(planReject.rejectPermission, "post-props reject composes free gate") && ok;
        ok                    = expect(!ShouldAllowCommandPermission(2, 5), "host inject reject dual-wire") && ok;
        ok                    = expect(!planReject.scheduleAudit, "reject forces scheduleAudit false") && ok;
    }

    // Production CCommandHandler::call path semantics:
    // allow → continue audit / onTrigger path
    // reject → warn Failure
    ok = expect(ShouldAllowCommandPermission(5, 3), "call allow → continue path") && ok;
    ok = expect(!ShouldAllowCommandPermission(0, 1), "call reject → Failure path") && ok;
    ok = expect(ShouldAllowCommandPermission(0, 0), "call equal zero → continue path") && ok;
    ok = expect(ShouldAllowCommandPermission(255, static_cast<int8>(-128)), "call negative perm → continue") && ok;

    return ok;
}
