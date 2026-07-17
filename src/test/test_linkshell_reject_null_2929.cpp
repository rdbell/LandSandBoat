#include "test_linkshell_reject_null_2929.h"

#include "map/linkshell_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "linkshell ShouldRejectNullAddMember 2929 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline AddMember null-char gate for dual-wire cross-check (slice 2929):
//   charNull
auto inlineShouldRejectNullAddMember(const bool charNull) -> bool
{
    return charNull;
}

} // namespace

// Pure dual-wire expansion for linkshellhelpers::ShouldRejectNullAddMember
// (PChar == nullptr identity; slice 2929).
auto runLinkshellRejectNull2929SelfTests() -> bool
{
    using linkshellhelpers::ShouldRejectDuplicateAddMember;
    using linkshellhelpers::ShouldRejectNullAddMember;

    bool ok = true;

    // Residual 1354 pins still hold under dual-wire.
    ok = expect(ShouldRejectNullAddMember(true), "residual null rejects") && ok;
    ok = expect(!ShouldRejectNullAddMember(false), "residual non-null allows") && ok;

    const struct
    {
        bool        charNull;
        bool        want;
        const char* label;
    } cases[] = {
        { true, true, "null char rejects" },
        { false, false, "non-null char allows" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRejectNullAddMember(c.charNull);
        const bool inlineF = inlineShouldRejectNullAddMember(c.charNull);
        const bool wantPin = c.charNull;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldRejectNullAddMember dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldRejectNullAddMember == pin formula") && ok;
    }

    // Pin composition: identity of charNull.
    ok = expect(ShouldRejectNullAddMember(true), "charNull true must reject") && ok;
    ok = expect(!ShouldRejectNullAddMember(false), "charNull false must not reject") && ok;

    // Dense compose: full boolean space free == inline == pin.
    for (const bool charNull : { false, true })
    {
        const bool got  = ShouldRejectNullAddMember(charNull);
        const bool want = charNull;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldRejectNullAddMember(charNull), "compose free == inline") && ok;
    }

    // Host inject compose: production AddMember evaluates null before duplicate.
    // When null rejects, residual duplicate is not observed by the host path.
    ok = expect(ShouldRejectNullAddMember(true), "host null short-circuit") && ok;
    ok = expect(!ShouldRejectNullAddMember(false), "host non-null continues") && ok;
    // Residual sibling still identity when null gate passes (not dual-wired here).
    ok = expect(ShouldRejectDuplicateAddMember(true), "residual dup rejects when reached") && ok;
    ok = expect(!ShouldRejectDuplicateAddMember(false), "residual dup allows when reached") && ok;

    // Explicit dual-wire: free function is identity of host (PChar == nullptr).
    for (const bool pCharIsNil : { false, true })
    {
        ok = expect(ShouldRejectNullAddMember(pCharIsNil) == pCharIsNil, "host inject identity") && ok;
        ok = expect(ShouldRejectNullAddMember(pCharIsNil) == inlineShouldRejectNullAddMember(pCharIsNil),
                    "host inject dual-wire free == inline") &&
             ok;
    }

    return ok;
}
