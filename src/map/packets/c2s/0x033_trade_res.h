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

#pragma once

#include <array>

#include "base.h"

enum class GP_CLI_COMMAND_TRADE_RES_KIND : uint32_t
{
    Start       = 0,
    Cancell     = 1,
    Make        = 2,
    MakeCancell = 3,
};

// The map host owns entity lookup, containers, packets, and item transfers.
// This describes the ordered, portable decisions made once those facts are
// available, mirroring GP_CLI_COMMAND_TRADE_RES::process.
namespace tradereshelpers
{

enum class Action : uint8
{
    ClearPendingTargets,
    SetInitiatorTradeContainer,
    NotifyTargetStart,
    SetTargetTradeContainer,
    NotifyInitiatorStart,
    CleanTargetTradeContainer,
    CleanInitiatorTradeContainer,
    NotifyTargetCancell,
    LockInitiatorTradeContainer,
    NotifyTargetMake,
    TradeInitiatorToTarget,
    NotifyTargetEnd,
    TradeTargetToInitiator,
    NotifyInitiatorEnd,
    NotifyInitiatorCancell,
};

struct RuntimeFacts
{
    bool peerResolved;
    bool initiatorContainerEmpty;
    bool targetContainerEmpty;
    bool withinTradeDistance;
    bool sameMogHouse;
    bool initiatorContainerIsTrade;
    bool targetContainerIsTrade;
    bool targetContainerLocked;
    bool initiatorCanTrade;
    bool targetCanTrade;
};

struct ActionPlan
{
    std::array<Action, 10> actions{};
    uint8                  count{};
};

[[nodiscard]] constexpr auto BuildActionPlan(const GP_CLI_COMMAND_TRADE_RES_KIND kind, const RuntimeFacts facts) -> ActionPlan
{
    ActionPlan plan;
    if (!facts.peerResolved)
    {
        return plan;
    }

    const auto append = [&plan](const Action action)
    {
        plan.actions[plan.count++] = action;
    };

    switch (kind)
    {
        case GP_CLI_COMMAND_TRADE_RES_KIND::Start:
            if (!facts.initiatorContainerEmpty || !facts.targetContainerEmpty || !facts.withinTradeDistance || !facts.sameMogHouse)
            {
                append(Action::ClearPendingTargets);
                return plan;
            }
            append(Action::SetInitiatorTradeContainer);
            append(Action::NotifyTargetStart);
            append(Action::SetTargetTradeContainer);
            append(Action::NotifyInitiatorStart);
            return plan;
        case GP_CLI_COMMAND_TRADE_RES_KIND::Cancell:
            if (facts.targetContainerIsTrade)
            {
                append(Action::CleanTargetTradeContainer);
            }
            if (facts.initiatorContainerIsTrade)
            {
                append(Action::CleanInitiatorTradeContainer);
            }
            append(Action::ClearPendingTargets);
            append(Action::NotifyTargetCancell);
            return plan;
        case GP_CLI_COMMAND_TRADE_RES_KIND::Make:
            append(Action::LockInitiatorTradeContainer);
            append(Action::NotifyTargetMake);
            if (!facts.targetContainerLocked)
            {
                return plan;
            }
            if (facts.initiatorCanTrade && facts.targetCanTrade)
            {
                append(Action::TradeInitiatorToTarget);
                append(Action::NotifyTargetEnd);
                append(Action::TradeTargetToInitiator);
                append(Action::NotifyInitiatorEnd);
            }
            else
            {
                append(Action::NotifyTargetCancell);
                append(Action::NotifyInitiatorCancell);
            }
            append(Action::CleanInitiatorTradeContainer);
            append(Action::CleanTargetTradeContainer);
            append(Action::ClearPendingTargets);
            return plan;
        case GP_CLI_COMMAND_TRADE_RES_KIND::MakeCancell:
            return plan;
    }
    return plan;
}

} // namespace tradereshelpers

// https://github.com/atom0s/XiPackets/tree/main/world/client/0x0033
// This packet is sent by the client when responding to a trade request.
GP_CLI_PACKET(GP_CLI_COMMAND_TRADE_RES,
              uint32_t Kind;         // PS2: Kind
              uint16_t TradeCounter; // PS2: TradeCounter
);
