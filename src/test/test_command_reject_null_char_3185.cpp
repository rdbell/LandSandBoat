#include "test_command_reject_null_char_3185.h"

#include "map/command_handler_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "command ShouldRejectNullChar 3185 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CCommandHandler::call null-PChar gate for dual-wire cross-check
// (dedicated 3185):
//   charNull
auto inlineShouldRejectNullChar(const bool charNull) -> bool
{
    return charNull;
}

// Compact dual-wire pin matching Go pinShouldRejectNullChar3185 / C++ capacity
// identity form (formula unchanged from 2792 / 2982):
//   charNull
auto pinShouldRejectNullChar(const bool charNull) -> bool
{
    return charNull;
}

} // namespace

// Pure dual-wire expansion for commandhandlerhelpers::ShouldRejectNullChar
// (charNull identity; OmegaXI internal/command; dedicated slice 3185;
// residual expand 2982 / pure 2792).
//
// Coverage:
//   - free == inline == pin identity form
//   - poles: null rejects; non-null proceeds
//   - residual 2982 / 2792 pins still hold
//   - host-style inject + residual independence
auto runCommandRejectNullChar3185SelfTests() -> bool
{
    using commandhandlerhelpers::ShouldAllowCommandPermission;
    using commandhandlerhelpers::ShouldAuditGMCommand;
    using commandhandlerhelpers::ShouldRejectEmptyCommandLine;
    using commandhandlerhelpers::ShouldRejectEmptyCommandName;
    using commandhandlerhelpers::ShouldRejectNullChar;

    bool ok = true;

    // Residual 2792 / 2982 pins still hold under dedicated dual-wire.
    ok = expect(ShouldRejectNullChar(true), "residual: null char rejects") && ok;
    ok = expect(!ShouldRejectNullChar(false), "residual: non-null char proceeds") && ok;

    // --- Core poles: free == inline == pin identity form ---
    const struct
    {
        bool        charNull;
        bool        want;
        const char* label;
    } cases[] = {
        // Pole: null PChar rejects (early Failure before parse).
        { true, true, "null char rejects" },

        // Pole: non-null PChar proceeds to name parse.
        { false, false, "non-null char proceeds" },

        // Residual 2982 / 2792 re-pins.
        { true, true, "residual 2982 null rejects" },
        { false, false, "residual 2982 non-null proceeds" },
        { true, true, "residual 2792 null rejects" },
        { false, false, "residual 2792 non-null proceeds" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRejectNullChar(c.charNull);
        const bool inlineF = inlineShouldRejectNullChar(c.charNull);
        const bool pinGot  = pinShouldRejectNullChar(c.charNull);
        const bool wantPin = c.charNull;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "dual-wire free==inline") && ok;
        ok = expect(got == pinGot, "dual-wire free==pin") && ok;
        ok = expect(got == wantPin, "dual-wire free==identity pin form") && ok;
    }

    // Free == pin across residual poles.
    ok = expect(ShouldRejectNullChar(true) == pinShouldRejectNullChar(true), "free==pin null char") && ok;
    ok = expect(ShouldRejectNullChar(false) == pinShouldRejectNullChar(false), "free==pin non-null char") && ok;

    // Dense compose: full 2^1 boolean space free == inline == pin.
    for (const bool charNull : { false, true })
    {
        const bool got     = ShouldRejectNullChar(charNull);
        const bool inlineF = inlineShouldRejectNullChar(charNull);
        const bool pinGot  = pinShouldRejectNullChar(charNull);
        ok                 = expect(got == inlineF, "compose free==inline") && ok;
        ok                 = expect(got == pinGot, "compose free==pin") && ok;
        ok                 = expect(got == charNull, "compose free==identity") && ok;
    }

    // Host-style inject poles: CCommandHandler::call injects PChar == nullptr.
    const struct
    {
        bool        charNull;
        bool        wantReject;
        const char* label;
    } hostCases[] = {
        { true, true, "PChar == nullptr → Failure before parse" },
        { false, false, "PChar non-null → proceed to name parse" },
    };

    for (const auto& c : hostCases)
    {
        const bool got     = ShouldRejectNullChar(c.charNull);
        const bool inlineF = inlineShouldRejectNullChar(c.charNull);
        const bool pinGot  = pinShouldRejectNullChar(c.charNull);

        ok = expect(got == c.wantReject, c.label) && ok;
        ok = expect(got == inlineF, "host compose free==inline") && ok;
        ok = expect(got == pinGot, "host compose free==pin") && ok;
    }

    // Production CCommandHandler::call path semantics:
    // null → Failure before parse; non-null → proceed to name parse.
    ok = expect(ShouldRejectNullChar(true) && pinShouldRejectNullChar(true), "call null → Failure path") && ok;
    ok = expect(!ShouldRejectNullChar(false) && !pinShouldRejectNullChar(false), "call non-null → proceed path") && ok;

    // Residual independence (2792 / 2940 / 2982 / 2990 / 3005 / 3011 / 3161):
    // null reject is distinct from empty-name / empty-line / permission / audit.
    ok = expect(ShouldRejectNullChar(true), "null char must reject via free gate") && ok;
    ok = expect(ShouldRejectEmptyCommandName(false), "invalid name still rejects under residual") && ok;
    ok = expect(ShouldRejectEmptyCommandLine(true), "empty line still rejects under residual") && ok;
    ok = expect(ShouldAllowCommandPermission(5, 3), "permission still allows under residual") && ok;
    ok = expect(ShouldAuditGMCommand(1, 5), "audit still schedules under residual") && ok;
    ok = expect(!ShouldRejectNullChar(false), "non-null must proceed even if later gates fail") && ok;
    ok = expect(!ShouldRejectEmptyCommandName(true), "valid name must not reject via empty-name") && ok;
    ok = expect(!ShouldRejectEmptyCommandLine(false), "non-empty line must not reject via empty-line") && ok;

    return ok;
}
