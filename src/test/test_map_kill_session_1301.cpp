#include "test_map_kill_session_1301.h"

#include "map/kill_session.h"

#include <iostream>
#include <limits>

namespace
{

struct FakeSession
{
    BLOWFISH status{ BLOWFISH_WAITING };
    int      destroys{};
};

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "map kill session 1301 self-test failed: " << label << '\n';
    }
    return condition;
}

auto runKill(const uint32 victimId, FakeSession* active, FakeSession* pending) -> void
{
    mapipc::HandleKillSession(
        ipc::KillSession{ .victimId = victimId },
        [active](const uint32)
        {
            return active;
        },
        [pending](const uint32)
        {
            return pending;
        },
        [](FakeSession* session)
        {
            return session->status;
        },
        [](FakeSession* session)
        {
            ++session->destroys;
        },
        [](FakeSession* session)
        {
            ++session->destroys;
        });
}

auto testPredicate() -> bool
{
    return expect(mapipc::ShouldDestroyKillSession(BLOWFISH_PENDING_ZONE), "pending zone destroys") &&
           expect(!mapipc::ShouldDestroyKillSession(BLOWFISH_WAITING), "waiting preserved") &&
           expect(!mapipc::ShouldDestroyKillSession(BLOWFISH_SENT), "sent preserved") &&
           expect(!mapipc::ShouldDestroyKillSession(BLOWFISH_ACCEPTED), "accepted preserved");
}

auto testIndependentTablesAndStatuses() -> bool
{
    FakeSession activePending{ .status = BLOWFISH_PENDING_ZONE };
    FakeSession pendingWaiting{ .status = BLOWFISH_WAITING };
    runKill(std::numeric_limits<uint32>::max(), &activePending, &pendingWaiting);

    FakeSession activeAccepted{ .status = BLOWFISH_ACCEPTED };
    FakeSession pendingPending{ .status = BLOWFISH_PENDING_ZONE };
    runKill(1, &activeAccepted, &pendingPending);

    FakeSession bothPendingA{ .status = BLOWFISH_PENDING_ZONE };
    FakeSession bothPendingB{ .status = BLOWFISH_PENDING_ZONE };
    runKill(2, &bothPendingA, &bothPendingB);

    FakeSession neitherA{ .status = BLOWFISH_SENT };
    FakeSession neitherB{ .status = BLOWFISH_ACCEPTED };
    runKill(3, &neitherA, &neitherB);

    return expect(activePending.destroys == 1 && pendingWaiting.destroys == 0, "active pending only") &&
           expect(activeAccepted.destroys == 0 && pendingPending.destroys == 1, "pending pending only") &&
           expect(bothPendingA.destroys == 1 && bothPendingB.destroys == 1, "both destroyed independently") &&
           expect(neitherA.destroys == 0 && neitherB.destroys == 0, "non-pending preserved");
}

auto testMissingLookups() -> bool
{
    FakeSession activePending{ .status = BLOWFISH_PENDING_ZONE };
    FakeSession pendingPending{ .status = BLOWFISH_PENDING_ZONE };

    uint32 lookedUpActive{};
    uint32 lookedUpPending{};
    mapipc::HandleKillSession(
        ipc::KillSession{ .victimId = 0x10203040 },
        [&](const uint32 victimId) -> FakeSession*
        {
            lookedUpActive = victimId;
            return &activePending;
        },
        [&](const uint32 victimId) -> FakeSession*
        {
            lookedUpPending = victimId;
            return nullptr;
        },
        [](FakeSession* session)
        {
            return session->status;
        },
        [](FakeSession* session)
        {
            ++session->destroys;
        },
        [](FakeSession* session)
        {
            ++session->destroys;
        });

    mapipc::HandleKillSession(
        ipc::KillSession{ .victimId = 0xA0B0C0D0 },
        [](const uint32) -> FakeSession*
        {
            return nullptr;
        },
        [&](const uint32 victimId) -> FakeSession*
        {
            lookedUpPending = victimId;
            return &pendingPending;
        },
        [](FakeSession* session)
        {
            return session->status;
        },
        [](FakeSession* session)
        {
            ++session->destroys;
        },
        [](FakeSession* session)
        {
            ++session->destroys;
        });

    int bothMissingDestroys{};
    mapipc::HandleKillSession(
        ipc::KillSession{},
        [](const uint32) -> FakeSession*
        {
            return nullptr;
        },
        [](const uint32) -> FakeSession*
        {
            return nullptr;
        },
        [&](FakeSession*)
        {
            ++bothMissingDestroys;
            return BLOWFISH_PENDING_ZONE;
        },
        [&](FakeSession*)
        {
            ++bothMissingDestroys;
        },
        [&](FakeSession*)
        {
            ++bothMissingDestroys;
        });

    return expect(lookedUpActive == 0x10203040u && activePending.destroys == 1, "active hit with missing pending") &&
           expect(lookedUpPending == 0xA0B0C0D0u && pendingPending.destroys == 1, "pending hit with missing active") &&
           expect(bothMissingDestroys == 0, "both missing complete no-op");
}

} // namespace

auto runMapKillSession1301SelfTests() -> bool
{
    return testPredicate() && testIndependentTablesAndStatuses() && testMissingLookups();
}
