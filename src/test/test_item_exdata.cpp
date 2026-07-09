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

auto testFishTableSerialization() -> bool
{
    sol::state lua;
    bool       ok = true;

    auto input = lua.create_table();
    input["size"]     = 300;
    input["weight"]   = 1500;
    input["isRanked"] = true;

    Exdata::Fish fish{};
    fish.fromTable(input);
    auto* fishRaw = reinterpret_cast<uint8*>(&fish);

    ok = expectUInt(fish.Size, 300, "fish size from table") && ok;
    ok = expectUInt(fish.Weight, 1500, "fish weight from table") && ok;
    ok = expectUInt(fish.IsRanked, 1, "fish ranked flag from table") && ok;
    ok = expectUInt(fishRaw[0], 0x2C, "fish raw size byte 0") && ok;
    ok = expectUInt(fishRaw[1], 0x01, "fish raw size byte 1") && ok;
    ok = expectUInt(fishRaw[2], 0xDC, "fish raw weight byte 0") && ok;
    ok = expectUInt(fishRaw[3], 0x05, "fish raw weight byte 1") && ok;
    ok = expectUInt(fishRaw[4], 0x01, "fish raw ranked byte") && ok;
    ok = expectUInt(fishRaw[5], 0x00, "fish raw padding byte 0") && ok;
    ok = expectUInt(fishRaw[23], 0x00, "fish raw padding byte 18") && ok;

    auto output = lua.create_table();
    fish.toTable(output);
    ok = expectUInt(output["size"].get<uint16>(), 300, "fish size to table") && ok;
    ok = expectUInt(output["weight"].get<uint16>(), 1500, "fish weight to table") && ok;
    ok = expectUInt(output["isRanked"].get<bool>() ? 1 : 0, 1, "fish ranked flag to table") && ok;

    fishRaw[4]  = 0xFE;
    fishRaw[5]  = 0xA1;
    fishRaw[23] = 0xB3;

    auto partial = lua.create_table();
    partial["weight"]   = 42;
    partial["isRanked"] = true;
    fish.fromTable(partial);
    ok = expectUInt(fish.Size, 300, "fish size preserved") && ok;
    ok = expectUInt(fish.Weight, 42, "fish weight partial update") && ok;
    ok = expectUInt(fish.IsRanked, 1, "fish ranked flag partial update") && ok;
    ok = expectUInt(fishRaw[4], 0xFF, "fish hidden ranked byte bits preserved") && ok;
    ok = expectUInt(fishRaw[5], 0xA1, "fish padding byte 0 preserved") && ok;
    ok = expectUInt(fishRaw[23], 0xB3, "fish padding byte 18 preserved") && ok;

    auto unranked = lua.create_table();
    unranked["isRanked"] = false;
    fish.fromTable(unranked);
    ok = expectUInt(fish.IsRanked, 0, "fish ranked flag cleared") && ok;
    ok = expectUInt(fishRaw[4], 0xFE, "fish hidden ranked byte bits preserved when cleared") && ok;

    return ok;
}

auto testChocoboEggTableSerialization() -> bool
{
    sol::state lua;
    bool       ok = true;

    auto input       = lua.create_table();
    auto dna         = lua.create_table();
    dna[1]           = 0x0B;
    dna[2]           = 5;
    dna[3]           = 7;
    input["dna"]     = dna;
    input["ability"] = 0x1D;
    input["plan"]    = 6;
    input["isBred"]  = true;

    Exdata::ChocoboEgg egg{};
    egg.fromTable(input);
    auto* eggRaw = reinterpret_cast<uint8*>(&egg);

    ok = expectUInt(egg.DNA1, 3, "chocobo egg dna 1 masked from table") && ok;
    ok = expectUInt(egg.DNA2, 5, "chocobo egg dna 2 from table") && ok;
    ok = expectUInt(egg.DNA3, 7, "chocobo egg dna 3 from table") && ok;
    ok = expectUInt(egg.Ability, 0x0D, "chocobo egg ability masked from table") && ok;
    ok = expectUInt(egg.Plan, 2, "chocobo egg plan masked from table") && ok;
    ok = expectUInt(egg.IsBred, 1, "chocobo egg bred flag from table") && ok;
    ok = expectUInt(eggRaw[0], 0xEB, "chocobo egg raw bitfield byte 0") && ok;
    ok = expectUInt(eggRaw[1], 0x9B, "chocobo egg raw bitfield byte 1") && ok;
    ok = expectUInt(eggRaw[2], 0x00, "chocobo egg raw bitfield byte 2") && ok;
    ok = expectUInt(eggRaw[3], 0x80, "chocobo egg raw bitfield byte 3") && ok;
    ok = expectUInt(eggRaw[4], 0x00, "chocobo egg raw padding byte 0") && ok;
    ok = expectUInt(eggRaw[23], 0x00, "chocobo egg raw padding byte 19") && ok;

    auto output = lua.create_table();
    egg.toTable(output);
    auto dnaOutput = output["dna"].get<sol::table>();
    ok = expectUInt(dnaOutput[1].get<uint32>(), 3, "chocobo egg dna 1 to table") && ok;
    ok = expectUInt(dnaOutput[2].get<uint32>(), 5, "chocobo egg dna 2 to table") && ok;
    ok = expectUInt(dnaOutput[3].get<uint32>(), 7, "chocobo egg dna 3 to table") && ok;
    ok = expectUInt(output["ability"].get<uint32>(), 0x0D, "chocobo egg ability to table") && ok;
    ok = expectUInt(output["plan"].get<uint32>(), 2, "chocobo egg plan to table") && ok;
    ok = expectUInt(output["isBred"].get<bool>() ? 1 : 0, 1, "chocobo egg bred flag to table") && ok;

    eggRaw[1] |= 0x20;
    eggRaw[2]  = 0xA5;
    eggRaw[3]  = 0xD2;
    eggRaw[4]  = 0xC1;
    eggRaw[23] = 0xD3;

    auto partial       = lua.create_table();
    auto partialDNA    = lua.create_table();
    partialDNA[2]      = 1;
    partial["dna"]     = partialDNA;
    partial["ability"] = 2;
    partial["isBred"]  = false;
    egg.fromTable(partial);
    ok = expectUInt(egg.DNA1, 3, "chocobo egg dna 1 preserved") && ok;
    ok = expectUInt(egg.DNA2, 1, "chocobo egg dna 2 partial update") && ok;
    ok = expectUInt(egg.DNA3, 7, "chocobo egg dna 3 preserved") && ok;
    ok = expectUInt(egg.Ability, 2, "chocobo egg ability partial update") && ok;
    ok = expectUInt(egg.Plan, 2, "chocobo egg plan preserved") && ok;
    ok = expectUInt(egg.IsBred, 0, "chocobo egg bred flag cleared") && ok;
    ok = expectUInt(eggRaw[0], 0xCB, "chocobo egg partial raw bitfield byte 0") && ok;
    ok = expectUInt(eggRaw[1], 0xA5, "chocobo egg partial raw bitfield byte 1") && ok;
    ok = expectUInt(eggRaw[2], 0xA5, "chocobo egg hidden bitfield byte 2 preserved") && ok;
    ok = expectUInt(eggRaw[3], 0x52, "chocobo egg hidden bitfield byte 3 preserved") && ok;
    ok = expectUInt(eggRaw[4], 0xC1, "chocobo egg padding byte 0 preserved") && ok;
    ok = expectUInt(eggRaw[23], 0xD3, "chocobo egg padding byte 19 preserved") && ok;

    return ok;
}

auto testChocoboCardTableSerialization() -> bool
{
    sol::state lua;
    bool       ok = true;

    auto makeStat = [&](bool trait, uint8 rp, uint8 rank)
    {
        auto stat   = lua.create_table();
        stat["trait"] = trait;
        stat["rp"]    = rp;
        stat["rank"]  = rank;
        return stat;
    };

    auto input             = lua.create_table();
    input["strength"]      = makeStat(true, 8, 3);
    input["endurance"]     = makeStat(false, 4, 2);
    input["discernment"]   = makeStat(true, 12, 4);
    auto receptivity       = lua.create_table();
    receptivity["rp"]      = 20;
    receptivity["rank"]    = 5;
    input["receptivity"]   = receptivity;
    auto dna               = lua.create_table();
    dna[1]                 = 0x0A;
    dna[2]                 = 4;
    dna[3]                 = 6;
    input["dna"]           = dna;
    auto abilities         = lua.create_table();
    abilities[1]           = 0x1B;
    abilities[2]           = 0x0C;
    input["abilities"]     = abilities;
    input["temperament"]   = 0x0D;
    input["weather"]       = 0x1E;
    input["gender"]        = 3;
    input["color"]         = 0x0D;
    input["size"]          = 0x0E;
    input["name"]          = "ChocoTest";

    Exdata::ChocoboCard card{};
    card.fromTable(input);
    auto* cardRaw = reinterpret_cast<uint8*>(&card);

    ok = expectUInt(card.STR.Trait, 1, "chocobo card strength trait from table") && ok;
    ok = expectUInt(card.STR.RP, 8, "chocobo card strength rp from table") && ok;
    ok = expectUInt(card.STR.Rank, 3, "chocobo card strength rank from table") && ok;
    ok = expectUInt(card.END.Trait, 0, "chocobo card endurance trait from table") && ok;
    ok = expectUInt(card.END.RP, 4, "chocobo card endurance rp from table") && ok;
    ok = expectUInt(card.END.Rank, 2, "chocobo card endurance rank from table") && ok;
    ok = expectUInt(card.DSC.Trait, 1, "chocobo card discernment trait from table") && ok;
    ok = expectUInt(card.DSC.RP, 12, "chocobo card discernment rp from table") && ok;
    ok = expectUInt(card.DSC.Rank, 4, "chocobo card discernment rank from table") && ok;
    ok = expectUInt(card.RCP.RP, 20, "chocobo card receptivity rp from table") && ok;
    ok = expectUInt(card.RCP.Rank, 5, "chocobo card receptivity rank from table") && ok;
    ok = expectUInt(card.DNA1, 2, "chocobo card dna 1 masked from table") && ok;
    ok = expectUInt(card.DNA2, 4, "chocobo card dna 2 from table") && ok;
    ok = expectUInt(card.DNA3, 6, "chocobo card dna 3 from table") && ok;
    ok = expectUInt(card.Ability1, 0x0B, "chocobo card ability 1 masked from table") && ok;
    ok = expectUInt(card.Ability2, 0x0C, "chocobo card ability 2 from table") && ok;
    ok = expectUInt(card.Temperament, 5, "chocobo card temperament masked from table") && ok;
    ok = expectUInt(card.Weather, 0x0E, "chocobo card weather masked from table") && ok;
    ok = expectUInt(card.Gender, 1, "chocobo card gender masked from table") && ok;
    ok = expectUInt(card.Color, 5, "chocobo card color masked from table") && ok;
    ok = expectUInt(card.Size, 6, "chocobo card size masked from table") && ok;
    ok = expectString(Exdata::decodeSignature(card.Signature), "ChocoTest", "chocobo card name from table") && ok;
    const uint8 expectedCardRaw[] = { 0x71, 0x48, 0x99, 0xB4, 0xA2, 0x97, 0xEB, 0x6B };
    for (std::size_t i = 0; i < sizeof(expectedCardRaw); ++i)
    {
        ok = expectUInt(cardRaw[i], expectedCardRaw[i], "chocobo card raw byte") && ok;
    }
    ok = expectUInt(cardRaw[8], 0x00, "chocobo card raw padding byte 0") && ok;
    ok = expectUInt(cardRaw[11], 0x00, "chocobo card raw padding byte 3") && ok;

    auto output = lua.create_table();
    card.toTable(output);
    ok = expectUInt(output["temperament"].get<uint32>(), 5, "chocobo card temperament to table") && ok;
    ok = expectUInt(output["weather"].get<uint32>(), 0x0E, "chocobo card weather to table") && ok;
    ok = expectUInt(output["gender"].get<uint32>(), 1, "chocobo card gender to table") && ok;
    ok = expectUInt(output["color"].get<uint32>(), 5, "chocobo card color to table") && ok;
    ok = expectUInt(output["size"].get<uint32>(), 6, "chocobo card size to table") && ok;
    ok = expectString(output["name"].get<std::string>(), "ChocoTest", "chocobo card name to table") && ok;

    cardRaw[7] |= 0x80;
    cardRaw[8]  = 0xC1;
    cardRaw[9]  = 0xC2;
    cardRaw[10] = 0xC3;
    cardRaw[11] = 0xC4;

    auto partial             = lua.create_table();
    auto partialStrength     = lua.create_table();
    partialStrength["rp"]    = 2;
    partial["strength"]      = partialStrength;
    auto partialReceptivity  = lua.create_table();
    partialReceptivity["rank"] = 1;
    partial["receptivity"]   = partialReceptivity;
    auto partialDNA          = lua.create_table();
    partialDNA[3]            = 1;
    partial["dna"]           = partialDNA;
    auto partialAbilities    = lua.create_table();
    partialAbilities[2]      = 3;
    partial["abilities"]     = partialAbilities;
    partial["weather"]       = 1;
    partial["name"]          = "Co";
    card.fromTable(partial);
    ok = expectUInt(card.STR.Trait, 1, "chocobo card strength trait preserved") && ok;
    ok = expectUInt(card.STR.RP, 2, "chocobo card strength rp partial update") && ok;
    ok = expectUInt(card.STR.Rank, 3, "chocobo card strength rank preserved") && ok;
    ok = expectUInt(card.END.RP, 4, "chocobo card endurance preserved") && ok;
    ok = expectUInt(card.RCP.RP, 20, "chocobo card receptivity rp preserved") && ok;
    ok = expectUInt(card.RCP.Rank, 1, "chocobo card receptivity rank partial update") && ok;
    ok = expectUInt(card.DNA1, 2, "chocobo card dna 1 preserved") && ok;
    ok = expectUInt(card.DNA2, 4, "chocobo card dna 2 preserved") && ok;
    ok = expectUInt(card.DNA3, 1, "chocobo card dna 3 partial update") && ok;
    ok = expectUInt(card.Ability1, 0x0B, "chocobo card ability 1 preserved") && ok;
    ok = expectUInt(card.Ability2, 3, "chocobo card ability 2 partial update") && ok;
    ok = expectUInt(card.Weather, 1, "chocobo card weather partial update") && ok;
    ok = expectUInt(card.Gender, 1, "chocobo card gender preserved") && ok;
    ok = expectUInt(card.Color, 5, "chocobo card color preserved") && ok;
    ok = expectUInt(card.Size, 6, "chocobo card size preserved") && ok;
    ok = expectString(Exdata::decodeSignature(card.Signature), "Co", "chocobo card short name partial update") && ok;
    const uint8 expectedPartialCardRaw[] = { 0x65, 0x48, 0x99, 0x34, 0x62, 0x76, 0x1A, 0xEB };
    for (std::size_t i = 0; i < sizeof(expectedPartialCardRaw); ++i)
    {
        ok = expectUInt(cardRaw[i], expectedPartialCardRaw[i], "chocobo card partial raw byte") && ok;
    }
    ok = expectUInt(cardRaw[8], 0xC1, "chocobo card padding byte 0 preserved") && ok;
    ok = expectUInt(cardRaw[9], 0xC2, "chocobo card padding byte 1 preserved") && ok;
    ok = expectUInt(cardRaw[10], 0xC3, "chocobo card padding byte 2 preserved") && ok;
    ok = expectUInt(cardRaw[11], 0xC4, "chocobo card padding byte 3 preserved") && ok;
    uint8 expectedShortSignature[12] = {};
    Exdata::encodeSignature(std::string("Co"), expectedShortSignature);
    for (std::size_t i = 0; i < sizeof(expectedShortSignature); ++i)
    {
        ok = expectUInt(cardRaw[12 + i], expectedShortSignature[i], "chocobo card short signature raw byte") && ok;
    }

    return ok;
}

auto testEscutcheonTableSerialization() -> bool
{
    sol::state lua;
    bool       ok = true;

    auto input                     = lua.create_table();
    input["status"]                = 0xA5;
    input["bonusObjective"]        = 0xBC;
    input["craftsmanship"]         = 0x1234;
    input["stage"]                 = 0x1D;
    input["successDownPenalty"]    = 0x1BCDE;
    input["signature"]             = "Escutcheon";

    Exdata::Escutcheon escutcheon{};
    escutcheon.fromTable(input);
    auto* escutcheonRaw = reinterpret_cast<uint8*>(&escutcheon);

    ok = expectUInt(static_cast<uint8>(escutcheon.AugmentKind), 0x02, "escutcheon augment kind from table") && ok;
    ok = expectUInt(static_cast<uint8>(escutcheon.AugmentSubKind), 0x0B, "escutcheon augment subkind from table") && ok;
    ok = expectUInt(escutcheon.Status, 0xA5, "escutcheon status from table") && ok;
    ok = expectUInt(escutcheon.BonusObjective, 0xBC, "escutcheon bonus objective from table") && ok;
    ok = expectUInt(escutcheon.Craftsmanship, 0x1234, "escutcheon craftsmanship from table") && ok;
    ok = expectUInt(escutcheon.Stage, 0x0D, "escutcheon stage masked from table") && ok;
    ok = expectUInt(escutcheon.SuccessDownPenalty, 0xBCDE, "escutcheon success penalty masked from table") && ok;
    ok = expectString(Exdata::decodeSignature(escutcheon.Signature), "Escutcheon", "escutcheon signature from table") && ok;

    const uint8 expectedRaw[] = { 0x02, 0x0B, 0x00, 0x00, 0xA5, 0xBC, 0x34, 0x12, 0xED, 0xCD, 0x0B, 0x00 };
    for (std::size_t i = 0; i < sizeof(expectedRaw); ++i)
    {
        ok = expectUInt(escutcheonRaw[i], expectedRaw[i], "escutcheon raw byte") && ok;
    }

    auto output = lua.create_table();
    escutcheon.toTable(output);
    ok = expectUInt(output["status"].get<uint8>(), 0xA5, "escutcheon status to table") && ok;
    ok = expectUInt(output["bonusObjective"].get<uint8>(), 0xBC, "escutcheon bonus objective to table") && ok;
    ok = expectUInt(output["craftsmanship"].get<uint16>(), 0x1234, "escutcheon craftsmanship to table") && ok;
    ok = expectUInt(output["stage"].get<uint32>(), 0x0D, "escutcheon stage to table") && ok;
    ok = expectUInt(output["successDownPenalty"].get<uint32>(), 0xBCDE, "escutcheon success penalty to table") && ok;
    ok = expectString(output["signature"].get<std::string>(), "Escutcheon", "escutcheon signature to table") && ok;

    escutcheonRaw[0]  = 0xFE;
    escutcheonRaw[1]  = 0xF0;
    escutcheonRaw[2]  = 0xA1;
    escutcheonRaw[3]  = 0xA2;
    escutcheonRaw[10] |= 0xF0;
    escutcheonRaw[11] = 0xA5;

    auto partial                  = lua.create_table();
    partial["stage"]              = 2;
    partial["successDownPenalty"] = 0x42;
    partial["signature"]          = "Sh";
    escutcheon.fromTable(partial);

    ok = expectUInt(static_cast<uint8>(escutcheon.AugmentKind), 0x02, "escutcheon augment kind reset") && ok;
    ok = expectUInt(static_cast<uint8>(escutcheon.AugmentSubKind), 0x0B, "escutcheon augment subkind reset") && ok;
    ok = expectUInt(escutcheon.Status, 0xA5, "escutcheon status preserved") && ok;
    ok = expectUInt(escutcheon.BonusObjective, 0xBC, "escutcheon bonus objective preserved") && ok;
    ok = expectUInt(escutcheon.Craftsmanship, 0x1234, "escutcheon craftsmanship preserved") && ok;
    ok = expectUInt(escutcheon.Stage, 2, "escutcheon stage partial update") && ok;
    ok = expectUInt(escutcheon.SuccessDownPenalty, 0x42, "escutcheon success penalty partial update") && ok;
    ok = expectString(Exdata::decodeSignature(escutcheon.Signature), "Sh", "escutcheon short signature partial update") && ok;

    const uint8 expectedPartialRaw[] = { 0x02, 0x0B, 0xA1, 0xA2, 0xA5, 0xBC, 0x34, 0x12, 0x22, 0x04, 0xF0, 0xA5 };
    for (std::size_t i = 0; i < sizeof(expectedPartialRaw); ++i)
    {
        ok = expectUInt(escutcheonRaw[i], expectedPartialRaw[i], "escutcheon partial raw byte") && ok;
    }

    uint8 expectedShortSignature[12] = {};
    Exdata::encodeSignature(std::string("Sh"), expectedShortSignature);
    for (std::size_t i = 0; i < sizeof(expectedShortSignature); ++i)
    {
        ok = expectUInt(escutcheonRaw[12 + i], expectedShortSignature[i], "escutcheon short signature raw byte") && ok;
    }

    auto preserveSignature = lua.create_table();
    preserveSignature["status"] = 0x11;
    escutcheon.fromTable(preserveSignature);
    ok = expectUInt(escutcheon.Status, 0x11, "escutcheon status update with omitted signature") && ok;
    ok = expectString(Exdata::decodeSignature(escutcheon.Signature), "Sh", "escutcheon omitted signature preserved") && ok;
    for (std::size_t i = 0; i < sizeof(expectedShortSignature); ++i)
    {
        ok = expectUInt(escutcheonRaw[12 + i], expectedShortSignature[i], "escutcheon omitted signature raw byte") && ok;
    }

    return ok;
}

auto testSerializedTableSerialization() -> bool
{
    sol::state lua;
    bool       ok = true;

    auto input                  = lua.create_table();
    input["serverIndex"]        = 0x7F;
    input["serialNumber"]       = 500;
    input["signature"]          = "Serialized";

    Exdata::Serialized serialized{};
    serialized.fromTable(input);
    auto* serializedRaw = reinterpret_cast<uint8*>(&serialized);

    ok = expectUInt(static_cast<uint8>(serialized.AugmentKind), 0x02, "serialized augment kind from table") && ok;
    ok = expectUInt(static_cast<uint8>(serialized.AugmentSubKind), 0x10, "serialized augment subkind from table") && ok;
    ok = expectUInt(serialized.ServerIndex, 0x7F, "serialized server index from table") && ok;
    ok = expectUInt(serialized.SerialNumber, 500, "serialized serial number from table") && ok;
    ok = expectString(Exdata::decodeSignature(serialized.Signature), "Serialized", "serialized signature from table") && ok;

    const uint8 expectedRaw[] = { 0x02, 0x10, 0x00, 0x00, 0x7F, 0x00, 0xF4, 0x01, 0x00, 0x00, 0x00, 0x00 };
    for (std::size_t i = 0; i < sizeof(expectedRaw); ++i)
    {
        ok = expectUInt(serializedRaw[i], expectedRaw[i], "serialized raw byte") && ok;
    }

    auto output = lua.create_table();
    serialized.toTable(output);
    ok = expectUInt(output["augmentKind"].get<uint8>(), 0x02, "serialized augment kind to table") && ok;
    ok = expectUInt(output["augmentSubKind"].get<uint8>(), 0x10, "serialized augment subkind to table") && ok;
    ok = expectUInt(output["serverIndex"].get<uint8>(), 0x7F, "serialized server index to table") && ok;
    ok = expectUInt(output["serialNumber"].get<uint16>(), 500, "serialized serial number to table") && ok;
    ok = expectString(output["signature"].get<std::string>(), "Serialized", "serialized signature to table") && ok;

    serializedRaw[0]  = 0xFE;
    serializedRaw[1]  = 0xF0;
    serializedRaw[2]  = 0xA1;
    serializedRaw[3]  = 0xA2;
    serializedRaw[5]  = 0xB1;
    serializedRaw[8]  = 0xC1;
    serializedRaw[9]  = 0xC2;
    serializedRaw[10] = 0xC3;
    serializedRaw[11] = 0xC4;

    auto rawFlagOutput = lua.create_table();
    serialized.toTable(rawFlagOutput);
    ok = expectUInt(rawFlagOutput["augmentKind"].get<uint8>(), 0xFE, "serialized raw augment kind to table") && ok;
    ok = expectUInt(rawFlagOutput["augmentSubKind"].get<uint8>(), 0xF0, "serialized raw augment subkind to table") && ok;

    auto partial            = lua.create_table();
    partial["serialNumber"] = 42;
    partial["signature"]    = "Se";
    serialized.fromTable(partial);

    ok = expectUInt(static_cast<uint8>(serialized.AugmentKind), 0x02, "serialized augment kind reset") && ok;
    ok = expectUInt(static_cast<uint8>(serialized.AugmentSubKind), 0x10, "serialized augment subkind reset") && ok;
    ok = expectUInt(serialized.ServerIndex, 0x7F, "serialized server index preserved") && ok;
    ok = expectUInt(serialized.SerialNumber, 42, "serialized serial number partial update") && ok;
    ok = expectString(Exdata::decodeSignature(serialized.Signature), "Se", "serialized short signature partial update") && ok;

    const uint8 expectedPartialRaw[] = { 0x02, 0x10, 0xA1, 0xA2, 0x7F, 0xB1, 0x2A, 0x00, 0xC1, 0xC2, 0xC3, 0xC4 };
    for (std::size_t i = 0; i < sizeof(expectedPartialRaw); ++i)
    {
        ok = expectUInt(serializedRaw[i], expectedPartialRaw[i], "serialized partial raw byte") && ok;
    }

    uint8 expectedShortSignature[12] = {};
    Exdata::encodeSignature(std::string("Se"), expectedShortSignature);
    for (std::size_t i = 0; i < sizeof(expectedShortSignature); ++i)
    {
        ok = expectUInt(serializedRaw[12 + i], expectedShortSignature[i], "serialized short signature raw byte") && ok;
    }

    auto preserveSignature           = lua.create_table();
    preserveSignature["serverIndex"] = 0x11;
    serialized.fromTable(preserveSignature);
    ok = expectUInt(serialized.ServerIndex, 0x11, "serialized server index update with omitted signature") && ok;
    ok = expectString(Exdata::decodeSignature(serialized.Signature), "Se", "serialized omitted signature preserved") && ok;
    for (std::size_t i = 0; i < sizeof(expectedShortSignature); ++i)
    {
        ok = expectUInt(serializedRaw[12 + i], expectedShortSignature[i], "serialized omitted signature raw byte") && ok;
    }

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

auto testSoulTableSerialization() -> bool
{
    sol::state lua;
    bool       ok = true;

    auto plateInput = lua.create_table();
    plateInput["signature"]   = "Goblin_Bounty_Hunter";
    plateInput["zoneId"]      = 0x1234;
    plateInput["familyId"]    = 0x5678;
    plateInput["poolId"]      = 0x9ABC;
    plateInput["level"]       = 0x9A;
    plateInput["feralSkill"]  = 0x1ABC;
    plateInput["feralPoints"] = 0xA2;
    plateInput["quality"]     = 0x7D;

    Exdata::SoulPlate plate{};
    plate.fromTable(plateInput);
    auto* plateRaw = reinterpret_cast<uint8*>(&plate);

    const uint8 expectedPlateSignature[] = {
        0x8F, 0xBF, 0x16, 0xCD, 0x3B, 0xA1, 0x6F, 0xEB, 0xBB, 0xA7, 0x99, 0x00, 0x00, 0x00,
    };
    for (std::size_t i = 0; i < sizeof(expectedPlateSignature); ++i)
    {
        ok = expectUInt(plateRaw[i], expectedPlateSignature[i], "soul plate signature raw byte") && ok;
    }
    ok = expectString(UnpackSoultrapperName(plate.Signature), "GoblinBountyH", "soul plate signature from table") && ok;
    ok = expectUInt(plate.ZoneId, 0x1234, "soul plate zone from table") && ok;
    ok = expectUInt(plate.FamilyId, 0x5678, "soul plate family from table") && ok;
    ok = expectUInt(plate.PoolId, 0x9ABC, "soul plate pool from table") && ok;
    ok = expectUInt(plate.Level, 0x1A, "soul plate level masked from table") && ok;
    ok = expectUInt(plate.FeralSkill, 0xABC, "soul plate feral skill masked from table") && ok;
    ok = expectUInt(plate.FeralPoints, 0x22, "soul plate feral points masked from table") && ok;
    ok = expectUInt(plate.Quality, 0x3D, "soul plate quality masked from table") && ok;
    ok = expectUInt(plateRaw[14], 0x34, "soul plate raw zone byte 0") && ok;
    ok = expectUInt(plateRaw[15], 0x12, "soul plate raw zone byte 1") && ok;
    ok = expectUInt(plateRaw[16], 0x78, "soul plate raw family byte 0") && ok;
    ok = expectUInt(plateRaw[17], 0x56, "soul plate raw family byte 1") && ok;
    ok = expectUInt(plateRaw[18], 0xBC, "soul plate raw pool byte 0") && ok;
    ok = expectUInt(plateRaw[19], 0x9A, "soul plate raw pool byte 1") && ok;
    ok = expectUInt(plateRaw[20], 0x1A, "soul plate raw bitfield byte 0") && ok;
    ok = expectUInt(plateRaw[21], 0x5E, "soul plate raw bitfield byte 1") && ok;
    ok = expectUInt(plateRaw[22], 0x15, "soul plate raw bitfield byte 2") && ok;
    ok = expectUInt(plateRaw[23], 0xF5, "soul plate raw bitfield byte 3") && ok;

    auto plateOutput = lua.create_table();
    plate.toTable(plateOutput);
    ok = expectString(plateOutput["signature"].get<std::string>(), "GoblinBountyH", "soul plate signature to table") && ok;
    ok = expectUInt(plateOutput["zoneId"].get<uint16>(), 0x1234, "soul plate zone to table") && ok;
    ok = expectUInt(plateOutput["familyId"].get<uint16>(), 0x5678, "soul plate family to table") && ok;
    ok = expectUInt(plateOutput["poolId"].get<uint16>(), 0x9ABC, "soul plate pool to table") && ok;
    ok = expectUInt(plateOutput["level"].get<uint32>(), 0x1A, "soul plate level to table") && ok;
    ok = expectUInt(plateOutput["feralSkill"].get<uint32>(), 0xABC, "soul plate feral skill to table") && ok;
    ok = expectUInt(plateOutput["feralPoints"].get<uint32>(), 0x22, "soul plate feral points to table") && ok;
    ok = expectUInt(plateOutput["quality"].get<uint32>(), 0x3D, "soul plate quality to table") && ok;

    auto platePartial = lua.create_table();
    platePartial["signature"] = "Crab";
    platePartial["quality"]   = 7;
    plate.fromTable(platePartial);
    ok = expectString(UnpackSoultrapperName(plate.Signature), "Crab", "soul plate signature partial update") && ok;
    ok = expectUInt(plate.ZoneId, 0x1234, "soul plate zone preserved") && ok;
    ok = expectUInt(plate.Level, 0x1A, "soul plate level preserved") && ok;
    ok = expectUInt(plate.Quality, 7, "soul plate quality partial update") && ok;
    const uint8 expectedShortPlateSignature[] = {
        0x87, 0xCB, 0x0E, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    };
    for (std::size_t i = 0; i < sizeof(expectedShortPlateSignature); ++i)
    {
        ok = expectUInt(plateRaw[i], expectedShortPlateSignature[i], "soul plate short signature raw byte") && ok;
    }

    auto reflectorInput = lua.create_table();
    reflectorInput["nameFirst"]      = 0x5A;
    reflectorInput["nameLast"]       = 0x3F;
    reflectorInput["poolId"]         = 0x1234;
    reflectorInput["exp"]            = 0x9A;
    reflectorInput["discipline"]     = 0xBC;
    reflectorInput["temperament"]    = 0x1D;
    reflectorInput["aggressiveness"] = 0x1A;
    reflectorInput["level"]          = 0xE3;

    auto reflectorSkills = lua.create_table();
    auto setReflectorSkill = [&](uint8 index, uint32 skillId, uint8 level)
    {
        auto slot       = lua.create_table();
        slot["skillId"] = skillId;
        slot["level"]   = level;
        reflectorSkills[index] = slot;
    };
    setReflectorSkill(1, 0x1ABC, 0x85);
    setReflectorSkill(2, 0x234, 0x12);
    setReflectorSkill(3, 0x345, 0x23);
    setReflectorSkill(4, 0x456, 0x34);
    setReflectorSkill(5, 0x567, 0x45);
    setReflectorSkill(6, 0x678, 0x56);
    setReflectorSkill(7, 0x789, 0x67);
    reflectorInput["feralSkills"] = reflectorSkills;

    Exdata::SoulReflector reflector{};
    reflector.fromTable(reflectorInput);
    auto* reflectorRaw = reinterpret_cast<uint8*>(&reflector);

    const uint8 expectedReflectorRaw[] = {
        0xDA, 0x4F, 0x23, 0xA1, 0xC9, 0xDB, 0x3A, 0xE6,
        0xD5, 0x02, 0x8D, 0x48, 0x8A, 0x66, 0x64, 0x45,
        0xB4, 0xB3, 0x2A, 0xE2, 0x99, 0x35, 0xF1, 0xCE,
    };
    for (std::size_t i = 0; i < sizeof(expectedReflectorRaw); ++i)
    {
        ok = expectUInt(reflectorRaw[i], expectedReflectorRaw[i], "soul reflector raw byte") && ok;
    }
    ok = expectUInt(reflector.NameFirst, 0x1A, "soul reflector first name masked from table") && ok;
    ok = expectUInt(reflector.NameLast, 0x3F, "soul reflector last name from table") && ok;
    ok = expectUInt(reflector.PoolId, 0x1234, "soul reflector pool from table") && ok;
    ok = expectUInt(reflector.Exp, 0x9A, "soul reflector exp from table") && ok;
    ok = expectUInt(reflector.Discipline, 0xBC, "soul reflector discipline from table") && ok;
    ok = expectUInt(reflector.Temperament, 0x0D, "soul reflector temperament masked from table") && ok;
    ok = expectUInt(reflector.Aggressiveness, 0x0A, "soul reflector aggressiveness masked from table") && ok;
    ok = expectUInt(reflector.Level, 0x63, "soul reflector level masked from table") && ok;

    auto reflectorOutput = lua.create_table();
    reflector.toTable(reflectorOutput);
    ok = expectUInt(reflectorOutput["nameFirst"].get<uint8>(), 0x1A, "soul reflector first name to table") && ok;
    ok = expectUInt(reflectorOutput["nameLast"].get<uint8>(), 0x3F, "soul reflector last name to table") && ok;
    ok = expectUInt(reflectorOutput["poolId"].get<uint16>(), 0x1234, "soul reflector pool to table") && ok;
    ok = expectUInt(reflectorOutput["exp"].get<uint8>(), 0x9A, "soul reflector exp to table") && ok;
    ok = expectUInt(reflectorOutput["discipline"].get<uint8>(), 0xBC, "soul reflector discipline to table") && ok;
    ok = expectUInt(reflectorOutput["temperament"].get<uint8>(), 0x0D, "soul reflector temperament to table") && ok;
    ok = expectUInt(reflectorOutput["aggressiveness"].get<uint8>(), 0x0A, "soul reflector aggressiveness to table") && ok;
    ok = expectUInt(reflectorOutput["level"].get<uint8>(), 0x63, "soul reflector level to table") && ok;

    auto reflectorPartial = lua.create_table();
    reflectorPartial["nameFirst"] = 2;
    auto partialSkills            = lua.create_table();
    auto partialSlot2             = lua.create_table();
    partialSlot2["level"]         = 0x7F;
    partialSkills[2]              = partialSlot2;
    auto partialSlot4             = lua.create_table();
    partialSlot4["skillId"]       = 1;
    partialSkills[4]              = partialSlot4;
    reflectorPartial["feralSkills"] = partialSkills;
    reflector.fromTable(reflectorPartial);
    ok = expectUInt(reflector.NameFirst, 2, "soul reflector first name partial update") && ok;
    ok = expectUInt(reflector.NameLast, 0x3F, "soul reflector last name preserved") && ok;

    const uint8 expectedPartialReflectorRaw[] = {
        0xC2, 0x4F, 0x23, 0xA1, 0xC9, 0xDB, 0x3A, 0xE6,
        0xD5, 0x02, 0x8D, 0xFC, 0x8B, 0x66, 0x14, 0x00,
        0xB4, 0xB3, 0x2A, 0xE2, 0x99, 0x35, 0xF1, 0xCE,
    };
    for (std::size_t i = 0; i < sizeof(expectedPartialReflectorRaw); ++i)
    {
        ok = expectUInt(reflectorRaw[i], expectedPartialReflectorRaw[i], "soul reflector partial raw byte") && ok;
    }

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

auto testMetadataTableSerialization() -> bool
{
    sol::state lua;
    bool       ok = true;

    auto evolithInput = lua.create_table();
    evolithInput["augment"]   = 0x555;
    evolithInput["shape"]     = 0x1B;
    evolithInput["element"]   = 0x0D;
    evolithInput["bonus"]     = 0x19;
    evolithInput["signature"] = "EvolithSig";

    Exdata::Evolith evolith{};
    evolith.fromTable(evolithInput);
    auto* evolithRaw = reinterpret_cast<uint8*>(&evolith);
    ok = expectUInt(evolith.Augment, 0x155, "evolith augment masked from table") && ok;
    ok = expectUInt(evolith.Shape, 0x0B, "evolith shape masked from table") && ok;
    ok = expectUInt(evolith.Element, 5, "evolith element masked from table") && ok;
    ok = expectUInt(evolith.Bonus, 9, "evolith bonus masked from table") && ok;
    ok = expectString(Exdata::decodeSignature(evolith.Signature), "EvolithSig", "evolith signature from table") && ok;
    ok = expectUInt(evolithRaw[0], 0x55, "evolith raw bitfield byte 0") && ok;
    ok = expectUInt(evolithRaw[1], 0x6D, "evolith raw bitfield byte 1") && ok;
    ok = expectUInt(evolithRaw[2], 0x13, "evolith raw bitfield byte 2") && ok;
    ok = expectUInt(evolithRaw[3], 0x00, "evolith raw bitfield byte 3") && ok;

    auto evolithOutput = lua.create_table();
    evolith.toTable(evolithOutput);
    ok = expectUInt(evolithOutput["augment"].get<uint32>(), 0x155, "evolith augment to table") && ok;
    ok = expectUInt(evolithOutput["shape"].get<uint32>(), 0x0B, "evolith shape to table") && ok;
    ok = expectUInt(evolithOutput["element"].get<uint32>(), 5, "evolith element to table") && ok;
    ok = expectUInt(evolithOutput["bonus"].get<uint32>(), 9, "evolith bonus to table") && ok;
    ok = expectString(evolithOutput["signature"].get<std::string>(), "EvolithSig", "evolith signature to table") && ok;

    evolithRaw[2] |= 0xE0;
    evolithRaw[3] = 0xA5;
    evolithRaw[4] = 0xC1;
    evolithRaw[5] = 0xC2;
    evolithRaw[6] = 0xC3;
    evolithRaw[7] = 0xC4;
    evolithRaw[8] = 0xC5;
    evolithRaw[9] = 0xC6;
    evolithRaw[10] = 0xC7;
    evolithRaw[11] = 0xC8;

    auto evolithPartial = lua.create_table();
    evolithPartial["shape"] = 2;
    evolith.fromTable(evolithPartial);
    ok = expectUInt(evolith.Augment, 0x155, "evolith augment preserved") && ok;
    ok = expectUInt(evolith.Shape, 2, "evolith shape partial update") && ok;
    ok = expectUInt(evolith.Element, 5, "evolith element preserved") && ok;
    ok = expectUInt(evolith.Bonus, 9, "evolith bonus preserved") && ok;
    ok = expectString(Exdata::decodeSignature(evolith.Signature), "EvolithSig", "evolith signature preserved") && ok;
    ok = expectUInt(evolithRaw[2] & 0xE0, 0xE0, "evolith hidden bitfield bits preserved") && ok;
    ok = expectUInt(evolithRaw[3], 0xA5, "evolith hidden bitfield byte preserved") && ok;
    ok = expectUInt(evolithRaw[4], 0xC1, "evolith padding byte 0 preserved") && ok;
    ok = expectUInt(evolithRaw[11], 0xC8, "evolith padding byte 7 preserved") && ok;

    auto craftingInput = lua.create_table();
    craftingInput["quality"]   = 0x1234;
    craftingInput["signature"] = "CraftedBy";

    Exdata::CraftingSet crafting{};
    crafting.fromTable(craftingInput);
    auto* craftingRaw = reinterpret_cast<uint8*>(&crafting);
    ok = expectUInt(crafting.Quality, 0x1234, "crafting set quality from table") && ok;
    ok = expectString(Exdata::decodeSignature(crafting.Signature), "CraftedBy", "crafting set signature from table") && ok;
    ok = expectUInt(craftingRaw[2], 0x34, "crafting set raw quality byte 0") && ok;
    ok = expectUInt(craftingRaw[3], 0x12, "crafting set raw quality byte 1") && ok;

    auto craftingOutput = lua.create_table();
    crafting.toTable(craftingOutput);
    ok = expectUInt(craftingOutput["quality"].get<uint16>(), 0x1234, "crafting set quality to table") && ok;
    ok = expectString(craftingOutput["signature"].get<std::string>(), "CraftedBy", "crafting set signature to table") && ok;

    craftingRaw[0]  = 0xA1;
    craftingRaw[1]  = 0xB2;
    craftingRaw[4]  = 0xD1;
    craftingRaw[5]  = 0xD2;
    craftingRaw[6]  = 0xD3;
    craftingRaw[7]  = 0xD4;
    craftingRaw[8]  = 0xD5;
    craftingRaw[9]  = 0xD6;
    craftingRaw[10] = 0xD7;
    craftingRaw[11] = 0xD8;

    auto craftingPartial = lua.create_table();
    craftingPartial["quality"] = 0x0102;
    crafting.fromTable(craftingPartial);
    ok = expectUInt(crafting.Quality, 0x0102, "crafting set quality partial update") && ok;
    ok = expectString(Exdata::decodeSignature(crafting.Signature), "CraftedBy", "crafting set signature preserved") && ok;
    ok = expectUInt(craftingRaw[0], 0xA1, "crafting set prefix padding byte 0 preserved") && ok;
    ok = expectUInt(craftingRaw[1], 0xB2, "crafting set prefix padding byte 1 preserved") && ok;
    ok = expectUInt(craftingRaw[4], 0xD1, "crafting set middle padding byte 0 preserved") && ok;
    ok = expectUInt(craftingRaw[11], 0xD8, "crafting set middle padding byte 7 preserved") && ok;

    return ok;
}

auto testFurnishingTableSerialization() -> bool
{
    sol::state lua;
    bool       ok = true;

    auto furnitureInput = lua.create_table();
    furnitureInput["on2ndFloor"] = false;
    furnitureInput["installed"]  = true;
    furnitureInput["x"]          = 5;
    furnitureInput["z"]          = 6;
    furnitureInput["y"]          = 7;
    furnitureInput["rotation"]   = 8;
    furnitureInput["order"]      = 9;
    furnitureInput["signature"]  = "FurnishSig";

    Exdata::Furniture furniture{};
    furniture.fromTable(furnitureInput);
    auto* furnitureRaw = reinterpret_cast<uint8*>(&furniture);
    ok = expectUInt(furniture.On2ndFloor, 0, "furniture on2ndFloor from table") && ok;
    ok = expectUInt(furniture.Installed, 1, "furniture installed from table") && ok;
    ok = expectUInt(furniture.X, 5, "furniture x from table") && ok;
    ok = expectUInt(furniture.Z, 6, "furniture z from table") && ok;
    ok = expectUInt(furniture.Y, 7, "furniture y from table") && ok;
    ok = expectUInt(furniture.Rotation, 8, "furniture rotation from table") && ok;
    ok = expectUInt(furniture.Order, 9, "furniture order from table") && ok;
    ok = expectString(Exdata::decodeSignature(furniture.Signature), "FurnishSig", "furniture signature from table") && ok;
    ok = expectUInt(furnitureRaw[1], 0x40, "furniture raw flags byte") && ok;
    ok = expectUInt(furnitureRaw[2], 9, "furniture raw order byte") && ok;
    ok = expectUInt(furnitureRaw[6], 5, "furniture raw x byte") && ok;
    ok = expectUInt(furnitureRaw[7], 6, "furniture raw z byte") && ok;
    ok = expectUInt(furnitureRaw[8], 7, "furniture raw y byte") && ok;
    ok = expectUInt(furnitureRaw[9], 8, "furniture raw rotation byte") && ok;

    auto furnitureOutput = lua.create_table();
    furniture.toTable(furnitureOutput);
    ok = expectUInt(furnitureOutput["on2ndFloor"].get<bool>() ? 1 : 0, 0, "furniture on2ndFloor to table") && ok;
    ok = expectUInt(furnitureOutput["installed"].get<bool>() ? 1 : 0, 1, "furniture installed to table") && ok;
    ok = expectUInt(furnitureOutput["x"].get<uint8>(), 5, "furniture x to table") && ok;
    ok = expectUInt(furnitureOutput["z"].get<uint8>(), 6, "furniture z to table") && ok;
    ok = expectUInt(furnitureOutput["y"].get<uint8>(), 7, "furniture y to table") && ok;
    ok = expectUInt(furnitureOutput["rotation"].get<uint8>(), 8, "furniture rotation to table") && ok;
    ok = expectUInt(furnitureOutput["order"].get<uint8>(), 9, "furniture order to table") && ok;
    ok = expectString(furnitureOutput["signature"].get<std::string>(), "FurnishSig", "furniture signature to table") && ok;

    furnitureRaw[0]  = 0xA0;
    furnitureRaw[1]  = 0xAA;
    furnitureRaw[3]  = 0xB1;
    furnitureRaw[4]  = 0xB2;
    furnitureRaw[5]  = 0xB3;
    furnitureRaw[22] = 0xC1;
    furnitureRaw[23] = 0xC2;

    auto furniturePartial = lua.create_table();
    furniturePartial["on2ndFloor"] = true;
    furniturePartial["x"]          = 10;
    furniture.fromTable(furniturePartial);
    ok = expectUInt(furniture.On2ndFloor, 1, "furniture on2ndFloor partial update") && ok;
    ok = expectUInt(furniture.Installed, 0, "furniture installed preserved from raw bit") && ok;
    ok = expectUInt(furniture.X, 10, "furniture x partial update") && ok;
    ok = expectString(Exdata::decodeSignature(furniture.Signature), "FurnishSig", "furniture signature preserved") && ok;
    ok = expectUInt(furnitureRaw[0], 0xA0, "furniture header preserved") && ok;
    ok = expectUInt(furnitureRaw[1], 0xAB, "furniture hidden flag bits preserved") && ok;
    ok = expectUInt(furnitureRaw[3], 0xB1, "furniture padding byte 0 preserved") && ok;
    ok = expectUInt(furnitureRaw[5], 0xB3, "furniture padding byte 2 preserved") && ok;
    ok = expectUInt(furnitureRaw[22], 0xC1, "furniture tail padding byte 0 preserved") && ok;
    ok = expectUInt(furnitureRaw[23], 0xC2, "furniture tail padding byte 1 preserved") && ok;

    auto flowerInput = lua.create_table();
    flowerInput["step"]         = 3;
    flowerInput["dried"]        = true;
    flowerInput["crystal1"]     = 1;
    flowerInput["crystal2"]     = 4;
    flowerInput["kind"]         = 2;
    flowerInput["examined"]     = true;
    flowerInput["strength"]     = 0xB2;
    flowerInput["x"]            = 5;
    flowerInput["z"]            = 6;
    flowerInput["y"]            = 7;
    flowerInput["rotation"]     = 8;
    flowerInput["timePlanted"]  = 0x11223344;
    flowerInput["timeNextStep"] = 0x55667788;

    Exdata::FlowerPot flower{};
    flower.fromTable(flowerInput);
    auto* flowerRaw = reinterpret_cast<uint8*>(&flower);
    ok = expectUInt(flower.Step, 3, "flowerpot step from table") && ok;
    ok = expectUInt(flower.Dried, 1, "flowerpot dried from table") && ok;
    ok = expectUInt(flower.Crystal1, 1, "flowerpot crystal1 from table") && ok;
    ok = expectUInt(flower.Crystal2, 4, "flowerpot crystal2 from table") && ok;
    ok = expectUInt(flower.Kind, 2, "flowerpot kind from table") && ok;
    ok = expectUInt(flower.Examined, 1, "flowerpot examined from table") && ok;
    ok = expectUInt(flower.Strength, 0x32, "flowerpot strength masked from table") && ok;
    ok = expectUInt(flower.X, 5, "flowerpot x from table") && ok;
    ok = expectUInt(flower.Z, 6, "flowerpot z from table") && ok;
    ok = expectUInt(flower.Y, 7, "flowerpot y from table") && ok;
    ok = expectUInt(flower.Rotation, 8, "flowerpot rotation from table") && ok;
    ok = expectUInt(flower.TimePlanted, 0x11223344, "flowerpot time planted from table") && ok;
    ok = expectUInt(flower.TimeNextStep, 0x55667788, "flowerpot time next step from table") && ok;
    ok = expectUInt(flowerRaw[1], 0x80, "flowerpot raw dried byte") && ok;
    ok = expectUInt(flowerRaw[5], 0x65, "flowerpot raw examined strength byte") && ok;
    ok = expectUInt(flowerRaw[6], 5, "flowerpot raw x byte") && ok;
    ok = expectUInt(flowerRaw[7], 6, "flowerpot raw z byte") && ok;
    ok = expectUInt(flowerRaw[8], 7, "flowerpot raw y byte") && ok;
    ok = expectUInt(flowerRaw[9], 8, "flowerpot raw rotation byte") && ok;
    ok = expectUInt(flowerRaw[12], 0x44, "flowerpot raw planted byte 0") && ok;
    ok = expectUInt(flowerRaw[15], 0x11, "flowerpot raw planted byte 3") && ok;
    ok = expectUInt(flowerRaw[16], 0x88, "flowerpot raw next byte 0") && ok;
    ok = expectUInt(flowerRaw[19], 0x55, "flowerpot raw next byte 3") && ok;

    auto flowerOutput = lua.create_table();
    flower.toTable(flowerOutput);
    ok = expectUInt(flowerOutput["step"].get<uint8>(), 3, "flowerpot step to table") && ok;
    ok = expectUInt(flowerOutput["dried"].get<bool>() ? 1 : 0, 1, "flowerpot dried to table") && ok;
    ok = expectUInt(flowerOutput["crystal1"].get<uint8>(), 1, "flowerpot crystal1 to table") && ok;
    ok = expectUInt(flowerOutput["crystal2"].get<uint8>(), 4, "flowerpot crystal2 to table") && ok;
    ok = expectUInt(flowerOutput["kind"].get<uint8>(), 2, "flowerpot kind to table") && ok;
    ok = expectUInt(flowerOutput["examined"].get<bool>() ? 1 : 0, 1, "flowerpot examined to table") && ok;
    ok = expectUInt(flowerOutput["strength"].get<uint8>(), 0x32, "flowerpot strength to table") && ok;
    ok = expectUInt(flowerOutput["x"].get<uint8>(), 5, "flowerpot x to table") && ok;
    ok = expectUInt(flowerOutput["z"].get<uint8>(), 6, "flowerpot z to table") && ok;
    ok = expectUInt(flowerOutput["y"].get<uint8>(), 7, "flowerpot y to table") && ok;
    ok = expectUInt(flowerOutput["rotation"].get<uint8>(), 8, "flowerpot rotation to table") && ok;
    ok = expectUInt(flowerOutput["timePlanted"].get<uint32>(), 0x11223344, "flowerpot time planted to table") && ok;
    ok = expectUInt(flowerOutput["timeNextStep"].get<uint32>(), 0x55667788, "flowerpot time next step to table") && ok;

    flowerRaw[1]  = 0x7E;
    flowerRaw[10] = 0xD1;
    flowerRaw[11] = 0xD2;
    flowerRaw[20] = 0xE1;
    flowerRaw[21] = 0xE2;
    flowerRaw[22] = 0xE3;
    flowerRaw[23] = 0xE4;

    auto flowerPartial = lua.create_table();
    flowerPartial["dried"]   = true;
    flowerPartial["strength"] = 0xFF;
    flower.fromTable(flowerPartial);
    ok = expectUInt(flower.Step, 3, "flowerpot step preserved") && ok;
    ok = expectUInt(flower.Dried, 1, "flowerpot dried partial update") && ok;
    ok = expectUInt(flower.Installed, 1, "flowerpot installed raw bit preserved") && ok;
    ok = expectUInt(flower.Strength, 0x7F, "flowerpot strength masked partial update") && ok;
    ok = expectUInt(flowerRaw[1], 0xFE, "flowerpot hidden flag bits preserved") && ok;
    ok = expectUInt(flowerRaw[5], 0xFF, "flowerpot examined bit preserved with strength update") && ok;
    ok = expectUInt(flowerRaw[10], 0xD1, "flowerpot padding byte 0 preserved") && ok;
    ok = expectUInt(flowerRaw[11], 0xD2, "flowerpot padding byte 1 preserved") && ok;
    ok = expectUInt(flowerRaw[20], 0xE1, "flowerpot unknown byte 0 preserved") && ok;
    ok = expectUInt(flowerRaw[23], 0xE4, "flowerpot unknown byte 3 preserved") && ok;

    auto mannequinInput = lua.create_table();
    mannequinInput["x"]        = 1;
    mannequinInput["z"]        = 2;
    mannequinInput["y"]        = 3;
    mannequinInput["rotation"] = 4;
    mannequinInput["main"]     = 5;
    mannequinInput["sub"]      = 6;
    mannequinInput["ranged"]   = 7;
    mannequinInput["head"]     = 8;
    mannequinInput["body"]     = 9;
    mannequinInput["hands"]    = 10;
    mannequinInput["legs"]     = 11;
    mannequinInput["feet"]     = 12;
    mannequinInput["race"]     = 13;
    mannequinInput["pose"]     = 14;

    Exdata::Mannequin mannequin{};
    mannequin.fromTable(mannequinInput);
    auto* mannequinRaw = reinterpret_cast<uint8*>(&mannequin);
    ok = expectUInt(mannequin.X, 1, "mannequin x from table") && ok;
    ok = expectUInt(mannequin.Z, 2, "mannequin z from table") && ok;
    ok = expectUInt(mannequin.Y, 3, "mannequin y from table") && ok;
    ok = expectUInt(mannequin.Rotation, 4, "mannequin rotation from table") && ok;
    ok = expectUInt(mannequin.EquipMain, 5, "mannequin main from table") && ok;
    ok = expectUInt(mannequin.EquipSub, 6, "mannequin sub from table") && ok;
    ok = expectUInt(mannequin.EquipRanged, 7, "mannequin ranged from table") && ok;
    ok = expectUInt(mannequin.EquipHead, 8, "mannequin head from table") && ok;
    ok = expectUInt(mannequin.EquipBody, 9, "mannequin body from table") && ok;
    ok = expectUInt(mannequin.EquipHands, 10, "mannequin hands from table") && ok;
    ok = expectUInt(mannequin.EquipLegs, 11, "mannequin legs from table") && ok;
    ok = expectUInt(mannequin.EquipFeet, 12, "mannequin feet from table") && ok;
    ok = expectUInt(mannequin.Race, 13, "mannequin race from table") && ok;
    ok = expectUInt(mannequin.Pose, 14, "mannequin pose from table") && ok;
    ok = expectUInt(mannequinRaw[6], 1, "mannequin raw x byte") && ok;
    ok = expectUInt(mannequinRaw[7], 2, "mannequin raw z byte") && ok;
    ok = expectUInt(mannequinRaw[8], 3, "mannequin raw y byte") && ok;
    ok = expectUInt(mannequinRaw[9], 4, "mannequin raw rotation byte") && ok;
    ok = expectUInt(mannequinRaw[10], 5, "mannequin raw main byte") && ok;
    ok = expectUInt(mannequinRaw[11], 6, "mannequin raw sub byte") && ok;
    ok = expectUInt(mannequinRaw[12], 7, "mannequin raw ranged byte") && ok;
    ok = expectUInt(mannequinRaw[13], 8, "mannequin raw head byte") && ok;
    ok = expectUInt(mannequinRaw[14], 9, "mannequin raw body byte") && ok;
    ok = expectUInt(mannequinRaw[15], 10, "mannequin raw hands byte") && ok;
    ok = expectUInt(mannequinRaw[16], 11, "mannequin raw legs byte") && ok;
    ok = expectUInt(mannequinRaw[17], 12, "mannequin raw feet byte") && ok;
    ok = expectUInt(mannequinRaw[18], 13, "mannequin raw race byte") && ok;
    ok = expectUInt(mannequinRaw[19], 14, "mannequin raw pose byte") && ok;

    auto mannequinOutput = lua.create_table();
    mannequin.toTable(mannequinOutput);
    ok = expectUInt(mannequinOutput["x"].get<uint8>(), 1, "mannequin x to table") && ok;
    ok = expectUInt(mannequinOutput["z"].get<uint8>(), 2, "mannequin z to table") && ok;
    ok = expectUInt(mannequinOutput["y"].get<uint8>(), 3, "mannequin y to table") && ok;
    ok = expectUInt(mannequinOutput["rotation"].get<uint8>(), 4, "mannequin rotation to table") && ok;
    ok = expectUInt(mannequinOutput["main"].get<uint8>(), 5, "mannequin main to table") && ok;
    ok = expectUInt(mannequinOutput["sub"].get<uint8>(), 6, "mannequin sub to table") && ok;
    ok = expectUInt(mannequinOutput["ranged"].get<uint8>(), 7, "mannequin ranged to table") && ok;
    ok = expectUInt(mannequinOutput["head"].get<uint8>(), 8, "mannequin head to table") && ok;
    ok = expectUInt(mannequinOutput["body"].get<uint8>(), 9, "mannequin body to table") && ok;
    ok = expectUInt(mannequinOutput["hands"].get<uint8>(), 10, "mannequin hands to table") && ok;
    ok = expectUInt(mannequinOutput["legs"].get<uint8>(), 11, "mannequin legs to table") && ok;
    ok = expectUInt(mannequinOutput["feet"].get<uint8>(), 12, "mannequin feet to table") && ok;
    ok = expectUInt(mannequinOutput["race"].get<uint8>(), 13, "mannequin race to table") && ok;
    ok = expectUInt(mannequinOutput["pose"].get<uint8>(), 14, "mannequin pose to table") && ok;

    mannequinRaw[0]  = 0xA1;
    mannequinRaw[1]  = 0xA2;
    mannequinRaw[2]  = 0xB1;
    mannequinRaw[3]  = 0xB2;
    mannequinRaw[4]  = 0xB3;
    mannequinRaw[5]  = 0xB4;
    mannequinRaw[20] = 0xC1;
    mannequinRaw[21] = 0xC2;
    mannequinRaw[22] = 0xC3;
    mannequinRaw[23] = 0xC4;

    auto mannequinPartial = lua.create_table();
    mannequinPartial["main"] = 22;
    mannequinPartial["pose"] = 21;
    mannequin.fromTable(mannequinPartial);
    ok = expectUInt(mannequin.EquipMain, 22, "mannequin main partial update") && ok;
    ok = expectUInt(mannequin.Pose, 21, "mannequin pose partial update") && ok;
    ok = expectUInt(mannequin.Race, 13, "mannequin race preserved") && ok;
    ok = expectUInt(mannequinRaw[0], 0xA1, "mannequin header preserved") && ok;
    ok = expectUInt(mannequinRaw[1], 0xA2, "mannequin flags preserved") && ok;
    ok = expectUInt(mannequinRaw[2], 0xB1, "mannequin padding byte 0 preserved") && ok;
    ok = expectUInt(mannequinRaw[5], 0xB4, "mannequin padding byte 3 preserved") && ok;
    ok = expectUInt(mannequinRaw[20], 0xC1, "mannequin tail padding byte 0 preserved") && ok;
    ok = expectUInt(mannequinRaw[23], 0xC4, "mannequin tail padding byte 3 preserved") && ok;

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
    ok      = testFishTableSerialization() && ok;
    ok      = testChocoboEggTableSerialization() && ok;
    ok      = testChocoboCardTableSerialization() && ok;
    ok      = testEscutcheonTableSerialization() && ok;
    ok      = testSerializedTableSerialization() && ok;
    ok      = testTimerInfoTableSerialization() && ok;
    ok      = testSoulTableSerialization() && ok;
    ok      = testLogTicketTableSerialization() && ok;
    ok      = testMetadataTableSerialization() && ok;
    ok      = testFurnishingTableSerialization() && ok;
    ok      = testPassTimerTableSerialization() && ok;
    return ok;
}
