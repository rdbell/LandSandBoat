#include "test_unity_load_online_add_3366.h"

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
        std::cerr << "unitychat ShouldLoadUnityChatOnOnlineAdd 3366 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline AddOnlineMember LoadUnityChat gate for dual-wire cross-check
// (slice 3366 dedicated expand residual 3050):
//   !foundInCache && leader != 0
// Direct return form matching production free function / capacity.
auto inlineShouldLoadUnityChatOnOnlineAdd(const bool foundInCache, const uint32 leader) -> bool
{
    return !foundInCache && leader != 0;
}

// Compact dual-wire pin matching free function / capacity body (slice 3366).
// Direct return only — same formula as production ShouldLoadUnityChatOnOnlineAdd.
auto pinShouldLoadUnityChatOnOnlineAdd3366(const bool foundInCache, const uint32 leader) -> bool
{
    return !foundInCache && leader != 0;
}

// Residual 3050 pin form retained for free == residual pin cross-check.
auto pinShouldLoadUnityChatOnOnlineAdd3050(const bool foundInCache, const uint32 leader) -> bool
{
    return !foundInCache && leader != 0;
}

} // namespace

// Pure dual-wire expansion for unitychathelpers::ShouldLoadUnityChatOnOnlineAdd
// (!foundInCache && leader != 0; OmegaXI internal/unitychat;
// slice 3366 dedicated expand residual 3050). Formula unchanged.
//
// Coverage:
//   - free == inline == pin (direct return formula)
//   - residual 1356 / 3050 pins still hold
//   - dense foundInCache × leader edges
//   - residual dual-wire suite retained: test_unity_load_online_add_3050
auto runUnityLoadOnlineAdd3366SelfTests() -> bool
{
    using unitychathelpers::FormatOnlineMemberNullWarning;
    using unitychathelpers::OnlineMemberAlwaysReturnsFalse;
    using unitychathelpers::ShouldAddMemberAfterOnlineLookup;
    using unitychathelpers::ShouldLoadUnityChatOnOnlineAdd;
    using unitychathelpers::ShouldRejectNullOnlineMember;

    bool ok = true;

    // Residual 1356 / 3050 pins still hold under dedicated dual-wire.
    ok = expect(ShouldLoadUnityChatOnOnlineAdd(false, 5), "residual miss + leader → load") && ok;
    ok = expect(!ShouldLoadUnityChatOnOnlineAdd(true, 5), "residual hit → no load") && ok;
    ok = expect(!ShouldLoadUnityChatOnOnlineAdd(false, 0), "residual leader 0 → no load") && ok;
    ok = expect(pinShouldLoadUnityChatOnOnlineAdd3050(false, 5), "prior residual 3050 miss load") && ok;
    ok = expect(!pinShouldLoadUnityChatOnOnlineAdd3050(true, 5), "prior residual 3050 hit skip") && ok;
    ok = expect(!pinShouldLoadUnityChatOnOnlineAdd3050(false, 0), "prior residual 3050 leader 0 skip") && ok;

    constexpr uint32 maxU32 = std::numeric_limits<uint32>::max();

    const struct
    {
        bool        foundInCache;
        uint32      leader;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles / residual 1356 / 3050.
        { false, 5, true, "residual cache miss nonzero leader loads" },
        { true, 5, false, "residual cache hit skips load" },
        { false, 0, false, "residual leader zero skips load" },
        { true, 0, false, "residual cache hit and leader zero skips" },

        // Nonzero edge: 1 and max uint32 both load on miss.
        { false, 1, true, "cache miss leader 1 loads" },
        { false, maxU32, true, "cache miss max uint32 loads" },
        { true, 1, false, "cache hit leader 1 skips" },
        { true, maxU32, false, "cache hit max uint32 skips" },

        // Residual polarity repeats for dual-wire stability.
        { false, 42, true, "identity miss+leader load" },
        { true, 42, false, "identity hit no load" },
        { false, 0, false, "identity miss+zero no load" },

        // Prior residual 3050 re-pins.
        { false, 7, true, "prior residual 3050 miss + leader loads" },
        { true, 7, false, "prior residual 3050 hit skips" },
        { false, 0, false, "prior residual 3050 leader 0 skips" },
        { true, 0, false, "prior residual 3050 hit + zero skips" },

        // Host inject path poles (AddOnlineMember after null reject).
        { false, 9, true, "host miss + leader → LoadUnityChat" },
        { true, 9, false, "host hit → reuse cache" },
        { false, 0, false, "host miss + leader 0 → no load" },
        { true, maxU32, false, "host hit + max leader → reuse" },
        { false, maxU32, true, "host miss + max leader → LoadUnityChat" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldLoadUnityChatOnOnlineAdd(c.foundInCache, c.leader);
        const bool inlineF = inlineShouldLoadUnityChatOnOnlineAdd(c.foundInCache, c.leader);
        const bool pin     = pinShouldLoadUnityChatOnOnlineAdd3366(c.foundInCache, c.leader);
        const bool wantF   = !c.foundInCache && c.leader != 0;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldLoadUnityChatOnOnlineAdd dual-wire == inline LSB formula") && ok;
        ok = expect(got == pin, "ShouldLoadUnityChatOnOnlineAdd == pin formula") && ok;
        ok = expect(got == wantF, "formula free==!found && leader!=0") && ok;
        ok = expect(got == pinShouldLoadUnityChatOnOnlineAdd3050(c.foundInCache, c.leader),
                    "free == residual pin 3050") &&
             ok;
    }

    // Pin composition: load only on miss with nonzero leader.
    ok = expect(ShouldLoadUnityChatOnOnlineAdd(false, 1), "miss + leader 1 must load") && ok;
    ok = expect(!ShouldLoadUnityChatOnOnlineAdd(true, 1), "hit must not load") && ok;
    ok = expect(!ShouldLoadUnityChatOnOnlineAdd(false, 0), "leader 0 must not load") && ok;

    // Dense compose: foundInCache × leader edges free == inline == pin.
    const uint32 leaders[] = { 0, 1, 5, 7, 9, 42, maxU32 };
    for (const bool found : { false, true })
    {
        for (const uint32 leader : leaders)
        {
            const bool got     = ShouldLoadUnityChatOnOnlineAdd(found, leader);
            const bool want    = !found && leader != 0;
            const bool inlineF = inlineShouldLoadUnityChatOnOnlineAdd(found, leader);
            const bool pin     = pinShouldLoadUnityChatOnOnlineAdd3366(found, leader);

            ok = expect(got == want, "compose free == pin formula") && ok;
            ok = expect(got == inlineF, "compose free == inline") && ok;
            ok = expect(got == pin, "compose free == pin 3366") && ok;
            ok = expect(got == pinShouldLoadUnityChatOnOnlineAdd3050(found, leader),
                        "compose free == residual pin 3050") &&
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
        { false, 1, true, "miss + leader 1 → LoadUnityChat" },
        { true, 1, false, "hit + leader 1 → reuse cache" },
    };
    for (const auto& h : hosts)
    {
        const bool load    = ShouldLoadUnityChatOnOnlineAdd(h.foundInCache, h.leader);
        const bool inlineF = inlineShouldLoadUnityChatOnOnlineAdd(h.foundInCache, h.leader);
        const bool pin     = pinShouldLoadUnityChatOnOnlineAdd3366(h.foundInCache, h.leader);
        ok                 = expect(load == h.wantLoad, h.label) && ok;
        ok                 = expect(load == inlineF, "host compose free == inline") && ok;
        ok                 = expect(load == pin, "host compose free == pin") && ok;
        ok                 = expect(load == pinShouldLoadUnityChatOnOnlineAdd3050(h.foundInCache, h.leader),
                    "host compose free == residual pin 3050") &&
             ok;
        ok = expect(!(load && h.foundInCache), "must not load when cache hit") && ok;
        ok = expect(!(load && h.leader == 0), "must not load when leader 0") && ok;
        ok = expect(!(!load && !h.foundInCache && h.leader != 0), "miss + nonzero must load") && ok;
    }

    // Production AddOnlineMember path semantics after null reject.
    ok = expect(ShouldLoadUnityChatOnOnlineAdd(false, 9), "AddOnlineMember miss → LoadUnityChat path") && ok;
    ok = expect(!ShouldLoadUnityChatOnOnlineAdd(true, 9), "AddOnlineMember hit → reuse path") && ok;
    ok = expect(!ShouldLoadUnityChatOnOnlineAdd(false, 0), "AddOnlineMember leader 0 → skip load") && ok;

    // Sibling dual-wire gates remain orthogonal (do not thrash add-member-after-lookup).
    ok = expect(ShouldRejectNullOnlineMember(true), "sibling residual: null char still rejects") && ok;
    ok = expect(!ShouldRejectNullOnlineMember(false), "sibling residual: non-null proceeds") && ok;
    ok = expect(ShouldAddMemberAfterOnlineLookup(true), "sibling residual: add when loaded") && ok;
    ok = expect(!ShouldAddMemberAfterOnlineLookup(false), "sibling residual: no add when null") && ok;
    ok = expect(!OnlineMemberAlwaysReturnsFalse(), "sibling residual: always returns false") && ok;
    ok = expect(FormatOnlineMemberNullWarning() == "PChar is null.", "sibling residual: null warning") && ok;

    // Explicit dual-wire poles: free == !found && leader!=0 for dense edges.
    for (const bool found : { false, true })
    {
        for (const uint32 leader : { uint32{ 0 }, uint32{ 1 }, uint32{ 5 }, maxU32 })
        {
            const bool got  = ShouldLoadUnityChatOnOnlineAdd(found, leader);
            const bool want = !found && leader != 0;
            ok              = expect(got == want, "host inject dual-wire identity") && ok;
            ok              = expect(got == inlineShouldLoadUnityChatOnOnlineAdd(found, leader),
                        "host inject free == inline") &&
                 ok;
            ok = expect(got == pinShouldLoadUnityChatOnOnlineAdd3366(found, leader),
                        "host inject free == pin 3366") &&
                 ok;
            ok = expect(got == pinShouldLoadUnityChatOnOnlineAdd3050(found, leader),
                        "host inject free == residual pin 3050") &&
                 ok;
        }
    }

    return ok;
}
