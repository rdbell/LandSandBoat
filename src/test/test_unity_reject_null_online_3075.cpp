#include "test_unity_reject_null_online_3075.h"

#include "map/unitychat_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "unitychat ShouldRejectNullOnlineMember 3075 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline AddOnlineMember / DelOnlineMember null-PChar gate for dual-wire
// cross-check (slice 3075):
//   charNull
auto inlineShouldRejectNullOnlineMember(const bool charNull) -> bool
{
    return charNull;
}

} // namespace

// Pure dual-wire expansion for unitychathelpers::ShouldRejectNullOnlineMember
// (charNull identity; slice 3075). Dense 2¹ over the single bool input.
// Residual 1356 pins still hold.
auto runUnityRejectNullOnline3075SelfTests() -> bool
{
    using unitychathelpers::FormatOnlineMemberNullWarning;
    using unitychathelpers::OnlineMemberAlwaysReturnsFalse;
    using unitychathelpers::ShouldAddMemberAfterOnlineLookup;
    using unitychathelpers::ShouldLoadUnityChatOnOnlineAdd;
    using unitychathelpers::ShouldRejectNullOnlineMember;

    bool ok = true;

    // Residual 1356 pins still hold under dual-wire.
    ok = expect(ShouldRejectNullOnlineMember(true), "residual: null char rejects") && ok;
    ok = expect(!ShouldRejectNullOnlineMember(false), "residual: non-null char proceeds") && ok;

    const struct
    {
        bool        charNull;
        bool        want;
        const char* label;
    } cases[] = {
        // Dense 2¹ dual poles.
        { false, false, "non-null char proceeds" },
        { true, true, "null char rejects" },

        // Residual 1356 pins.
        { true, true, "residual null char" },
        { false, false, "residual accept char" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRejectNullOnlineMember(c.charNull);
        const bool inlineF = inlineShouldRejectNullOnlineMember(c.charNull);
        const bool wantPin = c.charNull;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldRejectNullOnlineMember dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldRejectNullOnlineMember == pin formula charNull") && ok;
    }

    // Pin composition: reject only when charNull is true.
    ok = expect(!ShouldRejectNullOnlineMember(false), "non-null char must proceed") && ok;
    ok = expect(ShouldRejectNullOnlineMember(true), "null char must reject") && ok;

    // Dense 2¹ compose over the single bool domain (exactly two cells).
    for (const bool charNull : { false, true })
    {
        const bool got  = ShouldRejectNullOnlineMember(charNull);
        const bool want = charNull;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldRejectNullOnlineMember(charNull),
                    "compose free == inline") &&
             ok;
    }

    // Host path: AddOnlineMember / DelOnlineMember inject PChar == nullptr.
    const struct
    {
        bool        charNull;
        bool        wantReject;
        const char* label;
    } hosts[] = {
        { true, true, "PChar == nullptr → warning + always-false return" },
        { false, false, "PChar non-null → proceed to load / roster" },
    };
    for (const auto& h : hosts)
    {
        const bool reject = ShouldRejectNullOnlineMember(h.charNull);
        ok                = expect(reject == h.wantReject, h.label) && ok;
        ok                = expect(reject == inlineShouldRejectNullOnlineMember(h.charNull),
                    "host compose free == inline") &&
             ok;
        ok = expect(reject == h.charNull, "host compose free == charNull (identity)") && ok;
        if (reject)
        {
            ok = expect(FormatOnlineMemberNullWarning() == "PChar is null.",
                        "compose: null warn residual on reject path") &&
                 ok;
            ok = expect(!OnlineMemberAlwaysReturnsFalse(),
                        "compose: Add/DelOnlineMember still always returns false") &&
                 ok;
        }
    }

    // Production AddOnlineMember / DelOnlineMember path semantics.
    ok = expect(ShouldRejectNullOnlineMember(true), "AddOnlineMember null → reject path") && ok;
    ok = expect(!ShouldRejectNullOnlineMember(false), "AddOnlineMember non-null → proceed path") && ok;
    ok = expect(ShouldRejectNullOnlineMember(true), "DelOnlineMember null → reject path") && ok;
    ok = expect(!ShouldRejectNullOnlineMember(false), "DelOnlineMember non-null → proceed path") && ok;

    // Explicit dual-wire poles: free == charNull for dense 2¹.
    for (const bool charNull : { false, true })
    {
        const bool got  = ShouldRejectNullOnlineMember(charNull);
        const bool want = charNull;
        ok              = expect(got == want, "host inject dual-wire identity") && ok;
        ok              = expect(got == inlineShouldRejectNullOnlineMember(charNull),
                    "host inject free == inline") &&
             ok;
    }

    // Residual sibling gates remain orthogonal to this dual-wire surface.
    ok = expect(ShouldLoadUnityChatOnOnlineAdd(false, 9), "sibling residual: load on miss") && ok;
    ok = expect(!ShouldLoadUnityChatOnOnlineAdd(true, 9), "sibling residual: no load on hit") && ok;
    ok = expect(!ShouldLoadUnityChatOnOnlineAdd(false, 0), "sibling residual: no load leader 0") && ok;
    ok = expect(ShouldAddMemberAfterOnlineLookup(true), "sibling residual: add when loaded") && ok;
    ok = expect(!ShouldAddMemberAfterOnlineLookup(false), "sibling residual: no add when null") && ok;
    ok = expect(!OnlineMemberAlwaysReturnsFalse(), "sibling residual: always returns false") && ok;
    ok = expect(FormatOnlineMemberNullWarning() == "PChar is null.", "sibling residual: null warn") && ok;
    ok = expect(!ShouldRejectNullOnlineMember(false),
                "non-null must proceed even if later load/add/erase fails") &&
         ok;

    return ok;
}
