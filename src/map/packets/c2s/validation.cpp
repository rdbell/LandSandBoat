/*
===========================================================================

  Copyright (c) 2025 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses/

===========================================================================
*/

#include "validation.h"

#include "ai/ai_container.h"
#include "entities/char_entity.h"
#include "items/item_linkshell.h"
#include "status_effect_container.h"
#include "utils/charutils.h"
#include "utils/jailutils.h"

auto PacketValidator::blockedBy(const magic_enum::containers::bitset<BlockedState> states) -> PacketValidator&
{
    if (!result_.valid())
    {
        return *this;
    }

    if (snapshot_)
    {
        // Keep this order aligned with the live entity path below: callers
        // observe the first blocking state only.
        CHECK_BLOCKED(BlockedState::Jailed,         snapshot_->blockedStates & static_cast<uint16>(BlockedState::Jailed))
        CHECK_BLOCKED(BlockedState::Dead,           snapshot_->blockedStates & static_cast<uint16>(BlockedState::Dead))
        CHECK_BLOCKED(BlockedState::Crafting,       snapshot_->blockedStates & static_cast<uint16>(BlockedState::Crafting))
        CHECK_BLOCKED(BlockedState::Fishing,        snapshot_->blockedStates & static_cast<uint16>(BlockedState::Fishing))
        CHECK_BLOCKED(BlockedState::Sitting,        snapshot_->blockedStates & static_cast<uint16>(BlockedState::Sitting))
        CHECK_BLOCKED(BlockedState::Mounted,        snapshot_->blockedStates & static_cast<uint16>(BlockedState::Mounted))
        CHECK_BLOCKED(BlockedState::InEvent,        snapshot_->blockedStates & static_cast<uint16>(BlockedState::InEvent))
        CHECK_BLOCKED(BlockedState::Engaged,        snapshot_->blockedStates & static_cast<uint16>(BlockedState::Engaged))
        CHECK_BLOCKED(BlockedState::AbnormalStatus, snapshot_->blockedStates & static_cast<uint16>(BlockedState::AbnormalStatus))
        CHECK_BLOCKED(BlockedState::Monstrosity,    snapshot_->blockedStates & static_cast<uint16>(BlockedState::Monstrosity))
        CHECK_BLOCKED(BlockedState::Healing,        snapshot_->blockedStates & static_cast<uint16>(BlockedState::Healing))
        CHECK_BLOCKED(BlockedState::Charmed,        snapshot_->blockedStates & static_cast<uint16>(BlockedState::Charmed))
        CHECK_BLOCKED(BlockedState::PreventAction,  snapshot_->blockedStates & static_cast<uint16>(BlockedState::PreventAction))
        return *this;
    }

    // Checks do short-circuit, keep more expensive ones at the tail end
    // clang-format off
    CHECK_BLOCKED(BlockedState::Jailed,         jailutils::InPrison(PChar_))
    CHECK_BLOCKED(BlockedState::Dead,           PChar_->isDead())
    CHECK_BLOCKED(BlockedState::Crafting,       PChar_->isCrafting())
    CHECK_BLOCKED(BlockedState::Fishing,        PChar_->isFishing())
    CHECK_BLOCKED(BlockedState::Sitting,        PChar_->animation == ANIMATION_SIT || (PChar_->animation >= ANIMATION_SITCHAIR_0 && PChar_->animation <= ANIMATION_SITCHAIR_10))
    CHECK_BLOCKED(BlockedState::Mounted,        PChar_->isMounted())
    CHECK_BLOCKED(BlockedState::InEvent,        PChar_->isInEvent())
    CHECK_BLOCKED(BlockedState::Engaged,        PChar_->PAI->IsEngaged())
    CHECK_BLOCKED(BlockedState::AbnormalStatus, PChar_->status != STATUS_TYPE::NORMAL)
    CHECK_BLOCKED(BlockedState::Monstrosity,    PChar_->m_PMonstrosity != nullptr)
    CHECK_BLOCKED(BlockedState::Healing,        PChar_->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Healing) || PChar_->animation == ANIMATION_HEALING)
    CHECK_BLOCKED(BlockedState::Charmed,        PChar_->StatusEffectContainer->HasStatusEffect({ xi::StatusEffect::CharmI, xi::StatusEffect::CharmIi }))
    CHECK_BLOCKED(BlockedState::PreventAction,  PChar_->StatusEffectContainer->HasPreventActionEffect())
    // clang-format on

    return *this;
}

auto PacketValidator::isInEvent(Maybe<uint16_t> eventId) -> PacketValidator&
{
    if (!result_.valid())
    {
        return *this;
    }

    if (snapshot_)
    {
        if (!snapshot_->inEvent)
        {
            result_.addError("Not in an event.");
        }
        else if (eventId.has_value() && snapshot_->eventId != eventId)
        {
            result_.addError(std::format("Event ID mismatch {} != {}.", snapshot_->eventId.value_or(0), eventId.value()));
        }
        return *this;
    }

    if (!PChar_->isInEvent())
    {
        result_.addError("Not in an event.");
    }
    else
    {
        if (eventId.has_value())
        {
            if (PChar_->currentEvent->eventId != eventId.value())
            {
                result_.addError(std::format("Event ID mismatch {} != {}.", PChar_->currentEvent->eventId, eventId.value()));
            }
        }
    }

    return *this;
}

auto PacketValidator::hasLinkshellRank(const uint8_t slot, const LSTYPE rank) -> PacketValidator&
{
    if (!result_.valid())
    {
        return *this;
    }

    if (snapshot_)
    {
        if (slot < 1 || slot > 2)
        {
            result_.addError("Invalid linkshell slot.");
            return *this;
        }
        const auto index = slot - 1;
        if (!snapshot_->linkshellPresent[index] || !snapshot_->linkshellItems[index])
        {
            result_.addError("Invalid linkshell item.");
            return *this;
        }
        const auto actualRank = snapshot_->linkshellRanks[index];
        const auto required   = static_cast<uint8>(rank);
        const auto matchingRank = required == static_cast<uint8>(LSTYPE_LINKSHELL) ? actualRank == static_cast<uint8>(LSTYPE_LINKSHELL) :
                                  required == static_cast<uint8>(LSTYPE_PEARLSACK) ? actualRank == static_cast<uint8>(LSTYPE_LINKSHELL) || actualRank == static_cast<uint8>(LSTYPE_PEARLSACK) :
                                  required == static_cast<uint8>(LSTYPE_LINKPEARL) ? actualRank == static_cast<uint8>(LSTYPE_LINKSHELL) || actualRank == static_cast<uint8>(LSTYPE_PEARLSACK) || actualRank == static_cast<uint8>(LSTYPE_LINKPEARL) : false;
        if (!matchingRank)
        {
            result_.addError("Invalid linkshell rank.");
        }
        return *this;
    }

    CItemLinkshell* PItemLinkshell = nullptr;

    switch (slot)
    {
        case 1:
            PItemLinkshell = reinterpret_cast<CItemLinkshell*>(PChar_->getEquip(SLOT_LINK1));
            break;
        case 2:
            PItemLinkshell = reinterpret_cast<CItemLinkshell*>(PChar_->getEquip(SLOT_LINK2));
            break;
        default:
            result_.addError("Invalid linkshell slot.");
            return *this;
    }

    if (!PItemLinkshell || !PItemLinkshell->isType(ITEM_LINKSHELL))
    {
        result_.addError("Invalid linkshell item.");
        return *this;
    }

    const auto actualRank   = PItemLinkshell->GetLSType();
    auto       matchingRank = false;

    switch (rank)
    {
        case LSTYPE_LINKSHELL:
            matchingRank = actualRank == LSTYPE_LINKSHELL;
            break;
        case LSTYPE_PEARLSACK:
            matchingRank = (actualRank == LSTYPE_LINKSHELL ||
                            actualRank == LSTYPE_PEARLSACK);
            break;
        case LSTYPE_LINKPEARL:
            matchingRank = (actualRank == LSTYPE_LINKSHELL ||
                            actualRank == LSTYPE_LINKPEARL ||
                            actualRank == LSTYPE_PEARLSACK);
            break;
        default:
            matchingRank = false;
            break;
    }

    if (!matchingRank)
    {
        result_.addError("Invalid linkshell rank.");
    }

    return *this;
}

auto PacketValidator::hasZoneMiscFlag(const ZONEMISC flag) -> PacketValidator&
{
    if (!result_.valid())
    {
        return *this;
    }

    if (snapshot_)
    {
        if (snapshot_->gmLevel == 0 && (snapshot_->zoneMiscMask & static_cast<uint16>(flag)) != static_cast<uint16>(flag))
        {
            result_.addError(std::format("Zone {} does not allow misc flag {}.", snapshot_->zoneName, static_cast<uint16>(flag)));
        }
        return *this;
    }

    if (PChar_->m_GMlevel == 0 && !PChar_->loc.zone->CanUseMisc(flag))
    {
        result_.addError(std::format("Zone {} does not allow misc flag {}.", PChar_->loc.zone->getName(), static_cast<uint16_t>(flag)));
    }

    return *this;
}

auto PacketValidator::isPartyLeader() -> PacketValidator&
{
    if (!result_.valid())
    {
        return *this;
    }

    if (snapshot_)
    {
        if (!snapshot_->hasParty)
        {
            result_.addError("Not in a party.");
        }
        else if (!snapshot_->isPartyLeader)
        {
            result_.addError("Not the party leader.");
        }
        return *this;
    }

    if (!PChar_->PParty)
    {
        result_.addError("Not in a party.");
    }
    else if (PChar_->PParty->GetLeader() != PChar_)
    {
        result_.addError("Not the party leader.");
    }

    return *this;
}

auto PacketValidator::isAllianceLeader() -> PacketValidator&
{
    if (!result_.valid())
    {
        return *this;
    }

    if (snapshot_)
    {
        if (!snapshot_->hasParty)
        {
            result_.addError("Not in a party.");
        }
        else if (!snapshot_->hasAlliance)
        {
            result_.addError("Not in an alliance.");
        }
        else if (!snapshot_->hasAllianceMainParty)
        {
            result_.addError("No alliance main party.");
        }
        else if (!snapshot_->isAllianceLeader)
        {
            result_.addError("Not the alliance leader.");
        }
        return *this;
    }

    if (!PChar_->PParty)
    {
        result_.addError("Not in a party.");
    }
    else if (!PChar_->PParty->m_PAlliance)
    {
        result_.addError("Not in an alliance.");
    }
    else if (PChar_->PParty->m_PAlliance->getMainParty() == nullptr)
    {
        result_.addError("No alliance main party.");
    }
    else if (PChar_->PParty->m_PAlliance->getMainParty()->GetLeader() != PChar_)
    {
        result_.addError("Not the alliance leader.");
    }

    return *this;
}

auto PacketValidator::isEngaged() -> PacketValidator&
{
    if (!result_.valid())
    {
        return *this;
    }

    if (snapshot_)
    {
        if (!snapshot_->engaged)
        {
            result_.addError("Character is not engaged.");
        }
        return *this;
    }

    if (!PChar_->PAI->IsEngaged())
    {
        result_.addError("Character is not engaged.");
    }

    return *this;
}

auto PacketValidator::isInMogHouse() -> PacketValidator&
{
    if (!result_.valid())
    {
        return *this;
    }

    if (snapshot_)
    {
        if (!snapshot_->inMogHouse)
        {
            result_.addError("Character is not in Mog House.");
        }
        return *this;
    }

    if (!PChar_->inMogHouse())
    {
        result_.addError("Character is not in Mog House.");
    }

    return *this;
}

auto PacketValidator::hasKeyItem(const KeyItem keyItemId) -> PacketValidator&
{
    if (!result_.valid())
    {
        return *this;
    }

    if (snapshot_)
    {
        if (!snapshot_->keyItems.contains(static_cast<uint16>(keyItemId)))
        {
            result_.addError(std::format("Missing Key Item {}.", static_cast<uint16>(keyItemId)));
        }
        return *this;
    }

    if (!charutils::hasKeyItem(PChar_, keyItemId))
    {
        result_.addError(std::format("Missing Key Item {}.", static_cast<uint16_t>(keyItemId)));
    }

    return *this;
}

auto PacketValidator::requiresPriorPacket(PacketC2S expectedPacketId) -> PacketValidator&
{
    if (!result_.valid())
    {
        return *this;
    }

    if (snapshot_)
    {
        if (snapshot_->lastPacketType != static_cast<uint16>(expectedPacketId))
        {
            result_.addError(std::format("Expected prior packet {:#05x}, got {:#05x}.", static_cast<uint16>(expectedPacketId), snapshot_->lastPacketType));
        }
        return *this;
    }

    if (PChar_->m_LastPacketType != static_cast<uint16>(expectedPacketId))
    {
        result_.addError(std::format("Expected prior packet {:#05x}, got {:#05x}.", static_cast<uint16>(expectedPacketId), PChar_->m_LastPacketType));
    }

    return *this;
}
