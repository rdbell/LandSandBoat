#include "test_pet_detach_player_master_2987.h"

#include "map/pet_death_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "pet ShouldDetachPlayerMaster 2987 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CPetEntity::Die detach-player-master formula for dual-wire cross-check
// (slice 2987):
//   hasMaster && masterPetIsSelf && masterIsPlayer
auto inlineShouldDetachPlayerMaster(const bool hasMaster, const bool masterPetIsSelf,
                                    const bool masterIsPlayer) -> bool
{
    return hasMaster && masterPetIsSelf && masterIsPlayer;
}

} // namespace

// Pure dual-wire expansion for petdeathhelpers::ShouldDetachPlayerMaster
// (CPetEntity::Die player-master active-pet detach gate; slice 2987).
auto runPetDetachPlayerMaster2987SelfTests() -> bool
{
    using petdeathhelpers::ShouldDetachPlayerMaster;

    bool ok = true;

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

        // Residual 1414 / 2262 single-false poles.
        { false, true, true, false, "missing master" },
        { true, false, true, false, "different active pet" },
        { true, true, false, false, "non-player master" },

        // Additional multi-false poles.
        { false, false, false, false, "all false" },
        { false, true, false, false, "missing master + non-player" },
        { true, false, false, false, "different pet + non-player" },
        { false, false, true, false, "missing master + not self" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldDetachPlayerMaster(c.hasMaster, c.masterPetIsSelf, c.masterIsPlayer);
        const bool inlineF = inlineShouldDetachPlayerMaster(c.hasMaster, c.masterPetIsSelf, c.masterIsPlayer);
        const bool wantPin = c.hasMaster && c.masterPetIsSelf && c.masterIsPlayer;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldDetachPlayerMaster dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldDetachPlayerMaster == pin formula AND of three") && ok;
    }

    // Pin composition: only the full-true pole accepts.
    ok = expect(ShouldDetachPlayerMaster(true, true, true), "all true must accept") && ok;
    ok = expect(!ShouldDetachPlayerMaster(false, true, true), "hasMaster false must reject") && ok;
    ok = expect(!ShouldDetachPlayerMaster(true, false, true), "masterPetIsSelf false must reject") && ok;
    ok = expect(!ShouldDetachPlayerMaster(true, true, false), "masterIsPlayer false must reject") && ok;

    // Dense compose: full 2^3 boolean space.
    for (const bool hasMaster : { false, true })
    {
        for (const bool masterPetIsSelf : { false, true })
        {
            for (const bool masterIsPlayer : { false, true })
            {
                const bool got  = ShouldDetachPlayerMaster(hasMaster, masterPetIsSelf, masterIsPlayer);
                const bool want = hasMaster && masterPetIsSelf && masterIsPlayer;
                ok              = expect(got == want, "compose free == pin formula") && ok;
                ok              = expect(got == inlineShouldDetachPlayerMaster(hasMaster, masterPetIsSelf, masterIsPlayer),
                            "compose free == inline") &&
                     ok;
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

    // Host inject poles: missing master cascades false for self/player in
    // production; pure helper still evaluates the three injected bools.
    ok = expect(!ShouldDetachPlayerMaster(false, true, true),
                "missing master inject must reject even if self/player true") &&
         ok;
    ok = expect(!ShouldDetachPlayerMaster(false, false, false), "no master cascade rejects") && ok;

    // Explicit dual-wire: free == three-bool AND for host-style poles.
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
        const bool got  = ShouldDetachPlayerMaster(p.master, p.self, p.player);
        const bool want = p.master && p.self && p.player;
        ok              = expect(got == want, p.label) && ok;
        ok              = expect(got == inlineShouldDetachPlayerMaster(p.master, p.self, p.player),
                    "host inject free == inline") &&
             ok;
    }

    // Residual 1414 / 2262 poles still hold under dual-wire.
    ok = expect(ShouldDetachPlayerMaster(true, true, true), "residual attached player master") && ok;
    ok = expect(!ShouldDetachPlayerMaster(false, true, true), "residual missing master") && ok;
    ok = expect(!ShouldDetachPlayerMaster(true, false, true), "residual different active pet") && ok;
    ok = expect(!ShouldDetachPlayerMaster(true, true, false), "residual non-player master") && ok;

    return ok;
}
