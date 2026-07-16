/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#pragma once

#include "enums/emote.h"

// MOTIONMES derives its message number, parameter, and mode from an emote and
// selected equipment. Entity and equipment ownership stay in the packet
// constructors; these are the portable facts they read.
namespace motionmeshelpers
{

struct CharacterFacts
{
    uint8  nation{};
    bool   hasMainWeapon{};
    uint16 mainWeaponID{};
    bool   hasRangedWeapon{};
    uint16 rangedWeaponID{};
    bool   rangedWeaponIsThrowing{};
    bool   rangedWeaponIsArcheryOrMarksmanship{};
    bool   hasAmmoWeapon{};
    uint16 ammoWeaponID{};
};

struct Plan
{
    uint16    messageNumber{};
    uint16    parameter{};
    EmoteMode mode{};
};

[[nodiscard]] constexpr auto CharacterPlanFor(const Emote emote, const EmoteMode mode, const uint16 extra, const CharacterFacts& facts) -> Plan
{
    auto plan = Plan{
        .messageNumber = static_cast<uint16>(emote == Emote::Job ? static_cast<uint16>(static_cast<uint8>(emote) + (extra - 0x1F)) : static_cast<uint8>(emote)),
        .mode          = mode,
    };

    if (emote == Emote::Salute)
    {
        plan.parameter = facts.nation;
    }
    else if (emote == Emote::Hurray)
    {
        if (facts.hasMainWeapon && facts.mainWeaponID != 65535)
        {
            plan.parameter = facts.mainWeaponID;
        }
    }
    else if (emote == Emote::Aim)
    {
        plan.parameter = 65535;
        if (facts.hasRangedWeapon && facts.rangedWeaponID != 65535)
        {
            if (facts.rangedWeaponIsThrowing ||
                (facts.rangedWeaponIsArcheryOrMarksmanship && facts.hasAmmoWeapon && facts.ammoWeaponID != 65535))
            {
                plan.parameter = facts.rangedWeaponID;
            }
        }
    }
    else if (emote == Emote::Bell)
    {
        plan.mode      = EmoteMode::Motion;
        plan.parameter = static_cast<uint16>(extra - 0x06);
    }
    else if (emote == Emote::Job)
    {
        plan.parameter = static_cast<uint16>(extra - 0x1F);
    }

    return plan;
}

[[nodiscard]] constexpr auto NPCPlanFor(const Emote emote, const EmoteMode mode) -> Plan
{
    return { static_cast<uint8>(emote), 0, mode };
}

} // namespace motionmeshelpers
