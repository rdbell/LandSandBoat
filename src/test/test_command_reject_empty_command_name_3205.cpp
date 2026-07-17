#include "test_command_reject_empty_command_name_3205.h"

#include "map/command_handler_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "command ShouldRejectEmptyCommandName 3205 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CCommandHandler::call empty-name gate for dual-wire cross-check
// (dedicated 3205):
//   !valid
auto inlineShouldRejectEmptyCommandName(const bool valid) -> bool
{
    return !valid;
}

// Compact dual-wire pin matching Go pinShouldRejectEmptyCommandName3205 /
// C++ capacity identity-not form (formula unchanged from 2792 / 2990).
// Positive early-return form keeps dual-wire pins lint-stable (avoid
// QF1001-style De Morgan rewrites of compound nots; here the single-not is
// spelled as positive branches for free==inline==pin readability):
//   if (valid) { return false; }
//   return true;
auto pinShouldRejectEmptyCommandName(const bool valid) -> bool
{
    if (valid)
    {
        return false;
    }
    return true;
}

} // namespace

// Pure dual-wire expansion for commandhandlerhelpers::ShouldRejectEmptyCommandName
// (!valid identity-not; OmegaXI internal/command; dedicated slice 3205;
// residual expand 2990 / pure 2792).
//
// Coverage:
//   - free == inline == pin identity-not form
//   - poles: invalid name rejects; valid name proceeds
//   - residual 2990 / 2792 pins still hold
//   - host-style inject + residual independence
auto runCommandRejectEmptyCommandName3205SelfTests() -> bool
{
    using commandhandlerhelpers::ShouldAllowCommandPermission;
    using commandhandlerhelpers::ShouldAuditGMCommand;
    using commandhandlerhelpers::ShouldRejectEmptyCommandLine;
    using commandhandlerhelpers::ShouldRejectEmptyCommandName;
    using commandhandlerhelpers::ShouldRejectNullChar;

    bool ok = true;

    // Residual 2792 / 2990 pins still hold under dedicated dual-wire.
    ok = expect(ShouldRejectEmptyCommandName(false), "residual: invalid name rejects") && ok;
    ok = expect(!ShouldRejectEmptyCommandName(true), "residual: valid name proceeds") && ok;

    // --- Core poles: free == inline == pin identity-not form ---
    const struct
    {
        bool        valid;
        bool        want;
        const char* label;
    } cases[] = {
        // Pole: invalid name rejects (early Failure before Lua lookup).
        { false, true, "invalid name rejects" },

        // Pole: valid name proceeds to command table / permission.
        { true, false, "valid name proceeds" },

        // Residual 2990 / 2792 re-pins.
        { false, true, "residual 2990 invalid rejects" },
        { true, false, "residual 2990 valid proceeds" },
        { false, true, "residual 2792 invalid rejects" },
        { true, false, "residual 2792 valid proceeds" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRejectEmptyCommandName(c.valid);
        const bool inlineF = inlineShouldRejectEmptyCommandName(c.valid);
        const bool pinGot  = pinShouldRejectEmptyCommandName(c.valid);
        const bool wantPin = !c.valid;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "dual-wire free==inline") && ok;
        ok = expect(got == pinGot, "dual-wire free==pin") && ok;
        ok = expect(got == wantPin, "dual-wire free==identity-not pin form") && ok;
    }

    // Free == pin across residual poles.
    ok = expect(ShouldRejectEmptyCommandName(false) == pinShouldRejectEmptyCommandName(false),
                "free==pin invalid name") &&
         ok;
    ok = expect(ShouldRejectEmptyCommandName(true) == pinShouldRejectEmptyCommandName(true),
                "free==pin valid name") &&
         ok;

    // Dense compose: full 2^1 boolean space free == inline == pin.
    for (const bool valid : { false, true })
    {
        const bool got     = ShouldRejectEmptyCommandName(valid);
        const bool inlineF = inlineShouldRejectEmptyCommandName(valid);
        const bool pinGot  = pinShouldRejectEmptyCommandName(valid);
        ok                 = expect(got == inlineF, "compose free==inline") && ok;
        ok                 = expect(got == pinGot, "compose free==pin") && ok;
        ok                 = expect(got == !valid, "compose free==identity-not") && ok;
    }

    // Host-style inject poles: CCommandHandler::call injects parsedName.valid.
    const struct
    {
        bool        valid;
        bool        wantReject;
        const char* label;
    } hostCases[] = {
        { false, true, "parsedName.valid == false → Failure before Lua lookup" },
        { true, false, "parsedName.valid == true → proceed to command table" },
    };

    for (const auto& c : hostCases)
    {
        const bool got     = ShouldRejectEmptyCommandName(c.valid);
        const bool inlineF = inlineShouldRejectEmptyCommandName(c.valid);
        const bool pinGot  = pinShouldRejectEmptyCommandName(c.valid);

        ok = expect(got == c.wantReject, c.label) && ok;
        ok = expect(got == inlineF, "host compose free==inline") && ok;
        ok = expect(got == pinGot, "host compose free==pin") && ok;
    }

    // Production CCommandHandler::call path semantics:
    // invalid name → Failure before Lua; valid name → proceed to table.
    ok = expect(ShouldRejectEmptyCommandName(false) && pinShouldRejectEmptyCommandName(false),
                "call invalid name → Failure path") &&
         ok;
    ok = expect(!ShouldRejectEmptyCommandName(true) && !pinShouldRejectEmptyCommandName(true),
                "call valid name → proceed path") &&
         ok;

    // Residual independence (2792 / 2940 / 2982 / 2990 / 3005 / 3011 / 3161 / 3185):
    // empty-name reject is distinct from null-char / empty-line / permission / audit.
    ok = expect(ShouldRejectEmptyCommandName(false), "invalid name must reject via free gate") && ok;
    ok = expect(ShouldRejectNullChar(true), "null char still rejects under residual") && ok;
    ok = expect(ShouldRejectEmptyCommandLine(true), "empty line still rejects under residual") && ok;
    ok = expect(ShouldAllowCommandPermission(5, 3), "permission still allows under residual") && ok;
    ok = expect(ShouldAuditGMCommand(1, 5), "audit still schedules under residual") && ok;
    ok = expect(!ShouldRejectEmptyCommandName(true), "valid name must proceed even if prior gates fail") && ok;
    ok = expect(!ShouldRejectNullChar(false), "non-null must not reject via null-char") && ok;
    ok = expect(!ShouldRejectEmptyCommandLine(false), "non-empty line must not reject via empty-line") && ok;

    return ok;
}
