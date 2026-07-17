#include "test_command_reject_empty_line_3518.h"

#include "map/command_handler_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "command ShouldRejectEmptyCommandLine 3518 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline ParseCommandLine empty-line gate for dual-wire cross-check
// (dedicated 3518 expand residual 2836 / prior dedicated 3362 / prior 3005):
//   viewEmptyAfterTrim
auto inlineShouldRejectEmptyCommandLine(const bool viewEmptyAfterTrim) -> bool
{
    return viewEmptyAfterTrim;
}

// Compact dual-wire pin matching Go pinShouldRejectEmptyCommandLine3518 /
// C++ capacity identity form (formula unchanged from 2836 / 3005 / 3362).
auto pinShouldRejectEmptyCommandLine(const bool viewEmptyAfterTrim) -> bool
{
    return viewEmptyAfterTrim;
}

// Prior dedicated 3362 pin form (formula unchanged; independence under 3518).
auto pinShouldRejectEmptyCommandLine3362(const bool viewEmptyAfterTrim) -> bool
{
    return viewEmptyAfterTrim;
}

// Prior dedicated 3362 inline form (formula unchanged; independence under 3518).
auto inlineShouldRejectEmptyCommandLine3362(const bool viewEmptyAfterTrim) -> bool
{
    return viewEmptyAfterTrim;
}

} // namespace

// Pure dual-wire expansion for commandhandlerhelpers::ShouldRejectEmptyCommandLine
// (viewEmptyAfterTrim identity; OmegaXI internal/command; dedicated slice 3518
// expand residual 2836 / prior dedicated 3362 / prior dual-wire 3005).
//
// Coverage:
//   - free == inline == pin identity form
//   - poles: empty after trim rejects; non-empty proceeds
//   - residual 2836 / 3005 / prior dedicated 3362 pins still hold
//   - dense 2^1 free == inline == pin residual pins
//   - host-style inject + residual independence + prior 3362 independence
auto runCommandRejectEmptyLine3518SelfTests() -> bool
{
    using commandhandlerhelpers::ShouldAllowCommandPermission;
    using commandhandlerhelpers::ShouldAuditGMCommand;
    using commandhandlerhelpers::ShouldRejectEmptyCommandLine;
    using commandhandlerhelpers::ShouldRejectEmptyCommandName;
    using commandhandlerhelpers::ShouldRejectNullChar;

    bool ok = true;

    // Residual 2836 / prior dual-wire 3005 / prior dedicated 3362 pins still hold
    // under dedicated expand.
    ok = expect(ShouldRejectEmptyCommandLine(true), "residual: empty after trim rejects") && ok;
    ok = expect(!ShouldRejectEmptyCommandLine(false), "residual: non-empty after trim proceeds") && ok;

    // --- Core poles: free == inline == pin identity form ---
    const struct
    {
        bool        viewEmptyAfterTrim;
        bool        want;
        const char* label;
    } cases[] = {
        // Pole: empty after trim rejects (return {} before token extract).
        { true, true, "empty after trim rejects" },

        // Pole: non-empty after trim proceeds to popToken / parameter typing.
        { false, false, "non-empty after trim proceeds" },

        // Residual 2836 / 3005 / 3362 re-pins.
        { true, true, "residual 2836 empty rejects" },
        { false, false, "residual 2836 non-empty proceeds" },
        { true, true, "residual 3005 empty rejects" },
        { false, false, "residual 3005 non-empty proceeds" },
        { true, true, "residual 3362 empty rejects" },
        { false, false, "residual 3362 non-empty proceeds" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRejectEmptyCommandLine(c.viewEmptyAfterTrim);
        const bool inlineF = inlineShouldRejectEmptyCommandLine(c.viewEmptyAfterTrim);
        const bool pinGot  = pinShouldRejectEmptyCommandLine(c.viewEmptyAfterTrim);
        const bool wantPin = c.viewEmptyAfterTrim;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "dual-wire free==inline") && ok;
        ok = expect(got == pinGot, "dual-wire free==pin") && ok;
        ok = expect(got == wantPin, "dual-wire free==identity pin form") && ok;
    }

    // Free == pin across residual poles.
    ok = expect(ShouldRejectEmptyCommandLine(true) == pinShouldRejectEmptyCommandLine(true),
                "free==pin empty after trim") &&
         ok;
    ok = expect(ShouldRejectEmptyCommandLine(false) == pinShouldRejectEmptyCommandLine(false),
                "free==pin non-empty after trim") &&
         ok;

    // Prior dedicated 3362 independence: free == 3362 pin/inline under 3518.
    ok = expect(ShouldRejectEmptyCommandLine(true) == pinShouldRejectEmptyCommandLine3362(true) &&
                    ShouldRejectEmptyCommandLine(true) == inlineShouldRejectEmptyCommandLine3362(true),
                "prior 3362 pin/inline must still reject empty after trim") &&
         ok;
    ok = expect(ShouldRejectEmptyCommandLine(false) == pinShouldRejectEmptyCommandLine3362(false) &&
                    ShouldRejectEmptyCommandLine(false) == inlineShouldRejectEmptyCommandLine3362(false),
                "prior 3362 pin/inline must still proceed non-empty after trim") &&
         ok;
    ok = expect(pinShouldRejectEmptyCommandLine(true) == pinShouldRejectEmptyCommandLine3362(true) &&
                    pinShouldRejectEmptyCommandLine(false) == pinShouldRejectEmptyCommandLine3362(false),
                "3518 pin must match prior 3362 pin (formula unchanged)") &&
         ok;

    // Dense compose: full 2^1 boolean space free == inline == pin residual pins.
    for (const bool viewEmptyAfterTrim : { false, true })
    {
        const bool got     = ShouldRejectEmptyCommandLine(viewEmptyAfterTrim);
        const bool inlineF = inlineShouldRejectEmptyCommandLine(viewEmptyAfterTrim);
        const bool pinGot  = pinShouldRejectEmptyCommandLine(viewEmptyAfterTrim);
        ok                 = expect(got == inlineF, "compose free==inline") && ok;
        ok                 = expect(got == pinGot, "compose free==pin") && ok;
        ok                 = expect(got == viewEmptyAfterTrim, "compose free==identity") && ok;
        // Prior dedicated 3362 still dual-wires the same identity.
        ok = expect(got == inlineShouldRejectEmptyCommandLine3362(viewEmptyAfterTrim) &&
                        got == pinShouldRejectEmptyCommandLine3362(viewEmptyAfterTrim),
                    "prior 3362 compose free==inline==pin") &&
             ok;
    }

    // Host-style inject poles: ParseCommandLine injects view.empty() after trimLeft.
    const struct
    {
        bool        viewEmptyAfterTrim;
        bool        wantReject;
        const char* label;
    } hostCases[] = {
        { true, true, "view.empty() after trimLeft → return {} before token extract" },
        { false, false, "view non-empty after trimLeft → proceed to popToken" },
    };

    for (const auto& c : hostCases)
    {
        const bool got     = ShouldRejectEmptyCommandLine(c.viewEmptyAfterTrim);
        const bool inlineF = inlineShouldRejectEmptyCommandLine(c.viewEmptyAfterTrim);
        const bool pinGot  = pinShouldRejectEmptyCommandLine(c.viewEmptyAfterTrim);

        ok = expect(got == c.wantReject, c.label) && ok;
        ok = expect(got == inlineF, "host compose free==inline") && ok;
        ok = expect(got == pinGot, "host compose free==pin") && ok;
    }

    // When host would inject view.empty() after trimLeft of whitespace-only
    // input, pure gate still rejects (same as literal empty string).
    const bool whitespaceOnlyEmpty = true; // "   " / "\t\t" / " \t " after trim
    ok = expect(ShouldRejectEmptyCommandLine(whitespaceOnlyEmpty) &&
                    pinShouldRejectEmptyCommandLine(whitespaceOnlyEmpty) &&
                    inlineShouldRejectEmptyCommandLine(whitespaceOnlyEmpty),
                "whitespace-only free==inline==pin reject") &&
         ok;

    // Production ParseCommandLine path semantics:
    // empty after trim → return {}; non-empty → proceed to popToken.
    ok = expect(ShouldRejectEmptyCommandLine(true) && pinShouldRejectEmptyCommandLine(true),
                "Parse empty after trim → reject path") &&
         ok;
    ok = expect(!ShouldRejectEmptyCommandLine(false) && !pinShouldRejectEmptyCommandLine(false),
                "Parse non-empty after trim → proceed path") &&
         ok;

    // Residual independence (2792 / 2836 / 2940 / 2982 / 2990 / 3005 / 3011 / 3161 /
    // 3185 / 3205 / 3362): empty-line reject is distinct from null-char /
    // empty-name / permission / audit; prior dedicated 3362 remains independent.
    ok = expect(ShouldRejectEmptyCommandLine(true), "empty after trim must reject via free gate") && ok;
    ok = expect(ShouldRejectNullChar(true), "null char still rejects under residual") && ok;
    ok = expect(ShouldRejectEmptyCommandName(false), "invalid name still rejects under residual") && ok;
    ok = expect(ShouldAllowCommandPermission(5, 3), "permission still allows under residual") && ok;
    ok = expect(ShouldAuditGMCommand(1, 5), "audit still schedules under residual") && ok;
    ok = expect(!ShouldRejectEmptyCommandLine(false), "non-empty line must proceed even if later gates fail") && ok;
    ok = expect(!ShouldRejectNullChar(false), "non-null must not reject via null-char") && ok;
    ok = expect(!ShouldRejectEmptyCommandName(true), "valid name must not reject via empty-name") && ok;
    // Prior dedicated 3362 residual independence: same identity poles.
    ok = expect(pinShouldRejectEmptyCommandLine3362(true) && !pinShouldRejectEmptyCommandLine3362(false),
                "prior 3362 pin poles must still hold under 3518 independence") &&
         ok;

    return ok;
}
