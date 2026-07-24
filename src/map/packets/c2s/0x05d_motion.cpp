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

#include "0x05d_motion.h"

#include "entities/char_entity.h"
#include "items.h"
#include "lua/luautils.h"
#include "packets/s2c/0x029_battle_message.h"
#include "packets/s2c/0x05a_motionmes.h"
#include "utils/jailutils.h"

// Go host pure half: packetsystem.ValidateMotion / ProcessMotion /
// NewMotionHandler (6471); plan mappacket.ClientMotionDispatchPlanFor.

namespace
{

const std::set validBells = {
    DREAM_BELL,
    DREAM_BELL_P1,
    LADY_BELL,
    LADY_BELL_P1,
};

} // namespace

auto motionhelpers::MakeDispatchPlan(const bool inPrison, const uint16 styleMainItem,
                                     const uint16 equippedMainItem, const uint8 number,
                                     const uint16 param, const uint32 unlockedJobs) -> DispatchPlan
{
    if (inPrison)
    {
        return {
            .action              = DispatchAction::RejectInPrison,
            .sendCannotUseInArea = true,
        };
    }

    if (number == static_cast<uint8>(Emote::Bell))
    {
        const auto mainWeapon = styleMainItem != 0 ? styleMainItem : equippedMainItem;
        if (!validBells.contains(static_cast<ITEMID>(mainWeapon)) || param < 0x06 || param > 0x1E)
        {
            return {};
        }
    }
    else if (number == static_cast<uint8>(Emote::Job) && param != 0)
    {
        constexpr uint16 firstJobEmoteParam = 0x1E;
        constexpr uint16 jobEmoteCount      = 32;
        if (param < firstJobEmoteParam || param >= firstJobEmoteParam + jobEmoteCount ||
            !(unlockedJobs & (uint32{ 1 } << (param - firstJobEmoteParam))))
        {
            return {};
        }
    }

    return {
        .action                = DispatchAction::Dispatch,
        .broadcastMotionPacket = true,
        .invokePlayerEmoteLua  = true,
    };
}

auto GP_CLI_COMMAND_MOTION::validate(MapSession* PSession, const CCharEntity* PChar) const -> PacketValidationResult
{
    return PacketValidator(PChar)
        .blockedBy({ BlockedState::InEvent })
        .oneOf<EmoteMode>(this->Mode)
        .range("Number", this->Number, Emote::Point, Emote::Aim);
}

void GP_CLI_COMMAND_MOTION::process(MapSession* PSession, CCharEntity* PChar) const
{
    const auto styleMainItem = PChar->styleItems[SLOT_MAIN];
    const auto equippedMainItem = PChar->getEquip(SLOT_MAIN) != nullptr
                                      ? PChar->getEquip(SLOT_MAIN)->getID()
                                      : 0;
    const auto plan = motionhelpers::MakeDispatchPlan(
        jailutils::InPrison(PChar), styleMainItem, equippedMainItem, this->Number,
        this->Param, PChar->jobs.unlocked);

    if (plan.sendCannotUseInArea)
    {
        PChar->pushPacket<GP_SERV_COMMAND_BATTLE_MESSAGE>(PChar, PChar, 0, 0, MsgBasic::CannotUseInArea);
    }
    if (plan.action != motionhelpers::DispatchAction::Dispatch)
    {
        return;
    }

    PChar->loc.zone->PushPacket(PChar, CHAR_INRANGE_SELF, std::make_unique<GP_SERV_COMMAND_MOTIONMES>(PChar, this->UniqueNo, this->ActIndex, static_cast<Emote>(this->Number), static_cast<EmoteMode>(this->Mode), this->Param));

    luautils::OnPlayerEmote(PChar, static_cast<Emote>(this->Number));
}
