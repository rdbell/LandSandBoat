#include "test_linkshell_unload_3111.h"

#include "map/linkshell_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "linkshell ShouldUnloadLinkshell 3111 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline UnloadLinkshell find-hit gate for dual-wire cross-check (slice 3111):
//   foundInList
auto inlineShouldUnloadLinkshell(const bool foundInList) -> bool
{
    return foundInList;
}

} // namespace

// Pure dual-wire expansion for linkshellhelpers::ShouldUnloadLinkshell
// (foundInList; slice 3111). Dense 2¹ foundInList space. Residual 1355 pins
// still hold.
auto runLinkshellUnload3111SelfTests() -> bool
{
    using linkshellhelpers::ClassifyLoadLinkshell;
    using linkshellhelpers::load_linkshell_gate;
    using linkshellhelpers::OnlineMemberAlwaysReturnsFalse;
    using linkshellhelpers::ShouldAddMemberAfterOnlineLookup;
    using linkshellhelpers::ShouldEraseLinkshellAfterDelOnline;
    using linkshellhelpers::ShouldLoadLinkshellOnOnlineAdd;
    using linkshellhelpers::ShouldProcessLinkshellItem;
    using linkshellhelpers::ShouldRejectNullOnlineMember;
    using linkshellhelpers::ShouldUnloadLinkshell;

    bool ok = true;

    // Residual 1355 pins still hold under dual-wire.
    ok = expect(ShouldUnloadLinkshell(true), "residual hit → unload") && ok;
    ok = expect(!ShouldUnloadLinkshell(false), "residual miss → no unload") && ok;

    const struct
    {
        bool        foundInList;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles / residual 1355.
        { true, true, "find hit unloads" },
        { false, false, "find miss skips unload" },

        // Residual polarity repeats for dual-wire stability.
        { true, true, "identity hit unload" },
        { false, false, "identity miss no unload" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldUnloadLinkshell(c.foundInList);
        const bool inlineF = inlineShouldUnloadLinkshell(c.foundInList);
        const bool wantPin = c.foundInList;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldUnloadLinkshell dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldUnloadLinkshell == pin formula") && ok;
    }

    // Pin composition: unload only on hit.
    ok = expect(ShouldUnloadLinkshell(true), "hit must unload") && ok;
    ok = expect(!ShouldUnloadLinkshell(false), "miss must not unload") && ok;

    // Dense compose: full 2¹ foundInList space free == inline == pin.
    for (const bool found : { false, true })
    {
        const bool got  = ShouldUnloadLinkshell(found);
        const bool want = found;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldUnloadLinkshell(found),
                    "compose free == inline") &&
             ok;
    }

    // Host-style compose: UnloadLinkshell injects cache find.
    const struct
    {
        bool        foundInList;
        bool        wantUnload;
        const char* label;
    } hosts[] = {
        { true, true, "hit → LinkshellList.erase" },
        { false, false, "miss → no erase" },
    };
    for (const auto& h : hosts)
    {
        const bool unload = ShouldUnloadLinkshell(h.foundInList);
        ok                = expect(unload == h.wantUnload, h.label) && ok;
        ok                = expect(unload == inlineShouldUnloadLinkshell(h.foundInList),
                    "host compose free == inline") &&
             ok;
        ok = expect(!(unload && !h.foundInList), "must not unload when find miss") && ok;
        ok = expect(!(!unload && h.foundInList), "hit must unload") && ok;
    }

    // Production UnloadLinkshell path semantics.
    ok = expect(ShouldUnloadLinkshell(true), "UnloadLinkshell hit → erase path") && ok;
    ok = expect(!ShouldUnloadLinkshell(false), "UnloadLinkshell miss → no erase path") && ok;

    // Sibling dual-wire independence: 3055 / 3079 / 3099 remain orthogonal.
    ok = expect(ShouldLoadLinkshellOnOnlineAdd(false), "sibling 3055: load on miss") && ok;
    ok = expect(!ShouldLoadLinkshellOnOnlineAdd(true), "sibling 3055: no load on hit") && ok;
    ok = expect(ShouldRejectNullOnlineMember(true), "sibling 3079: null char rejects") && ok;
    ok = expect(!ShouldRejectNullOnlineMember(false), "sibling 3079: non-null proceeds") && ok;
    ok = expect(ShouldProcessLinkshellItem(true, true), "sibling 3099: process ls item") && ok;
    ok = expect(!ShouldProcessLinkshellItem(false, true), "sibling 3099: null item no process") && ok;

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
        const bool got  = ShouldUnloadLinkshell(found);
        const bool want = found;
        ok              = expect(got == want, "host inject dual-wire identity") && ok;
        ok              = expect(got == inlineShouldUnloadLinkshell(found),
                    "host inject free == inline") &&
             ok;
    }

    return ok;
}
