#include "test_command_audit_gm_3011.h"

#include "map/command_handler_capacity.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "command ShouldAuditGMCommand 3011 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CCommandHandler::call audit gate for dual-wire cross-check (slice 3011):
//   static_cast<int>(auditLevel) <= static_cast<int>(permission) && auditLevel > 0
auto inlineShouldAuditGMCommand(const uint8 auditLevel, const int8 permission) -> bool
{
    return static_cast<int>(auditLevel) <= static_cast<int>(permission) && auditLevel > 0;
}

} // namespace

// Pure dual-wire expansion for commandhandlerhelpers::ShouldAuditGMCommand
// (auditLevel <= permission && auditLevel > 0 after int promotions; slice 3011).
auto runCommandAuditGM3011SelfTests() -> bool
{
    using commandhandlerhelpers::PlanCommandCallPostProps;
    using commandhandlerhelpers::ShouldAuditGMCommand;

    bool ok = true;

    const struct
    {
        uint8       auditLevel;
        int8        permission;
        bool        want;
        const char* label;
    } cases[] = {
        // Edge: audit 0 always false (disabled) regardless of permission.
        { 0, 5, false, "audit 0 disabled" },
        { 0, 0, false, "audit 0 perm 0" },
        { 0, static_cast<int8>(-1), false, "audit 0 perm negative" },
        { 0, static_cast<int8>(127), false, "audit 0 perm max" },

        // Equal / under boundary.
        { 1, 1, true, "audit 1 == perm 1" },
        { 1, 5, true, "audit 1 < perm 5" },
        { 5, 5, true, "audit 5 == perm 5" },

        // Edge: audit > permission false.
        { 5, 4, false, "audit 5 > perm 4" },
        { 1, 0, false, "audit 1 > perm 0" },
        { 255, static_cast<int8>(127), false, "audit 255 > perm 127" },

        // Edge: negative permission — auditLevel promotes to signed int;
        // positive auditLevel is never <= negative permission.
        { 1, static_cast<int8>(-1), false, "audit 1 perm -1" },
        { 1, static_cast<int8>(-128), false, "audit 1 perm -128" },
        { 255, static_cast<int8>(-1), false, "audit 255 perm -1" },

        // Residual 2792 pins.
        { 1, 3, true, "residual audit mid allow" },
        { 5, 3, false, "residual audit mid reject" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldAuditGMCommand(c.auditLevel, c.permission);
        const bool inlineF = inlineShouldAuditGMCommand(c.auditLevel, c.permission);
        const bool wantPin = static_cast<int>(c.auditLevel) <= static_cast<int>(c.permission) && c.auditLevel > 0;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldAuditGMCommand dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldAuditGMCommand == pin formula auditLevel <= permission && auditLevel > 0") && ok;
    }

    // Pin composition edges from the slice brief.
    ok = expect(!ShouldAuditGMCommand(0, 5), "audit 0 must always be false") && ok;
    ok = expect(!ShouldAuditGMCommand(0, 0), "audit 0 perm 0 must be false") && ok;
    ok = expect(!ShouldAuditGMCommand(5, 4), "audit > permission must be false") && ok;
    ok = expect(!ShouldAuditGMCommand(1, static_cast<int8>(-1)), "negative permission must not schedule audit") && ok;
    ok = expect(ShouldAuditGMCommand(1, 1), "equal positive audit must schedule") && ok;
    ok = expect(ShouldAuditGMCommand(5, 5), "equal mid audit must schedule") && ok;

    // Dense compose over common audit levels and permission span.
    for (int audit = 0; audit <= 10; ++audit)
    {
        for (int perm = -2; perm <= 12; ++perm)
        {
            const auto auditLevel = static_cast<uint8>(audit);
            const auto permission = static_cast<int8>(perm);
            const bool got        = ShouldAuditGMCommand(auditLevel, permission);
            const bool want       = static_cast<int>(auditLevel) <= static_cast<int>(permission) && auditLevel > 0;
            ok                    = expect(got == want, "compose free == pin formula") && ok;
            ok                    = expect(got == inlineShouldAuditGMCommand(auditLevel, permission),
                        "compose free == inline") &&
                 ok;
        }
    }

    // Host-style inject poles: AUDIT_GM_CMD (uint8) + Lua cmdprops permission (int8).
    // Production call uses PlanCommandCallPostProps which dual-wires this free function.
    {
        const auto planAllowAudit = PlanCommandCallPostProps(/*gmLevel=*/5, /*permission=*/3, /*auditLevel=*/1);
        ok                        = expect(!planAllowAudit.rejectPermission && planAllowAudit.scheduleAudit,
                    "post-props allow+audit composes free gate") &&
             ok;
        ok = expect(ShouldAuditGMCommand(1, 3), "host inject audit dual-wire") && ok;
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
