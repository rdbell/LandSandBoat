#include "test_command_reject_null_char_2982.h"

#include "map/command_handler_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "command ShouldRejectNullChar 2982 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CCommandHandler::call null-PChar gate for dual-wire cross-check
// (slice 2982):
//   charNull
auto inlineShouldRejectNullChar(const bool charNull) -> bool
{
    return charNull;
}

} // namespace

// Pure dual-wire expansion for commandhandlerhelpers::ShouldRejectNullChar
// (charNull identity; slice 2982).
auto runCommandRejectNullChar2982SelfTests() -> bool
{
    using commandhandlerhelpers::ShouldRejectEmptyCommandLine;
    using commandhandlerhelpers::ShouldRejectEmptyCommandName;
    using commandhandlerhelpers::ShouldRejectNullChar;

    bool ok = true;

    // Residual 2792 truth-table pins.
    ok = expect(ShouldRejectNullChar(true), "residual: null char rejects") && ok;
    ok = expect(!ShouldRejectNullChar(false), "residual: non-null char proceeds") && ok;

    const struct
    {
        bool        charNull;
        bool        want;
        const char* label;
    } cases[] = {
        { true, true, "null char rejects" },
        { false, false, "non-null char proceeds" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRejectNullChar(c.charNull);
        const bool inlineF = inlineShouldRejectNullChar(c.charNull);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldRejectNullChar dual-wire == inline LSB formula") && ok;
        ok = expect(got == c.charNull, "free == charNull (identity)") && ok;
    }

    // Pin composition: identity of charNull only.
    ok = expect(ShouldRejectNullChar(true), "charNull must reject") && ok;
    ok = expect(!ShouldRejectNullChar(false), "!charNull must not reject") && ok;

    // Host path: CCommandHandler::call before name parse.
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

        ok = expect(got == c.wantReject, c.label) && ok;
        ok = expect(got == inlineF, "host compose dual-wire free == inline") && ok;
        ok = expect(got == c.charNull, "host compose free == charNull (identity)") && ok;
    }

    // Dense compose: free == inline == pin formula for both bools.
    for (const bool charNull : { false, true })
    {
        const bool got  = ShouldRejectNullChar(charNull);
        const bool want = charNull;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldRejectNullChar(charNull),
                    "compose free == inline") &&
             ok;
    }

    // Residual independence (2792 / 2836): null reject is distinct from
    // empty-name and empty-line gates.
    ok = expect(ShouldRejectNullChar(true) && ShouldRejectEmptyCommandName(false),
                "null reject and invalid name both block via different helpers") &&
         ok;
    ok = expect(ShouldRejectEmptyCommandLine(true),
                "empty command line still rejects under dual-wire residual") &&
         ok;
    ok = expect(!ShouldRejectNullChar(false),
                "non-null must proceed even if later name/line fails") &&
         ok;
    ok = expect(!ShouldRejectEmptyCommandName(true),
                "valid name must not reject via empty-name gate") &&
         ok;
    ok = expect(!ShouldRejectEmptyCommandLine(false),
                "non-empty line must not reject via empty-line gate") &&
         ok;

    return ok;
}
