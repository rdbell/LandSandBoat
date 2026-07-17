#include "test_linkshell_add_member_after_lookup_3324.h"

#include "map/linkshell_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "linkshell ShouldAddMemberAfterOnlineLookup 3324 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline AddOnlineMember post-lookup AddMember gate for dual-wire
// cross-check (slice 3324):
//   linkshellLoaded
auto inlineShouldAddMemberAfterOnlineLookup3324(const bool linkshellLoaded) -> bool
{
    return linkshellLoaded;
}

// Direct-return dual-wire pin matching free / capacity body (slice 3324).
// Identity form only — free == inline == pin == linkshellLoaded.
// Formula unchanged from residual 3050 / pure 1355 / prior dedicated 3294.
auto pinShouldAddMemberAfterOnlineLookup3324(const bool linkshellLoaded) -> bool
{
    return linkshellLoaded;
}

} // namespace

// Pure dual-wire expansion for linkshellhelpers::ShouldAddMemberAfterOnlineLookup
// (linkshellLoaded identity; dedicated slice 3324; residual expand 3050 /
// pure 1355; prior dedicated expand 3294).
//
// Coverage:
//   - free == inline == pin (direct return identity)
//   - residual 3050 / 1355 / prior dedicated 3294 pins still hold
//   - dense 2¹ free == inline == pin
//   - host inject AddOnlineMember path semantics
auto runLinkshellAddMemberAfterLookup3324SelfTests() -> bool
{
    using linkshellhelpers::FormatOnlineMemberNullWarning;
    using linkshellhelpers::OnlineMemberAlwaysReturnsFalse;
    using linkshellhelpers::ShouldAddMemberAfterOnlineLookup;
    using linkshellhelpers::ShouldLoadLinkshellOnOnlineAdd;
    using linkshellhelpers::ShouldProcessLinkshellItem;
    using linkshellhelpers::ShouldRejectNullOnlineMember;

    bool ok = true;

    // Residual 1355 / 3050 / prior dedicated 3294 pins still hold under dedicated dual-wire.
    ok = expect(ShouldAddMemberAfterOnlineLookup(true), "residual 1355/3050: linkshell loaded → add") && ok;
    ok = expect(!ShouldAddMemberAfterOnlineLookup(false), "residual 1355/3050: linkshell null → no add") && ok;
    ok = expect(ShouldAddMemberAfterOnlineLookup(true), "prior dedicated 3294: linkshell loaded → add") && ok;
    ok = expect(!ShouldAddMemberAfterOnlineLookup(false), "prior dedicated 3294: linkshell null → no add") && ok;

    // Core poles: free == inline == pin (direct return identity).
    const struct
    {
        bool        linkshellLoaded;
        bool        want;
        const char* label;
    } cases[] = {
        // Dense 2¹ dual poles.
        { false, false, "linkshell null skips AddMember" },
        { true, true, "linkshell loaded calls AddMember" },

        // Residual 3050 / 1355 re-pins.
        { true, true, "residual 3050 add when loaded" },
        { false, false, "residual 3050 no add when null" },
        { true, true, "residual 1355 add when loaded" },
        { false, false, "residual 1355 no add when null" },

        // Prior dedicated 3294 re-pins.
        { true, true, "prior dedicated 3294 add when loaded" },
        { false, false, "prior dedicated 3294 no add when null" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldAddMemberAfterOnlineLookup(c.linkshellLoaded);
        const bool inlineF = inlineShouldAddMemberAfterOnlineLookup3324(c.linkshellLoaded);
        const bool pin     = pinShouldAddMemberAfterOnlineLookup3324(c.linkshellLoaded);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldAddMemberAfterOnlineLookup dual-wire == inline LSB formula") && ok;
        ok = expect(got == pin, "ShouldAddMemberAfterOnlineLookup == direct return pin") && ok;
    }

    // Free == pin across residual poles (direct return pin form).
    ok = expect(ShouldAddMemberAfterOnlineLookup(true) == pinShouldAddMemberAfterOnlineLookup3324(true),
                "free == pin residual loaded add") &&
         ok;
    ok = expect(ShouldAddMemberAfterOnlineLookup(false) == pinShouldAddMemberAfterOnlineLookup3324(false),
                "free == pin residual null skip") &&
         ok;

    // Dense compose: full 2¹ boolean space free == inline == pin.
    for (const bool linkshellLoaded : { false, true })
    {
        const bool got     = ShouldAddMemberAfterOnlineLookup(linkshellLoaded);
        const bool inlineF = inlineShouldAddMemberAfterOnlineLookup3324(linkshellLoaded);
        const bool pin     = pinShouldAddMemberAfterOnlineLookup3324(linkshellLoaded);
        ok                 = expect(got == pin, "compose free == direct return pin") && ok;
        ok                 = expect(got == inlineF, "compose free == inline") && ok;
        ok                 = expect(got == linkshellLoaded, "compose free == linkshellLoaded identity") && ok;
    }

    // Host-style compose: AddOnlineMember injects PLinkshell != nullptr.
    const struct
    {
        bool        linkshellLoaded;
        bool        wantAdd;
        const char* label;
    } hosts[] = {
        { true, true, "PLinkshell non-null → AddMember" },
        { false, false, "PLinkshell null → skip AddMember" },
    };
    for (const auto& h : hosts)
    {
        const bool add     = ShouldAddMemberAfterOnlineLookup(h.linkshellLoaded);
        const bool inlineF = inlineShouldAddMemberAfterOnlineLookup3324(h.linkshellLoaded);
        const bool pin     = pinShouldAddMemberAfterOnlineLookup3324(h.linkshellLoaded);
        ok                 = expect(add == h.wantAdd, h.label) && ok;
        ok                 = expect(add == inlineF && add == pin, "host compose free == inline == pin") && ok;
        if (add)
        {
            ok = expect(!OnlineMemberAlwaysReturnsFalse(),
                        "compose: AddOnlineMember still always returns false after AddMember") &&
                 ok;
        }
    }

    // Sibling dual-wire gates remain orthogonal (3055 / 3079 / 3099 leave alone).
    ok = expect(ShouldLoadLinkshellOnOnlineAdd(false), "sibling residual: load on miss") && ok;
    ok = expect(!ShouldLoadLinkshellOnOnlineAdd(true), "sibling residual: no load on hit") && ok;
    ok = expect(ShouldRejectNullOnlineMember(true), "sibling residual: null char rejects") && ok;
    ok = expect(!ShouldRejectNullOnlineMember(false), "sibling residual: non-null proceeds") && ok;
    ok = expect(ShouldProcessLinkshellItem(true, true), "sibling residual: process ls item") && ok;
    ok = expect(!ShouldProcessLinkshellItem(false, true), "sibling residual: null item no process") && ok;
    ok = expect(!OnlineMemberAlwaysReturnsFalse(), "sibling residual: always returns false") && ok;
    ok = expect(FormatOnlineMemberNullWarning() == "PChar is null.", "sibling residual: null warn") && ok;
    ok = expect(ShouldAddMemberAfterOnlineLookup(true),
                "loaded must add even if earlier load was cache reuse") &&
         ok;
    ok = expect(!ShouldAddMemberAfterOnlineLookup(false),
                "null must skip add even if earlier null reject admitted") &&
         ok;

    return ok;
}
