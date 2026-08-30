#include "test_battlefield.h"
#include "omega_self_test_registry.h"

#include "common/mmo.h"
#include "common/timer.h"
#include "sol/sol.hpp"

#include <functional>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#define private public
#include "map/battlefield.h"
#undef private
#include "map/entities/char_entity.h"

#include <chrono>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "battlefield self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto testMembershipAndCleanupState() -> bool
{
    CCharEntity initiator;
    initiator.id = 0xFFFFFFFF;
    // Native self-tests run before Lua initialization, while CBattlefield's
    // destructor unconditionally dispatches OnBattlefieldDestroy. Keep this
    // value alive for process lifetime so this membership fixture does not
    // invoke an unavailable Lua host during teardown.
    auto* battlefield = new CBattlefield(12, nullptr, 1, &initiator);

    bool ok = true;
    ok      = expect(battlefield->IsRegistered(&initiator), "constructor registers initiator") && ok;
    ok      = expect(!battlefield->isEntered(&initiator), "registration does not imply entry") && ok;
    ok      = expect(battlefield->GetPlayerCount() == 0, "initial entered count") && ok;
    ok      = expect(!battlefield->IsOccupied(), "initial occupancy") && ok;
    ok      = expect(battlefield->CanCleanup(false), "empty membership permits cleanup") && ok;

    battlefield->m_EnteredPlayers.emplace(initiator.id);
    ok = expect(battlefield->isEntered(&initiator), "entered identity lookup") && ok;
    ok = expect(battlefield->GetPlayerCount() == 1, "entered count") && ok;
    ok = expect(battlefield->IsOccupied(), "occupied after entry") && ok;
    ok = expect(!battlefield->CanCleanup(false), "occupied membership blocks cleanup") && ok;
    ok = expect(battlefield->CanCleanup(true), "explicit cleanup request") && ok;
    ok = expect(battlefield->CanCleanup(false), "cleanup request remains sticky") && ok;

    battlefield->m_EnteredPlayers.erase(initiator.id);
    ok = expect(battlefield->IsRegistered(&initiator), "leaving retains registration") && ok;
    return ok;
}

auto testRegistrationCapacityOrdering() -> bool
{
    CCharEntity initiator;
    initiator.id = 10;
    auto* battlefield = new CBattlefield(13, nullptr, 1, &initiator);
    battlefield->SetMaxParticipants(1);

    CCharEntity candidate;
    candidate.id = 20;

    bool ok = true;
    ok      = expect(battlefield->InsertEntity(&candidate, false), "registration below capacity") && ok;
    ok      = expect(battlefield->InsertEntity(&candidate, false), "duplicate registration below capacity") && ok;
    ok      = expect(battlefield->IsRegistered(&candidate), "candidate registered") && ok;
    ok      = expect(battlefield->m_RegisteredPlayers.size() == 2, "registration set uniqueness") && ok;

    battlefield->m_EnteredPlayers.emplace(initiator.id);
    CCharEntity fullCandidate;
    fullCandidate.id = 30;
    ok = expect(!battlefield->InsertEntity(&fullCandidate, false), "capacity checked before registration") && ok;
    ok = expect(!battlefield->IsRegistered(&fullCandidate), "rejected candidate remains unregistered") && ok;
    return ok;
}

auto testRemainingTime() -> bool
{
    using namespace std::chrono_literals;

    CCharEntity initiator;
    initiator.id      = 40;
    auto* battlefield = new CBattlefield(14, nullptr, 1, &initiator);

    const auto setTimes = [battlefield](const timer::duration limit, const timer::duration elapsed)
    {
        battlefield->m_StartTime = timer::time_point{};
        battlefield->m_Tick      = timer::time_point{ elapsed };
        battlefield->m_TimeLimit = limit;
    };

    bool ok = true;

    setTimes(10s, 0s);
    ok = expect(battlefield->GetRemainingTime() == 10s, "remaining time at start") && ok;

    setTimes(10s, 4s);
    ok = expect(battlefield->GetRemainingTime() == 6s, "remaining time subtracts elapsed") && ok;

    setTimes(10s, 10s);
    ok = expect(battlefield->GetRemainingTime() == timer::duration::zero(), "equal limit and elapsed returns zero") && ok;

    setTimes(10s, 11s);
    ok = expect(battlefield->GetRemainingTime() == timer::duration::zero(), "elapsed beyond limit returns zero") && ok;

    setTimes(-2s, -5s);
    ok = expect(battlefield->GetRemainingTime() == 3s, "negative durations retain strict comparison and subtraction") && ok;

    setTimes(-5s, -2s);
    ok = expect(battlefield->GetRemainingTime() == timer::duration::zero(), "negative elapsed beyond limit returns zero") && ok;

    setTimes(timer::duration::max(), 1ns);
    ok = expect(battlefield->GetRemainingTime() == timer::duration::max() - 1ns, "high duration boundary") && ok;

    setTimes(timer::duration::min() + 1ns, timer::duration::min());
    ok = expect(battlefield->GetRemainingTime() == 1ns, "low duration boundary") && ok;

    return ok;
}

} // namespace

auto runBattlefieldSelfTests() -> bool
{
    return testMembershipAndCleanupState() && testRegistrationCapacityOrdering();
}

OMEGA_REGISTER_SELF_TEST("battlefield-remaining-time-8058", testRemainingTime);
