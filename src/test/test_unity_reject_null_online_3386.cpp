#include "test_unity_reject_null_online_3386.h"

#include "map/unitychat_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "unitychat ShouldRejectNullOnlineMember 3386 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline AddOnlineMember / DelOnlineMember null-PChar gate for dual-wire
// cross-check (slice 3386 dedicated expand residual 3075):
//   charNull
// Direct return form matching production free function / capacity.
auto inlineShouldRejectNullOnlineMember(const bool charNull) -> bool
{
    return charNull;
}

// Compact dual-wire pin matching free function / capacity body (slice 3386).
// Direct return only — same formula as production ShouldRejectNullOnlineMember.
auto pinShouldRejectNullOnlineMember3386(const bool charNull) -> bool
{
    return charNull;
}

// Residual 3075 pin form retained for free == residual pin cross-check.
auto pinShouldRejectNullOnlineMember3075(const bool charNull) -> bool
{
    return charNull;
}

} // namespace

// Pure dual-wire expansion for unitychathelpers::ShouldRejectNullOnlineMember
// (charNull identity; OmegaXI internal/unitychat;
// slice 3386 dedicated expand residual 3075). Formula unchanged.
//
// Coverage:
//   - free == inline == pin (direct return formula)
//   - residual 1356 / 3075 pins still hold
//   - dense 2¹
//   - residual dual-wire suite retained: test_unity_reject_null_online_3075
auto runUnityRejectNullOnline3386SelfTests() -> bool
{
    using unitychathelpers::FormatOnlineMemberNullWarning;
    using unitychathelpers::OnlineMemberAlwaysReturnsFalse;
    using unitychathelpers::ShouldAddMemberAfterOnlineLookup;
    using unitychathelpers::ShouldLoadUnityChatOnOnlineAdd;
    using unitychathelpers::ShouldRejectNullOnlineMember;

    bool ok = true;

    // Residual 1356 / 3075 pins still hold under dedicated dual-wire.
    ok = expect(ShouldRejectNullOnlineMember(true), "residual: null char rejects") && ok;
    ok = expect(!ShouldRejectNullOnlineMember(false), "residual: non-null char proceeds") && ok;
    ok = expect(pinShouldRejectNullOnlineMember3075(true), "prior residual 3075 null rejects") && ok;
    ok = expect(!pinShouldRejectNullOnlineMember3075(false), "prior residual 3075 non-null proceeds") && ok;

    const struct
    {
        bool        charNull;
        bool        want;
        const char* label;
    } cases[] = {
        // Dense 2¹ dual poles / residual 1356 / 3075.
        { false, false, "residual non-null char proceeds" },
        { true, true, "residual null char rejects" },

        // Residual polarity repeats for dual-wire stability.
        { true, true, "identity null char" },
        { false, false, "identity accept char" },

        // Prior residual 3075 re-pins.
        { true, true, "prior residual 3075 null rejects" },
        { false, false, "prior residual 3075 non-null proceeds" },

        // Host inject path poles (AddOnlineMember / DelOnlineMember).
        { true, true, "host PChar == nullptr → reject" },
        { false, false, "host PChar non-null → proceed" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRejectNullOnlineMember(c.charNull);
        const bool inlineF = inlineShouldRejectNullOnlineMember(c.charNull);
        const bool pin     = pinShouldRejectNullOnlineMember3386(c.charNull);
        const bool wantF   = c.charNull;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldRejectNullOnlineMember dual-wire == inline LSB formula") && ok;
        ok = expect(got == pin, "ShouldRejectNullOnlineMember == pin formula") && ok;
        ok = expect(got == wantF, "formula free==charNull") && ok;
        ok = expect(got == pinShouldRejectNullOnlineMember3075(c.charNull),
                    "free == residual pin 3075") &&
             ok;
    }

    // Pin composition: reject only when charNull is true.
    ok = expect(!ShouldRejectNullOnlineMember(false), "non-null char must proceed") && ok;
    ok = expect(ShouldRejectNullOnlineMember(true), "null char must reject") && ok;

    // Dense 2¹ compose over the single bool domain (exactly two cells).
    for (const bool charNull : { false, true })
    {
        const bool got     = ShouldRejectNullOnlineMember(charNull);
        const bool want    = charNull;
        const bool inlineF = inlineShouldRejectNullOnlineMember(charNull);
        const bool pin     = pinShouldRejectNullOnlineMember3386(charNull);

        ok = expect(got == want, "compose free == pin formula") && ok;
        ok = expect(got == inlineF, "compose free == inline") && ok;
        ok = expect(got == pin, "compose free == pin 3386") && ok;
        ok = expect(got == pinShouldRejectNullOnlineMember3075(charNull),
                    "compose free == residual pin 3075") &&
             ok;
    }

    // Host-style compose: AddOnlineMember / DelOnlineMember inject PChar == nullptr.
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
        const bool reject  = ShouldRejectNullOnlineMember(h.charNull);
        const bool inlineF = inlineShouldRejectNullOnlineMember(h.charNull);
        const bool pin     = pinShouldRejectNullOnlineMember3386(h.charNull);
        ok                 = expect(reject == h.wantReject, h.label) && ok;
        ok                 = expect(reject == inlineF, "host compose free == inline") && ok;
        ok                 = expect(reject == pin, "host compose free == pin") && ok;
        ok                 = expect(reject == pinShouldRejectNullOnlineMember3075(h.charNull),
                    "host compose free == residual pin 3075") &&
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

    // Sibling dual-wire gates remain orthogonal (do not thrash load-on-online-add).
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
    ok = expect(ShouldRejectNullOnlineMember(true),
                "null must reject even when load gate would admit") &&
         ok;

    // Explicit dual-wire poles: free == charNull for dense 2¹.
    for (const bool charNull : { false, true })
    {
        const bool got  = ShouldRejectNullOnlineMember(charNull);
        const bool want = charNull;
        ok              = expect(got == want, "host inject dual-wire identity") && ok;
        ok              = expect(got == inlineShouldRejectNullOnlineMember(charNull),
                    "host inject free == inline") &&
             ok;
        ok = expect(got == pinShouldRejectNullOnlineMember3386(charNull),
                    "host inject free == pin 3386") &&
             ok;
        ok = expect(got == pinShouldRejectNullOnlineMember3075(charNull),
                    "host inject free == residual pin 3075") &&
             ok;
    }

    return ok;
}
