#include "test_petentity_despawn_zoning_3233.h"

#include "map/pet_death_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "petentity ShouldDespawnForZoning 3233 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CPetEntity::Die zoning-despawn formula for dual-wire cross-check
// (dedicated slice 3233) — direct return AND-chain:
//   hpPositive && hasMaster && masterIsPlayer && respawnPet
auto inlineShouldDespawnForZoning(const bool hpPositive, const bool hasMaster, const bool masterIsPlayer,
                                  const bool respawnPet) -> bool
{
    return hpPositive && hasMaster && masterIsPlayer && respawnPet;
}

// Positive if/else pin matching free function / capacity body (slice 3233).
// Avoid De Morgan rewrites of the four-bool conjunction (QF1001).
auto pinShouldDespawnForZoning(const bool hpPositive, const bool hasMaster, const bool masterIsPlayer,
                               const bool respawnPet) -> bool
{
    if (hpPositive)
    {
        if (hasMaster)
        {
            if (masterIsPlayer)
            {
                if (respawnPet)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

} // namespace

// Pure dual-wire expansion for petdeathhelpers::ShouldDespawnForZoning
// (CPetEntity::Die living player-pet zoning-despawn gate; dedicated slice 3233;
// residual expand 2951 / pure 1414 / 2261 / prior dedicated 3170).
//
// Coverage:
//   - free == inline == pin (positive if/else form; avoid QF1001)
//   - residual 1414 / 2261 / 2951 pins still hold
//   - dense 2^4 poles
//   - sibling ShouldDetachPlayerMaster (2987) residual only
auto runPetentityDespawnZoning3233SelfTests() -> bool
{
    using petdeathhelpers::ShouldDespawnForZoning;
    using petdeathhelpers::ShouldDetachPlayerMaster;

    bool ok = true;

    // Residual 1414 / 2261 / 2951 pins still hold under dual-wire.
    ok = expect(ShouldDespawnForZoning(true, true, true, true), "residual: living player pet with respawn") && ok;
    ok = expect(!ShouldDespawnForZoning(false, true, true, true), "residual: dead pet") && ok;
    ok = expect(!ShouldDespawnForZoning(true, false, true, true), "residual: missing master") && ok;
    ok = expect(!ShouldDespawnForZoning(true, true, false, true), "residual: non-player master") && ok;
    ok = expect(!ShouldDespawnForZoning(true, true, true, false), "residual: no respawn flag") && ok;

    // --- Composition table: free == inline == pin (positive if/else) ---
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

        // Residual 1414 / 2261 / 2951 single-false poles.
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
        { false, false, false, true, false, "only respawn true" },
        { true, false, false, false, false, "only hp true" },
        { false, true, false, false, false, "only master true" },
        { false, false, true, false, false, "only player true" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldDespawnForZoning(c.hpPositive, c.hasMaster, c.masterIsPlayer, c.respawnPet);
        const bool inlineF = inlineShouldDespawnForZoning(c.hpPositive, c.hasMaster, c.masterIsPlayer, c.respawnPet);
        const bool pin     = pinShouldDespawnForZoning(c.hpPositive, c.hasMaster, c.masterIsPlayer, c.respawnPet);
        const bool wantAnd = c.hpPositive && c.hasMaster && c.masterIsPlayer && c.respawnPet;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF && got == pin, "ShouldDespawnForZoning dual-wire free==inline==pin") && ok;
        ok = expect(got == wantAnd, "ShouldDespawnForZoning == AND of four") && ok;
    }

    // Pin composition: only the full-true pole accepts.
    ok = expect(ShouldDespawnForZoning(true, true, true, true), "all true must accept") && ok;
    ok = expect(!ShouldDespawnForZoning(false, true, true, true), "hpPositive false must reject") && ok;
    ok = expect(!ShouldDespawnForZoning(true, false, true, true), "hasMaster false must reject") && ok;
    ok = expect(!ShouldDespawnForZoning(true, true, false, true), "masterIsPlayer false must reject") && ok;
    ok = expect(!ShouldDespawnForZoning(true, true, true, false), "respawnPet false must reject") && ok;

    // Dense compose: full 2^4 boolean space — free == inline == pin.
    for (const bool hpPositive : { false, true })
    {
        for (const bool hasMaster : { false, true })
        {
            for (const bool masterIsPlayer : { false, true })
            {
                for (const bool respawnPet : { false, true })
                {
                    const bool got     = ShouldDespawnForZoning(hpPositive, hasMaster, masterIsPlayer, respawnPet);
                    const bool inlineF = inlineShouldDespawnForZoning(hpPositive, hasMaster, masterIsPlayer, respawnPet);
                    const bool pin     = pinShouldDespawnForZoning(hpPositive, hasMaster, masterIsPlayer, respawnPet);
                    const bool want    = hpPositive && hasMaster && masterIsPlayer && respawnPet;
                    ok                 = expect(got == want, "compose free == pin formula") && ok;
                    ok                 = expect(got == inlineF && got == pin, "compose free == inline == pin") && ok;
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

    // Host inject poles: free == inline == pin.
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
        { true, true, false, true, "non-player + respawn inject" },
    };

    for (const auto& p : hostPoles)
    {
        const bool got     = ShouldDespawnForZoning(p.hp, p.master, p.player, p.respawn);
        const bool inlineF = inlineShouldDespawnForZoning(p.hp, p.master, p.player, p.respawn);
        const bool pin     = pinShouldDespawnForZoning(p.hp, p.master, p.player, p.respawn);
        const bool want    = p.hp && p.master && p.player && p.respawn;
        ok                 = expect(got == want, p.label) && ok;
        ok                 = expect(got == inlineF && got == pin, "host inject free == inline == pin") && ok;
    }

    // ApplyDeath host-style: zoning gate selects despawn vs enterDeath.
    // Dual-wire only pins the gate; orchestration remains residual 1414.
    ok = expect(ShouldDespawnForZoning(true, true, true, true), "ApplyDeath zoning=true when gate accepts") && ok;
    ok = expect(!ShouldDespawnForZoning(true, true, true, false), "ApplyDeath zoning=false when respawn cleared") && ok;

    // Sibling residual 2987 still holds (not re-expanded under 3233).
    ok = expect(ShouldDetachPlayerMaster(true, true, true), "sibling residual: full detach accepts (2987)") && ok;
    ok = expect(!ShouldDetachPlayerMaster(false, true, true), "sibling residual: missing master rejects detach (2987)") &&
         ok;
    ok = expect(!ShouldDetachPlayerMaster(true, false, true), "sibling residual: pet not self rejects detach (2987)") &&
         ok;
    ok = expect(!ShouldDetachPlayerMaster(true, true, false),
                "sibling residual: non-player master rejects detach (2987)") &&
         ok;

    return ok;
}
