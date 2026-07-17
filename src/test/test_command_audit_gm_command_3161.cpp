#include "test_command_audit_gm_command_3161.h"

#include "map/command_handler_capacity.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "command ShouldAuditGMCommand 3161 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CCommandHandler::call audit gate for dual-wire cross-check (dedicated 3161):
//   static_cast<int>(auditLevel) <= static_cast<int>(permission) && auditLevel > 0
auto inlineShouldAuditGMCommand(const uint8 auditLevel, const int8 permission) -> bool
{
    return static_cast<int>(auditLevel) <= static_cast<int>(permission) && auditLevel > 0;
}

// Compact dual-wire pin matching Go pinShouldAuditGMCommand3161 / C++ capacity
// positive form (formula unchanged from 2792 / 3011):
//   int(auditLevel) <= int(permission) && auditLevel > 0
auto pinShouldAuditGMCommand(const uint8 auditLevel, const int8 permission) -> bool
{
    return static_cast<int>(auditLevel) <= static_cast<int>(permission) && auditLevel > 0;
}

} // namespace

// Pure dual-wire expansion for commandhandlerhelpers::ShouldAuditGMCommand
// (auditLevel <= permission && auditLevel > 0 after int promotions;
// OmegaXI internal/command; dedicated slice 3161; residual expand 3011 / pure 2792).
//
// Coverage:
//   - free == inline == pin positive form
//   - poles: audit 0 always false; level vs permission; signed permission edge
//   - residual 3011 / 2792 pins still hold
//   - post-props host-style inject + residual independence
auto runCommandAuditGMCommand3161SelfTests() -> bool
{
    using commandhandlerhelpers::PlanCommandCallPostProps;
    using commandhandlerhelpers::ShouldAuditGMCommand;

    bool ok = true;

    // Residual 2792 / 3011 pins still hold under dedicated dual-wire.
    ok = expect(!ShouldAuditGMCommand(0, 5), "residual: audit 0 disabled") && ok;
    ok = expect(!ShouldAuditGMCommand(0, 0), "residual: audit 0 perm 0") && ok;
    ok = expect(ShouldAuditGMCommand(1, 1), "residual: audit 1 == perm 1") && ok;
    ok = expect(ShouldAuditGMCommand(1, 5), "residual: audit 1 < perm 5") && ok;
    ok = expect(!ShouldAuditGMCommand(5, 4), "residual: audit 5 > perm 4") && ok;
    ok = expect(ShouldAuditGMCommand(5, 5), "residual: audit 5 == perm 5") && ok;
    ok = expect(!ShouldAuditGMCommand(1, static_cast<int8>(-1)), "residual: audit 1 perm -1") && ok;
    ok = expect(!ShouldAuditGMCommand(1, 0), "residual: audit 1 perm 0") && ok;
    ok = expect(ShouldAuditGMCommand(1, 3), "residual 3011: audit mid allow") && ok;
    ok = expect(!ShouldAuditGMCommand(5, 3), "residual 3011: audit mid reject") && ok;

    // --- Core poles: free == inline == pin positive form ---
    const struct
    {
        uint8       auditLevel;
        int8        permission;
        bool        want;
        const char* label;
    } cases[] = {
        // Pole: audit 0 always false (disabled) regardless of permission.
        { 0, 5, false, "audit 0 disabled" },
        { 0, 0, false, "audit 0 perm 0" },
        { 0, static_cast<int8>(-1), false, "audit 0 perm negative" },
        { 0, static_cast<int8>(127), false, "audit 0 perm max" },
        { 0, static_cast<int8>(-128), false, "audit 0 perm min" },

        // Pole: level vs permission (equal / under / over).
        { 1, 1, true, "audit 1 == perm 1" },
        { 1, 5, true, "audit 1 < perm 5" },
        { 5, 5, true, "audit 5 == perm 5" },
        { 3, 10, true, "audit 3 < perm 10" },
        { 5, 4, false, "audit 5 > perm 4" },
        { 1, 0, false, "audit 1 > perm 0" },
        { 255, static_cast<int8>(127), false, "audit 255 > perm 127" },
        { 10, 9, false, "audit 10 > perm 9" },
        { 127, static_cast<int8>(127), true, "audit 127 == perm 127" },

        // Pole: signed permission edge — positive audit never <= negative perm.
        { 1, static_cast<int8>(-1), false, "audit 1 perm -1" },
        { 1, static_cast<int8>(-128), false, "audit 1 perm -128" },
        { 255, static_cast<int8>(-1), false, "audit 255 perm -1" },
        { 5, static_cast<int8>(-5), false, "audit 5 perm -5" },
        { 127, static_cast<int8>(-128), false, "audit 127 perm -128" },

        // Residual 3011 / 2792 re-pins.
        { 1, 3, true, "residual 3011 audit mid allow" },
        { 5, 3, false, "residual 3011 audit mid reject" },
        { 0, 5, false, "residual 2792 audit 0 disabled" },
        { 1, 1, true, "residual 2792 equal positive" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldAuditGMCommand(c.auditLevel, c.permission);
        const bool inlineF = inlineShouldAuditGMCommand(c.auditLevel, c.permission);
        const bool pinGot  = pinShouldAuditGMCommand(c.auditLevel, c.permission);
        const bool wantPin = static_cast<int>(c.auditLevel) <= static_cast<int>(c.permission) && c.auditLevel > 0;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "dual-wire free==inline") && ok;
        ok = expect(got == pinGot, "dual-wire free==pin") && ok;
        ok = expect(got == wantPin, "dual-wire free==positive pin form") && ok;
    }

    // Free == pin across residual poles.
    ok = expect(ShouldAuditGMCommand(0, 5) == pinShouldAuditGMCommand(0, 5), "free==pin audit 0") && ok;
    ok = expect(ShouldAuditGMCommand(1, 1) == pinShouldAuditGMCommand(1, 1), "free==pin equal positive") && ok;
    ok = expect(ShouldAuditGMCommand(5, 4) == pinShouldAuditGMCommand(5, 4), "free==pin audit > permission") && ok;
    ok = expect(ShouldAuditGMCommand(1, static_cast<int8>(-1)) == pinShouldAuditGMCommand(1, static_cast<int8>(-1)),
                "free==pin negative permission") &&
         ok;

    // Dense compose over common audit levels and permission span —
    // free == inline == pin.
    for (int audit = 0; audit <= 10; ++audit)
    {
        for (int perm = -2; perm <= 12; ++perm)
        {
            const auto auditLevel = static_cast<uint8>(audit);
            const auto permission = static_cast<int8>(perm);
            const bool got        = ShouldAuditGMCommand(auditLevel, permission);
            const bool inlineF    = inlineShouldAuditGMCommand(auditLevel, permission);
            const bool pinGot     = pinShouldAuditGMCommand(auditLevel, permission);
            ok                    = expect(got == inlineF, "compose free==inline") && ok;
            ok                    = expect(got == pinGot, "compose free==pin") && ok;
        }
    }

    // Host-style inject poles: AUDIT_GM_CMD (uint8) + Lua cmdprops permission (int8).
    // Production call uses PlanCommandCallPostProps which dual-wires this free function.
    {
        const auto planAllowAudit = PlanCommandCallPostProps(/*gmLevel=*/5, /*permission=*/3, /*auditLevel=*/1);
        ok                        = expect(!planAllowAudit.rejectPermission && planAllowAudit.scheduleAudit,
                    "post-props allow+audit composes free gate") &&
             ok;
        ok = expect(ShouldAuditGMCommand(1, 3) && pinShouldAuditGMCommand(1, 3), "host inject audit dual-wire") && ok;
    }
    {
        const auto planNoAudit = PlanCommandCallPostProps(/*gmLevel=*/5, /*permission=*/3, /*auditLevel=*/0);
        ok                     = expect(!planNoAudit.rejectPermission && !planNoAudit.scheduleAudit,
                    "post-props allow no audit composes free gate") &&
             ok;
        ok = expect(!ShouldAuditGMCommand(0, 3), "host inject audit-0 dual-wire") && ok;
    }
    {
        // Permission reject forces scheduleAudit false even when free audit would match.
        const auto planReject = PlanCommandCallPostProps(/*gmLevel=*/2, /*permission=*/5, /*auditLevel=*/1);
        ok                    = expect(planReject.rejectPermission && !planReject.scheduleAudit,
                    "post-props reject forces no audit") &&
             ok;
        ok = expect(ShouldAuditGMCommand(1, 5), "audit free gate independent of permission reject") && ok;
    }

    // Production CCommandHandler::call path semantics:
    // schedule → audit_gm INSERT worker
    // no schedule → continue onTrigger without audit insert
    ok = expect(ShouldAuditGMCommand(1, 5), "call audit → schedule path") && ok;
    ok = expect(!ShouldAuditGMCommand(0, 5), "call audit 0 → no schedule path") && ok;
    ok = expect(!ShouldAuditGMCommand(5, 4), "call audit > perm → no schedule path") && ok;
    ok = expect(!ShouldAuditGMCommand(1, static_cast<int8>(-1)), "call negative perm → no schedule") && ok;

    return ok;
}
