#include "test_linkshell_return_cached_3126.h"

#include "map/linkshell_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "linkshell ShouldReturnCachedLinkshell 3126 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline GetLinkshell find-hit gate for dual-wire cross-check (slice 3126):
//   foundInList
auto inlineShouldReturnCachedLinkshell(const bool foundInList) -> bool
{
    return foundInList;
}

} // namespace

// Pure dual-wire expansion for linkshellhelpers::ShouldReturnCachedLinkshell
// (foundInList; slice 3126). Dense 2¹ foundInList space. Residual 1355 pins
// still hold.
auto runLinkshellReturnCached3126SelfTests() -> bool
{
    using linkshellhelpers::ClassifyLoadLinkshell;
    using linkshellhelpers::load_linkshell_gate;
    using linkshellhelpers::OnlineMemberAlwaysReturnsFalse;
    using linkshellhelpers::ShouldAddMemberAfterOnlineLookup;
    using linkshellhelpers::ShouldEraseLinkshellAfterDelOnline;
    using linkshellhelpers::ShouldLoadLinkshellOnOnlineAdd;
    using linkshellhelpers::ShouldProcessLinkshellItem;
    using linkshellhelpers::ShouldRejectNullOnlineMember;
    using linkshellhelpers::ShouldReturnCachedLinkshell;
    using linkshellhelpers::ShouldUnloadLinkshell;

    bool ok = true;

    // Residual 1355 pins still hold under dual-wire.
    ok = expect(ShouldReturnCachedLinkshell(true), "residual hit → return cached") && ok;
    ok = expect(!ShouldReturnCachedLinkshell(false), "residual miss → no cached") && ok;

    const struct
    {
        bool        foundInList;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles / residual 1355.
        { true, true, "find hit returns cached" },
        { false, false, "find miss skips cached" },

        // Residual polarity repeats for dual-wire stability.
        { true, true, "identity hit return cached" },
        { false, false, "identity miss no cached" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldReturnCachedLinkshell(c.foundInList);
        const bool inlineF = inlineShouldReturnCachedLinkshell(c.foundInList);
        const bool wantPin = c.foundInList;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldReturnCachedLinkshell dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldReturnCachedLinkshell == pin formula") && ok;
    }

    // Pin composition: return cached only on hit.
    ok = expect(ShouldReturnCachedLinkshell(true), "hit must return cached") && ok;
    ok = expect(!ShouldReturnCachedLinkshell(false), "miss must not return cached") && ok;

    // Dense compose: full 2¹ foundInList space free == inline == pin.
    for (const bool found : { false, true })
    {
        const bool got  = ShouldReturnCachedLinkshell(found);
        const bool want = found;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldReturnCachedLinkshell(found),
                    "compose free == inline") &&
             ok;
    }

    // Host-style compose: GetLinkshell injects cache find.
    const struct
    {
        bool        foundInList;
        bool        wantCached;
        const char* label;
    } hosts[] = {
        { true, true, "hit → return LinkshellList.find(id)->second" },
        { false, false, "miss → return nullptr" },
    };
    for (const auto& h : hosts)
    {
        const bool cached = ShouldReturnCachedLinkshell(h.foundInList);
        ok                = expect(cached == h.wantCached, h.label) && ok;
        ok                = expect(cached == inlineShouldReturnCachedLinkshell(h.foundInList),
                    "host compose free == inline") &&
             ok;
        ok = expect(!(cached && !h.foundInList), "must not return cached when find miss") && ok;
        ok = expect(!(!cached && h.foundInList), "hit must return cached") && ok;
    }

    // Production GetLinkshell path semantics.
    ok = expect(ShouldReturnCachedLinkshell(true), "GetLinkshell hit → return cached path") && ok;
    ok = expect(!ShouldReturnCachedLinkshell(false), "GetLinkshell miss → nullptr path") && ok;

    // Sibling dual-wire independence: 3055 / 3079 / 3099 / 3111 remain orthogonal.
    ok = expect(ShouldLoadLinkshellOnOnlineAdd(false), "sibling 3055: load on miss") && ok;
    ok = expect(!ShouldLoadLinkshellOnOnlineAdd(true), "sibling 3055: no load on hit") && ok;
    ok = expect(ShouldRejectNullOnlineMember(true), "sibling 3079: null char rejects") && ok;
    ok = expect(!ShouldRejectNullOnlineMember(false), "sibling 3079: non-null proceeds") && ok;
    ok = expect(ShouldProcessLinkshellItem(true, true), "sibling 3099: process ls item") && ok;
    ok = expect(!ShouldProcessLinkshellItem(false, true), "sibling 3099: null item no process") && ok;
    ok = expect(ShouldUnloadLinkshell(true), "sibling 3111: unload on hit") && ok;
    ok = expect(!ShouldUnloadLinkshell(false), "sibling 3111: no unload on miss") && ok;

    // Residual sibling gates remain orthogonal to this dual-wire surface.
    ok = expect(ShouldAddMemberAfterOnlineLookup(true), "sibling residual: add when loaded") && ok;
    ok = expect(!ShouldAddMemberAfterOnlineLookup(false), "sibling residual: no add when null") && ok;
    ok = expect(!OnlineMemberAlwaysReturnsFalse(), "sibling residual: always returns false") && ok;
    ok = expect(ShouldEraseLinkshellAfterDelOnline(false), "sibling residual: erase empty") && ok;
    ok = expect(!ShouldEraseLinkshellAfterDelOnline(true), "sibling residual: keep non-empty") && ok;
    ok = expect(ClassifyLoadLinkshell(true, true) == load_linkshell_gate::FOUND,
                "sibling residual: load found") &&
         ok;
    ok = expect(ClassifyLoadLinkshell(false, true) == load_linkshell_gate::NOT_FOUND,
                "sibling residual: load fail") &&
         ok;

    // Explicit dual-wire poles: free == foundInList for dense 2¹ space.
    for (const bool found : { false, true })
    {
        const bool got  = ShouldReturnCachedLinkshell(found);
        const bool want = found;
        ok              = expect(got == want, "host inject dual-wire identity") && ok;
        ok              = expect(got == inlineShouldReturnCachedLinkshell(found),
                    "host inject free == inline") &&
             ok;
    }

    return ok;
}
