#include "test_map_session_timeout_plan_2804.h"

#include "map/map_session_container_capacity.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "map session timeout plan 2804 self-test failed: " << label << '\n';
    }
    return condition;
}

// Mirrors OmegaXI referenceTimeoutActions / PlanTimeoutCleanup order.
auto referenceTimeoutPlan(const mapsessionhelpers::TimeoutDecision input) -> mapsessionhelpers::DecisionPlan
{
    mapsessionhelpers::DecisionPlan plan{};

    if (!input.hasCharacter)
    {
        if (!input.otherMap)
        {
            plan.actions[plan.count++] = mapsessionhelpers::CleanupAction::DeleteDatabaseSession;
        }
        plan.actions[plan.count++] = mapsessionhelpers::CleanupAction::RemoveSessionIndex;
        plan.actions[plan.count++] = mapsessionhelpers::CleanupAction::EraseSession;
        return plan;
    }

    if (!input.otherMap)
    {
        plan.actions[plan.count++] = mapsessionhelpers::CleanupAction::SaveStatusEffects;
        plan.actions[plan.count++] = mapsessionhelpers::CleanupAction::DeleteDatabaseSession;
        if (input.shuttingDown == 0 || input.shuttingDown == 1)
        {
            plan.actions[plan.count++] = mapsessionhelpers::CleanupAction::SaveCharacterPosition;
        }
    }
    if (input.hasMobPet)
    {
        plan.actions[plan.count++] = mapsessionhelpers::CleanupAction::DespawnMobPet;
    }
    plan.actions[plan.count++] = mapsessionhelpers::CleanupAction::SetCharacterShutdown;
    plan.actions[plan.count++] = mapsessionhelpers::CleanupAction::RemoveCharacterFromZone;
    plan.actions[plan.count++] = mapsessionhelpers::CleanupAction::ReleaseCharacter;
    plan.actions[plan.count++] = mapsessionhelpers::CleanupAction::RemoveSessionIndex;
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

auto runMapSessionTimeoutPlan2804SelfTests() -> bool
{
    using mapsessionhelpers::CleanupAction;
    using mapsessionhelpers::PlanTimeoutCleanup;
    using mapsessionhelpers::TimeoutDecision;

    bool ok = true;

    // Full truth table: hasCharacter x otherMap x hasMobPet x shuttingDown{0,1,2}
    // Matches OmegaXI TestPlanTimeoutCleanupTruthTable.
    for (int hasCharacter = 0; hasCharacter < 2; ++hasCharacter)
    {
        for (int otherMap = 0; otherMap < 2; ++otherMap)
        {
            for (int hasMobPet = 0; hasMobPet < 2; ++hasMobPet)
            {
                for (uint8 shutdown = 0; shutdown <= 2; ++shutdown)
                {
                    const TimeoutDecision input{
                        .hasCharacter = hasCharacter != 0,
                        .otherMap     = otherMap != 0,
                        .hasMobPet    = hasMobPet != 0,
                        .shuttingDown = shutdown,
                    };
                    const auto got  = PlanTimeoutCleanup(input);
                    const auto want = referenceTimeoutPlan(input);
                    const std::string label =
                        "truth hasChar=" + std::to_string(hasCharacter) +
                        " otherMap=" + std::to_string(otherMap) +
                        " hasMobPet=" + std::to_string(hasMobPet) +
                        " shutdown=" + std::to_string(shutdown);
                    ok = expect(plansEqual(got, want), label.c_str()) && ok;
                }
            }
        }
    }

    // Spot-check fixed branch shapes.
    {
        const auto noCharOther = PlanTimeoutCleanup(TimeoutDecision{
            .hasCharacter = false,
            .otherMap     = true,
        });
        ok = expect(noCharOther.count == 2, "no char otherMap count") && ok;
        ok = expect(noCharOther.actions[0] == CleanupAction::RemoveSessionIndex, "no char otherMap index") && ok;
        ok = expect(noCharOther.actions[1] == CleanupAction::EraseSession, "no char otherMap erase") && ok;
        ok = expect(!containsAction(noCharOther, CleanupAction::DeleteDatabaseSession),
                    "no char otherMap skips db delete") &&
            ok;
    }
    {
        const auto noCharLocal = PlanTimeoutCleanup(TimeoutDecision{
            .hasCharacter = false,
            .otherMap     = false,
        });
        ok = expect(noCharLocal.count == 3, "no char local count") && ok;
        ok = expect(noCharLocal.actions[0] == CleanupAction::DeleteDatabaseSession, "no char local db") && ok;
        ok = expect(noCharLocal.actions[1] == CleanupAction::RemoveSessionIndex, "no char local index") && ok;
        ok = expect(noCharLocal.actions[2] == CleanupAction::EraseSession, "no char local erase") && ok;
    }
    {
        // Local char, no pet, shuttingDown 0: save effects, delete, position, then char teardown.
        const auto local = PlanTimeoutCleanup(TimeoutDecision{
            .hasCharacter = true,
            .otherMap     = false,
            .hasMobPet    = false,
            .shuttingDown = 0,
        });
        ok = expect(local.count == 8, "local char count") && ok;
        ok = expect(local.actions[0] == CleanupAction::SaveStatusEffects, "local save effects") && ok;
        ok = expect(local.actions[1] == CleanupAction::DeleteDatabaseSession, "local delete db") && ok;
        ok = expect(local.actions[2] == CleanupAction::SaveCharacterPosition, "local save pos") && ok;
        ok = expect(local.actions[3] == CleanupAction::SetCharacterShutdown, "local shutdown") && ok;
        ok = expect(local.actions[4] == CleanupAction::RemoveCharacterFromZone, "local remove zone") && ok;
        ok = expect(local.actions[5] == CleanupAction::ReleaseCharacter, "local release") && ok;
        ok = expect(local.actions[6] == CleanupAction::RemoveSessionIndex, "local index") && ok;
        ok = expect(local.actions[7] == CleanupAction::EraseSession, "local erase") && ok;
        ok = expect(!containsAction(local, CleanupAction::DespawnMobPet), "local no pet skips despawn") && ok;
    }
    {
        // Other-map char with mob pet: skip local persistence; still despawn + teardown.
        const auto other = PlanTimeoutCleanup(TimeoutDecision{
            .hasCharacter = true,
            .otherMap     = true,
            .hasMobPet    = true,
            .shuttingDown = 0,
        });
        ok = expect(other.count == 6, "other map char+pet count") && ok;
        ok = expect(other.actions[0] == CleanupAction::DespawnMobPet, "other map despawn first") && ok;
        ok = expect(other.actions[1] == CleanupAction::SetCharacterShutdown, "other map shutdown") && ok;
        ok = expect(!containsAction(other, CleanupAction::SaveStatusEffects), "other map skips save effects") && ok;
        ok = expect(!containsAction(other, CleanupAction::DeleteDatabaseSession), "other map skips db delete") && ok;
        ok = expect(!containsAction(other, CleanupAction::SaveCharacterPosition), "other map skips position") && ok;
    }

    // Save position only for shuttingDown 0 or 1 (exact, not <=).
    for (const uint8 shutdown : { uint8{ 0 }, uint8{ 1 }, uint8{ 2 }, uint8{ 255 } })
    {
        const auto plan = PlanTimeoutCleanup(TimeoutDecision{
            .hasCharacter = true,
            .otherMap     = false,
            .hasMobPet    = false,
            .shuttingDown = shutdown,
        });
        const bool got  = containsAction(plan, CleanupAction::SaveCharacterPosition);
        const bool want = shutdown == 0 || shutdown == 1;
        const std::string label = "save pos shuttingDown=" + std::to_string(shutdown);
        ok = expect(got == want, label.c_str()) && ok;
    }

    // Timeout plans never emit DecreaseZoneCounter (destroySession-only).
    {
        const auto plan = PlanTimeoutCleanup(TimeoutDecision{
            .hasCharacter = true,
            .otherMap     = false,
            .hasMobPet    = true,
            .shuttingDown = 1,
        });
        ok = expect(!containsAction(plan, CleanupAction::DecreaseZoneCounter),
                    "timeout excludes DecreaseZoneCounter") &&
            ok;
        ok = expect(plan.count == 9, "max local char+pet+pos count") && ok;
        ok = expect(plan.actions[plan.count - 1] == CleanupAction::EraseSession, "timeout ends with erase") && ok;
        ok = expect(plan.actions[plan.count - 2] == CleanupAction::RemoveSessionIndex, "timeout ends with index then erase") &&
            ok;
    }

    // hasMobPet is ignored when there is no character (no-char branches).
    {
        const auto withPetFlag = PlanTimeoutCleanup(TimeoutDecision{
            .hasCharacter = false,
            .otherMap     = false,
            .hasMobPet    = true,
            .shuttingDown = 0,
        });
        ok = expect(!containsAction(withPetFlag, CleanupAction::DespawnMobPet),
                    "no char ignores hasMobPet") &&
            ok;
        ok = expect(withPetFlag.count == 3, "no char hasMobPet still three actions") && ok;
    }

    return ok;
}
