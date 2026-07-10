#include "test_battlefield.h"

#include "map/battlefield.h"
#include "map/entities/char_entity.h"

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

} // namespace

auto runBattlefieldSelfTests() -> bool
{
    return testMembershipAndCleanupState() && testRegistrationCapacityOrdering();
}
