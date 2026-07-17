#include "test_command_reject_empty_name_2990.h"

#include "map/command_handler_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "command ShouldRejectEmptyCommandName 2990 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CCommandHandler::call empty-name gate for dual-wire cross-check
// (slice 2990):
//   !valid
auto inlineShouldRejectEmptyCommandName(const bool valid) -> bool
{
    return !valid;
}

} // namespace

// Pure dual-wire expansion for commandhandlerhelpers::ShouldRejectEmptyCommandName
// (!valid; slice 2990).
auto runCommandRejectEmptyName2990SelfTests() -> bool
{
    using commandhandlerhelpers::ShouldRejectEmptyCommandLine;
    using commandhandlerhelpers::ShouldRejectEmptyCommandName;
    using commandhandlerhelpers::ShouldRejectNullChar;

    bool ok = true;

    // Residual 2792 truth-table pins.
    ok = expect(ShouldRejectEmptyCommandName(false), "residual: invalid name rejects") && ok;
    ok = expect(!ShouldRejectEmptyCommandName(true), "residual: valid name proceeds") && ok;

    const struct
    {
        bool        valid;
        bool        want;
        const char* label;
    } cases[] = {
        { false, true, "invalid name rejects" },
        { true, false, "valid name proceeds" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRejectEmptyCommandName(c.valid);
        const bool inlineF = inlineShouldRejectEmptyCommandName(c.valid);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldRejectEmptyCommandName dual-wire == inline LSB formula") && ok;
        ok = expect(got == !c.valid, "free == !valid") && ok;
    }

    // Pin composition: negation of valid only.
    ok = expect(ShouldRejectEmptyCommandName(false), "!valid must reject") && ok;
    ok = expect(!ShouldRejectEmptyCommandName(true), "valid must not reject") && ok;

    // Host path: CCommandHandler::call after name-only ParseCommandLine.
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

        ok = expect(got == c.wantReject, c.label) && ok;
        ok = expect(got == inlineF, "host compose dual-wire free == inline") && ok;
        ok = expect(got == !c.valid, "host compose free == !valid") && ok;
    }

    // Dense compose: free == inline == pin formula for both bools.
    for (const bool valid : { false, true })
    {
        const bool got  = ShouldRejectEmptyCommandName(valid);
        const bool want = !valid;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldRejectEmptyCommandName(valid),
                    "compose free == inline") &&
             ok;
    }

    // Residual independence (2792 / 2836 / 2982): empty-name reject is distinct
    // from null-char and empty-line gates.
    ok = expect(ShouldRejectEmptyCommandName(false) && ShouldRejectNullChar(true),
                "invalid name and null char both block via different helpers") &&
         ok;
    ok = expect(ShouldRejectEmptyCommandLine(true),
                "empty command line still rejects under dual-wire residual") &&
         ok;
    ok = expect(!ShouldRejectEmptyCommandName(true),
                "valid name must proceed even if prior null/line would fail") &&
         ok;
    ok = expect(!ShouldRejectNullChar(false),
                "non-null must not reject via null-char gate") &&
         ok;
    ok = expect(!ShouldRejectEmptyCommandLine(false),
                "non-empty line must not reject via empty-line gate") &&
         ok;

    return ok;
}
