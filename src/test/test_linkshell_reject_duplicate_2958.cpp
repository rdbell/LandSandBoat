#include "test_linkshell_reject_duplicate_2958.h"

#include "map/linkshell_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "linkshell ShouldRejectDuplicateAddMember 2958 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline AddMember already-in-list gate for dual-wire cross-check (slice 2958):
//   alreadyInList
auto inlineShouldRejectDuplicateAddMember(const bool alreadyInList) -> bool
{
    return alreadyInList;
}

} // namespace

// Pure dual-wire expansion for linkshellhelpers::ShouldRejectDuplicateAddMember
// (find-hit identity; slice 2958).
auto runLinkshellRejectDuplicate2958SelfTests() -> bool
{
    using linkshellhelpers::FormatAddMemberAlreadyWarning;
    using linkshellhelpers::ShouldRejectDuplicateAddMember;
    using linkshellhelpers::ShouldRejectNullAddMember;

    bool ok = true;

    // Residual 1354 pins still hold under dual-wire.
    ok = expect(ShouldRejectDuplicateAddMember(true), "residual already-in-list rejects") && ok;
    ok = expect(!ShouldRejectDuplicateAddMember(false), "residual not-in-list allows") && ok;

    const struct
    {
        bool        alreadyInList;
        bool        want;
        const char* label;
    } cases[] = {
        { true, true, "already in list rejects" },
        { false, false, "not in list allows" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRejectDuplicateAddMember(c.alreadyInList);
        const bool inlineF = inlineShouldRejectDuplicateAddMember(c.alreadyInList);
        const bool wantPin = c.alreadyInList;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldRejectDuplicateAddMember dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldRejectDuplicateAddMember == pin formula") && ok;
    }

    // Pin composition: identity of alreadyInList.
    ok = expect(ShouldRejectDuplicateAddMember(true), "alreadyInList true must reject") && ok;
    ok = expect(!ShouldRejectDuplicateAddMember(false), "alreadyInList false must not reject") && ok;

    // Dense compose: full boolean space free == inline == pin.
    for (const bool alreadyInList : { false, true })
    {
        const bool got  = ShouldRejectDuplicateAddMember(alreadyInList);
        const bool want = alreadyInList;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldRejectDuplicateAddMember(alreadyInList), "compose free == inline") && ok;
    }

    // Host inject compose: production AddMember evaluates null before duplicate.
    // When null rejects, residual duplicate is not observed by the host path.
    ok = expect(ShouldRejectNullAddMember(true), "host null short-circuit (sibling 2929)") && ok;
    ok = expect(!ShouldRejectNullAddMember(false), "host non-null continues to dup") && ok;
    // Duplicate gate when null passes.
    ok = expect(ShouldRejectDuplicateAddMember(true), "dup rejects when reached") && ok;
    ok = expect(!ShouldRejectDuplicateAddMember(false), "dup allows when reached") && ok;

    // Explicit dual-wire: free function is identity of host find-hit inject.
    for (const bool alreadyInList : { false, true })
    {
        ok = expect(ShouldRejectDuplicateAddMember(alreadyInList) == alreadyInList, "host inject identity") && ok;
        ok = expect(ShouldRejectDuplicateAddMember(alreadyInList) == inlineShouldRejectDuplicateAddMember(alreadyInList),
                    "host inject dual-wire free == inline") &&
             ok;
    }

    // Residual warning text shape used when host rejects on duplicate.
    ok = expect(FormatAddMemberAlreadyWarning("Alice") ==
                    "CLinkshell::AddMember attempted to add member 'Alice' who is already in the online member list.",
                "residual FormatAddMemberAlreadyWarning shape") &&
         ok;

    return ok;
}
