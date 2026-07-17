#include "test_command_reject_empty_line_3005.h"

#include "map/command_handler_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "command ShouldRejectEmptyCommandLine 3005 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline ParseCommandLine empty-line gate for dual-wire cross-check
// (slice 3005):
//   viewEmptyAfterTrim
auto inlineShouldRejectEmptyCommandLine(const bool viewEmptyAfterTrim) -> bool
{
    return viewEmptyAfterTrim;
}

} // namespace

// Pure dual-wire expansion for commandhandlerhelpers::ShouldRejectEmptyCommandLine
// (viewEmptyAfterTrim identity; slice 3005).
auto runCommandRejectEmptyLine3005SelfTests() -> bool
{
    using commandhandlerhelpers::ShouldRejectEmptyCommandLine;
    using commandhandlerhelpers::ShouldRejectEmptyCommandName;
    using commandhandlerhelpers::ShouldRejectNullChar;

    bool ok = true;

    // Residual 2836 truth-table pins.
    ok = expect(ShouldRejectEmptyCommandLine(true), "residual: empty after trim rejects") && ok;
    ok = expect(!ShouldRejectEmptyCommandLine(false), "residual: non-empty after trim proceeds") && ok;

    const struct
    {
        bool        viewEmptyAfterTrim;
        bool        want;
        const char* label;
    } cases[] = {
        { true, true, "empty after trim rejects" },
        { false, false, "non-empty after trim proceeds" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRejectEmptyCommandLine(c.viewEmptyAfterTrim);
        const bool inlineF = inlineShouldRejectEmptyCommandLine(c.viewEmptyAfterTrim);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldRejectEmptyCommandLine dual-wire == inline LSB formula") && ok;
        ok = expect(got == c.viewEmptyAfterTrim, "free == viewEmptyAfterTrim (identity)") && ok;
    }

    // Pin composition: identity of viewEmptyAfterTrim only.
    ok = expect(ShouldRejectEmptyCommandLine(true), "viewEmptyAfterTrim must reject") && ok;
    ok = expect(!ShouldRejectEmptyCommandLine(false), "!viewEmptyAfterTrim must not reject") && ok;

    // Host path: ParseCommandLine after trimLeft.
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

        ok = expect(got == c.wantReject, c.label) && ok;
        ok = expect(got == inlineF, "host compose dual-wire free == inline") && ok;
        ok = expect(got == c.viewEmptyAfterTrim, "host compose free == viewEmptyAfterTrim (identity)") && ok;
    }

    // Dense compose: free == inline == pin formula for both bools.
    for (const bool viewEmptyAfterTrim : { false, true })
    {
        const bool got  = ShouldRejectEmptyCommandLine(viewEmptyAfterTrim);
        const bool want = viewEmptyAfterTrim;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldRejectEmptyCommandLine(viewEmptyAfterTrim),
                    "compose free == inline") &&
             ok;
    }

    // When host would inject view.empty() after trimLeft of whitespace-only
    // input, pure gate still rejects (same as literal empty string).
    const bool whitespaceOnlyEmpty = true; // "   " / "\t\t" / " \t " after trim
    ok = expect(ShouldRejectEmptyCommandLine(whitespaceOnlyEmpty), "whitespace-only reject") && ok;
    ok = expect(inlineShouldRejectEmptyCommandLine(whitespaceOnlyEmpty),
                "whitespace-only inline reject") &&
         ok;

    // Residual independence (2836 / 2792 / 2982 / 2990): empty-line reject is
    // distinct from null-char and empty-name gates.
    ok = expect(ShouldRejectEmptyCommandLine(true) && ShouldRejectNullChar(true),
                "empty line and null char both block via different helpers") &&
         ok;
    ok = expect(ShouldRejectEmptyCommandName(false),
                "invalid name still rejects under dual-wire residual") &&
         ok;
    ok = expect(!ShouldRejectEmptyCommandLine(false),
                "non-empty line must proceed even if later null/name fails") &&
         ok;
    ok = expect(!ShouldRejectNullChar(false),
                "non-null must not reject via null-char gate") &&
         ok;
    ok = expect(!ShouldRejectEmptyCommandName(true),
                "valid name must not reject via empty-name gate") &&
         ok;

    return ok;
}
