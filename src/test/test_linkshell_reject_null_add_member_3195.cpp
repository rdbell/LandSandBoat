#include "test_linkshell_reject_null_add_member_3195.h"

#include "map/linkshell_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "linkshell ShouldRejectNullAddMember 3195 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CLinkshell::AddMember null-char gate for dual-wire cross-check
// (dedicated 3195):
//   charNull
auto inlineShouldRejectNullAddMember(const bool charNull) -> bool
{
    return charNull;
}

// Compact dual-wire pin matching Go pinShouldRejectNullAddMember3195 / C++
// capacity identity form (formula unchanged from 1354 / 2929):
//   charNull
auto pinShouldRejectNullAddMember(const bool charNull) -> bool
{
    return charNull;
}

} // namespace

// Pure dual-wire expansion for linkshellhelpers::ShouldRejectNullAddMember
// (charNull identity; OmegaXI internal/linkshell; dedicated slice 3195;
// residual expand 2929 / pure 1354).
//
// Coverage:
//   - free == inline == pin identity form
//   - poles: null rejects; non-null allows
//   - residual 2929 / 1354 pins still hold
//   - host-style inject + residual independence (2958 / 3079)
auto runLinkshellRejectNullAddMember3195SelfTests() -> bool
{
    using linkshellhelpers::ShouldRejectDuplicateAddMember;
    using linkshellhelpers::ShouldRejectNullAddMember;
    using linkshellhelpers::ShouldRejectNullOnlineMember;

    bool ok = true;

    // Residual 1354 / 2929 pins still hold under dedicated dual-wire.
    ok = expect(ShouldRejectNullAddMember(true), "residual: null char rejects") && ok;
    ok = expect(!ShouldRejectNullAddMember(false), "residual: non-null char allows") && ok;

    // --- Core poles: free == inline == pin identity form ---
    const struct
    {
        bool        charNull;
        bool        want;
        const char* label;
    } cases[] = {
        // Pole: null PChar rejects (early return before duplicate / slot).
        { true, true, "null char rejects" },

        // Pole: non-null PChar allows (null gate passes).
        { false, false, "non-null char allows" },

        // Residual 2929 / 1354 re-pins.
        { true, true, "residual 2929 null rejects" },
        { false, false, "residual 2929 non-null allows" },
        { true, true, "residual 1354 null rejects" },
        { false, false, "residual 1354 non-null allows" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRejectNullAddMember(c.charNull);
        const bool inlineF = inlineShouldRejectNullAddMember(c.charNull);
        const bool pinGot  = pinShouldRejectNullAddMember(c.charNull);
        const bool wantPin = c.charNull;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "dual-wire free==inline") && ok;
        ok = expect(got == pinGot, "dual-wire free==pin") && ok;
        ok = expect(got == wantPin, "dual-wire free==identity pin form") && ok;
    }

    // Free == pin across residual poles.
    ok = expect(ShouldRejectNullAddMember(true) == pinShouldRejectNullAddMember(true), "free==pin null char") && ok;
    ok = expect(ShouldRejectNullAddMember(false) == pinShouldRejectNullAddMember(false), "free==pin non-null char") && ok;

    // Dense compose: full 2^1 boolean space free == inline == pin.
    for (const bool charNull : { false, true })
    {
        const bool got     = ShouldRejectNullAddMember(charNull);
        const bool inlineF = inlineShouldRejectNullAddMember(charNull);
        const bool pinGot  = pinShouldRejectNullAddMember(charNull);
        ok                 = expect(got == inlineF, "compose free==inline") && ok;
        ok                 = expect(got == pinGot, "compose free==pin") && ok;
        ok                 = expect(got == charNull, "compose free==identity") && ok;
    }

    // Host-style inject poles: CLinkshell::AddMember injects PChar == nullptr.
    const struct
    {
        bool        charNull;
        bool        wantReject;
        const char* label;
    } hostCases[] = {
        { true, true, "PChar == nullptr → early return before duplicate" },
        { false, false, "PChar non-null → null gate passes (continue to dup)" },
    };

    for (const auto& c : hostCases)
    {
        const bool got     = ShouldRejectNullAddMember(c.charNull);
        const bool inlineF = inlineShouldRejectNullAddMember(c.charNull);
        const bool pinGot  = pinShouldRejectNullAddMember(c.charNull);

        ok = expect(got == c.wantReject, c.label) && ok;
        ok = expect(got == inlineF, "host compose free==inline") && ok;
        ok = expect(got == pinGot, "host compose free==pin") && ok;
    }

    // Production CLinkshell::AddMember path semantics:
    // null → early return; non-null → continue to duplicate / slot.
    ok = expect(ShouldRejectNullAddMember(true) && pinShouldRejectNullAddMember(true), "AddMember null → early return path") && ok;
    ok = expect(!ShouldRejectNullAddMember(false) && !pinShouldRejectNullAddMember(false), "AddMember non-null → continue path") && ok;

    // Residual independence (1354 / 2929 / 2958 / 3079):
    // AddMember null reject is distinct from duplicate-add and online-null gates.
    ok = expect(ShouldRejectNullAddMember(true), "null char must reject via free gate") && ok;
    ok = expect(ShouldRejectDuplicateAddMember(true), "duplicate still rejects under residual (2958)") && ok;
    ok = expect(ShouldRejectNullOnlineMember(true), "online null still rejects under residual (3079)") && ok;
    ok = expect(!ShouldRejectNullAddMember(false), "non-null must proceed even if later gates reject") && ok;
    ok = expect(!ShouldRejectDuplicateAddMember(false), "not-in-list must not reject via duplicate gate") && ok;
    ok = expect(!ShouldRejectNullOnlineMember(false), "non-null online must not reject via online-null gate") && ok;

    return ok;
}
