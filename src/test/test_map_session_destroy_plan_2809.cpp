#include "test_map_session_destroy_plan_2809.h"

#include "map/map_session_container_capacity.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "map session destroy plan 2809 self-test failed: " << label << '\n';
    }
    return condition;
}

// Mirrors OmegaXI PlanDestroy / TestPlanDestroyTruthTable order.
auto referenceDestroyPlan(const mapsessionhelpers::DestroyDecision input) -> mapsessionhelpers::DecisionPlan
{
    mapsessionhelpers::DecisionPlan plan{};

    if (input.shuttingDown == 1)
    {
        plan.actions[plan.count++] = mapsessionhelpers::CleanupAction::DeleteDatabaseSession;
    }
    if (input.hasCharacter)
    {
        if (input.hasZone)
        {
            plan.actions[plan.count++] = mapsessionhelpers::CleanupAction::DecreaseZoneCounter;
        }
        plan.actions[plan.count++] = mapsessionhelpers::CleanupAction::ReleaseCharacter;
    }
    plan.actions[plan.count++] = mapsessionhelpers::CleanupAction::EraseSession;
    return plan;
}

auto plansEqual(const mapsessionhelpers::DecisionPlan& a, const mapsessionhelpers::DecisionPlan& b) -> bool
{
    if (a.count != b.count)
    {
        return false;
    }
    for (uint8 i = 0; i < a.count; ++i)
    {
        if (a.actions[i] != b.actions[i])
        {
            return false;
        }
    }
    return true;
}

auto containsAction(const mapsessionhelpers::DecisionPlan& plan, const mapsessionhelpers::CleanupAction target) -> bool
{
    for (uint8 i = 0; i < plan.count; ++i)
    {
        if (plan.actions[i] == target)
        {
            return true;
        }
    }
    return false;
}

} // namespace

auto runMapSessionDestroyPlan2809SelfTests() -> bool
{
    using mapsessionhelpers::CleanupAction;
    using mapsessionhelpers::DestroyDecision;
    using mapsessionhelpers::PlanDestroy;

    bool ok = true;

    // Full truth table: hasCharacter x hasZone x shuttingDown{0..3}
    // Matches OmegaXI TestPlanDestroyTruthTableAndImmutability.
    for (int hasCharacter = 0; hasCharacter < 2; ++hasCharacter)
    {
        for (int hasZone = 0; hasZone < 2; ++hasZone)
        {
            for (uint8 shutdown = 0; shutdown <= 3; ++shutdown)
            {
                const DestroyDecision input{
                    .hasCharacter = hasCharacter != 0,
                    .hasZone      = hasZone != 0,
                    .shuttingDown = shutdown,
                };
                const auto got  = PlanDestroy(input);
                const auto want = referenceDestroyPlan(input);
                const std::string label =
                    "truth hasChar=" + std::to_string(hasCharacter) +
                    " hasZone=" + std::to_string(hasZone) +
                    " shutdown=" + std::to_string(shutdown);
                ok = expect(plansEqual(got, want), label.c_str()) && ok;
            }
        }
    }

    // Spot-check fixed branch shapes.
    {
        // Minimal: no char, not logout — erase only.
        const auto bare = PlanDestroy(DestroyDecision{});
        ok = expect(bare.count == 1, "bare count") && ok;
        ok = expect(bare.actions[0] == CleanupAction::EraseSession, "bare erase only") && ok;
        ok = expect(!containsAction(bare, CleanupAction::DeleteDatabaseSession), "bare no db") && ok;
        ok = expect(!containsAction(bare, CleanupAction::ReleaseCharacter), "bare no release") && ok;
        ok = expect(!containsAction(bare, CleanupAction::DecreaseZoneCounter), "bare no zone") && ok;
    }
    {
        // Explicit logout without character: delete db then erase.
        const auto logout = PlanDestroy(DestroyDecision{
            .hasCharacter = false,
            .hasZone      = false,
            .shuttingDown = 1,
        });
        ok = expect(logout.count == 2, "logout no-char count") && ok;
        ok = expect(logout.actions[0] == CleanupAction::DeleteDatabaseSession, "logout db") && ok;
        ok = expect(logout.actions[1] == CleanupAction::EraseSession, "logout erase") && ok;
    }
    {
        // Character without zone: release then erase (no DecreaseZoneCounter).
        const auto noZone = PlanDestroy(DestroyDecision{
            .hasCharacter = true,
            .hasZone      = false,
            .shuttingDown = 0,
        });
        ok = expect(noZone.count == 2, "char no-zone count") && ok;
        ok = expect(noZone.actions[0] == CleanupAction::ReleaseCharacter, "char no-zone release") && ok;
        ok = expect(noZone.actions[1] == CleanupAction::EraseSession, "char no-zone erase") && ok;
        ok = expect(!containsAction(noZone, CleanupAction::DecreaseZoneCounter), "char no-zone skips counter") && ok;
        ok = expect(!containsAction(noZone, CleanupAction::DeleteDatabaseSession), "char no-zone skips db") && ok;
    }
    {
        // Full path: logout + char + zone.
        const auto full = PlanDestroy(DestroyDecision{
            .hasCharacter = true,
            .hasZone      = true,
            .shuttingDown = 1,
        });
        ok = expect(full.count == 4, "full destroy count") && ok;
        ok = expect(full.actions[0] == CleanupAction::DeleteDatabaseSession, "full db") && ok;
        ok = expect(full.actions[1] == CleanupAction::DecreaseZoneCounter, "full zone") && ok;
        ok = expect(full.actions[2] == CleanupAction::ReleaseCharacter, "full release") && ok;
        ok = expect(full.actions[3] == CleanupAction::EraseSession, "full erase") && ok;
    }
    {
        // hasZone without hasCharacter is ignored (host should not set this,
        // but pure plan must not emit DecreaseZoneCounter or ReleaseCharacter).
        const auto orphanZone = PlanDestroy(DestroyDecision{
            .hasCharacter = false,
            .hasZone      = true,
            .shuttingDown = 0,
        });
        ok = expect(orphanZone.count == 1, "orphan zone count") && ok;
        ok = expect(orphanZone.actions[0] == CleanupAction::EraseSession, "orphan zone erase only") && ok;
        ok = expect(!containsAction(orphanZone, CleanupAction::DecreaseZoneCounter),
                    "orphan zone ignores hasZone") &&
            ok;
        ok = expect(!containsAction(orphanZone, CleanupAction::ReleaseCharacter),
                    "orphan zone ignores release") &&
            ok;
    }

    // Database delete only for shuttingDown == 1 (exact, not zoning 2 / other).
    for (const uint8 shutdown : { uint8{ 0 }, uint8{ 1 }, uint8{ 2 }, uint8{ 255 } })
    {
        const auto plan = PlanDestroy(DestroyDecision{
            .hasCharacter = true,
            .hasZone      = true,
            .shuttingDown = shutdown,
        });
        const bool got  = containsAction(plan, CleanupAction::DeleteDatabaseSession);
        const bool want = shutdown == 1;
        const std::string label = "db delete shuttingDown=" + std::to_string(shutdown);
        ok = expect(got == want, label.c_str()) && ok;
    }

    // Destroy plans never emit timeout-only actions (index already removed).
    {
        const auto plan = PlanDestroy(DestroyDecision{
            .hasCharacter = true,
            .hasZone      = true,
            .shuttingDown = 1,
        });
        ok = expect(!containsAction(plan, CleanupAction::RemoveSessionIndex),
                    "destroy excludes RemoveSessionIndex") &&
            ok;
        ok = expect(!containsAction(plan, CleanupAction::SaveStatusEffects),
                    "destroy excludes SaveStatusEffects") &&
            ok;
        ok = expect(!containsAction(plan, CleanupAction::SaveCharacterPosition),
                    "destroy excludes SaveCharacterPosition") &&
            ok;
        ok = expect(!containsAction(plan, CleanupAction::DespawnMobPet), "destroy excludes DespawnMobPet") && ok;
        ok = expect(!containsAction(plan, CleanupAction::SetCharacterShutdown),
                    "destroy excludes SetCharacterShutdown") &&
            ok;
        ok = expect(!containsAction(plan, CleanupAction::RemoveCharacterFromZone),
                    "destroy excludes RemoveCharacterFromZone") &&
            ok;
        ok = expect(plan.actions[plan.count - 1] == CleanupAction::EraseSession, "destroy ends with erase") && ok;
    }

    return ok;
}
