#include "test_linkshell_load_online_add_3055.h"

#include "map/linkshell_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "linkshell ShouldLoadLinkshellOnOnlineAdd 3055 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline AddOnlineMember LoadLinkshell gate for dual-wire cross-check (slice 3055):
//   !foundInCache
auto inlineShouldLoadLinkshellOnOnlineAdd(const bool foundInCache) -> bool
{
    return !foundInCache;
}

} // namespace

// Pure dual-wire expansion for linkshellhelpers::ShouldLoadLinkshellOnOnlineAdd
// (!foundInCache; slice 3055). Dense 2¹ foundInCache space. Residual 1355 pins
// still hold.
auto runLinkshellLoadOnlineAdd3055SelfTests() -> bool
{
    using linkshellhelpers::OnlineMemberAlwaysReturnsFalse;
    using linkshellhelpers::ShouldAddMemberAfterOnlineLookup;
    using linkshellhelpers::ShouldLoadLinkshellOnOnlineAdd;
    using linkshellhelpers::ShouldProcessLinkshellItem;
    using linkshellhelpers::ShouldRejectNullOnlineMember;

    bool ok = true;

    // Residual 1355 pins still hold under dual-wire.
    ok = expect(ShouldLoadLinkshellOnOnlineAdd(false), "residual miss → load") && ok;
    ok = expect(!ShouldLoadLinkshellOnOnlineAdd(true), "residual hit → no load") && ok;

    const struct
    {
        bool        foundInCache;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles / residual 1355.
        { false, true, "cache miss loads" },
        { true, false, "cache hit skips load" },

        // Residual polarity repeats for dual-wire stability.
        { false, true, "identity miss load" },
        { true, false, "identity hit no load" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldLoadLinkshellOnOnlineAdd(c.foundInCache);
        const bool inlineF = inlineShouldLoadLinkshellOnOnlineAdd(c.foundInCache);
        const bool wantPin = !c.foundInCache;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldLoadLinkshellOnOnlineAdd dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldLoadLinkshellOnOnlineAdd == pin formula") && ok;
    }

    // Pin composition: load only on miss.
    ok = expect(ShouldLoadLinkshellOnOnlineAdd(false), "miss must load") && ok;
    ok = expect(!ShouldLoadLinkshellOnOnlineAdd(true), "hit must not load") && ok;

    // Dense compose: full 2¹ foundInCache space free == inline == pin.
    for (const bool found : { false, true })
    {
        const bool got  = ShouldLoadLinkshellOnOnlineAdd(found);
        const bool want = !found;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldLoadLinkshellOnOnlineAdd(found),
                    "compose free == inline") &&
             ok;
    }

    // Host-style compose: AddOnlineMember injects cache find.
    const struct
    {
        bool        foundInCache;
        bool        wantLoad;
        const char* label;
    } hosts[] = {
        { false, true, "miss → LoadLinkshell" },
        { true, false, "hit → reuse cache, no LoadLinkshell" },
    };
    for (const auto& h : hosts)
    {
        const bool load = ShouldLoadLinkshellOnOnlineAdd(h.foundInCache);
        ok              = expect(load == h.wantLoad, h.label) && ok;
        ok              = expect(load == inlineShouldLoadLinkshellOnOnlineAdd(h.foundInCache),
                    "host compose free == inline") &&
             ok;
        ok = expect(!(load && h.foundInCache), "must not load when cache hit") && ok;
        ok = expect(!(!load && !h.foundInCache), "miss must load") && ok;
    }

    // Production AddOnlineMember path semantics after null reject + item gate.
    ok = expect(ShouldLoadLinkshellOnOnlineAdd(false), "AddOnlineMember miss → LoadLinkshell path") && ok;
    ok = expect(!ShouldLoadLinkshellOnOnlineAdd(true), "AddOnlineMember hit → reuse path") && ok;

    // Residual sibling gates remain orthogonal to this dual-wire surface.
    ok = expect(ShouldRejectNullOnlineMember(true), "sibling residual: null char still rejects") && ok;
    ok = expect(ShouldProcessLinkshellItem(true, true), "sibling residual: process ls item") && ok;
    ok = expect(!ShouldProcessLinkshellItem(false, true), "sibling residual: null item no process") && ok;
    ok = expect(ShouldAddMemberAfterOnlineLookup(true), "sibling residual: add when loaded") && ok;
    ok = expect(!ShouldAddMemberAfterOnlineLookup(false), "sibling residual: no add when null") && ok;
    ok = expect(!OnlineMemberAlwaysReturnsFalse(), "sibling residual: always returns false") && ok;

    // Explicit dual-wire poles: free == !foundInCache for dense 2¹ space.
    for (const bool found : { false, true })
    {
        const bool got  = ShouldLoadLinkshellOnOnlineAdd(found);
        const bool want = !found;
        ok              = expect(got == want, "host inject dual-wire identity") && ok;
        ok              = expect(got == inlineShouldLoadLinkshellOnOnlineAdd(found),
                    "host inject free == inline") &&
             ok;
    }

    return ok;
}
