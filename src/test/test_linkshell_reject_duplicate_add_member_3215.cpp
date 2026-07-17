#include "test_linkshell_reject_duplicate_add_member_3215.h"

#include "map/linkshell_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "linkshell ShouldRejectDuplicateAddMember 3215 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CLinkshell::AddMember already-in-list gate for dual-wire cross-check
// (dedicated 3215):
//   alreadyInList
auto inlineShouldRejectDuplicateAddMember(const bool alreadyInList) -> bool
{
    return alreadyInList;
}

// Compact dual-wire pin matching Go pinShouldRejectDuplicateAddMember3215 / C++
// capacity identity form (formula unchanged from 1354 / 2958):
//   alreadyInList
auto pinShouldRejectDuplicateAddMember(const bool alreadyInList) -> bool
{
    return alreadyInList;
}

} // namespace

// Pure dual-wire expansion for linkshellhelpers::ShouldRejectDuplicateAddMember
// (alreadyInList identity; OmegaXI internal/linkshell; dedicated slice 3215;
// residual expand 2958 / pure 1354).
//
// Coverage:
//   - free == inline == pin identity form
//   - poles: already-in-list rejects; not-in-list allows
//   - residual 2958 / 1354 pins still hold
//   - host-style inject + residual independence (3195 / 3079)
auto runLinkshellRejectDuplicateAddMember3215SelfTests() -> bool
{
    using linkshellhelpers::FormatAddMemberAlreadyWarning;
    using linkshellhelpers::ShouldRejectDuplicateAddMember;
    using linkshellhelpers::ShouldRejectNullAddMember;
    using linkshellhelpers::ShouldRejectNullOnlineMember;

    bool ok = true;

    // Residual 1354 / 2958 pins still hold under dedicated dual-wire.
    ok = expect(ShouldRejectDuplicateAddMember(true), "residual: already in list rejects") && ok;
    ok = expect(!ShouldRejectDuplicateAddMember(false), "residual: not in list allows") && ok;

    // --- Core poles: free == inline == pin identity form ---
    const struct
    {
        bool        alreadyInList;
        bool        want;
        const char* label;
    } cases[] = {
        // Pole: already in online members rejects (warn + early return).
        { true, true, "already in list rejects" },

        // Pole: not in list allows (duplicate gate passes).
        { false, false, "not in list allows" },

        // Residual 2958 / 1354 re-pins.
        { true, true, "residual 2958 already-in-list rejects" },
        { false, false, "residual 2958 not-in-list allows" },
        { true, true, "residual 1354 already-in-list rejects" },
        { false, false, "residual 1354 not-in-list allows" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRejectDuplicateAddMember(c.alreadyInList);
        const bool inlineF = inlineShouldRejectDuplicateAddMember(c.alreadyInList);
        const bool pinGot  = pinShouldRejectDuplicateAddMember(c.alreadyInList);
        const bool wantPin = c.alreadyInList;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "dual-wire free==inline") && ok;
        ok = expect(got == pinGot, "dual-wire free==pin") && ok;
        ok = expect(got == wantPin, "dual-wire free==identity pin form") && ok;
    }

    // Free == pin across residual poles.
    ok = expect(ShouldRejectDuplicateAddMember(true) == pinShouldRejectDuplicateAddMember(true),
                "free==pin already-in-list") &&
         ok;
    ok = expect(ShouldRejectDuplicateAddMember(false) == pinShouldRejectDuplicateAddMember(false),
                "free==pin not-in-list") &&
         ok;

    // Dense compose: full 2^1 boolean space free == inline == pin.
    for (const bool alreadyInList : { false, true })
    {
        const bool got     = ShouldRejectDuplicateAddMember(alreadyInList);
        const bool inlineF = inlineShouldRejectDuplicateAddMember(alreadyInList);
        const bool pinGot  = pinShouldRejectDuplicateAddMember(alreadyInList);
        ok                 = expect(got == inlineF, "compose free==inline") && ok;
        ok                 = expect(got == pinGot, "compose free==pin") && ok;
        ok                 = expect(got == alreadyInList, "compose free==identity") && ok;
    }

    // Host-style inject poles: CLinkshell::AddMember injects find-hit.
    const struct
    {
        bool        alreadyInList;
        bool        wantReject;
        const char* label;
    } hostCases[] = {
        { true, true, "find hit → warn + early return before slot/DB" },
        { false, false, "find miss → duplicate gate passes (continue to slot)" },
    };

    for (const auto& c : hostCases)
    {
        const bool got     = ShouldRejectDuplicateAddMember(c.alreadyInList);
        const bool inlineF = inlineShouldRejectDuplicateAddMember(c.alreadyInList);
        const bool pinGot  = pinShouldRejectDuplicateAddMember(c.alreadyInList);

        ok = expect(got == c.wantReject, c.label) && ok;
        ok = expect(got == inlineF, "host compose free==inline") && ok;
        ok = expect(got == pinGot, "host compose free==pin") && ok;
    }

    // Production CLinkshell::AddMember path semantics:
    // already-in-list → early return; not-in-list → continue to slot / DB.
    ok = expect(ShouldRejectDuplicateAddMember(true) && pinShouldRejectDuplicateAddMember(true),
                "AddMember already-in-list → early return path") &&
         ok;
    ok = expect(!ShouldRejectDuplicateAddMember(false) && !pinShouldRejectDuplicateAddMember(false),
                "AddMember not-in-list → continue path") &&
         ok;

    // Residual independence (1354 / 2958 / 3195 / 3079):
    // AddMember duplicate reject is distinct from null-add and online-null gates.
    // Sibling ShouldRejectNullAddMember (3195) is left alone; only independence pins.
    ok = expect(ShouldRejectDuplicateAddMember(true), "already-in-list must reject via free gate") && ok;
    ok = expect(ShouldRejectNullAddMember(true), "null still rejects under residual (3195)") && ok;
    ok = expect(ShouldRejectNullOnlineMember(true), "online null still rejects under residual (3079)") && ok;
    ok = expect(!ShouldRejectDuplicateAddMember(false), "not-in-list must proceed even if other gates reject") && ok;
    ok = expect(!ShouldRejectNullAddMember(false), "non-null must not reject via null-add gate") && ok;
    ok = expect(!ShouldRejectNullOnlineMember(false), "non-null online must not reject via online-null gate") && ok;

    // Residual warning text shape used when host rejects on duplicate (1354).
    ok = expect(FormatAddMemberAlreadyWarning("Alice") ==
                    "CLinkshell::AddMember attempted to add member 'Alice' who is already in the online member list.",
                "residual FormatAddMemberAlreadyWarning shape") &&
         ok;

    return ok;
}
