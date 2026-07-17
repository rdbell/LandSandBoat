#include "test_follow_can_follow_3084.h"

#include "map/follow_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "follow CanFollow 3084 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua xi.follow.follow spawn+zone gate for dual-wire cross-check
// (slice 3084; residual 2883):
//   not (not leader:isSpawned() or not follower:isSpawned()
//        or leader:getZoneID() ~= follower:getZoneID())
// ≡ followerSpawned && leaderSpawned && followerZone == leaderZone
auto inlineCanFollow(const bool followerSpawned, const bool leaderSpawned, const uint16 followerZone, const uint16 leaderZone) -> bool
{
    return followerSpawned && leaderSpawned && followerZone == leaderZone;
}

// Documented dual-wire pin formula (slice 3084).
auto pinCanFollow(const bool followerSpawned, const bool leaderSpawned, const uint16 followerZone, const uint16 leaderZone) -> bool
{
    return followerSpawned && leaderSpawned && followerZone == leaderZone;
}

} // namespace

// Pure dual-wire expansion for followhelpers::CanFollow
// (Lua xi.follow.follow spawn+zone eligibility gate; slice 3084 of residual 2883).
auto runFollowCanFollow3084SelfTests() -> bool
{
    using followhelpers::CanFollow;

    bool ok = true;

    // Residual 2883 / 0945 pins still hold under dual-wire.
    ok = expect(CanFollow(true, true, 10, 10), "residual both spawned same zone") && ok;
    ok = expect(!CanFollow(false, true, 10, 10), "residual follower not spawned") && ok;
    ok = expect(!CanFollow(true, false, 10, 10), "residual leader not spawned") && ok;
    ok = expect(!CanFollow(true, true, 10, 11), "residual different zones") && ok;

    const struct
    {
        bool        followerSpawned;
        bool        leaderSpawned;
        uint16      followerZone;
        uint16      leaderZone;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles.
        { true, true, 10, 10, true, "both spawned same zone" },
        { false, true, 10, 10, false, "follower not spawned" },
        { true, false, 10, 10, false, "leader not spawned" },
        { false, false, 10, 10, false, "neither spawned" },
        { true, true, 10, 11, false, "different zones" },

        // Residual 2883 pins.
        { true, true, 0, 0, true, "zone zero same" },
        { true, true, 255, 255, true, "high zone same" },
        { true, true, 1, 0, false, "zone one vs zero" },
        { false, false, 5, 6, false, "all fail" },
        { true, false, 0, 0, false, "leader down zone match" },
        { false, true, 0, 0, false, "follower down zone match" },

        // Extra dense poles.
        { true, true, 255, 254, false, "high zone mismatch" },
        { true, true, 65535, 65535, true, "max uint16 same" },
        { true, true, 65535, 0, false, "max vs zero" },
        { true, true, 1, 1, true, "spawned both zone 1" },
        { false, true, 1, 2, false, "follower only zone mismatch" },
        { true, false, 1, 2, false, "leader only zone mismatch" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanFollow(c.followerSpawned, c.leaderSpawned, c.followerZone, c.leaderZone);
        const bool inlineF = inlineCanFollow(c.followerSpawned, c.leaderSpawned, c.followerZone, c.leaderZone);
        const bool pin     = pinCanFollow(c.followerSpawned, c.leaderSpawned, c.followerZone, c.leaderZone);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "CanFollow dual-wire == inline Lua formula") && ok;
        ok = expect(got == pin, "CanFollow == pin formula") && ok;
    }

    // Dense compose: full 2^2 spawn flag space x same/different zone.
    // free == inline == pin formula.
    for (const bool followerSpawned : { false, true })
    {
        for (const bool leaderSpawned : { false, true })
        {
            for (const bool sameZone : { false, true })
            {
                const uint16 followerZone = 42;
                const uint16 leaderZone   = sameZone ? static_cast<uint16>(42) : static_cast<uint16>(43);
                const bool   got          = CanFollow(followerSpawned, leaderSpawned, followerZone, leaderZone);
                const bool   want         = followerSpawned && leaderSpawned && sameZone;
                ok                        = expect(got == want, "dense free == pin formula") && ok;
                ok                        = expect(got == inlineCanFollow(followerSpawned, leaderSpawned, followerZone, leaderZone),
                            "dense free == inline") &&
                     ok;
                ok = expect(got == pinCanFollow(followerSpawned, leaderSpawned, followerZone, leaderZone),
                            "dense free == pin") &&
                     ok;
            }
        }
    }

    // --- Production xi.follow.follow path semantics ---
    // Host injects:
    //   fSpawned = follower:isSpawned(); lSpawned = leader:isSpawned()
    //   fZone = follower:getZoneID(); lZone = leader:getZoneID()
    // when true  → host continues Follow bookkeeping / entity follow()
    // when false → return false without mutating state
    ok = expect(CanFollow(true, true, 100, 100), "xi.follow.follow eligible → continue path") && ok;
    ok = expect(!CanFollow(false, true, 100, 100), "xi.follow.follow follower unspawned → early return") && ok;
    ok = expect(!CanFollow(true, false, 100, 100), "xi.follow.follow leader unspawned → early return") && ok;
    ok = expect(!CanFollow(true, true, 100, 101), "xi.follow.follow zone mismatch → early return") && ok;

    // Explicit dual-wire: free == inline == pin for host poles.
    const struct
    {
        bool        followerSpawned;
        bool        leaderSpawned;
        uint16      followerZone;
        uint16      leaderZone;
        const char* label;
    } hostPoles[] = {
        { true, true, 10, 10, "eligible follow path" },
        { false, true, 10, 10, "follower unspawned early return" },
        { true, false, 10, 10, "leader unspawned early return" },
        { true, true, 10, 11, "zone mismatch early return" },
    };
    for (const auto& p : hostPoles)
    {
        const bool got     = CanFollow(p.followerSpawned, p.leaderSpawned, p.followerZone, p.leaderZone);
        const bool inlineF = inlineCanFollow(p.followerSpawned, p.leaderSpawned, p.followerZone, p.leaderZone);
        const bool pin     = pinCanFollow(p.followerSpawned, p.leaderSpawned, p.followerZone, p.leaderZone);
        ok                 = expect(got == pin, p.label) && ok;
        ok                 = expect(got == inlineF, "host inject dual-wire free == inline") && ok;
    }

    // Compose table: gate polarity (mirrors Go TestCanFollowCompose3084 cases).
    const struct
    {
        bool        followerSpawned;
        bool        leaderSpawned;
        uint16      followerZone;
        uint16      leaderZone;
        bool        want;
        const char* label;
    } composeCases[] = {
        { true, true, 100, 100, true, "compose eligible" },
        { true, true, 100, 101, false, "compose zone mismatch" },
        { false, true, 100, 100, false, "compose follower unspawned" },
        { true, false, 100, 100, false, "compose leader unspawned" },
        { false, false, 50, 50, false, "compose neither spawned" },
        { false, false, 50, 51, false, "compose both fail" },
        { true, true, 0, 0, true, "compose zone zero eligible" },
    };

    for (const auto& c : composeCases)
    {
        const bool got     = CanFollow(c.followerSpawned, c.leaderSpawned, c.followerZone, c.leaderZone);
        const bool inlineF = inlineCanFollow(c.followerSpawned, c.leaderSpawned, c.followerZone, c.leaderZone);
        const bool pin     = pinCanFollow(c.followerSpawned, c.leaderSpawned, c.followerZone, c.leaderZone);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "compose dual-wire free == inline") && ok;
        ok = expect(got == pin, "compose free == pin") && ok;
    }

    return ok;
}
