#include "test_unity_load_online_add_3050.h"

#include "map/unitychat_capacity.h"

#include <cstdint>
#include <iostream>
#include <limits>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "unitychat ShouldLoadUnityChatOnOnlineAdd 3050 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline AddOnlineMember LoadUnityChat gate for dual-wire cross-check (slice 3050):
//   !foundInCache && leader != 0
auto inlineShouldLoadUnityChatOnOnlineAdd(const bool foundInCache, const uint32 leader) -> bool
{
    return !foundInCache && leader != 0;
}

} // namespace

// Pure dual-wire expansion for unitychathelpers::ShouldLoadUnityChatOnOnlineAdd
// (!foundInCache && leader != 0; slice 3050). Dense foundInCache × leader
// (0, nonzero, max uint32). Residual 1356 pins still hold.
auto runUnityLoadOnlineAdd3050SelfTests() -> bool
{
    using unitychathelpers::OnlineMemberAlwaysReturnsFalse;
    using unitychathelpers::ShouldAddMemberAfterOnlineLookup;
    using unitychathelpers::ShouldLoadUnityChatOnOnlineAdd;
    using unitychathelpers::ShouldRejectNullOnlineMember;

    bool ok = true;

    // Residual 1356 pins still hold under dual-wire.
    ok = expect(ShouldLoadUnityChatOnOnlineAdd(false, 5), "residual miss + leader → load") && ok;
    ok = expect(!ShouldLoadUnityChatOnOnlineAdd(true, 5), "residual hit → no load") && ok;
    ok = expect(!ShouldLoadUnityChatOnOnlineAdd(false, 0), "residual leader 0 → no load") && ok;

    constexpr uint32 maxU32 = std::numeric_limits<uint32>::max();

    const struct
    {
        bool        foundInCache;
        uint32      leader;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles / residual 1356.
        { false, 5, true, "cache miss nonzero leader loads" },
        { true, 5, false, "cache hit skips load" },
        { false, 0, false, "leader zero skips load" },
        { true, 0, false, "cache hit and leader zero skips" },

        // Nonzero edge: 1 and max uint32 both load on miss.
        { false, 1, true, "cache miss leader 1 loads" },
        { false, maxU32, true, "cache miss max uint32 loads" },
        { true, 1, false, "cache hit leader 1 skips" },
        { true, maxU32, false, "cache hit max uint32 skips" },

        // Residual polarity repeats for dual-wire stability.
        { false, 42, true, "identity miss+leader load" },
        { true, 42, false, "identity hit no load" },
        { false, 0, false, "identity miss+zero no load" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldLoadUnityChatOnOnlineAdd(c.foundInCache, c.leader);
        const bool inlineF = inlineShouldLoadUnityChatOnOnlineAdd(c.foundInCache, c.leader);
        const bool wantPin = !c.foundInCache && c.leader != 0;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldLoadUnityChatOnOnlineAdd dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldLoadUnityChatOnOnlineAdd == pin formula") && ok;
    }

    // Pin composition: load only on miss with nonzero leader.
    ok = expect(ShouldLoadUnityChatOnOnlineAdd(false, 1), "miss + leader 1 must load") && ok;
    ok = expect(!ShouldLoadUnityChatOnOnlineAdd(true, 1), "hit must not load") && ok;
    ok = expect(!ShouldLoadUnityChatOnOnlineAdd(false, 0), "leader 0 must not load") && ok;

    // Dense compose: foundInCache × leader edges free == inline == pin.
    const uint32 leaders[] = { 0, 1, 5, maxU32 };
    for (const bool found : { false, true })
    {
        for (const uint32 leader : leaders)
        {
            const bool got  = ShouldLoadUnityChatOnOnlineAdd(found, leader);
            const bool want = !found && leader != 0;
            ok              = expect(got == want, "compose free == pin formula") && ok;
            ok              = expect(got == inlineShouldLoadUnityChatOnOnlineAdd(found, leader),
                        "compose free == inline") &&
                 ok;
        }
    }

    // Host-style compose: AddOnlineMember injects cache find + leader.
    const struct
    {
        bool        foundInCache;
        uint32      leader;
        bool        wantLoad;
        const char* label;
    } hosts[] = {
        { false, 7, true, "miss + leader → LoadUnityChat" },
        { true, 7, false, "hit → reuse cache, no LoadUnityChat" },
        { false, 0, false, "miss + leader 0 → no LoadUnityChat" },
        { true, 0, false, "hit + leader 0 → no LoadUnityChat" },
        { false, maxU32, true, "miss + max leader → LoadUnityChat" },
        { true, maxU32, false, "hit + max leader → reuse cache" },
    };
    for (const auto& h : hosts)
    {
        const bool load = ShouldLoadUnityChatOnOnlineAdd(h.foundInCache, h.leader);
        ok              = expect(load == h.wantLoad, h.label) && ok;
        ok              = expect(load == inlineShouldLoadUnityChatOnOnlineAdd(h.foundInCache, h.leader),
                    "host compose free == inline") &&
             ok;
        ok = expect(!(load && h.foundInCache), "must not load when cache hit") && ok;
        ok = expect(!(load && h.leader == 0), "must not load when leader 0") && ok;
        ok = expect(!(!load && !h.foundInCache && h.leader != 0), "miss + nonzero must load") && ok;
    }

    // Production AddOnlineMember path semantics after null reject.
    ok = expect(ShouldLoadUnityChatOnOnlineAdd(false, 9), "AddOnlineMember miss → LoadUnityChat path") && ok;
    ok = expect(!ShouldLoadUnityChatOnOnlineAdd(true, 9), "AddOnlineMember hit → reuse path") && ok;
    ok = expect(!ShouldLoadUnityChatOnOnlineAdd(false, 0), "AddOnlineMember leader 0 → skip load") && ok;

    // Residual sibling gates remain orthogonal to this dual-wire surface.
    ok = expect(ShouldRejectNullOnlineMember(true), "sibling residual: null char still rejects") && ok;
    ok = expect(ShouldAddMemberAfterOnlineLookup(true), "sibling residual: add when loaded") && ok;
    ok = expect(!ShouldAddMemberAfterOnlineLookup(false), "sibling residual: no add when null") && ok;
    ok = expect(!OnlineMemberAlwaysReturnsFalse(), "sibling residual: always returns false") && ok;

    // Explicit dual-wire poles: free == !found && leader!=0 for dense edges.
    for (const bool found : { false, true })
    {
        for (const uint32 leader : { uint32{ 0 }, uint32{ 1 }, maxU32 })
        {
            const bool got  = ShouldLoadUnityChatOnOnlineAdd(found, leader);
            const bool want = !found && leader != 0;
            ok              = expect(got == want, "host inject dual-wire identity") && ok;
            ok              = expect(got == inlineShouldLoadUnityChatOnOnlineAdd(found, leader),
                        "host inject free == inline") &&
                 ok;
        }
    }

    return ok;
}
