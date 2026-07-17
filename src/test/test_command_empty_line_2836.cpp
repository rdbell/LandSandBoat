#include "test_command_empty_line_2836.h"

#include "map/command_handler_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "command empty line 2836 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runCommandEmptyLine2836SelfTests() -> bool
{
    using commandhandlerhelpers::ShouldRejectEmptyCommandLine;
    using commandhandlerhelpers::ShouldRejectEmptyCommandName;

    bool ok = true;

    // Identity pure: post-trim empty flag passes through unchanged.
    ok = expect(ShouldRejectEmptyCommandLine(true), "empty after trim reject") && ok;
    ok = expect(!ShouldRejectEmptyCommandLine(false), "non-empty after trim allow") && ok;

    // Compose with empty-name call gate: empty line → valid false → reject name.
    // (ParseCommandLine returns {} with valid=false when empty-line gate fires.)
    ok = expect(ShouldRejectEmptyCommandName(false), "invalid name from empty line") && ok;
    ok = expect(!ShouldRejectEmptyCommandName(true), "valid name allow") && ok;

    // When host would inject view.empty() after trimLeft of whitespace-only
    // input, pure gate still rejects (same as literal empty string).
    const bool whitespaceOnlyEmpty = true; // "   " / "\t\t" / " \t " after trim
    ok = expect(ShouldRejectEmptyCommandLine(whitespaceOnlyEmpty), "whitespace-only reject") && ok;

    return ok;
}
