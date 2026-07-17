#include "test_command_permission_3323.h"

#include "map/command_handler_capacity.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "command ShouldAllowCommandPermission 3323 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CCommandHandler::call permission gate for dual-wire cross-check (dedicated 3323):
//   static_cast<int>(permission) <= static_cast<int>(gmLevel)
auto inlineShouldAllowCommandPermission(const uint8 gmLevel, const int8 permission) -> bool
{
    return static_cast<int>(permission) <= static_cast<int>(gmLevel);
}

// Compact dual-wire pin matching Go pinShouldAllowCommandPermission3323 / C++ capacity
// positive form (formula unchanged from 2792 / 2940 / 3263 / 3293):
//   int(permission) <= int(gmLevel)
auto pinShouldAllowCommandPermission(const uint8 gmLevel, const int8 permission) -> bool
{
    return static_cast<int>(permission) <= static_cast<int>(gmLevel);
}

} // namespace

// Pure dual-wire expansion for commandhandlerhelpers::ShouldAllowCommandPermission
// (permission <= m_GMlevel after int promotions; OmegaXI internal/command;
// dedicated slice 3323; residual expand 2940 / pure 2792; prior dedicated expand 3293 / 3263).
//
// Coverage:
//   - free == inline == pin positive form
//   - poles: equal / under / over; signed permission edge; high gm vs mid perm
//   - residual 2940 / 2792 / 3263 / 3293 pins still hold
//   - post-props host-style inject + residual independence
auto runCommandPermission3323SelfTests() -> bool
{
    using commandhandlerhelpers::PlanCommandCallPostProps;
    using commandhandlerhelpers::ShouldAllowCommandPermission;
    using commandhandlerhelpers::ShouldAuditGMCommand;
    using commandhandlerhelpers::ShouldRejectEmptyCommandLine;
    using commandhandlerhelpers::ShouldRejectEmptyCommandName;
    using commandhandlerhelpers::ShouldRejectNullChar;

    bool ok = true;

    // Residual 2792 / 2940 / 3263 / 3293 pins still hold under dedicated dual-wire.
    ok = expect(ShouldAllowCommandPermission(0, 0), "residual: perm 0 gm 0 allow") && ok;
    ok = expect(ShouldAllowCommandPermission(5, 5), "residual: perm 5 gm 5 allow") && ok;
    ok = expect(ShouldAllowCommandPermission(5, 0), "residual: perm 0 gm 5 allow") && ok;
    ok = expect(!ShouldAllowCommandPermission(0, 1), "residual: perm 1 gm 0 reject") && ok;
    ok = expect(!ShouldAllowCommandPermission(4, 5), "residual: perm 5 gm 4 reject") && ok;
    ok = expect(ShouldAllowCommandPermission(0, static_cast<int8>(-1)), "residual: perm -1 gm 0 allow") && ok;
    ok = expect(ShouldAllowCommandPermission(5, 3), "residual 2940: allow mid") && ok;
    ok = expect(!ShouldAllowCommandPermission(2, 5), "residual 2940: reject high") && ok;
    ok = expect(ShouldAllowCommandPermission(10, 9), "residual 3263: perm 9 gm 10 allow") && ok;
    ok = expect(!ShouldAllowCommandPermission(10, 11), "residual 3263: perm 11 gm 10 reject") && ok;
    ok = expect(ShouldAllowCommandPermission(127, static_cast<int8>(127)), "residual 3293: perm 127 gm 127 allow") && ok;
    ok = expect(!ShouldAllowCommandPermission(100, static_cast<int8>(127)), "residual 3293: perm 127 gm 100 reject") && ok;

    // --- Core poles: free == inline == pin positive form ---
    const struct
    {
        uint8       gmLevel;
        int8        permission;
        bool        want;
        const char* label;
    } cases[] = {
        // Pole: classic dual poles / equal boundary.
        { 0, 0, true, "perm 0 gm 0 allow" },
        { 5, 5, true, "perm 5 gm 5 allow" },
        { 5, 0, true, "perm 0 gm 5 allow" },
        { 5, 4, true, "perm 4 gm 5 allow" },
        { 0, 1, false, "perm 1 gm 0 reject" },
        { 4, 5, false, "perm 5 gm 4 reject" },
        { 10, 9, true, "perm 9 gm 10 allow" },
        { 10, 11, false, "perm 11 gm 10 reject" },
        { 127, static_cast<int8>(127), true, "perm 127 gm 127 allow" },

        // Pole: negative int8 permission promotes to signed int; never > any uint8 gmLevel.
        { 0, static_cast<int8>(-1), true, "perm -1 gm 0 allow" },
        { 255, static_cast<int8>(-128), true, "perm -128 gm 255 allow" },
        { 1, static_cast<int8>(-5), true, "perm -5 gm 1 allow" },
        { 0, static_cast<int8>(-128), true, "perm -128 gm 0 allow" },

        // Pole: high gmLevel vs mid permission after promotion to int.
        { 255, static_cast<int8>(127), true, "perm 127 gm 255 allow" },
        { 100, static_cast<int8>(127), false, "perm 127 gm 100 reject" },
        { 255, 0, true, "perm 0 gm 255 allow" },
        { 50, static_cast<int8>(127), false, "perm 127 gm 50 reject" },

        // Residual 2940 / 2792 / 3263 / 3293 re-pins.
        { 5, 3, true, "residual 2940 allow mid" },
        { 2, 5, false, "residual 2940 reject high" },
        { 0, 0, true, "residual 2792 equal zero" },
        { 5, 5, true, "residual 2792 equal mid" },
        { 10, 9, true, "residual 3263 just under" },
        { 10, 11, false, "residual 3263 just over" },
        { 127, static_cast<int8>(127), true, "residual 3293 equal max int8" },
        { 100, static_cast<int8>(127), false, "residual 3293 high perm reject" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldAllowCommandPermission(c.gmLevel, c.permission);
        const bool inlineF = inlineShouldAllowCommandPermission(c.gmLevel, c.permission);
        const bool pinGot  = pinShouldAllowCommandPermission(c.gmLevel, c.permission);
        const bool wantPin = static_cast<int>(c.permission) <= static_cast<int>(c.gmLevel);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "dual-wire free==inline") && ok;
        ok = expect(got == pinGot, "dual-wire free==pin") && ok;
        ok = expect(got == wantPin, "dual-wire free==positive pin form") && ok;
    }

    // Free == pin across residual poles.
    ok = expect(ShouldAllowCommandPermission(5, 5) == pinShouldAllowCommandPermission(5, 5),
                "free==pin equal permission") &&
         ok;
    ok = expect(ShouldAllowCommandPermission(5, 0) == pinShouldAllowCommandPermission(5, 0),
                "free==pin lower permission") &&
         ok;
    ok = expect(ShouldAllowCommandPermission(0, 1) == pinShouldAllowCommandPermission(0, 1),
                "free==pin higher permission") &&
         ok;
    ok = expect(ShouldAllowCommandPermission(0, static_cast<int8>(-1)) ==
                    pinShouldAllowCommandPermission(0, static_cast<int8>(-1)),
                "free==pin negative permission") &&
         ok;

    // Dense compose over common GM levels and permission span —
    // free == inline == pin.
    for (int gm = 0; gm <= 10; ++gm)
    {
        for (int perm = -2; perm <= 12; ++perm)
        {
            const auto gmLevel    = static_cast<uint8>(gm);
            const auto permission = static_cast<int8>(perm);
            const bool got        = ShouldAllowCommandPermission(gmLevel, permission);
            const bool inlineF    = inlineShouldAllowCommandPermission(gmLevel, permission);
            const bool pinGot     = pinShouldAllowCommandPermission(gmLevel, permission);
            ok                    = expect(got == inlineF, "compose free==inline") && ok;
            ok                    = expect(got == pinGot, "compose free==pin") && ok;
            ok                    = expect(got == (static_cast<int>(permission) <= static_cast<int>(gmLevel)),
                        "compose free==pin formula") &&
                 ok;
        }
    }

    // Host-style inject poles: m_GMlevel (uint8) + Lua cmdprops permission (int8).
    // Production call uses PlanCommandCallPostProps which dual-wires this free function.
    {
        const auto planAllow = PlanCommandCallPostProps(/*gmLevel=*/5, /*permission=*/3, /*auditLevel=*/0);
        ok                   = expect(!planAllow.rejectPermission, "post-props allow composes free gate") && ok;
        ok                   = expect(ShouldAllowCommandPermission(5, 3) && pinShouldAllowCommandPermission(5, 3),
                    "host inject allow dual-wire") &&
             ok;
    }
    {
        const auto planReject = PlanCommandCallPostProps(/*gmLevel=*/2, /*permission=*/5, /*auditLevel=*/1);
        ok                    = expect(planReject.rejectPermission, "post-props reject composes free gate") && ok;
        ok                    = expect(!planReject.scheduleAudit, "reject forces scheduleAudit false") && ok;
        ok                    = expect(!ShouldAllowCommandPermission(2, 5) && !pinShouldAllowCommandPermission(2, 5),
                    "host inject reject dual-wire") &&
             ok;
    }

    // Production CCommandHandler::call path semantics:
    // allow → continue audit / onTrigger path
    // reject → warn Failure
    ok = expect(ShouldAllowCommandPermission(5, 3) && pinShouldAllowCommandPermission(5, 3),
                "call allow → continue path") &&
         ok;
    ok = expect(!ShouldAllowCommandPermission(0, 1) && !pinShouldAllowCommandPermission(0, 1),
                "call reject → Failure path") &&
         ok;
    ok = expect(ShouldAllowCommandPermission(0, 0) && pinShouldAllowCommandPermission(0, 0),
                "call equal zero → continue path") &&
         ok;
    ok = expect(ShouldAllowCommandPermission(255, static_cast<int8>(-128)) &&
                    pinShouldAllowCommandPermission(255, static_cast<int8>(-128)),
                "call negative perm → continue") &&
         ok;

    // Residual independence (2792 / 2940 / 2982 / 2990 / 3005 / 3011 / 3161 / 3185 / 3205 / 3263 / 3293):
    // permission allow is distinct from null / empty-name / empty-line / audit gates.
    ok = expect(ShouldAllowCommandPermission(5, 3), "permission mid must allow via free gate") && ok;
    ok = expect(ShouldRejectNullChar(true), "null char still rejects under residual") && ok;
    ok = expect(ShouldRejectEmptyCommandName(false), "invalid name still rejects under residual") && ok;
    ok = expect(ShouldRejectEmptyCommandLine(true), "empty line still rejects under residual") && ok;
    ok = expect(ShouldAuditGMCommand(1, 5), "audit still schedules under residual") && ok;
    ok = expect(!ShouldAllowCommandPermission(2, 5), "higher permission must reject independently") && ok;
    ok = expect(!ShouldRejectNullChar(false), "non-null must not reject via null-char") && ok;
    ok = expect(!ShouldRejectEmptyCommandLine(false), "non-empty line must not reject via empty-line") && ok;

    return ok;
}
