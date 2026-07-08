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

} // namespace

auto runItemExdataSelfTests() -> bool
{
    bool ok = true;
    ok      = testTypeEnumGoldenValues() && ok;
    ok      = testItemIDTypeDispatch() && ok;
    ok      = testPredicateTypeDispatchAndPrecedence() && ok;
    ok      = testRawExdataOverlay() && ok;
    ok      = testTimerInfoTableSerialization() && ok;
    return ok;
}
