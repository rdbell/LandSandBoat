#include "test_petentity_despawn_zoning_2951.h"

#include "map/pet_death_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "petentity ShouldDespawnForZoning 2951 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CPetEntity::Die zoning-despawn formula for dual-wire cross-check
// (slice 2951):
//   hpPositive && hasMaster && masterIsPlayer && respawnPet
auto inlineShouldDespawnForZoning(const bool hpPositive, const bool hasMaster, const bool masterIsPlayer,
                                  const bool respawnPet) -> bool
{
    return hpPositive && hasMaster && masterIsPlayer && respawnPet;
}

} // namespace

// Pure dual-wire expansion for petdeathhelpers::ShouldDespawnForZoning
// (CPetEntity::Die living player-pet zoning-despawn gate; slice 2951).
auto runPetentityDespawnZoning2951SelfTests() -> bool
{
    using petdeathhelpers::ShouldDespawnForZoning;

    bool ok = true;

    const struct
    {
        bool        hpPositive;
        bool        hasMaster;
        bool        masterIsPlayer;
        bool        respawnPet;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic accept pole — all four true.
        { true, true, true, true, true, "living player pet with respawn flag" },

        // Residual 1414 / 2261 single-false poles.
        { false, true, true, true, false, "dead pet" },
        { true, false, true, true, false, "missing master" },
        { true, true, false, true, false, "non-player master" },
        { true, true, true, false, false, "no respawn flag" },

        // Additional multi-false poles.
        { false, false, false, false, false, "all false" },
        { false, true, true, false, false, "dead + no respawn" },
        { true, false, false, true, false, "no master + non-player inject" },
        { true, true, false, false, false, "non-player + no respawn" },
        { false, false, true, true, false, "dead + missing master" },
        { true, false, true, false, false, "missing master + no respawn" },
        { false, true, false, true, false, "dead + non-player master" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldDespawnForZoning(c.hpPositive, c.hasMaster, c.masterIsPlayer, c.respawnPet);
        const bool inlineF = inlineShouldDespawnForZoning(c.hpPositive, c.hasMaster, c.masterIsPlayer, c.respawnPet);
        const bool wantPin = c.hpPositive && c.hasMaster && c.masterIsPlayer && c.respawnPet;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldDespawnForZoning dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldDespawnForZoning == pin formula AND of four") && ok;
    }

    // Pin composition: only the full-true pole accepts.
    ok = expect(ShouldDespawnForZoning(true, true, true, true), "all true must accept") && ok;
    ok = expect(!ShouldDespawnForZoning(false, true, true, true), "hpPositive false must reject") && ok;
    ok = expect(!ShouldDespawnForZoning(true, false, true, true), "hasMaster false must reject") && ok;
    ok = expect(!ShouldDespawnForZoning(true, true, false, true), "masterIsPlayer false must reject") && ok;
    ok = expect(!ShouldDespawnForZoning(true, true, true, false), "respawnPet false must reject") && ok;

    // Dense compose: full 2^4 boolean space.
    for (const bool hpPositive : { false, true })
    {
        for (const bool hasMaster : { false, true })
        {
            for (const bool masterIsPlayer : { false, true })
            {
                for (const bool respawnPet : { false, true })
                {
                    const bool got  = ShouldDespawnForZoning(hpPositive, hasMaster, masterIsPlayer, respawnPet);
                    const bool want = hpPositive && hasMaster && masterIsPlayer && respawnPet;
                    ok              = expect(got == want, "compose free == pin formula") && ok;
                    ok              = expect(got == inlineShouldDespawnForZoning(hpPositive, hasMaster, masterIsPlayer, respawnPet),
                                "compose free == inline") &&
                         ok;
                }
            }
        }
    }

    // --- Production CPetEntity::Die path semantics ---
    // Host injects:
    //   hpPositive     = health.hp > 0
    //   hasMaster      = PMaster != nullptr
    //   masterIsPlayer = PMaster != nullptr && PMaster->objtype == TYPE_PC
    //   respawnPet     = masterIsPlayer && petZoningInfo.respawnPet
    // when true  → Internal_Despawn(true) branch of Apply
    // when false → Internal_Die(2500ms) branch of Apply
    ok = expect(ShouldDespawnForZoning(true, true, true, true), "Die living player pet respawn → despawn path") && ok;
    ok = expect(!ShouldDespawnForZoning(false, true, true, true), "Die dead pet → death-state path") && ok;

    // Host inject poles: non-player master injects respawnPet=false in
    // production; pure helper still evaluates the four injected bools.
    ok = expect(!ShouldDespawnForZoning(true, true, false, true),
                "non-player master inject must reject even if respawnPet true") &&
         ok;
    ok = expect(!ShouldDespawnForZoning(true, true, false, false), "non-player master + false respawn rejects") && ok;

    // Explicit dual-wire: free == four-bool AND for host-style poles.
    const struct
    {
        bool        hp;
        bool        master;
        bool        player;
        bool        respawn;
        const char* label;
    } hostPoles[] = {
        { true, true, true, true, "full accept" },
        { false, true, true, true, "hp zero" },
        { true, false, false, false, "no master cascade" },
        { true, true, true, false, "respawn cleared" },
        { true, true, false, false, "non-PC master" },
        { false, false, false, false, "all off" },
        { true, false, true, true, "missing master with player/respawn inject" },
        { false, true, false, false, "dead non-player no respawn" },
    };

    for (const auto& p : hostPoles)
    {
        const bool got  = ShouldDespawnForZoning(p.hp, p.master, p.player, p.respawn);
        const bool want = p.hp && p.master && p.player && p.respawn;
        ok              = expect(got == want, p.label) && ok;
        ok              = expect(got == inlineShouldDespawnForZoning(p.hp, p.master, p.player, p.respawn),
                    "host inject free == inline") &&
             ok;
    }

    // Residual 1414 / 2261 poles still hold under dual-wire.
    ok = expect(ShouldDespawnForZoning(true, true, true, true), "residual living player pet with respawn") && ok;
    ok = expect(!ShouldDespawnForZoning(false, true, true, true), "residual dead pet") && ok;
    ok = expect(!ShouldDespawnForZoning(true, false, true, true), "residual missing master") && ok;
    ok = expect(!ShouldDespawnForZoning(true, true, false, true), "residual non-player master") && ok;
    ok = expect(!ShouldDespawnForZoning(true, true, true, false), "residual no respawn flag") && ok;

    return ok;
}
