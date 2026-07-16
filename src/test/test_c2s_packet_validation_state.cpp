#include "test_c2s_packet_validation_state.h"

#include <iostream>
#include <string>

#include "map/items/item_linkshell.h"
#include "map/packets/c2s/validation.h"

namespace
{
auto expect(const PacketValidationResult& result, const std::string& want, const std::string& label) -> bool
{
    if (result.errorString() != want)
    {
        std::cerr << "c2s packet validation state self-test failed: " << label << " got \"" << result.errorString() << "\" expected \"" << want << "\"\n";
        return false;
    }
    return true;
}

auto testStatePredicates() -> bool
{
    bool ok = true;
    PacketValidationSnapshot state{};
    state.blockedStates = static_cast<uint16>(BlockedState::Dead) | static_cast<uint16>(BlockedState::Healing);
    auto blocked = PacketValidator(state);
    blocked.blockedBy(magic_enum::containers::bitset<BlockedState>{ BlockedState::Dead, BlockedState::Healing });
    ok = expect(static_cast<PacketValidationResult>(blocked), "Invalid state: Dead", "blocked first error") && ok;

    state = {};
    state.inEvent = true;
    state.eventId = 12;
    auto event = PacketValidator(state);
    event.isInEvent(13);
    ok = expect(static_cast<PacketValidationResult>(event), "Event ID mismatch 12 != 13.", "event mismatch") && ok;

    state = {};
    state.linkshellPresent[0] = true;
    state.linkshellItems[0] = true;
    state.linkshellRanks[0] = LSTYPE_LINKPEARL;
    auto linkshell = PacketValidator(state);
    linkshell.hasLinkshellRank(1, LSTYPE_PEARLSACK);
    ok = expect(static_cast<PacketValidationResult>(linkshell), "Invalid linkshell rank.", "linkshell rank") && ok;

    state = {};
    state.zoneName = "Bastok";
    auto zone = PacketValidator(state);
    zone.hasZoneMiscFlag(MISC_MOUNT);
    ok = expect(static_cast<PacketValidationResult>(zone), "Zone Bastok does not allow misc flag 4.", "zone normal") && ok;
    state.gmLevel = 1;
    auto gm = PacketValidator(state);
    gm.hasZoneMiscFlag(MISC_MOUNT);
    ok = expect(static_cast<PacketValidationResult>(gm), "", "zone GM bypass") && ok;

    state = {};
    state.hasParty = state.hasAlliance = state.hasAllianceMainParty = true;
    auto party = PacketValidator(state);
    party.isPartyLeader();
    ok = expect(static_cast<PacketValidationResult>(party), "Not the party leader.", "party leader") && ok;
    auto alliance = PacketValidator(state);
    alliance.isAllianceLeader();
    ok = expect(static_cast<PacketValidationResult>(alliance), "Not the alliance leader.", "alliance leader") && ok;

    state = {};
    auto chained = PacketValidator(state);
    chained.isEngaged().isInMogHouse().hasKeyItem(static_cast<KeyItem>(42));
    ok = expect(static_cast<PacketValidationResult>(chained), "Character is not engaged.", "state short circuit") && ok;
    state.engaged = true;
    auto mogHouse = PacketValidator(state);
    mogHouse.isInMogHouse();
    ok = expect(static_cast<PacketValidationResult>(mogHouse), "Character is not in Mog House.", "mog house") && ok;
    state.inMogHouse = true;
    auto keyItem = PacketValidator(state);
    keyItem.hasKeyItem(static_cast<KeyItem>(42));
    ok = expect(static_cast<PacketValidationResult>(keyItem), "Missing Key Item 42.", "key item") && ok;
    state.lastPacketType = 0x020;
    auto prior = PacketValidator(state);
    prior.requiresPriorPacket(static_cast<PacketC2S>(0x01A));
    ok = expect(static_cast<PacketValidationResult>(prior), "Expected prior packet 0x01a, got 0x020.", "prior packet") && ok;
    return ok;
}
} // namespace

auto runC2SPacketValidationStateSelfTests() -> bool
{
    return testStatePredicates();
}
