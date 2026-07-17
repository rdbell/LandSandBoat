#include "test_unitychat_add_member_after_lookup_3254.h"

#include "map/unitychat_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "unitychat ShouldAddMemberAfterOnlineLookup 3254 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline AddOnlineMember post-lookup AddMember gate for dual-wire
// cross-check (slice 3254):
//   unityLoaded
auto inlineShouldAddMemberAfterOnlineLookup3254(const bool unityLoaded) -> bool
{
    return unityLoaded;
}

// Direct-return dual-wire pin matching free / capacity body (slice 3254).
// Identity form only — free == inline == pin == unityLoaded.
auto pinShouldAddMemberAfterOnlineLookup3254(const bool unityLoaded) -> bool
{
    return unityLoaded;
}

} // namespace

// Pure dual-wire expansion for unitychathelpers::ShouldAddMemberAfterOnlineLookup
// (unityLoaded identity; dedicated slice 3254; residual expand 3096 / pure 1356).
//
// Coverage:
//   - free == inline == pin (direct return identity)
//   - residual 3096 / 1356 pins still hold
//   - dense 2¹ free == inline == pin
//   - host inject AddOnlineMember path semantics
auto runUnitychatAddMemberAfterLookup3254SelfTests() -> bool
{
    using unitychathelpers::FormatOnlineMemberNullWarning;
    using unitychathelpers::OnlineMemberAlwaysReturnsFalse;
    using unitychathelpers::ShouldAddMemberAfterOnlineLookup;
    using unitychathelpers::ShouldLoadUnityChatOnOnlineAdd;
    using unitychathelpers::ShouldRejectNullOnlineMember;

    bool ok = true;

    // Residual 1356 / 3096 pins still hold under dedicated dual-wire.
    ok = expect(ShouldAddMemberAfterOnlineLookup(true), "residual 1356/3096: unity loaded → add") && ok;
    ok = expect(!ShouldAddMemberAfterOnlineLookup(false), "residual 1356/3096: unity null → no add") && ok;

    // Core poles: free == inline == pin (direct return identity).
    const struct
    {
        bool        unityLoaded;
        bool        want;
        const char* label;
    } cases[] = {
        // Dense 2¹ dual poles.
        { false, false, "unity null skips AddMember" },
        { true, true, "unity loaded calls AddMember" },

        // Residual 3096 / 1356 re-pins.
        { true, true, "residual 3096 add when loaded" },
        { false, false, "residual 3096 no add when null" },
        { true, true, "residual 1356 add when loaded" },
        { false, false, "residual 1356 no add when null" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldAddMemberAfterOnlineLookup(c.unityLoaded);
        const bool inlineF = inlineShouldAddMemberAfterOnlineLookup3254(c.unityLoaded);
        const bool pin     = pinShouldAddMemberAfterOnlineLookup3254(c.unityLoaded);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldAddMemberAfterOnlineLookup dual-wire == inline LSB formula") && ok;
        ok = expect(got == pin, "ShouldAddMemberAfterOnlineLookup == direct return pin") && ok;
    }

    // Free == pin across residual poles (direct return pin form).
    ok = expect(ShouldAddMemberAfterOnlineLookup(true) == pinShouldAddMemberAfterOnlineLookup3254(true),
                "free == pin residual loaded add") &&
         ok;
    ok = expect(ShouldAddMemberAfterOnlineLookup(false) == pinShouldAddMemberAfterOnlineLookup3254(false),
                "free == pin residual null skip") &&
         ok;

    // Dense compose: full 2¹ boolean space free == inline == pin.
    for (const bool unityLoaded : { false, true })
    {
        const bool got     = ShouldAddMemberAfterOnlineLookup(unityLoaded);
        const bool inlineF = inlineShouldAddMemberAfterOnlineLookup3254(unityLoaded);
        const bool pin     = pinShouldAddMemberAfterOnlineLookup3254(unityLoaded);
        ok                 = expect(got == pin, "compose free == direct return pin") && ok;
        ok                 = expect(got == inlineF, "compose free == inline") && ok;
        ok                 = expect(got == unityLoaded, "compose free == unityLoaded identity") && ok;
    }

    // Host-style compose: AddOnlineMember injects PUnity != nullptr.
    const struct
    {
        bool        unityLoaded;
        bool        wantAdd;
        const char* label;
    } hosts[] = {
        { true, true, "PUnity non-null → AddMember" },
        { false, false, "PUnity null → skip AddMember" },
    };
    for (const auto& h : hosts)
    {
        const bool add     = ShouldAddMemberAfterOnlineLookup(h.unityLoaded);
        const bool inlineF = inlineShouldAddMemberAfterOnlineLookup3254(h.unityLoaded);
        const bool pin     = pinShouldAddMemberAfterOnlineLookup3254(h.unityLoaded);
        ok                 = expect(add == h.wantAdd, h.label) && ok;
        ok                 = expect(add == inlineF && add == pin, "host compose free == inline == pin") && ok;
        if (add)
        {
            ok = expect(!OnlineMemberAlwaysReturnsFalse(),
                        "compose: AddOnlineMember still always returns false after AddMember") &&
                 ok;
        }
    }

    // Sibling dual-wire gates remain orthogonal (3050 / 3075 leave alone).
    ok = expect(ShouldLoadUnityChatOnOnlineAdd(false, 9), "sibling residual: load on miss") && ok;
    ok = expect(!ShouldLoadUnityChatOnOnlineAdd(true, 9), "sibling residual: no load on hit") && ok;
    ok = expect(!ShouldLoadUnityChatOnOnlineAdd(false, 0), "sibling residual: no load leader 0") && ok;
    ok = expect(ShouldRejectNullOnlineMember(true), "sibling residual: null char rejects") && ok;
    ok = expect(!ShouldRejectNullOnlineMember(false), "sibling residual: non-null proceeds") && ok;
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
