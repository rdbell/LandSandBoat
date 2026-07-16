/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_s2c_motionmes_runtime.h"

#include <iostream>

#include "map/packets/s2c/motionmes_runtime.h"

namespace
{

auto expect(const motionmeshelpers::Plan actual, const motionmeshelpers::Plan expected, const char* label) -> bool
{
    if (actual.messageNumber != expected.messageNumber || actual.parameter != expected.parameter || actual.mode != expected.mode)
    {
        std::cerr << "s2c MOTIONMES runtime self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runS2CMotionMesRuntimeSelfTests() -> bool
{
    using namespace motionmeshelpers;

    const auto facts = CharacterFacts{
        .nation                              = 3,
        .hasMainWeapon                       = true,
        .mainWeaponID                        = 123,
        .hasRangedWeapon                     = true,
        .rangedWeaponID                      = 456,
        .rangedWeaponIsThrowing              = true,
        .rangedWeaponIsArcheryOrMarksmanship = false,
        .hasAmmoWeapon                       = false,
        .ammoWeaponID                        = 65535,
    };

    bool ok = true;
    ok      = expect(CharacterPlanFor(Emote::Salute, EmoteMode::Text, 0, facts), { static_cast<uint8>(Emote::Salute), 3, EmoteMode::Text }, "salute uses nation") && ok;
    ok      = expect(CharacterPlanFor(Emote::Hurray, EmoteMode::All, 0, facts), { static_cast<uint8>(Emote::Hurray), 123, EmoteMode::All }, "hurray uses main weapon") && ok;
    ok      = expect(CharacterPlanFor(Emote::Hurray, EmoteMode::All, 0, { .hasMainWeapon = true, .mainWeaponID = 65535 }), { static_cast<uint8>(Emote::Hurray), 0, EmoteMode::All }, "hurray ignores empty main weapon") && ok;
    ok      = expect(CharacterPlanFor(Emote::Aim, EmoteMode::All, 0, facts), { static_cast<uint8>(Emote::Aim), 456, EmoteMode::All }, "aim accepts throwing weapon") && ok;
    ok      = expect(CharacterPlanFor(Emote::Aim, EmoteMode::All, 0, {}), { static_cast<uint8>(Emote::Aim), 65535, EmoteMode::All }, "aim defaults to no weapon") && ok;

    auto archery = facts;
    archery.rangedWeaponIsThrowing              = false;
    archery.rangedWeaponIsArcheryOrMarksmanship = true;
    ok = expect(CharacterPlanFor(Emote::Aim, EmoteMode::All, 0, archery), { static_cast<uint8>(Emote::Aim), 65535, EmoteMode::All }, "aim requires ammo for archery") && ok;
    archery.hasAmmoWeapon = true;
    archery.ammoWeaponID  = 654;
    ok = expect(CharacterPlanFor(Emote::Aim, EmoteMode::All, 0, archery), { static_cast<uint8>(Emote::Aim), 456, EmoteMode::All }, "aim accepts archery with ammo") && ok;

    ok = expect(CharacterPlanFor(Emote::Bell, EmoteMode::Text, 9, facts), { static_cast<uint8>(Emote::Bell), 3, EmoteMode::Motion }, "bell forces motion and note") && ok;
    ok = expect(CharacterPlanFor(Emote::Job, EmoteMode::All, 0x21, facts), { 76, 2, EmoteMode::All }, "job adjusts message and parameter") && ok;
    ok = expect(NPCPlanFor(Emote::Bell, EmoteMode::Text), { static_cast<uint8>(Emote::Bell), 0, EmoteMode::Text }, "npc does not apply character shaping") && ok;
    return ok;
}
