#include "test_unity_return_cached_3130.h"

#include "map/unitychat_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "unitychat ShouldReturnCachedUnityChat 3130 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline GetUnityChat find-hit gate for dual-wire cross-check (slice 3130):
//   foundInList
auto inlineShouldReturnCachedUnityChat(const bool foundInList) -> bool
{
    return foundInList;
}

} // namespace

// Pure dual-wire expansion for unitychathelpers::ShouldReturnCachedUnityChat
// (foundInList; slice 3130). Dense 2¹ foundInList space. Residual 1356 pins
// still hold.
auto runUnityReturnCached3130SelfTests() -> bool
{
    using unitychathelpers::FormatDelOnlineMemberException;
    using unitychathelpers::FormatOnlineMemberNullWarning;
    using unitychathelpers::OnlineMemberAlwaysReturnsFalse;
    using unitychathelpers::ShouldAddMemberAfterOnlineLookup;
    using unitychathelpers::ShouldEraseUnityChatAfterDelOnline;
    using unitychathelpers::ShouldLoadUnityChatOnOnlineAdd;
    using unitychathelpers::ShouldRejectNullOnlineMember;
    using unitychathelpers::ShouldReturnCachedUnityChat;
    using unitychathelpers::ShouldUnloadUnityChat;

    bool ok = true;

    // Residual 1356 pins still hold under dual-wire.
    ok = expect(ShouldReturnCachedUnityChat(true), "residual hit → return cached") && ok;
    ok = expect(!ShouldReturnCachedUnityChat(false), "residual miss → no cached") && ok;

    const struct
    {
        bool        foundInList;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles / residual 1356.
        { true, true, "find hit returns cached" },
        { false, false, "find miss skips cached" },

        // Residual polarity repeats for dual-wire stability.
        { true, true, "identity hit return cached" },
        { false, false, "identity miss no cached" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldReturnCachedUnityChat(c.foundInList);
        const bool inlineF = inlineShouldReturnCachedUnityChat(c.foundInList);
        const bool wantPin = c.foundInList;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldReturnCachedUnityChat dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldReturnCachedUnityChat == pin formula") && ok;
    }

    // Pin composition: return cached only on hit.
    ok = expect(ShouldReturnCachedUnityChat(true), "hit must return cached") && ok;
    ok = expect(!ShouldReturnCachedUnityChat(false), "miss must not return cached") && ok;

    // Dense compose: full 2¹ foundInList space free == inline == pin.
    for (const bool found : { false, true })
    {
        const bool got  = ShouldReturnCachedUnityChat(found);
        const bool want = found;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldReturnCachedUnityChat(found),
                    "compose free == inline") &&
             ok;
    }

    // Host-style compose: GetUnityChat injects cache find.
    const struct
    {
        bool        foundInList;
        bool        wantCached;
        const char* label;
    } hosts[] = {
        { true, true, "hit → return UnityChatList.find(leader)->second" },
        { false, false, "miss → return nullptr" },
    };
    for (const auto& h : hosts)
    {
        const bool cached = ShouldReturnCachedUnityChat(h.foundInList);
        ok                = expect(cached == h.wantCached, h.label) && ok;
        ok                = expect(cached == inlineShouldReturnCachedUnityChat(h.foundInList),
                    "host compose free == inline") &&
             ok;
        ok = expect(!(cached && !h.foundInList), "must not return cached when find miss") && ok;
        ok = expect(!(!cached && h.foundInList), "hit must return cached") && ok;
    }

    // Production GetUnityChat path semantics.
    ok = expect(ShouldReturnCachedUnityChat(true), "GetUnityChat hit → return cached path") && ok;
    ok = expect(!ShouldReturnCachedUnityChat(false), "GetUnityChat miss → nullptr path") && ok;

    // Sibling dual-wire independence: 3050 / 3075 / 3096 / 3116 remain orthogonal.
    ok = expect(ShouldLoadUnityChatOnOnlineAdd(false, 5), "sibling 3050: load on miss") && ok;
    ok = expect(!ShouldLoadUnityChatOnOnlineAdd(true, 5), "sibling 3050: no load on hit") && ok;
    ok = expect(!ShouldLoadUnityChatOnOnlineAdd(false, 0), "sibling 3050: no load leader 0") && ok;
    ok = expect(ShouldRejectNullOnlineMember(true), "sibling 3075: null char rejects") && ok;
    ok = expect(!ShouldRejectNullOnlineMember(false), "sibling 3075: non-null proceeds") && ok;
    ok = expect(ShouldAddMemberAfterOnlineLookup(true), "sibling 3096: add when loaded") && ok;
    ok = expect(!ShouldAddMemberAfterOnlineLookup(false), "sibling 3096: no add when null") && ok;
    ok = expect(ShouldEraseUnityChatAfterDelOnline(false), "sibling 3116: erase empty") && ok;
    ok = expect(!ShouldEraseUnityChatAfterDelOnline(true), "sibling 3116: keep non-empty") && ok;

    // Residual sibling gates remain orthogonal to this dual-wire surface.
    ok = expect(ShouldUnloadUnityChat(true), "sibling residual: unload on hit") && ok;
    ok = expect(!ShouldUnloadUnityChat(false), "sibling residual: no unload on miss") && ok;
    ok = expect(!OnlineMemberAlwaysReturnsFalse(), "sibling residual: always returns false") && ok;
    ok = expect(FormatOnlineMemberNullWarning() == "PChar is null.", "sibling residual: null warn") && ok;
    ok = expect(FormatDelOnlineMemberException("boom") ==
                    "unitychat::DelOnlineMember caught exception: boom",
                "sibling residual: del exception format") &&
         ok;

    // Explicit dual-wire poles: free == foundInList for dense 2¹ space.
    for (const bool found : { false, true })
    {
        const bool got  = ShouldReturnCachedUnityChat(found);
        const bool want = found;
        ok              = expect(got == want, "host inject dual-wire identity") && ok;
        ok              = expect(got == inlineShouldReturnCachedUnityChat(found),
                    "host inject free == inline") &&
             ok;
    }

    return ok;
}
