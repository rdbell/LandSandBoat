#include "test_map_party_alliance_updates_1295.h"

#include "map/party_alliance_updates.h"

#include <iostream>
#include <limits>
#include <vector>

namespace
{

struct FakePlayer
{
    void ReloadPartyInc()
    {
        ++reloadCalls;
    }

    int reloadCalls{};
};

struct FakeParty
{
    void DisbandParty(const bool playerInitiated)
    {
        ++disbandCalls;
        initiated = playerInitiated;
    }

    int  disbandCalls{};
    bool initiated{ true };
};

struct FakeAlliance
{
    void dissolveAlliance(const bool playerInitiated)
    {
        ++dissolveCalls;
        initiated = playerInitiated;
    }

    int  dissolveCalls{};
    bool initiated{ true };
};

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "map party/alliance updates 1295 self-test failed: " << label << '\n';
    }
    return condition;
}

auto testPartyReloadPreservesRowsAndMisses() -> bool
{
    FakePlayer          player{};
    uint32              lookedUpPartyId{};
    std::vector<uint32> characterLookups{};
    int                 memberLookups{};

    mapipc::HandlePartyReload(
        ipc::PartyReload{ .partyId = std::numeric_limits<uint32>::max() },
        [&](const uint32 partyId, auto&& visit)
        {
            ++memberLookups;
            lookedUpPartyId = partyId;
            std::invoke(visit, 11);
            std::invoke(visit, 22);
            std::invoke(visit, 11);
        },
        [&](const uint32 characterId) -> FakePlayer*
        {
            characterLookups.push_back(characterId);
            return characterId == 11 ? &player : nullptr;
        });

    return expect(memberLookups == 1 && lookedUpPartyId == std::numeric_limits<uint32>::max(), "party member lookup") &&
           expect(characterLookups == std::vector<uint32>{ 11, 22, 11 }, "party character lookup order and duplicates") &&
           expect(player.reloadCalls == 2, "party present rows refreshed independently");
}

auto testAllianceReloadAndEmptyRows() -> bool
{
    FakePlayer player{};
    uint32     lookedUpAllianceId{};
    int        characterLookups{};

    mapipc::HandleAllianceReload(
        ipc::AllianceReload{ .allianceId = 0xABCDEF01 },
        [&](const uint32 allianceId, auto&& visit)
        {
            lookedUpAllianceId = allianceId;
            std::invoke(visit, std::numeric_limits<uint32>::max());
            std::invoke(visit, 0);
        },
        [&](const uint32 characterId) -> FakePlayer*
        {
            ++characterLookups;
            return characterId == std::numeric_limits<uint32>::max() ? &player : nullptr;
        });

    int emptyCharacterLookups{};
    mapipc::HandlePartyReload(
        ipc::PartyReload{},
        [](const uint32, auto&&)
        {
        },
        [&](const uint32) -> FakePlayer*
        {
            ++emptyCharacterLookups;
            return &player;
        });

    return expect(lookedUpAllianceId == 0xABCDEF01, "alliance member lookup ID") &&
           expect(characterLookups == 2 && player.reloadCalls == 1, "alliance rows processed") &&
           expect(emptyCharacterLookups == 0, "empty member rows avoid character lookup");
}

auto testPartyDisband() -> bool
{
    FakeParty party{};
    uint32    lookedUpId{};
    int       lookups{};
    mapipc::HandlePartyDisband(
        ipc::PartyDisband{ .partyId = 0xFFFFFFFF },
        [&](const uint32 partyId)
        {
            ++lookups;
            lookedUpId = partyId;
            return &party;
        });

    int missingLookups{};
    mapipc::HandlePartyDisband(
        ipc::PartyDisband{},
        [&](const uint32) -> FakeParty*
        {
            ++missingLookups;
            return nullptr;
        });

    return expect(lookups == 1 && lookedUpId == 0xFFFFFFFF, "party disband lookup") &&
           expect(party.disbandCalls == 1 && !party.initiated, "party disband remote flag") &&
           expect(missingLookups == 1, "missing party looked up once");
}

auto testAllianceDissolve() -> bool
{
    FakeAlliance alliance{};
    uint32       lookedUpId{};
    int          lookups{};
    mapipc::HandleAllianceDissolve(
        ipc::AllianceDissolve{ .allianceId = 0xFEDCBA98 },
        [&](const uint32 allianceId)
        {
            ++lookups;
            lookedUpId = allianceId;
            return &alliance;
        });

    int missingLookups{};
    mapipc::HandleAllianceDissolve(
        ipc::AllianceDissolve{},
        [&](const uint32) -> FakeAlliance*
        {
            ++missingLookups;
            return nullptr;
        });

    return expect(lookups == 1 && lookedUpId == 0xFEDCBA98, "alliance dissolve lookup") &&
           expect(alliance.dissolveCalls == 1 && !alliance.initiated, "alliance dissolve remote flag") &&
           expect(missingLookups == 1, "missing alliance looked up once");
}

} // namespace

auto runMapPartyAllianceUpdates1295SelfTests() -> bool
{
    return testPartyReloadPreservesRowsAndMisses() && testAllianceReloadAndEmptyRows() && testPartyDisband() &&
           testAllianceDissolve();
}
