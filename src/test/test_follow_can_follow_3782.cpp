#include "test_follow_can_follow_3782.h"

#include "map/follow_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "follow CanFollow 3782 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua xi.follow.follow spawn+zone gate for dual-wire cross-check
// (slice 3782 dedicated expand residual 2883; prior dedicated 3737 / 3692 /
// 3647 / 3602 / 3557 / 3506 / 3459 / 3405 / 3331 / 3237 / 3084):
//   not (not leader:isSpawned() or not follower:isSpawned()
//        or leader:getZoneID() ~= follower:getZoneID())
// ≡ followerSpawned && leaderSpawned && followerZone == leaderZone
// Direct return form matching production free function / capacity.
auto inlineCanFollow(const bool followerSpawned, const bool leaderSpawned, const uint16 followerZone, const uint16 leaderZone) -> bool
{
    return followerSpawned && leaderSpawned && followerZone == leaderZone;
}

// Compact dual-wire pin matching free function / capacity body (slice 3782).
// Direct AND/EQ form only (no De Morgan) — same formula as production CanFollow.
auto pinCanFollow3782(const bool followerSpawned, const bool leaderSpawned, const uint16 followerZone, const uint16 leaderZone) -> bool
{
    return followerSpawned && leaderSpawned && followerZone == leaderZone;
}

// Prior dedicated 3737 pin retained for free == pin3737 dual-wire agreement.
auto pinCanFollow3737(const bool followerSpawned, const bool leaderSpawned, const uint16 followerZone, const uint16 leaderZone) -> bool
{
    return followerSpawned && leaderSpawned && followerZone == leaderZone;
}

} // namespace

// Pure dual-wire expansion for followhelpers::CanFollow
// (Lua xi.follow.follow spawn+zone eligibility gate; slice 3782 dedicated
// expand residual 2883; prior dedicated 3737 / 3692 / 3647 / 3602 / 3557 /
// 3506 / 3459 / 3405 / 3331 / 3237 / 3084). Formula unchanged.
//
// Coverage:
//   - free == inline == pin == pin3737 (direct AND/EQ form, no De Morgan)
//   - residual 2883 / 0945 / prior dedicated 3084 / 3237 / 3331 / 3405 / 3459 / 3506 / 3557 / 3602 / 3647 / 3692 / 3737 poles still hold
//   - residual poles + dense bool combos × zone equal/unequal
//     (0, same, different, max uint16)
//   - spawn/zone poles; same-zone vs cross-zone; both unspawned
auto runFollowCanFollow3782SelfTests() -> bool
{
    using followhelpers::CanFollow;

    bool ok = true;

    // Residual 2883 / 0945 / prior dedicated 3084 / 3237 / 3331 / 3405 / 3459 / 3506 / 3557 / 3602 / 3647 / 3692 / 3737 pins still hold.
    ok = expect(CanFollow(true, true, 10, 10), "residual both spawned same zone") && ok;
    ok = expect(!CanFollow(false, true, 10, 10), "residual follower not spawned") && ok;
    ok = expect(!CanFollow(true, false, 10, 10), "residual leader not spawned") && ok;
    ok = expect(!CanFollow(true, true, 10, 11), "residual different zones") && ok;
    ok = expect(CanFollow(true, true, 0, 0), "residual zone zero same") && ok;
    ok = expect(CanFollow(true, true, 65535, 65535), "residual max uint16 same") && ok;
    ok = expect(!CanFollow(true, true, 65535, 0), "residual max vs zero") && ok;

    const struct
    {
        bool        followerSpawned;
        bool        leaderSpawned;
        uint16      followerZone;
        uint16      leaderZone;
        bool        want;
        const char* label;
    } poles[] = {
        // Residual 2883 / 3084 classic dual poles.
        { true, true, 10, 10, true, "residual both spawned same zone" },
        { false, true, 10, 10, false, "residual follower not spawned" },
        { true, false, 10, 10, false, "residual leader not spawned" },
        { false, false, 10, 10, false, "residual neither spawned" },
        { true, true, 10, 11, false, "residual different zones" },
        { true, true, 0, 0, true, "residual zone zero same" },
        { true, true, 255, 255, true, "residual high zone same" },
        { true, true, 1, 0, false, "residual zone one vs zero" },
        { false, false, 5, 6, false, "residual all fail" },
        { true, false, 0, 0, false, "residual leader down zone match" },
        { false, true, 0, 0, false, "residual follower down zone match" },

        // Prior dedicated 3084 dense poles.
        { true, true, 255, 254, false, "prior high zone mismatch" },
        { true, true, 65535, 65535, true, "prior max uint16 same" },
        { true, true, 65535, 0, false, "prior max vs zero" },
        { true, true, 1, 1, true, "prior spawned both zone 1" },
        { false, true, 1, 2, false, "prior follower only zone mismatch" },
        { true, false, 1, 2, false, "prior leader only zone mismatch" },

        // Prior dedicated 3237 expand poles.
        { true, true, 100, 100, true, "prior3237 host eligible continue path" },
        { false, true, 100, 100, false, "prior3237 host follower unspawned" },
        { true, false, 100, 100, false, "prior3237 host leader unspawned" },
        { true, true, 100, 101, false, "prior3237 host zone mismatch" },

        // Prior dedicated 3331 expand poles.
        { true, true, 100, 100, true, "prior3331 host eligible continue path" },
        { false, true, 100, 100, false, "prior3331 host follower unspawned" },
        { true, false, 100, 100, false, "prior3331 host leader unspawned" },
        { true, true, 100, 101, false, "prior3331 host zone mismatch" },

        // Prior dedicated 3405 expand poles.
        { true, true, 100, 100, true, "prior3405 host eligible continue path" },
        { false, true, 100, 100, false, "prior3405 host follower unspawned" },
        { true, false, 100, 100, false, "prior3405 host leader unspawned" },
        { true, true, 100, 101, false, "prior3405 host zone mismatch" },

        // Prior dedicated 3459 expand poles.
        { true, true, 100, 100, true, "prior3459 host eligible continue path" },
        { false, true, 100, 100, false, "prior3459 host follower unspawned" },
        { true, false, 100, 100, false, "prior3459 host leader unspawned" },
        { true, true, 100, 101, false, "prior3459 host zone mismatch" },

        // Prior dedicated 3506 expand poles.
        { true, true, 100, 100, true, "prior3506 host eligible continue path" },
        { false, true, 100, 100, false, "prior3506 host follower unspawned" },
        { true, false, 100, 100, false, "prior3506 host leader unspawned" },
        { true, true, 100, 101, false, "prior3506 host zone mismatch" },

        // Prior dedicated 3557 expand poles.
        { true, true, 100, 100, true, "prior3557 host eligible continue path" },
        { false, true, 100, 100, false, "prior3557 host follower unspawned" },
        { true, false, 100, 100, false, "prior3557 host leader unspawned" },
        { true, true, 100, 101, false, "prior3557 host zone mismatch" },

        // Prior dedicated 3602 expand poles.
        { true, true, 100, 100, true, "prior3602 host eligible continue path" },
        { false, true, 100, 100, false, "prior3602 host follower unspawned" },
        { true, false, 100, 100, false, "prior3602 host leader unspawned" },
        { true, true, 100, 101, false, "prior3602 host zone mismatch" },

        // Prior dedicated 3647 expand poles.
        { true, true, 100, 100, true, "prior3647 host eligible continue path" },
        { false, true, 100, 100, false, "prior3647 host follower unspawned" },
        { true, false, 100, 100, false, "prior3647 host leader unspawned" },
        { true, true, 100, 101, false, "prior3647 host zone mismatch" },

        // Prior dedicated 3692 expand poles.
        { true, true, 100, 100, true, "prior3692 host eligible continue path" },
        { false, true, 100, 100, false, "prior3692 host follower unspawned" },
        { true, false, 100, 100, false, "prior3692 host leader unspawned" },
        { true, true, 100, 101, false, "prior3692 host zone mismatch" },

        // Prior dedicated 3737 expand poles.
        { true, true, 100, 100, true, "prior3737 host eligible continue path" },
        { false, true, 100, 100, false, "prior3737 host follower unspawned" },
        { true, false, 100, 100, false, "prior3737 host leader unspawned" },
        { true, true, 100, 101, false, "prior3737 host zone mismatch" },

        // Host inject path poles (xi.follow.follow).
        { true, true, 100, 100, true, "host eligible continue path" },
        { false, true, 100, 100, false, "host follower unspawned early return" },
        { true, false, 100, 100, false, "host leader unspawned early return" },
        { true, true, 100, 101, false, "host zone mismatch early return" },
    };

    for (const auto& p : poles)
    {
        const bool got     = CanFollow(p.followerSpawned, p.leaderSpawned, p.followerZone, p.leaderZone);
        const bool inlineF = inlineCanFollow(p.followerSpawned, p.leaderSpawned, p.followerZone, p.leaderZone);
        const bool pin     = pinCanFollow3782(p.followerSpawned, p.leaderSpawned, p.followerZone, p.leaderZone);
        const bool pin3737 = pinCanFollow3737(p.followerSpawned, p.leaderSpawned, p.followerZone, p.leaderZone);

        ok = expect(got == p.want, p.label) && ok;
        ok = expect(got == inlineF, "CanFollow dual-wire == inline Lua formula") && ok;
        ok = expect(got == pin, "CanFollow == pin formula") && ok;
        ok = expect(got == pin3737, "CanFollow == pin3737 formula") && ok;
    }

    // Dense bool combos × zone equal/unequal (spawn/zone poles):
    //   zones: 0 same, mid same, different, max uint16 same, max vs 0, max vs mid
    // free == inline == pin == pin3737 == formula (direct AND/EQ, no De Morgan).
    const struct
    {
        uint16      followerZone;
        uint16      leaderZone;
        bool        equal;
        const char* label;
    } zonePairs[] = {
        { 0, 0, true, "zone zero equal" },
        { 42, 42, true, "zone mid equal" },
        { 42, 43, false, "zone mid unequal" },
        { 1, 0, false, "zone one vs zero unequal" },
        { 65535, 65535, true, "zone max uint16 equal" },
        { 65535, 0, false, "zone max vs zero unequal" },
        { 65535, 42, false, "zone max vs mid unequal" },
        { 255, 255, true, "zone high equal" },
        { 255, 254, false, "zone high unequal" },
    };

    for (const bool followerSpawned : { false, true })
    {
        for (const bool leaderSpawned : { false, true })
        {
            for (const auto& zp : zonePairs)
            {
                const bool got     = CanFollow(followerSpawned, leaderSpawned, zp.followerZone, zp.leaderZone);
                const bool inlineF = inlineCanFollow(followerSpawned, leaderSpawned, zp.followerZone, zp.leaderZone);
                const bool pin     = pinCanFollow3782(followerSpawned, leaderSpawned, zp.followerZone, zp.leaderZone);
                const bool pin3737 = pinCanFollow3737(followerSpawned, leaderSpawned, zp.followerZone, zp.leaderZone);
                const bool want    = followerSpawned && leaderSpawned && zp.equal;

                ok = expect(got == want, "dense free == pin formula") && ok;
                ok = expect(got == inlineF, "dense free == inline") && ok;
                ok = expect(got == pin, "dense free == pin") && ok;
                ok = expect(got == pin3737, "dense free == pin3737") && ok;
            }
        }
    }

    // Explicit free-path poles matching capacity body (direct AND/EQ).
    ok = expect(CanFollow(true, true, 0, 0), "zone zero same must apply") && ok;
    ok = expect(CanFollow(true, true, 65535, 65535), "max uint16 same must apply") && ok;
    ok = expect(!CanFollow(true, true, 0, 65535), "zero vs max must reject") && ok;
    ok = expect(!CanFollow(false, false, 65535, 65535), "neither spawned max zone must reject") && ok;
    ok = expect(!CanFollow(true, false, 65535, 65535), "leader unspawned max zone must reject") && ok;
    ok = expect(!CanFollow(false, true, 0, 0), "follower unspawned zone zero must reject") && ok;

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

    // Explicit dual-wire: free == inline == pin == pin3737 for host poles.
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
        { true, true, 0, 0, "zone zero eligible" },
        { true, true, 65535, 65535, "max zone eligible" },
        { true, true, 65535, 0, "max vs zero reject" },
    };
    for (const auto& p : hostPoles)
    {
        const bool got     = CanFollow(p.followerSpawned, p.leaderSpawned, p.followerZone, p.leaderZone);
        const bool inlineF = inlineCanFollow(p.followerSpawned, p.leaderSpawned, p.followerZone, p.leaderZone);
        const bool pin     = pinCanFollow3782(p.followerSpawned, p.leaderSpawned, p.followerZone, p.leaderZone);
        const bool pin3737 = pinCanFollow3737(p.followerSpawned, p.leaderSpawned, p.followerZone, p.leaderZone);
        ok                 = expect(got == pin, p.label) && ok;
        ok                 = expect(got == inlineF, "host inject dual-wire free == inline") && ok;
        ok                 = expect(got == pin3737, "host inject dual-wire free == pin3737") && ok;
    }

    return ok;
}
