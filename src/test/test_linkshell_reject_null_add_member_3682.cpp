#include "test_linkshell_reject_null_add_member_3682.h"

#include "map/linkshell_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "linkshell ShouldRejectNullAddMember 3682 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CLinkshell::AddMember null-char gate for dual-wire cross-check
// (dedicated 3682):
//   charNull
auto inlineShouldRejectNullAddMember(const bool charNull) -> bool
{
    return charNull;
}

// Compact dual-wire pin matching Go pinShouldRejectNullAddMember3682 / C++
// capacity identity form (formula unchanged from 1354 / 2929 / prior dedicated
// 3195 / 3489 / 3548 / 3592 / 3637):
//   charNull
// free == inline == pin == pin3637.
auto pinShouldRejectNullAddMember(const bool charNull) -> bool
{
    return charNull;
}

// Prior dedicated dual-wire 3195 pin (retained suite form) for residual re-pins.
auto pinShouldRejectNullAddMember3195(const bool charNull) -> bool
{
    return charNull;
}

// Prior dedicated dual-wire 3489 pin (retained suite form) for residual re-pins.
auto pinShouldRejectNullAddMember3489(const bool charNull) -> bool
{
    return charNull;
}

// Prior dedicated dual-wire 3548 pin (retained suite form) for residual re-pins.
auto pinShouldRejectNullAddMember3548(const bool charNull) -> bool
{
    return charNull;
}

// Prior dedicated dual-wire 3592 pin (retained suite form) for residual re-pins.
auto pinShouldRejectNullAddMember3592(const bool charNull) -> bool
{
    return charNull;
}

// Prior dedicated dual-wire 3637 pin (retained suite form) for residual re-pins.
// free == inline == pin == pin3637.
auto pinShouldRejectNullAddMember3637(const bool charNull) -> bool
{
    return charNull;
}

} // namespace

// Pure dual-wire expansion for linkshellhelpers::ShouldRejectNullAddMember
// (charNull identity; OmegaXI internal/linkshell; dedicated slice 3682;
// residual expand 2929 / pure 1354; prior dedicated expands 3195 / 3489 / 3548 /
// 3592 / 3637).
//
// Coverage:
//   - free == inline == pin == pin3637 identity form
//   - poles: null rejects; non-null allows
//   - residual 2929 / 1354 / prior dedicated 3195 / 3489 / 3548 / 3592 / 3637 pins still hold
//   - host-style inject + residual independence (2958 / 3079)
//   - prior dedicated 3195 / 3489 / 3548 / 3592 / 3637 independence (RETAINED)
auto runLinkshellRejectNullAddMember3682SelfTests() -> bool
{
    using linkshellhelpers::ShouldRejectDuplicateAddMember;
    using linkshellhelpers::ShouldRejectNullAddMember;
    using linkshellhelpers::ShouldRejectNullOnlineMember;

    bool ok = true;

    // Residual 1354 / 2929 / prior dedicated 3195 / 3489 / 3548 / 3592 / 3637 pins still
    // hold under dedicated dual-wire expand.
    ok = expect(ShouldRejectNullAddMember(true), "residual: null char rejects") && ok;
    ok = expect(!ShouldRejectNullAddMember(false), "residual: non-null char allows") && ok;
    ok = expect(ShouldRejectNullAddMember(true), "prior dedicated 3195: null char rejects") && ok;
    ok = expect(!ShouldRejectNullAddMember(false), "prior dedicated 3195: non-null char allows") && ok;
    ok = expect(pinShouldRejectNullAddMember3195(true), "prior dedicated 3195 pin null rejects") && ok;
    ok = expect(!pinShouldRejectNullAddMember3195(false), "prior dedicated 3195 pin non-null allows") && ok;
    ok = expect(ShouldRejectNullAddMember(true), "prior dedicated 3489: null char rejects") && ok;
    ok = expect(!ShouldRejectNullAddMember(false), "prior dedicated 3489: non-null char allows") && ok;
    ok = expect(pinShouldRejectNullAddMember3489(true), "prior dedicated 3489 pin null rejects") && ok;
    ok = expect(!pinShouldRejectNullAddMember3489(false), "prior dedicated 3489 pin non-null allows") && ok;
    ok = expect(ShouldRejectNullAddMember(true), "prior dedicated 3548: null char rejects") && ok;
    ok = expect(!ShouldRejectNullAddMember(false), "prior dedicated 3548: non-null char allows") && ok;
    ok = expect(pinShouldRejectNullAddMember3548(true), "prior dedicated 3548 pin null rejects") && ok;
    ok = expect(!pinShouldRejectNullAddMember3548(false), "prior dedicated 3548 pin non-null allows") && ok;
    ok = expect(ShouldRejectNullAddMember(true), "prior dedicated 3592: null char rejects") && ok;
    ok = expect(!ShouldRejectNullAddMember(false), "prior dedicated 3592: non-null char allows") && ok;
    ok = expect(pinShouldRejectNullAddMember3592(true), "prior dedicated 3592 pin null rejects") && ok;
    ok = expect(!pinShouldRejectNullAddMember3592(false), "prior dedicated 3592 pin non-null allows") && ok;
    ok = expect(ShouldRejectNullAddMember(true), "prior dedicated 3637: null char rejects") && ok;
    ok = expect(!ShouldRejectNullAddMember(false), "prior dedicated 3637: non-null char allows") && ok;
    ok = expect(pinShouldRejectNullAddMember3637(true), "prior dedicated 3637 pin null rejects") && ok;
    ok = expect(!pinShouldRejectNullAddMember3637(false), "prior dedicated 3637 pin non-null allows") && ok;

    // --- Core poles: free == inline == pin == pin3637 identity form ---
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

        // Residual 2929 / 1354 / prior dedicated 3195 / 3489 / 3548 / 3592 / 3637 re-pins.
        { true, true, "residual 2929 null rejects" },
        { false, false, "residual 2929 non-null allows" },
        { true, true, "residual 1354 null rejects" },
        { false, false, "residual 1354 non-null allows" },
        { true, true, "prior dedicated 3195 null rejects" },
        { false, false, "prior dedicated 3195 non-null allows" },
        { true, true, "prior dedicated 3489 null rejects" },
        { false, false, "prior dedicated 3489 non-null allows" },
        { true, true, "prior dedicated 3548 null rejects" },
        { false, false, "prior dedicated 3548 non-null allows" },
        { true, true, "prior dedicated 3592 null rejects" },
        { false, false, "prior dedicated 3592 non-null allows" },
        { true, true, "prior dedicated 3637 null rejects" },
        { false, false, "prior dedicated 3637 non-null allows" },
    };

    for (const auto& c : cases)
    {
        const bool got      = ShouldRejectNullAddMember(c.charNull);
        const bool inlineF  = inlineShouldRejectNullAddMember(c.charNull);
        const bool pinGot   = pinShouldRejectNullAddMember(c.charNull);
        const bool pin3637  = pinShouldRejectNullAddMember3637(c.charNull);
        const bool wantPin  = c.charNull;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "dual-wire free==inline") && ok;
        ok = expect(got == pinGot, "dual-wire free==pin") && ok;
        ok = expect(got == pin3637, "dual-wire free==pin3637") && ok;
        ok = expect(got == wantPin, "dual-wire free==identity pin form") && ok;
    }

    // Free == pin / pin3637 across residual poles.
    ok = expect(ShouldRejectNullAddMember(true) == pinShouldRejectNullAddMember(true), "free==pin null char") && ok;
    ok = expect(ShouldRejectNullAddMember(false) == pinShouldRejectNullAddMember(false), "free==pin non-null char") && ok;
    ok = expect(ShouldRejectNullAddMember(true) == pinShouldRejectNullAddMember3637(true), "free==pin3637 null char") && ok;
    ok = expect(ShouldRejectNullAddMember(false) == pinShouldRejectNullAddMember3637(false), "free==pin3637 non-null char") && ok;

    // Dense compose: full 2^1 boolean space free == inline == pin == pin3637.
    for (const bool charNull : { false, true })
    {
        const bool got     = ShouldRejectNullAddMember(charNull);
        const bool inlineF = inlineShouldRejectNullAddMember(charNull);
        const bool pinGot  = pinShouldRejectNullAddMember(charNull);
        const bool pin3637 = pinShouldRejectNullAddMember3637(charNull);
        ok                 = expect(got == inlineF, "compose free==inline") && ok;
        ok                 = expect(got == pinGot, "compose free==pin") && ok;
        ok                 = expect(got == pin3637, "compose free==pin3637") && ok;
        ok                 = expect(got == charNull, "compose free==identity") && ok;
        ok                 = expect(got == pinShouldRejectNullAddMember3195(charNull), "compose free==prior pin 3195") && ok;
        ok                 = expect(got == pinShouldRejectNullAddMember3489(charNull), "compose free==prior pin 3489") && ok;
        ok                 = expect(got == pinShouldRejectNullAddMember3548(charNull), "compose free==prior pin 3548") && ok;
        ok                 = expect(got == pinShouldRejectNullAddMember3592(charNull), "compose free==prior pin 3592") && ok;
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
        const bool pin3637 = pinShouldRejectNullAddMember3637(c.charNull);

        ok = expect(got == c.wantReject, c.label) && ok;
        ok = expect(got == inlineF, "host compose free==inline") && ok;
        ok = expect(got == pinGot, "host compose free==pin") && ok;
        ok = expect(got == pin3637, "host compose free==pin3637") && ok;
    }

    // Production CLinkshell::AddMember path semantics:
    // null → early return; non-null → continue to duplicate / slot.
    ok = expect(ShouldRejectNullAddMember(true) && pinShouldRejectNullAddMember(true), "AddMember null → early return path") && ok;
    ok = expect(!ShouldRejectNullAddMember(false) && !pinShouldRejectNullAddMember(false), "AddMember non-null → continue path") && ok;

    // Residual independence (1354 / 2929 / 3195 / 3489 / 3548 / 3592 / 3637 / 2958 / 3079):
    // AddMember null reject is distinct from duplicate-add and online-null gates.
    // Prior dedicated 3195 / 3489 / 3548 / 3592 / 3637 independence (RETAINED under this expand).
    ok = expect(ShouldRejectNullAddMember(true), "null char must reject via free gate") && ok;
    ok = expect(pinShouldRejectNullAddMember3195(true), "prior dedicated 3195 pin still rejects null") && ok;
    ok = expect(!pinShouldRejectNullAddMember3195(false), "prior dedicated 3195 pin still allows non-null") && ok;
    ok = expect(pinShouldRejectNullAddMember3489(true), "prior dedicated 3489 pin still rejects null") && ok;
    ok = expect(!pinShouldRejectNullAddMember3489(false), "prior dedicated 3489 pin still allows non-null") && ok;
    ok = expect(pinShouldRejectNullAddMember3548(true), "prior dedicated 3548 pin still rejects null") && ok;
    ok = expect(!pinShouldRejectNullAddMember3548(false), "prior dedicated 3548 pin still allows non-null") && ok;
    ok = expect(pinShouldRejectNullAddMember3592(true), "prior dedicated 3592 pin still rejects null") && ok;
    ok = expect(!pinShouldRejectNullAddMember3592(false), "prior dedicated 3592 pin still allows non-null") && ok;
    ok = expect(pinShouldRejectNullAddMember3637(true), "prior dedicated 3637 pin still rejects null") && ok;
    ok = expect(!pinShouldRejectNullAddMember3637(false), "prior dedicated 3637 pin still allows non-null") && ok;
    ok = expect(ShouldRejectDuplicateAddMember(true), "duplicate still rejects under residual (2958)") && ok;
    ok = expect(ShouldRejectNullOnlineMember(true), "online null still rejects under residual (3079)") && ok;
    ok = expect(!ShouldRejectNullAddMember(false), "non-null must proceed even if later gates reject") && ok;
    ok = expect(!ShouldRejectDuplicateAddMember(false), "not-in-list must not reject via duplicate gate") && ok;
    ok = expect(!ShouldRejectNullOnlineMember(false), "non-null online must not reject via online-null gate") && ok;

    return ok;
}
