#include "test_follow_can_follow_2883.h"

#include "map/follow_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "follow CanFollow 2883 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua xi.follow.follow spawn+zone gate for dual-wire cross-check:
//   not (not leader:isSpawned() or not follower:isSpawned()
//        or leader:getZoneID() ~= follower:getZoneID())
// ≡ followerSpawned && leaderSpawned && followerZone == leaderZone
auto inlineCanFollow(const bool followerSpawned, const bool leaderSpawned, const uint16 followerZone, const uint16 leaderZone) -> bool
{
    return followerSpawned && leaderSpawned && followerZone == leaderZone;
}

} // namespace

// Pure dual-wire expansion for followhelpers::CanFollow
// (Lua xi.follow.follow spawn+zone eligibility gate).
auto runFollowCanFollow2883SelfTests() -> bool
{
    using followhelpers::CanFollow;

    bool ok = true;

    // Primary truth pins for CanFollow.
    ok = expect(CanFollow(true, true, 10, 10), "both spawned same zone") && ok;
    ok = expect(!CanFollow(false, true, 10, 10), "follower not spawned") && ok;
    ok = expect(!CanFollow(true, false, 10, 10), "leader not spawned") && ok;
    ok = expect(!CanFollow(false, false, 10, 10), "neither spawned") && ok;
    ok = expect(!CanFollow(true, true, 10, 11), "different zones") && ok;
    ok = expect(CanFollow(true, true, 0, 0), "zone zero same") && ok;

    // Dual-wire matches inline formula across a table.
    const struct
    {
        bool        followerSpawned;
        bool        leaderSpawned;
        uint16      followerZone;
        uint16      leaderZone;
        bool        want;
        const char* label;
    } cases[] = {
        { true, true, 10, 10, true, "table both spawned same zone" },
        { false, true, 10, 10, false, "table follower not spawned" },
        { true, false, 10, 10, false, "table leader not spawned" },
        { false, false, 10, 10, false, "table neither spawned" },
        { true, true, 10, 11, false, "table different zones" },
        { true, true, 0, 0, true, "table zone zero same" },
        { true, true, 255, 255, true, "table high zone same" },
        { true, true, 1, 0, false, "table zone one vs zero" },
        { false, false, 5, 6, false, "table all fail" },
        { true, false, 0, 0, false, "table leader down zone match" },
        { false, true, 0, 0, false, "table follower down zone match" },
    };

    for (const auto& c : cases)
    {
        const bool got       = CanFollow(c.followerSpawned, c.leaderSpawned, c.followerZone, c.leaderZone);
        const bool inlineGot = inlineCanFollow(c.followerSpawned, c.leaderSpawned, c.followerZone, c.leaderZone);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineGot, "dual-wire free == inline Lua formula") && ok;
    }

    // Host compose: zone equality is independent of spawn flags when either
    // entity is not spawned (gate still false).
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
    };

    for (const auto& c : composeCases)
    {
        const bool got       = CanFollow(c.followerSpawned, c.leaderSpawned, c.followerZone, c.leaderZone);
        const bool inlineGot = inlineCanFollow(c.followerSpawned, c.leaderSpawned, c.followerZone, c.leaderZone);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineGot, "compose dual-wire free == inline") && ok;
    }

    return ok;
}
