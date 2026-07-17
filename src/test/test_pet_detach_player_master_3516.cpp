#include "test_pet_detach_player_master_3516.h"

#include "map/pet_death_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "pet ShouldDetachPlayerMaster 3516 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CPetEntity::Die detach-player-master formula for dual-wire cross-check
// (dedicated slice 3516 expand residual 2987; prior dedicated 3468 / 3414 / 3337) —
// direct return AND-chain:
//   hasMaster && masterPetIsSelf && masterIsPlayer
auto inlineShouldDetachPlayerMaster(const bool hasMaster, const bool masterPetIsSelf,
                                    const bool masterIsPlayer) -> bool
{
    return hasMaster && masterPetIsSelf && masterIsPlayer;
}

// Positive if/else pin matching free function / capacity body (slice 3516).
// Avoid De Morgan rewrites of the three-bool conjunction (QF1001).
auto pinShouldDetachPlayerMaster(const bool hasMaster, const bool masterPetIsSelf,
                                 const bool masterIsPlayer) -> bool
{
    if (hasMaster)
    {
        if (masterPetIsSelf)
        {
            if (masterIsPlayer)
            {
                return true;
            }
        }
    }
    return false;
}

// Prior dedicated 3468 pin (independence under 3516 expand).
auto pinShouldDetachPlayerMaster3468(const bool hasMaster, const bool masterPetIsSelf,
                                     const bool masterIsPlayer) -> bool
{
    if (hasMaster)
    {
        if (masterPetIsSelf)
        {
            if (masterIsPlayer)
            {
                return true;
            }
        }
    }
    return false;
}

// Prior dedicated 3414 pin (independence under 3516 expand).
auto pinShouldDetachPlayerMaster3414(const bool hasMaster, const bool masterPetIsSelf,
                                     const bool masterIsPlayer) -> bool
{
    if (hasMaster)
    {
        if (masterPetIsSelf)
        {
            if (masterIsPlayer)
            {
                return true;
            }
        }
    }
    return false;
}

// Prior dedicated 3337 pin (independence under 3516 expand).
auto pinShouldDetachPlayerMaster3337(const bool hasMaster, const bool masterPetIsSelf,
                                     const bool masterIsPlayer) -> bool
{
    if (hasMaster)
    {
        if (masterPetIsSelf)
        {
            if (masterIsPlayer)
            {
                return true;
            }
        }
    }
    return false;
}

} // namespace

// Pure dual-wire expansion for petdeathhelpers::ShouldDetachPlayerMaster
// (CPetEntity::Die player-master active-pet detach gate; dedicated slice 3516;
// residual expand 2987 / pure 1414 / 2262; prior dedicated 3468 / 3414 / 3337).
//
// Coverage:
//   - free == inline == pin (positive if/else form; avoid QF1001)
//   - residual 1414 / 2262 / 2987 / prior 3468 / 3414 / 3337 pins still hold
//   - dense 2^3 poles
//   - sibling ShouldDespawnForZoning (3233) residual only
auto runPetDetachPlayerMaster3516SelfTests() -> bool
{
    using petdeathhelpers::ShouldDetachPlayerMaster;
    using petdeathhelpers::ShouldDespawnForZoning;

    bool ok = true;

    // Residual 1414 / 2262 / 2987 / prior 3468 / 3414 / 3337 pins still hold under dual-wire.
    ok = expect(ShouldDetachPlayerMaster(true, true, true), "residual: attached player master pet") && ok;
    ok = expect(!ShouldDetachPlayerMaster(false, true, true), "residual: missing master") && ok;
    ok = expect(!ShouldDetachPlayerMaster(true, false, true), "residual: different active pet") && ok;
    ok = expect(!ShouldDetachPlayerMaster(true, true, false), "residual: non-player master") && ok;

    // --- Composition table: free == inline == pin (positive if/else) ---
    const struct
    {
        bool        hasMaster;
        bool        masterPetIsSelf;
        bool        masterIsPlayer;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic accept pole — all three true.
        { true, true, true, true, "attached player master pet" },

        // Residual 1414 / 2262 / 2987 / prior 3468 / 3414 / 3337 single-false poles.
        { false, true, true, false, "missing master" },
        { true, false, true, false, "different active pet" },
        { true, true, false, false, "non-player master" },

        // Additional multi-false poles.
        { false, false, false, false, "all false" },
        { false, true, false, false, "missing master + non-player" },
        { true, false, false, false, "different pet + non-player" },
        { false, false, true, false, "missing master + not self" },
        { false, true, true, false, "missing master self+player inject" },
        { true, false, true, false, "not self player inject" },
        { true, true, false, false, "not player self inject" },
        { false, false, false, false, "only false cascade" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldDetachPlayerMaster(c.hasMaster, c.masterPetIsSelf, c.masterIsPlayer);
        const bool inlineF = inlineShouldDetachPlayerMaster(c.hasMaster, c.masterPetIsSelf, c.masterIsPlayer);
        const bool pin     = pinShouldDetachPlayerMaster(c.hasMaster, c.masterPetIsSelf, c.masterIsPlayer);
        const bool wantAnd = c.hasMaster && c.masterPetIsSelf && c.masterIsPlayer;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF && got == pin, "ShouldDetachPlayerMaster dual-wire free==inline==pin") && ok;
        ok = expect(got == wantAnd, "ShouldDetachPlayerMaster == AND of three") && ok;
    }

    // Pin composition: only the full-true pole accepts.
    ok = expect(ShouldDetachPlayerMaster(true, true, true), "all true must accept") && ok;
    ok = expect(!ShouldDetachPlayerMaster(false, true, true), "hasMaster false must reject") && ok;
    ok = expect(!ShouldDetachPlayerMaster(true, false, true), "masterPetIsSelf false must reject") && ok;
    ok = expect(!ShouldDetachPlayerMaster(true, true, false), "masterIsPlayer false must reject") && ok;

    // Dense compose: full 2^3 boolean space — free == inline == pin.
    for (const bool hasMaster : { false, true })
    {
        for (const bool masterPetIsSelf : { false, true })
        {
            for (const bool masterIsPlayer : { false, true })
            {
                const bool got     = ShouldDetachPlayerMaster(hasMaster, masterPetIsSelf, masterIsPlayer);
                const bool inlineF = inlineShouldDetachPlayerMaster(hasMaster, masterPetIsSelf, masterIsPlayer);
                const bool pin     = pinShouldDetachPlayerMaster(hasMaster, masterPetIsSelf, masterIsPlayer);
                const bool want    = hasMaster && masterPetIsSelf && masterIsPlayer;
                ok                 = expect(got == want, "compose free == pin formula") && ok;
                ok                 = expect(got == inlineF && got == pin, "compose free == inline == pin") && ok;
            }
        }
    }

    // --- Production CPetEntity::Die path semantics ---
    // Host injects:
    //   hasMaster       = PMaster != nullptr
    //   masterPetIsSelf = PMaster != nullptr && PMaster->PPet == this
    //   masterIsPlayer  = PMaster != nullptr && PMaster->objtype == TYPE_PC
    // when true  → petutils::DetachPet(PMaster) branch of Apply
    // when false → leave master's PPet alone
    ok = expect(ShouldDetachPlayerMaster(true, true, true), "Die attached player master pet → detach path") && ok;
    ok = expect(!ShouldDetachPlayerMaster(true, false, true), "Die different active pet → no detach") && ok;

    // Host inject poles: free == inline == pin.
    const struct
    {
        bool        master;
        bool        self;
        bool        player;
        const char* label;
    } hostPoles[] = {
        { true, true, true, "full accept" },
        { false, false, false, "no master cascade" },
        { true, false, true, "different active pet" },
        { true, true, false, "non-PC master" },
        { false, true, true, "missing master with self/player inject" },
        { true, false, false, "different pet non-player" },
        { false, false, true, "missing master player inject" },
        { false, true, false, "missing master self inject" },
    };

    for (const auto& p : hostPoles)
    {
        const bool got     = ShouldDetachPlayerMaster(p.master, p.self, p.player);
        const bool inlineF = inlineShouldDetachPlayerMaster(p.master, p.self, p.player);
        const bool pin     = pinShouldDetachPlayerMaster(p.master, p.self, p.player);
        const bool want    = p.master && p.self && p.player;
        ok                 = expect(got == want, p.label) && ok;
        ok                 = expect(got == inlineF && got == pin, "host inject free == inline == pin") && ok;
    }

    // ApplyDeath host-style: detach gate selects detachMaster callback.
    // Dual-wire only pins the gate; orchestration remains residual 1414.
    ok = expect(ShouldDetachPlayerMaster(true, true, true), "ApplyDeath detach=true when gate accepts") && ok;
    ok = expect(!ShouldDetachPlayerMaster(true, false, true),
                "ApplyDeath detach=false when pet is not master's active pet") &&
         ok;

    // Sibling residual 3233 still holds (not re-expanded under 3516).
    ok = expect(ShouldDespawnForZoning(true, true, true, true),
                "sibling residual: full zoning despawn accepts (3233)") &&
         ok;
    ok = expect(!ShouldDespawnForZoning(false, true, true, true),
                "sibling residual: dead pet rejects zoning (3233)") &&
         ok;
    ok = expect(!ShouldDespawnForZoning(true, false, true, true),
                "sibling residual: missing master rejects zoning (3233)") &&
         ok;
    ok = expect(!ShouldDespawnForZoning(true, true, false, true),
                "sibling residual: non-player master rejects zoning (3233)") &&
         ok;
    ok = expect(!ShouldDespawnForZoning(true, true, true, false),
                "sibling residual: no respawn flag rejects zoning (3233)") &&
         ok;

    // Prior dedicated 3468 pin independence under 3516 expand.
    ok = expect(ShouldDetachPlayerMaster(true, true, true) ==
                    pinShouldDetachPlayerMaster3468(true, true, true),
                "prior 3468 pin under 3516 for full accept") &&
         ok;
    ok = expect(ShouldDetachPlayerMaster(false, true, true) ==
                    pinShouldDetachPlayerMaster3468(false, true, true),
                "prior 3468 pin under 3516 for missing master") &&
         ok;
    ok = expect(ShouldDetachPlayerMaster(true, false, true) ==
                    pinShouldDetachPlayerMaster3468(true, false, true),
                "prior 3468 pin under 3516 for different active pet") &&
         ok;
    ok = expect(ShouldDetachPlayerMaster(true, true, false) ==
                    pinShouldDetachPlayerMaster3468(true, true, false),
                "prior 3468 pin under 3516 for non-player master") &&
         ok;

    // Prior dedicated 3414 pin independence under 3516 expand.
    ok = expect(ShouldDetachPlayerMaster(true, true, true) ==
                    pinShouldDetachPlayerMaster3414(true, true, true),
                "prior 3414 pin under 3516 for full accept") &&
         ok;
    ok = expect(ShouldDetachPlayerMaster(false, true, true) ==
                    pinShouldDetachPlayerMaster3414(false, true, true),
                "prior 3414 pin under 3516 for missing master") &&
         ok;
    ok = expect(ShouldDetachPlayerMaster(true, false, true) ==
                    pinShouldDetachPlayerMaster3414(true, false, true),
                "prior 3414 pin under 3516 for different active pet") &&
         ok;
    ok = expect(ShouldDetachPlayerMaster(true, true, false) ==
                    pinShouldDetachPlayerMaster3414(true, true, false),
                "prior 3414 pin under 3516 for non-player master") &&
         ok;

    // Prior dedicated 3337 pin independence under 3516 expand.
    ok = expect(ShouldDetachPlayerMaster(true, true, true) ==
                    pinShouldDetachPlayerMaster3337(true, true, true),
                "prior 3337 pin under 3516 for full accept") &&
         ok;
    ok = expect(ShouldDetachPlayerMaster(false, true, true) ==
                    pinShouldDetachPlayerMaster3337(false, true, true),
                "prior 3337 pin under 3516 for missing master") &&
         ok;
    ok = expect(ShouldDetachPlayerMaster(true, false, true) ==
                    pinShouldDetachPlayerMaster3337(true, false, true),
                "prior 3337 pin under 3516 for different active pet") &&
         ok;
    ok = expect(ShouldDetachPlayerMaster(true, true, false) ==
                    pinShouldDetachPlayerMaster3337(true, true, false),
                "prior 3337 pin under 3516 for non-player master") &&
         ok;

    // Residual 2987 suite still holds under dedicated expand.
    ok = expect(ShouldDetachPlayerMaster(true, true, true), "residual 2987 attached under 3516") && ok;
    ok = expect(!ShouldDetachPlayerMaster(false, true, true), "residual 2987 missing master under 3516") && ok;
    ok = expect(!ShouldDetachPlayerMaster(true, false, true), "residual 2987 different active pet under 3516") && ok;
    ok = expect(!ShouldDetachPlayerMaster(true, true, false), "residual 2987 non-player master under 3516") && ok;

    return ok;
}
