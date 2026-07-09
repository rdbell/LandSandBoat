/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
  Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses/

===========================================================================
*/

#include "test_item_exdata.h"

#include "map/items/exdata.h"
#include "map/items/item.h"
#include "map/items/item_equipment.h"
#include "map/items/item_flowerpot.h"
#include "map/items/item_furnishing.h"
#include "map/items/item_linkshell.h"
#include "map/items/item_usable.h"
#include "map/items/item_weapon.h"
#include "map/items.h"
#include "map/utils/fishingutils.h"

#include <cstdint>
#include <iostream>
#include <map>

namespace fishingutils
{
extern std::map<uint32, fish_t*> FishList;
}

namespace
{

auto expectType(Exdata::Type actual, Exdata::Type expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "item exdata self-test failed: " << label << " got "
                  << static_cast<int>(actual) << " expected " << static_cast<int>(expected) << '\n';
        return false;
    }
    return true;
}

auto expectUInt(std::uint64_t actual, std::uint64_t expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "item exdata self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectString(const std::string& actual, const std::string& expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "item exdata self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

struct LocalExdata
{
    uint16 Marker;
    uint8  Payload[22];
};

static_assert(sizeof(LocalExdata) == CItem::extra_size);

auto testTypeEnumGoldenValues() -> bool
{
    bool ok = true;
    ok      = expectUInt(static_cast<uint8>(Exdata::Type::None), 0, "Type::None") && ok;
    ok      = expectUInt(static_cast<uint8>(Exdata::Type::Augment), 1, "Type::Augment") && ok;
    ok      = expectUInt(static_cast<uint8>(Exdata::Type::Usable), 2, "Type::Usable") && ok;
    ok      = expectUInt(static_cast<uint8>(Exdata::Type::Mannequin), 3, "Type::Mannequin") && ok;
    ok      = expectUInt(static_cast<uint8>(Exdata::Type::Furniture), 4, "Type::Furniture") && ok;
    ok      = expectUInt(static_cast<uint8>(Exdata::Type::FlowerPot), 5, "Type::FlowerPot") && ok;
    ok      = expectUInt(static_cast<uint8>(Exdata::Type::Linkshell), 6, "Type::Linkshell") && ok;
    ok      = expectUInt(static_cast<uint8>(Exdata::Type::Fish), 7, "Type::Fish") && ok;
    ok      = expectUInt(static_cast<uint8>(Exdata::Type::BettingSlip), 8, "Type::BettingSlip") && ok;
    ok      = expectUInt(static_cast<uint8>(Exdata::Type::SoulPlate), 9, "Type::SoulPlate") && ok;
    ok      = expectUInt(static_cast<uint8>(Exdata::Type::SoulReflector), 10, "Type::SoulReflector") && ok;
    ok      = expectUInt(static_cast<uint8>(Exdata::Type::AssaultLog), 11, "Type::AssaultLog") && ok;
    ok      = expectUInt(static_cast<uint8>(Exdata::Type::LotteryTicket), 12, "Type::LotteryTicket") && ok;
    ok      = expectUInt(static_cast<uint8>(Exdata::Type::Tabula), 13, "Type::Tabula") && ok;
    ok      = expectUInt(static_cast<uint8>(Exdata::Type::Evolith), 14, "Type::Evolith") && ok;
    ok      = expectUInt(static_cast<uint8>(Exdata::Type::CraftingSet), 15, "Type::CraftingSet") && ok;
    ok      = expectUInt(static_cast<uint8>(Exdata::Type::BrennerBook), 16, "Type::BrennerBook") && ok;
    ok      = expectUInt(static_cast<uint8>(Exdata::Type::GlowingLamp), 17, "Type::GlowingLamp") && ok;
    ok      = expectUInt(static_cast<uint8>(Exdata::Type::LegionPass), 18, "Type::LegionPass") && ok;
    ok      = expectUInt(static_cast<uint8>(Exdata::Type::Serialized), 19, "Type::Serialized") && ok;
    ok      = expectUInt(static_cast<uint8>(Exdata::Type::PerpetualHourglass), 20, "Type::PerpetualHourglass") && ok;
    ok      = expectUInt(static_cast<uint8>(Exdata::Type::ChocoboEgg), 21, "Type::ChocoboEgg") && ok;
    ok      = expectUInt(static_cast<uint8>(Exdata::Type::ChocoboCard), 22, "Type::ChocoboCard") && ok;
    ok      = expectUInt(static_cast<uint8>(Exdata::Type::Escutcheon), 23, "Type::Escutcheon") && ok;
    ok      = expectUInt(static_cast<uint8>(Exdata::Type::RaceCertificate), 24, "Type::RaceCertificate") && ok;
    ok      = expectUInt(static_cast<uint8>(Exdata::Type::MeebleGrimoire), 25, "Type::MeebleGrimoire") && ok;
    ok      = expectUInt(static_cast<uint8>(Exdata::Type::HoneymoonTicket), 26, "Type::HoneymoonTicket") && ok;
    ok      = expectUInt(static_cast<uint8>(Exdata::Type::WeaponUnlock), 27, "Type::WeaponUnlock") && ok;
    ok      = expectUInt(static_cast<uint8>(Exdata::AugmentKindFlags::HasAugments), 0x02, "AugmentKindFlags::HasAugments") && ok;
    ok      = expectUInt(static_cast<uint8>(Exdata::AugmentKindFlags::Bundled), 0x03, "AugmentKindFlags::Bundled") && ok;
    ok      = expectUInt(static_cast<uint8>(Exdata::AugmentSubKindFlags::Standard), 0x03, "AugmentSubKindFlags::Standard") && ok;
    ok      = expectUInt(static_cast<uint8>(Exdata::AugmentSubKindFlags::Escutcheon), 0x08, "AugmentSubKindFlags::Escutcheon") && ok;
    ok      = expectUInt(static_cast<uint8>(Exdata::AugmentSubKindFlags::Serialized), 0x10, "AugmentSubKindFlags::Serialized") && ok;
    ok      = expectUInt(static_cast<uint8>(Exdata::AugmentSubKindFlags::Mezzotint), 0x20, "AugmentSubKindFlags::Mezzotint") && ok;
    ok      = expectUInt(static_cast<uint8>(Exdata::AugmentSubKindFlags::Trial), 0x40, "AugmentSubKindFlags::Trial") && ok;
    ok      = expectUInt(static_cast<uint8>(Exdata::AugmentSubKindFlags::Evolith), 0x80, "AugmentSubKindFlags::Evolith") && ok;
    return ok;
}

auto testItemIDTypeDispatch() -> bool
{
    struct Case
    {
        uint16       id;
        Exdata::Type expected;
        const char*  label;
    };

    const Case cases[] = {
        { LEGION_PASS, Exdata::Type::LegionPass, "legion pass" },
        { PERPETUAL_HOURGLASS, Exdata::Type::PerpetualHourglass, "perpetual hourglass" },
        { COPY_OF_THE_WYVERN_CODEX, Exdata::Type::BrennerBook, "wyvern codex" },
        { COPY_OF_THE_BALLISTA_REDBOOK, Exdata::Type::BrennerBook, "ballista redbook lower" },
        { PAGE_OF_THE_BALLISTA_WHITEBOOK, Exdata::Type::BrennerBook, "ballista whitebook upper" },
        { COPY_OF_THE_BRENNER_BLUEBOOK, Exdata::Type::BrennerBook, "brenner bluebook lower" },
        { PAGE_OF_THE_BRENNER_BLACKBOOK, Exdata::Type::BrennerBook, "brenner blackbook upper" },
        { SOUL_PLATE, Exdata::Type::SoulPlate, "soul plate" },
        { GAUGER_PLATE, Exdata::Type::SoulPlate, "gauger plate" },
        { FIEND_PLATE, Exdata::Type::SoulPlate, "fiend plate" },
        { SOUL_REFLECTOR, Exdata::Type::SoulReflector, "soul reflector" },
        { OFFICIAL_SOUL_REFLECTOR, Exdata::Type::SoulReflector, "official soul reflector" },
        { CHOCOBET_TICKET, Exdata::Type::BettingSlip, "chocobet ticket" },
        { RACE_COMPLETION_CERTIFICATE, Exdata::Type::RaceCertificate, "race certificate" },
        { VCS_HONEYMOON_TICKET, Exdata::Type::HoneymoonTicket, "honeymoon ticket" },
        { DILIGENCE_GRIMOIRE, Exdata::Type::MeebleGrimoire, "meeble grimoire lower" },
        { SANCTITY_GRIMOIRE, Exdata::Type::MeebleGrimoire, "meeble grimoire upper" },
        { LEUJAOAM_OBSERVATION_LOG, Exdata::Type::AssaultLog, "assault log lower" },
        { ILRUSI_TRAVEL_LEDGER, Exdata::Type::AssaultLog, "assault log upper" },
        { BONANZA_PEARL, Exdata::Type::LotteryTicket, "bonanza pearl" },
        { MOG_BONANZA_MARBLE, Exdata::Type::LotteryTicket, "bonanza marble" },
        { MAZE_TABULA_M01, Exdata::Type::Tabula, "maze tabula m lower" },
        { MAZE_TABULA_R03, Exdata::Type::Tabula, "maze tabula r upper" },
        { EVOLITH, Exdata::Type::Evolith, "evolith" },
        { WOODWORKING_SET_25, Exdata::Type::CraftingSet, "crafting set lower" },
        { COOKING_SET_95, Exdata::Type::CraftingSet, "crafting set upper" },
        { GLOWING_LAMP, Exdata::Type::GlowingLamp, "glowing lamp" },
        { CHOCOBO_EGG_FAINTLY, Exdata::Type::ChocoboEgg, "chocobo egg faintly" },
        { CHOCOBO_EGG_SOMEWHAT, Exdata::Type::ChocoboEgg, "chocobo egg somewhat" },
        { VCS_REGISTRATION_CARD, Exdata::Type::ChocoboCard, "registration card" },
        { CHOCOCARD_M, Exdata::Type::ChocoboCard, "chococard m" },
        { CHOCOCARD_F, Exdata::Type::ChocoboCard, "chococard f" },
        { CRA_RACING_FORM, Exdata::Type::ChocoboCard, "racing form" },
        { JOINERS_ASPIS, Exdata::Type::Escutcheon, "escutcheon lower" },
        { static_cast<uint16>(JOINERS_ASPIS + 4), Exdata::Type::None, "finished escutcheon" },
        { CHEFS_SHIELD, Exdata::Type::Escutcheon, "escutcheon upper" },
        { LU_SHANGS_FISHING_ROD_P1, Exdata::Type::Serialized, "lu shangs rod +1" },
        { EBISU_FISHING_ROD_P1, Exdata::Type::Serialized, "ebisu rod +1" },
    };

    bool ok = true;
    for (const auto& testCase : cases)
    {
        CItem item(testCase.id);
        ok = expectType(Exdata::getType(&item), testCase.expected, testCase.label) && ok;
    }
    return ok;
}

auto testPredicateTypeDispatchAndPrecedence() -> bool
{
    bool ok = true;

    ok = expectType(Exdata::getType(nullptr), Exdata::Type::None, "null item") && ok;

    CItem none(0x1234);
    ok = expectType(Exdata::getType(&none), Exdata::Type::None, "plain item") && ok;

    CItemLinkshell linkshell(0x1234);
    ok = expectType(Exdata::getType(&linkshell), Exdata::Type::Linkshell, "linkshell type") && ok;

    CItem fishItem(0x3000);
    fish_t fish{};
    fishingutils::FishList[fishItem.getID()] = &fish;
    ok = expectType(Exdata::getType(&fishItem), Exdata::Type::Fish, "fish list membership") && ok;
    fishingutils::FishList.erase(fishItem.getID());

    CItem fishBeforeID(LEGION_PASS);
    fishingutils::FishList[fishBeforeID.getID()] = &fish;
    ok = expectType(Exdata::getType(&fishBeforeID), Exdata::Type::Fish, "fish before id dispatch") && ok;
    fishingutils::FishList.erase(fishBeforeID.getID());

    CItemFlowerpot flowerpot(0x4000);
    ok = expectType(Exdata::getType(&flowerpot), Exdata::Type::FlowerPot, "flowerpot before furnishing") && ok;

    CItemFurnishing mannequin(256);
    ok = expectType(Exdata::getType(&mannequin), Exdata::Type::Mannequin, "mannequin furnishing") && ok;

    CItemFurnishing furniture(0x4001);
    ok = expectType(Exdata::getType(&furniture), Exdata::Type::Furniture, "regular furnishing") && ok;

    CItemWeapon weapon(0x5000);
    weapon.setSkillType(SKILL_SWORD);
    weapon.setTotalUnlockPointsNeeded(250);
    ok = expectType(Exdata::getType(&weapon), Exdata::Type::WeaponUnlock, "unlockable weapon before equipment") && ok;

    CItemWeapon nonUnlockableWeapon(0x5004);
    ok = expectType(Exdata::getType(&nonUnlockableWeapon), Exdata::Type::Augment, "non-unlockable weapon equipment fallback") && ok;

    CItemWeapon chargedWeapon(0x5001);
    chargedWeapon.setSubType(ITEM_CHARGED);
    ok = expectType(Exdata::getType(&chargedWeapon), Exdata::Type::Usable, "charged weapon") && ok;

    CItemUsable chargedUsable(0x5002);
    chargedUsable.setSubType(ITEM_CHARGED);
    ok = expectType(Exdata::getType(&chargedUsable), Exdata::Type::Usable, "charged usable") && ok;

    CItemEquipment equipment(0x5003);
    ok = expectType(Exdata::getType(&equipment), Exdata::Type::Augment, "equipment augment fallback") && ok;
    equipment.setSubType(ITEM_CHARGED);
    ok = expectType(Exdata::getType(&equipment), Exdata::Type::Usable, "charged equipment before augment") && ok;

    return ok;
}

auto testRawExdataOverlay() -> bool
{
    CItem item(0x6000);

    auto& exdata = item.exdata<LocalExdata>();
    exdata.Marker = 0x1234;
    exdata.Payload[0] = 0x56;
    exdata.Payload[21] = 0x78;

    bool ok = true;
    ok      = expectUInt(item.m_extra[0], 0x34, "raw marker low byte") && ok;
    ok      = expectUInt(item.m_extra[1], 0x12, "raw marker high byte") && ok;
    ok      = expectUInt(item.m_extra[2], 0x56, "raw payload first byte") && ok;
    ok      = expectUInt(item.m_extra[23], 0x78, "raw payload last byte") && ok;

    const CItem copy(item);
    ok = expectUInt(copy.exdata<LocalExdata>().Marker, 0x1234, "copy marker") && ok;
    ok = expectUInt(copy.exdata<LocalExdata>().Payload[0], 0x56, "copy payload first byte") && ok;
    ok = expectUInt(copy.exdata<LocalExdata>().Payload[21], 0x78, "copy payload last byte") && ok;
    return ok;
}

auto testTimerInfoTableSerialization() -> bool
{
    sol::state lua;
    auto       input = lua.create_table();
    input["remainingCharges"] = 7;
    input["flags"]            = 0xC000;
    input["timeValue1"]       = 0x11223344;
    input["timeValue2"]       = 0x55667788;
    input["signature"]        = "OmegaXI2026";

    Exdata::ItemTimerInfo timer{};
    timer.fromTable(input);

    bool ok = true;
    ok      = expectUInt(timer.Header, 1, "timer header from table") && ok;
    ok      = expectUInt(timer.RemainingCharges, 7, "timer remaining charges from table") && ok;
    ok      = expectUInt(timer.Flags, 0xC000, "timer flags from table") && ok;
    ok      = expectUInt(timer.TimeValue1, 0x11223344, "timer time value 1 from table") && ok;
    ok      = expectUInt(timer.TimeValue2, 0x55667788, "timer time value 2 from table") && ok;
    ok      = expectString(Exdata::decodeSignature(timer.Signature), "OmegaXI2026", "timer signature from table") && ok;

    auto output = lua.create_table();
    timer.toTable(output);
    ok = expectUInt(output["remainingCharges"].get<uint8>(), 7, "timer remaining charges to table") && ok;
    ok = expectUInt(output["flags"].get<uint16>(), 0xC000, "timer flags to table") && ok;
    ok = expectUInt(output["timeValue1"].get<uint32>(), 0x11223344, "timer time value 1 to table") && ok;
    ok = expectUInt(output["timeValue2"].get<uint32>(), 0x55667788, "timer time value 2 to table") && ok;
    ok = expectString(output["signature"].get<std::string>(), "OmegaXI2026", "timer signature to table") && ok;

    auto partial = lua.create_table();
    partial["flags"] = 0x9000;
    timer.fromTable(partial);
    ok = expectUInt(timer.Header, 1, "timer header partial update") && ok;
    ok = expectUInt(timer.RemainingCharges, 7, "timer remaining charges preserved") && ok;
    ok = expectUInt(timer.Flags, 0x9000, "timer flags partial update") && ok;
    ok = expectUInt(timer.TimeValue1, 0x11223344, "timer time value 1 preserved") && ok;
    ok = expectUInt(timer.TimeValue2, 0x55667788, "timer time value 2 preserved") && ok;
    ok = expectString(Exdata::decodeSignature(timer.Signature), "OmegaXI2026", "timer signature preserved") && ok;
    return ok;
}

auto testLogTicketTableSerialization() -> bool
{
    sol::state lua;
    bool       ok = true;

    auto assaultInput = lua.create_table();
    auto flagsInput   = lua.create_table();
    flagsInput[1]     = true;
    flagsInput[3]     = true;
    flagsInput[10]    = true;
    assaultInput["flags"] = flagsInput;

    Exdata::AssaultLog assault{};
    assault.fromTable(assaultInput);
    const auto* assaultRaw = reinterpret_cast<const uint8*>(&assault);
    ok = expectUInt(assault.Flag1, 1, "assault flag 1 from table") && ok;
    ok = expectUInt(assault.Flag2, 0, "assault flag 2 from table") && ok;
    ok = expectUInt(assault.Flag3, 1, "assault flag 3 from table") && ok;
    ok = expectUInt(assault.Flag10, 1, "assault flag 10 from table") && ok;
    ok = expectUInt(assaultRaw[0], 0x05, "assault raw byte 0") && ok;
    ok = expectUInt(assaultRaw[1], 0x02, "assault raw byte 1") && ok;

    auto assaultOutput = lua.create_table();
    assault.toTable(assaultOutput);
    auto flagsOutput = assaultOutput["flags"].get<sol::table>();
    ok = expectUInt(flagsOutput[1].get<bool>(), true, "assault flag 1 to table") && ok;
    ok = expectUInt(flagsOutput[2].get<bool>(), false, "assault flag 2 to table") && ok;
    ok = expectUInt(flagsOutput[3].get<bool>(), true, "assault flag 3 to table") && ok;
    ok = expectUInt(flagsOutput[10].get<bool>(), true, "assault flag 10 to table") && ok;

    auto assaultPartial = lua.create_table();
    auto partialFlags   = lua.create_table();
    partialFlags[2]     = true;
    assaultPartial["flags"] = partialFlags;
    assault.fromTable(assaultPartial);
    ok = expectUInt(assault.Flag1, 1, "assault flag 1 preserved") && ok;
    ok = expectUInt(assault.Flag2, 1, "assault flag 2 partial update") && ok;
    ok = expectUInt(assault.Flag3, 1, "assault flag 3 preserved") && ok;
    ok = expectUInt(assault.Flag10, 1, "assault flag 10 preserved") && ok;

    auto bettingInput = lua.create_table();
    bettingInput["raceId"]       = 0x23456;
    bettingInput["raceGrade"]    = 0x2A;
    bettingInput["racePairingL"] = 0x0B;
    bettingInput["racePairingR"] = 0x0C;
    bettingInput["quills"]       = 0x155;

    Exdata::BettingSlip betting{};
    betting.fromTable(bettingInput);
    const auto* bettingRaw = reinterpret_cast<const uint8*>(&betting);
    ok = expectUInt(betting.RaceId, 0x23456, "betting race id from table") && ok;
    ok = expectUInt(betting.RaceGrade, 0x2A, "betting race grade from table") && ok;
    ok = expectUInt(betting.RacePairingL, 0x0B, "betting left pairing from table") && ok;
    ok = expectUInt(betting.RacePairingR, 0x0C, "betting right pairing from table") && ok;
    ok = expectUInt(betting.Quills, 0x155, "betting quills from table") && ok;
    ok = expectUInt(bettingRaw[0], 0x56, "betting raw byte 0") && ok;
    ok = expectUInt(bettingRaw[1], 0x34, "betting raw byte 1") && ok;
    ok = expectUInt(bettingRaw[2], 0xAA, "betting raw byte 2") && ok;
    ok = expectUInt(bettingRaw[3], 0xCB, "betting raw byte 3") && ok;
    ok = expectUInt(bettingRaw[4], 0x55, "betting raw byte 4") && ok;
    ok = expectUInt(bettingRaw[5], 0x01, "betting raw byte 5") && ok;

    auto bettingOutput = lua.create_table();
    betting.toTable(bettingOutput);
    ok = expectUInt(bettingOutput["raceId"].get<uint32>(), 0x23456, "betting race id to table") && ok;
    ok = expectUInt(bettingOutput["raceGrade"].get<uint32>(), 0x2A, "betting race grade to table") && ok;
    ok = expectUInt(bettingOutput["racePairingL"].get<uint32>(), 0x0B, "betting left pairing to table") && ok;
    ok = expectUInt(bettingOutput["racePairingR"].get<uint32>(), 0x0C, "betting right pairing to table") && ok;
    ok = expectUInt(bettingOutput["quills"].get<uint16>(), 0x155, "betting quills to table") && ok;

    auto bettingPartial = lua.create_table();
    bettingPartial["quills"] = 0x2AA;
    betting.fromTable(bettingPartial);
    ok = expectUInt(betting.RaceId, 0x23456, "betting race id preserved") && ok;
    ok = expectUInt(betting.Quills, 0x2AA, "betting quills partial update") && ok;

    auto brennerInput = lua.create_table();
    brennerInput["timeValue"] = 0x11223344;
    brennerInput["level"]     = 75;

    Exdata::BrennerBook brenner{};
    brenner.fromTable(brennerInput);
    const auto* brennerRaw = reinterpret_cast<const uint8*>(&brenner);
    ok = expectUInt(brenner.TimeValue, 0x11223344, "brenner time value from table") && ok;
    ok = expectUInt(brenner.Level, 75, "brenner level from table") && ok;
    ok = expectUInt(brenner.Mode, 1, "brenner mode from table") && ok;
    ok = expectUInt(brennerRaw[0], 0x44, "brenner raw byte 0") && ok;
    ok = expectUInt(brennerRaw[3], 0x11, "brenner raw byte 3") && ok;
    ok = expectUInt(brennerRaw[4], 0x4B, "brenner raw byte 4") && ok;
    ok = expectUInt(brennerRaw[11], 0x01, "brenner raw mode byte") && ok;

    auto brennerOutput = lua.create_table();
    brenner.toTable(brennerOutput);
    ok = expectUInt(brennerOutput["timeValue"].get<uint32>(), 0x11223344, "brenner time value to table") && ok;
    ok = expectUInt(brennerOutput["level"].get<uint32>(), 75, "brenner level to table") && ok;

    auto brennerPartial = lua.create_table();
    brennerPartial["level"] = 50;
    brenner.fromTable(brennerPartial);
    ok = expectUInt(brenner.TimeValue, 0x11223344, "brenner time value preserved") && ok;
    ok = expectUInt(brenner.Level, 50, "brenner level partial update") && ok;
    ok = expectUInt(brenner.Mode, 1, "brenner mode partial update") && ok;

    auto lotteryInput = lua.create_table();
    lotteryInput["number"] = 0x345678;
    lotteryInput["title"]  = 0x9A;

    Exdata::LotteryTicket lottery{};
    lottery.fromTable(lotteryInput);
    const auto* lotteryRaw = reinterpret_cast<const uint8*>(&lottery);
    ok = expectUInt(lottery.Number, 0x345678, "lottery number from table") && ok;
    ok = expectUInt(lottery.Title, 0x9A, "lottery title from table") && ok;
    ok = expectUInt(lotteryRaw[0], 0x78, "lottery raw byte 0") && ok;
    ok = expectUInt(lotteryRaw[1], 0x56, "lottery raw byte 1") && ok;
    ok = expectUInt(lotteryRaw[2], 0x34, "lottery raw byte 2") && ok;
    ok = expectUInt(lotteryRaw[3], 0x9A, "lottery raw byte 3") && ok;

    auto lotteryOutput = lua.create_table();
    lottery.toTable(lotteryOutput);
    ok = expectUInt(lotteryOutput["number"].get<uint32>(), 0x345678, "lottery number to table") && ok;
    ok = expectUInt(lotteryOutput["title"].get<uint8>(), 0x9A, "lottery title to table") && ok;

    auto lotteryPartial = lua.create_table();
    lotteryPartial["title"] = 0x7B;
    lottery.fromTable(lotteryPartial);
    ok = expectUInt(lottery.Number, 0x345678, "lottery number preserved") && ok;
    ok = expectUInt(lottery.Title, 0x7B, "lottery title partial update") && ok;

    auto certificateInput = lua.create_table();
    certificateInput["raceId"]    = 0x23456;
    certificateInput["raceGrade"] = 0x2A;

    Exdata::RaceCertificate certificate{};
    certificate.fromTable(certificateInput);
    const auto* certificateRaw = reinterpret_cast<const uint8*>(&certificate);
    ok = expectUInt(certificate.RaceId, 0x23456, "race certificate race id from table") && ok;
    ok = expectUInt(certificate.RaceGrade, 0x2A, "race certificate grade from table") && ok;
    ok = expectUInt(certificateRaw[0], 0x56, "race certificate raw byte 0") && ok;
    ok = expectUInt(certificateRaw[1], 0x34, "race certificate raw byte 1") && ok;
    ok = expectUInt(certificateRaw[2], 0xAA, "race certificate raw byte 2") && ok;
    ok = expectUInt(certificateRaw[3], 0x00, "race certificate raw byte 3") && ok;

    auto certificateOutput = lua.create_table();
    certificate.toTable(certificateOutput);
    ok = expectUInt(certificateOutput["raceId"].get<uint32>(), 0x23456, "race certificate race id to table") && ok;
    ok = expectUInt(certificateOutput["raceGrade"].get<uint32>(), 0x2A, "race certificate grade to table") && ok;

    auto certificatePartial = lua.create_table();
    certificatePartial["raceGrade"] = 0x15;
    certificate.fromTable(certificatePartial);
    ok = expectUInt(certificate.RaceId, 0x23456, "race certificate race id preserved") && ok;
    ok = expectUInt(certificate.RaceGrade, 0x15, "race certificate grade partial update") && ok;

    return ok;
}

auto testPassTimerTableSerialization() -> bool
{
    sol::state lua;
    bool       ok = true;

    auto lampInput = lua.create_table();
    lampInput["chamberId"] = 3;
    lampInput["flags"]     = 5;
    lampInput["startTime"] = 0x55667788;
    lampInput["endTime"]   = 0x11223344;

    Exdata::GlowingLamp lamp{};
    lamp.fromTable(lampInput);
    const auto* lampRaw = reinterpret_cast<const uint8*>(&lamp);
    ok = expectUInt(lamp.ChamberId, 0x20, "glowing lamp chamber id from table") && ok;
    ok = expectUInt(lamp.Flags, 5, "glowing lamp flags from table") && ok;
    ok = expectUInt(lamp.StartTime, 0x55667788, "glowing lamp start time from table") && ok;
    ok = expectUInt(lamp.EndTime, 0x11223344, "glowing lamp end time from table") && ok;
    ok = expectUInt(lampRaw[0], 0x20, "glowing lamp raw chamber low byte") && ok;
    ok = expectUInt(lampRaw[1], 0x00, "glowing lamp raw chamber high byte") && ok;
    ok = expectUInt(lampRaw[2], 0x05, "glowing lamp raw flags byte") && ok;
    ok = expectUInt(lampRaw[8], 0x44, "glowing lamp raw end byte 0") && ok;
    ok = expectUInt(lampRaw[11], 0x11, "glowing lamp raw end byte 3") && ok;
    ok = expectUInt(lampRaw[12], 0x88, "glowing lamp raw start byte 0") && ok;
    ok = expectUInt(lampRaw[15], 0x55, "glowing lamp raw start byte 3") && ok;

    auto lampOutput = lua.create_table();
    lamp.toTable(lampOutput);
    ok = expectUInt(lampOutput["chamberId"].get<uint16>(), 3, "glowing lamp chamber id to table") && ok;
    ok = expectUInt(lampOutput["flags"].get<uint8>(), 5, "glowing lamp flags to table") && ok;
    ok = expectUInt(lampOutput["startTime"].get<uint32>(), 0x55667788, "glowing lamp start time to table") && ok;
    ok = expectUInt(lampOutput["endTime"].get<uint32>(), 0x11223344, "glowing lamp end time to table") && ok;

    auto lampPartial = lua.create_table();
    lampPartial["chamberId"] = 0;
    lampPartial["startTime"] = 0xAABBCCDD;
    lamp.fromTable(lampPartial);
    ok = expectUInt(lamp.ChamberId, 0, "glowing lamp chamber zero update") && ok;
    ok = expectUInt(lamp.Flags, 5, "glowing lamp flags preserved") && ok;
    ok = expectUInt(lamp.StartTime, 0xAABBCCDD, "glowing lamp start partial update") && ok;
    ok = expectUInt(lamp.EndTime, 0x11223344, "glowing lamp end preserved") && ok;

    auto lampMaskedFlags = lua.create_table();
    lampMaskedFlags["flags"] = 0x0D;
    lamp.fromTable(lampMaskedFlags);
    lamp.toTable(lampOutput);
    ok = expectUInt(lamp.Flags, 5, "glowing lamp flags masked to bitfield") && ok;
    ok = expectUInt(lampOutput["flags"].get<uint8>(), 5, "glowing lamp masked flags to table") && ok;

    auto legionInput = lua.create_table();
    legionInput["timestamp"] = 0x11223344;
    legionInput["title"]     = 0x55667788;
    legionInput["signature"] = "OmegaXI2026";

    Exdata::LegionPass legion{};
    legion.fromTable(legionInput);
    auto* legionRaw = reinterpret_cast<uint8*>(&legion);
    ok = expectUInt(legion.Timestamp, 0x11223344, "legion timestamp from table") && ok;
    ok = expectUInt(legion.Title, 0x55667788, "legion title from table") && ok;
    ok = expectString(Exdata::decodeSignature(legion.Signature), "OmegaXI2026", "legion signature from table") && ok;
    ok = expectUInt(legionRaw[0], 0x44, "legion raw timestamp byte 0") && ok;
    ok = expectUInt(legionRaw[3], 0x11, "legion raw timestamp byte 3") && ok;
    ok = expectUInt(legionRaw[4], 0x88, "legion raw title byte 0") && ok;
    ok = expectUInt(legionRaw[7], 0x55, "legion raw title byte 3") && ok;
    legionRaw[8]  = 0x9A;
    legionRaw[9]  = 0xBC;
    legionRaw[10] = 0xDE;
    legionRaw[11] = 0xF0;

    auto legionOutput = lua.create_table();
    legion.toTable(legionOutput);
    ok = expectUInt(legionOutput["timestamp"].get<uint32>(), 0x11223344, "legion timestamp to table") && ok;
    ok = expectUInt(legionOutput["title"].get<uint32>(), 0x55667788, "legion title to table") && ok;
    ok = expectString(legionOutput["signature"].get<std::string>(), "OmegaXI2026", "legion signature to table") && ok;

    auto legionPartial = lua.create_table();
    legionPartial["title"] = 0x01020304;
    legion.fromTable(legionPartial);
    ok = expectUInt(legion.Timestamp, 0x11223344, "legion timestamp preserved") && ok;
    ok = expectUInt(legion.Title, 0x01020304, "legion title partial update") && ok;
    ok = expectString(Exdata::decodeSignature(legion.Signature), "OmegaXI2026", "legion signature preserved") && ok;
    ok = expectUInt(legionRaw[8], 0x9A, "legion raw padding byte 0 preserved") && ok;
    ok = expectUInt(legionRaw[9], 0xBC, "legion raw padding byte 1 preserved") && ok;
    ok = expectUInt(legionRaw[10], 0xDE, "legion raw padding byte 2 preserved") && ok;
    ok = expectUInt(legionRaw[11], 0xF0, "legion raw padding byte 3 preserved") && ok;

    auto hourglassInput = lua.create_table();
    hourglassInput["flags"]     = 6;
    hourglassInput["startTime"] = 0x55667788;
    hourglassInput["endTime"]   = 0x11223344;
    hourglassInput["zoneId"]    = 0x1234;

    Exdata::PerpetualHourglass hourglass{};
    hourglass.fromTable(hourglassInput);
    const auto* hourglassRaw = reinterpret_cast<const uint8*>(&hourglass);
    ok = expectUInt(hourglass.Flags, 6, "hourglass flags from table") && ok;
    ok = expectUInt(hourglass.StartTime, 0x55667788, "hourglass start time from table") && ok;
    ok = expectUInt(hourglass.EndTime, 0x11223344, "hourglass end time from table") && ok;
    ok = expectUInt(hourglass.ZoneId, 0x1234, "hourglass zone id from table") && ok;
    ok = expectUInt(hourglassRaw[2], 0x06, "hourglass raw flags byte") && ok;
    ok = expectUInt(hourglassRaw[8], 0x44, "hourglass raw end byte 0") && ok;
    ok = expectUInt(hourglassRaw[11], 0x11, "hourglass raw end byte 3") && ok;
    ok = expectUInt(hourglassRaw[12], 0x88, "hourglass raw start byte 0") && ok;
    ok = expectUInt(hourglassRaw[15], 0x55, "hourglass raw start byte 3") && ok;
    ok = expectUInt(hourglassRaw[16], 0x34, "hourglass raw zone byte 0") && ok;
    ok = expectUInt(hourglassRaw[17], 0x12, "hourglass raw zone byte 1") && ok;

    auto hourglassOutput = lua.create_table();
    hourglass.toTable(hourglassOutput);
    ok = expectUInt(hourglassOutput["flags"].get<uint8>(), 6, "hourglass flags to table") && ok;
    ok = expectUInt(hourglassOutput["startTime"].get<uint32>(), 0x55667788, "hourglass start time to table") && ok;
    ok = expectUInt(hourglassOutput["endTime"].get<uint32>(), 0x11223344, "hourglass end time to table") && ok;
    ok = expectUInt(hourglassOutput["zoneId"].get<uint16>(), 0x1234, "hourglass zone id to table") && ok;

    auto hourglassPartial = lua.create_table();
    hourglassPartial["flags"] = 3;
    hourglass.fromTable(hourglassPartial);
    ok = expectUInt(hourglass.Flags, 3, "hourglass flags partial update") && ok;
    ok = expectUInt(hourglass.StartTime, 0x55667788, "hourglass start preserved") && ok;
    ok = expectUInt(hourglass.EndTime, 0x11223344, "hourglass end preserved") && ok;
    ok = expectUInt(hourglass.ZoneId, 0x1234, "hourglass zone preserved") && ok;

    auto hourglassMaskedFlags = lua.create_table();
    hourglassMaskedFlags["flags"] = 0x0E;
    hourglass.fromTable(hourglassMaskedFlags);
    hourglass.toTable(hourglassOutput);
    ok = expectUInt(hourglass.Flags, 6, "hourglass flags masked to bitfield") && ok;
    ok = expectUInt(hourglassOutput["flags"].get<uint8>(), 6, "hourglass masked flags to table") && ok;

    auto honeymoonInput = lua.create_table();
    honeymoonInput["plan"] = 2;

    Exdata::HoneymoonTicket honeymoon{};
    honeymoon.fromTable(honeymoonInput);
    const auto* honeymoonRaw = reinterpret_cast<const uint8*>(&honeymoon);
    ok = expectUInt(honeymoon.Plan, 2, "honeymoon plan from table") && ok;
    ok = expectString(Exdata::decodeSignature(honeymoon.Signature), "PlanB", "honeymoon plan signature from table") && ok;
    ok = expectUInt(honeymoonRaw[0], 0x02, "honeymoon raw plan byte") && ok;

    auto honeymoonOutput = lua.create_table();
    honeymoon.toTable(honeymoonOutput);
    ok = expectUInt(honeymoonOutput["plan"].get<uint8>(), 2, "honeymoon plan to table") && ok;

    auto honeymoonPartial = lua.create_table();
    honeymoonPartial["plan"] = 5;
    honeymoon.fromTable(honeymoonPartial);
    ok = expectUInt(honeymoon.Plan, 5, "honeymoon out-of-range plan update") && ok;
    ok = expectString(Exdata::decodeSignature(honeymoon.Signature), "PlanB", "honeymoon out-of-range signature preserved") && ok;

    return ok;
}

} // namespace

auto runItemExdataSelfTests() -> bool
{
    bool ok = true;
    ok      = testTypeEnumGoldenValues() && ok;
    ok      = testItemIDTypeDispatch() && ok;
    ok      = testPredicateTypeDispatchAndPrecedence() && ok;
    ok      = testRawExdataOverlay() && ok;
    ok      = testTimerInfoTableSerialization() && ok;
    ok      = testLogTicketTableSerialization() && ok;
    ok      = testPassTimerTableSerialization() && ok;
    return ok;
}
