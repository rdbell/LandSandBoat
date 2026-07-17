#include "test_linkshell_reject_null_online_3079.h"

#include "map/linkshell_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "linkshell ShouldRejectNullOnlineMember 3079 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline AddOnlineMember / DelOnlineMember null-PChar gate for dual-wire
// cross-check (slice 3079):
//   charNull
auto inlineShouldRejectNullOnlineMember(const bool charNull) -> bool
{
    return charNull;
}

} // namespace

// Pure dual-wire expansion for linkshellhelpers::ShouldRejectNullOnlineMember
// (charNull identity; slice 3079). Dense 2¹ over the single bool input.
// Residual 1355 pins still hold.
auto runLinkshellRejectNullOnline3079SelfTests() -> bool
{
    using linkshellhelpers::FormatOnlineMemberNullWarning;
    using linkshellhelpers::OnlineMemberAlwaysReturnsFalse;
    using linkshellhelpers::ShouldAddMemberAfterOnlineLookup;
    using linkshellhelpers::ShouldEraseLinkshellAfterDelOnline;
    using linkshellhelpers::ShouldLoadLinkshellOnOnlineAdd;
    using linkshellhelpers::ShouldProcessLinkshellItem;
    using linkshellhelpers::ShouldRejectNullOnlineMember;

    bool ok = true;

    // Residual 1355 pins still hold under dual-wire.
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

        // Residual 1355 pins.
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
        { false, false, "PChar non-null → proceed to process item / load / roster" },
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
    ok = expect(ShouldProcessLinkshellItem(true, true), "sibling residual: process item admits") && ok;
    ok = expect(!ShouldProcessLinkshellItem(false, true), "sibling residual: null item rejects") && ok;
    ok = expect(ShouldLoadLinkshellOnOnlineAdd(false), "sibling residual: load on miss") && ok;
    ok = expect(!ShouldLoadLinkshellOnOnlineAdd(true), "sibling residual: no load on hit") && ok;
    ok = expect(ShouldAddMemberAfterOnlineLookup(true), "sibling residual: add when loaded") && ok;
    ok = expect(!ShouldAddMemberAfterOnlineLookup(false), "sibling residual: no add when null") && ok;
    ok = expect(!OnlineMemberAlwaysReturnsFalse(), "sibling residual: always returns false") && ok;
    ok = expect(FormatOnlineMemberNullWarning() == "PChar is null.", "sibling residual: null warn") && ok;
    ok = expect(ShouldEraseLinkshellAfterDelOnline(false), "sibling residual: erase empty") && ok;
    ok = expect(!ShouldEraseLinkshellAfterDelOnline(true), "sibling residual: keep non-empty") && ok;
    ok = expect(!ShouldRejectNullOnlineMember(false),
                "non-null must proceed even if later process/load/add/erase fails") &&
         ok;

    return ok;
}
