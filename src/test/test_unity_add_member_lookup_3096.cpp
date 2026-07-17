#include "test_unity_add_member_lookup_3096.h"

#include "map/unitychat_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "unitychat ShouldAddMemberAfterOnlineLookup 3096 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline AddOnlineMember post-lookup AddMember gate for dual-wire
// cross-check (slice 3096):
//   unityLoaded
auto inlineShouldAddMemberAfterOnlineLookup(const bool unityLoaded) -> bool
{
    return unityLoaded;
}

} // namespace

// Pure dual-wire expansion for unitychathelpers::ShouldAddMemberAfterOnlineLookup
// (unityLoaded identity; slice 3096). Dense 2¹ over the single bool input.
// Residual 1356 pins still hold.
auto runUnityAddMemberLookup3096SelfTests() -> bool
{
    using unitychathelpers::FormatOnlineMemberNullWarning;
    using unitychathelpers::OnlineMemberAlwaysReturnsFalse;
    using unitychathelpers::ShouldAddMemberAfterOnlineLookup;
    using unitychathelpers::ShouldLoadUnityChatOnOnlineAdd;
    using unitychathelpers::ShouldRejectNullOnlineMember;

    bool ok = true;

    // Residual 1356 pins still hold under dual-wire.
    ok = expect(ShouldAddMemberAfterOnlineLookup(true), "residual: unity loaded → add") && ok;
    ok = expect(!ShouldAddMemberAfterOnlineLookup(false), "residual: unity null → no add") && ok;

    const struct
    {
        bool        unityLoaded;
        bool        want;
        const char* label;
    } cases[] = {
        // Dense 2¹ dual poles.
        { false, false, "unity null skips AddMember" },
        { true, true, "unity loaded calls AddMember" },

        // Residual 1356 pins.
        { true, true, "residual add when loaded" },
        { false, false, "residual no add when null" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldAddMemberAfterOnlineLookup(c.unityLoaded);
        const bool inlineF = inlineShouldAddMemberAfterOnlineLookup(c.unityLoaded);
        const bool wantPin = c.unityLoaded;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldAddMemberAfterOnlineLookup dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldAddMemberAfterOnlineLookup == pin formula unityLoaded") && ok;
    }

    // Pin composition: add only when unityLoaded is true.
    ok = expect(!ShouldAddMemberAfterOnlineLookup(false), "unity null must skip AddMember") && ok;
    ok = expect(ShouldAddMemberAfterOnlineLookup(true), "unity loaded must call AddMember") && ok;

    // Dense 2¹ compose over the single bool domain (exactly two cells).
    for (const bool unityLoaded : { false, true })
    {
        const bool got  = ShouldAddMemberAfterOnlineLookup(unityLoaded);
        const bool want = unityLoaded;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldAddMemberAfterOnlineLookup(unityLoaded),
                    "compose free == inline") &&
             ok;
    }

    // Host path: AddOnlineMember injects PUnity != nullptr after load/cache.
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
        const bool add = ShouldAddMemberAfterOnlineLookup(h.unityLoaded);
        ok             = expect(add == h.wantAdd, h.label) && ok;
        ok             = expect(add == inlineShouldAddMemberAfterOnlineLookup(h.unityLoaded),
                    "host compose free == inline") &&
             ok;
        ok = expect(add == h.unityLoaded, "host compose free == unityLoaded (identity)") && ok;
        if (add)
        {
            ok = expect(!OnlineMemberAlwaysReturnsFalse(),
                        "compose: AddOnlineMember still always returns false after AddMember") &&
                 ok;
        }
    }

    // Production AddOnlineMember path semantics after null reject + load.
    ok = expect(ShouldAddMemberAfterOnlineLookup(true), "AddOnlineMember loaded → AddMember path") && ok;
    ok = expect(!ShouldAddMemberAfterOnlineLookup(false), "AddOnlineMember null PUnity → skip AddMember path") && ok;

    // Explicit dual-wire poles: free == unityLoaded for dense 2¹.
    for (const bool unityLoaded : { false, true })
    {
        const bool got  = ShouldAddMemberAfterOnlineLookup(unityLoaded);
        const bool want = unityLoaded;
        ok              = expect(got == want, "host inject dual-wire identity") && ok;
        ok              = expect(got == inlineShouldAddMemberAfterOnlineLookup(unityLoaded),
                    "host inject free == inline") &&
             ok;
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
