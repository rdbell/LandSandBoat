#include "test_unity_erase_after_del_3116.h"

#include "map/unitychat_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "unitychat ShouldEraseUnityChatAfterDelOnline 3116 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline DelOnlineMember post-DelMember erase gate for dual-wire
// cross-check (slice 3116):
//   !delMemberRemaining
auto inlineShouldEraseUnityChatAfterDelOnline(const bool delMemberRemaining) -> bool
{
    return !delMemberRemaining;
}

} // namespace

// Pure dual-wire expansion for unitychathelpers::ShouldEraseUnityChatAfterDelOnline
// (!delMemberRemaining; slice 3116). Dense 2¹ over the single bool input.
// Residual 1356 pins still hold.
auto runUnityEraseAfterDel3116SelfTests() -> bool
{
    using unitychathelpers::DelMemberRemaining;
    using unitychathelpers::FormatDelOnlineMemberException;
    using unitychathelpers::FormatOnlineMemberNullWarning;
    using unitychathelpers::OnlineMemberAlwaysReturnsFalse;
    using unitychathelpers::ShouldAddMemberAfterOnlineLookup;
    using unitychathelpers::ShouldEraseUnityChatAfterDelOnline;
    using unitychathelpers::ShouldLoadUnityChatOnOnlineAdd;
    using unitychathelpers::ShouldRejectNullOnlineMember;

    bool ok = true;

    // Residual 1356 pins still hold under dual-wire.
    ok = expect(ShouldEraseUnityChatAfterDelOnline(false), "residual: empty roster after del → erase") && ok;
    ok = expect(!ShouldEraseUnityChatAfterDelOnline(true), "residual: members remain after del → keep") && ok;

    const struct
    {
        bool        delMemberRemaining;
        bool        want;
        const char* label;
    } cases[] = {
        // Dense 2¹ dual poles.
        { false, true, "empty roster erases UnityChatList entry" },
        { true, false, "remaining members keep UnityChatList entry" },

        // Residual 1356 pins.
        { false, true, "residual erase when empty" },
        { true, false, "residual keep when remaining" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldEraseUnityChatAfterDelOnline(c.delMemberRemaining);
        const bool inlineF = inlineShouldEraseUnityChatAfterDelOnline(c.delMemberRemaining);
        const bool wantPin = !c.delMemberRemaining;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldEraseUnityChatAfterDelOnline dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldEraseUnityChatAfterDelOnline == pin formula !delMemberRemaining") && ok;
    }

    // Pin composition: erase only when delMemberRemaining is false.
    ok = expect(ShouldEraseUnityChatAfterDelOnline(false), "empty roster must erase") && ok;
    ok = expect(!ShouldEraseUnityChatAfterDelOnline(true), "remaining members must keep") && ok;

    // Dense 2¹ compose over the single bool domain (exactly two cells).
    for (const bool delMemberRemaining : { false, true })
    {
        const bool got  = ShouldEraseUnityChatAfterDelOnline(delMemberRemaining);
        const bool want = !delMemberRemaining;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldEraseUnityChatAfterDelOnline(delMemberRemaining),
                    "compose free == inline") &&
             ok;
    }

    // Host path: DelOnlineMember injects DelMember(PChar) return.
    const struct
    {
        bool        delMemberRemaining;
        bool        wantErase;
        const char* label;
    } hosts[] = {
        { false, true, "DelMember empty → UnityChatList.erase(leader)" },
        { true, false, "DelMember remaining → keep UnityChatList entry" },
    };
    for (const auto& h : hosts)
    {
        const bool erase = ShouldEraseUnityChatAfterDelOnline(h.delMemberRemaining);
        ok               = expect(erase == h.wantErase, h.label) && ok;
        ok               = expect(erase == inlineShouldEraseUnityChatAfterDelOnline(h.delMemberRemaining),
                    "host compose free == inline") &&
             ok;
        ok = expect(erase == !h.delMemberRemaining, "host compose free == !delMemberRemaining") && ok;
        ok = expect(!OnlineMemberAlwaysReturnsFalse(),
                    "compose: DelOnlineMember still always returns false") &&
             ok;
    }

    // Production DelOnlineMember path semantics after null reject.
    ok = expect(ShouldEraseUnityChatAfterDelOnline(false), "DelOnlineMember empty roster → erase path") && ok;
    ok = expect(!ShouldEraseUnityChatAfterDelOnline(true), "DelOnlineMember remaining members → keep path") && ok;

    // DelMemberRemaining host inject model: size after erase attempt.
    ok = expect(!DelMemberRemaining(0), "DelMemberRemaining(0) must be false (empty)") && ok;
    ok = expect(DelMemberRemaining(1), "DelMemberRemaining(1) must be true (remaining)") && ok;
    const bool sizeZeroRemaining = DelMemberRemaining(0); // false
    const bool sizeOneRemaining  = DelMemberRemaining(1); // true
    const bool sizeManyRemaining = DelMemberRemaining(3); // true
    ok = expect(ShouldEraseUnityChatAfterDelOnline(sizeZeroRemaining), "compose inject size 0 remaining must erase") &&
         ok;
    ok = expect(!ShouldEraseUnityChatAfterDelOnline(sizeOneRemaining), "compose inject size 1 remaining must keep") &&
         ok;
    ok = expect(!ShouldEraseUnityChatAfterDelOnline(sizeManyRemaining), "compose inject size N remaining must keep") &&
         ok;
    ok = expect(inlineShouldEraseUnityChatAfterDelOnline(sizeZeroRemaining),
                "compose inject size 0 inline must erase") &&
         ok;
    ok = expect(!inlineShouldEraseUnityChatAfterDelOnline(sizeOneRemaining),
                "compose inject size 1 inline must keep") &&
         ok;

    // Explicit dual-wire poles: free == !delMemberRemaining for dense 2¹.
    for (const bool delMemberRemaining : { false, true })
    {
        const bool got  = ShouldEraseUnityChatAfterDelOnline(delMemberRemaining);
        const bool want = !delMemberRemaining;
        ok              = expect(got == want, "host inject dual-wire identity") && ok;
        ok              = expect(got == inlineShouldEraseUnityChatAfterDelOnline(delMemberRemaining),
                    "host inject free == inline") &&
             ok;
    }

    // Residual sibling gates remain orthogonal to this dual-wire surface.
    ok = expect(ShouldLoadUnityChatOnOnlineAdd(false, 9), "sibling residual: load on miss") && ok;
    ok = expect(!ShouldLoadUnityChatOnOnlineAdd(true, 9), "sibling residual: no load on hit") && ok;
    ok = expect(!ShouldLoadUnityChatOnOnlineAdd(false, 0), "sibling residual: no load leader 0") && ok;
    ok = expect(ShouldRejectNullOnlineMember(true), "sibling residual: null rejects") && ok;
    ok = expect(!ShouldRejectNullOnlineMember(false), "sibling residual: non-null proceeds") && ok;
    ok = expect(ShouldAddMemberAfterOnlineLookup(true), "sibling residual: add when loaded") && ok;
    ok = expect(!ShouldAddMemberAfterOnlineLookup(false), "sibling residual: no add when null") && ok;
    ok = expect(!OnlineMemberAlwaysReturnsFalse(), "sibling residual: always returns false") && ok;
    ok = expect(FormatOnlineMemberNullWarning() == "PChar is null.", "sibling residual: null warn") && ok;
    ok = expect(FormatDelOnlineMemberException("boom") == "unitychat::DelOnlineMember caught exception: boom",
                "sibling residual: del exception format") &&
         ok;
    ok = expect(!ShouldEraseUnityChatAfterDelOnline(true),
                "remaining members must keep even if earlier gates admitted") &&
         ok;

    return ok;
}
