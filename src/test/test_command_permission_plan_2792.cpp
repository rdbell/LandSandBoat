#include "test_command_permission_plan_2792.h"

#include "map/command_handler_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "command permission plan 2792 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runCommandPermissionPlan2792SelfTests() -> bool
{
    using commandhandlerhelpers::PlanCommandCallPostProps;
    using commandhandlerhelpers::ShouldAllowCommandPermission;
    using commandhandlerhelpers::ShouldAuditGMCommand;
    using commandhandlerhelpers::ShouldRejectEmptyCommandName;
    using commandhandlerhelpers::ShouldRejectNullChar;

    bool ok = true;

    // --- Null char ---
    ok = expect(ShouldRejectNullChar(true), "null char reject") && ok;
    ok = expect(!ShouldRejectNullChar(false), "non-null char allow") && ok;

    // --- Empty command name (valid flag from ParseCommandLine) ---
    ok = expect(ShouldRejectEmptyCommandName(false), "invalid name reject") && ok;
    ok = expect(!ShouldRejectEmptyCommandName(true), "valid name allow") && ok;

    // --- Permission: permission > gmLevel fails (int8 vs uint8) ---
    // Equal permission allowed.
    ok = expect(ShouldAllowCommandPermission(/*gmLevel=*/0, /*permission=*/0), "perm 0 gm 0") && ok;
    ok = expect(ShouldAllowCommandPermission(5, 5), "perm 5 gm 5") && ok;
    // Lower permission allowed.
    ok = expect(ShouldAllowCommandPermission(5, 0), "perm 0 gm 5") && ok;
    ok = expect(ShouldAllowCommandPermission(5, 4), "perm 4 gm 5") && ok;
    // Higher permission rejected.
    ok = expect(!ShouldAllowCommandPermission(0, 1), "perm 1 gm 0 reject") && ok;
    ok = expect(!ShouldAllowCommandPermission(4, 5), "perm 5 gm 4 reject") && ok;

    // Negative permission (int8): promotes to signed int; never > any uint8 gmLevel.
    ok = expect(ShouldAllowCommandPermission(0, static_cast<int8>(-1)), "perm -1 gm 0 allow") && ok;
    ok = expect(ShouldAllowCommandPermission(255, static_cast<int8>(-128)), "perm -128 gm 255 allow") && ok;

    // High gmLevel vs mid permission: 127 > 255 is false after promotion to int.
    ok = expect(ShouldAllowCommandPermission(255, static_cast<int8>(127)), "perm 127 gm 255 allow") && ok;
    // permission 127 vs gm 100: 127 > 100 → reject.
    ok = expect(!ShouldAllowCommandPermission(100, static_cast<int8>(127)), "perm 127 gm 100 reject") && ok;

    // --- Audit: auditLevel <= permission && auditLevel > 0 ---
    ok = expect(!ShouldAuditGMCommand(0, 5), "audit 0 disabled") && ok;
    ok = expect(!ShouldAuditGMCommand(0, 0), "audit 0 perm 0") && ok;
    ok = expect(ShouldAuditGMCommand(1, 1), "audit 1 == perm 1") && ok;
    ok = expect(ShouldAuditGMCommand(1, 5), "audit 1 < perm 5") && ok;
    ok = expect(!ShouldAuditGMCommand(5, 4), "audit 5 > perm 4") && ok;
    ok = expect(ShouldAuditGMCommand(5, 5), "audit 5 == perm 5") && ok;

    // Negative permission: auditLevel (uint8) promotes to int; e.g. 1 <= -1 is false.
    ok = expect(!ShouldAuditGMCommand(1, static_cast<int8>(-1)), "audit 1 perm -1") && ok;
    ok = expect(!ShouldAuditGMCommand(1, static_cast<int8>(0)), "audit 1 perm 0") && ok;

    // --- Post-props plan ---
    {
        const auto plan = PlanCommandCallPostProps(/*gmLevel=*/5, /*permission=*/3, /*auditLevel=*/1);
        ok = expect(!plan.rejectPermission && plan.scheduleAudit, "plan allow+audit") && ok;
    }
    {
        const auto plan = PlanCommandCallPostProps(5, 3, 0);
        ok = expect(!plan.rejectPermission && !plan.scheduleAudit, "plan allow no audit") && ok;
    }
    {
        const auto plan = PlanCommandCallPostProps(2, 5, 1);
        ok = expect(plan.rejectPermission && !plan.scheduleAudit, "plan reject forces no audit") && ok;
    }
    {
        // Permission reject even when audit would have matched.
        const auto plan = PlanCommandCallPostProps(0, 1, 1);
        ok = expect(plan.rejectPermission && !plan.scheduleAudit, "plan reject overrides audit") && ok;
    }
    {
        const auto plan = PlanCommandCallPostProps(5, 5, 5);
        ok = expect(!plan.rejectPermission && plan.scheduleAudit, "plan equal perm audit") && ok;
    }

    return ok;
}
