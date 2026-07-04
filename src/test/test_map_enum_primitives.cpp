/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

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

#include "test_map_enum_primitives.h"

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "map/enums/alter_ego_points.h"
#include "map/enums/automaton.h"
#include "map/enums/blocked_state.h"
#include "map/enums/chat_message_area.h"
#include "map/enums/furnishing_placement.h"
#include "map/enums/item_flag.h"
#include "map/enums/item_lockflg.h"
#include "map/enums/item_state.h"
#include "map/enums/item_types.h"
#include "map/enums/loot_recast.h"
#include "map/enums/mission_log.h"
#include "map/enums/music_slot.h"
#include "map/enums/quest_log.h"
#include "map/enums/recast.h"
#include "map/enums/synthesis_effect.h"
#include "map/enums/synthesis_result.h"
#include "map/enums/terrain_type.h"
#include "map/enums/weather.h"

namespace
{

struct EnumCase
{
    std::int64_t actual;
    std::int64_t expected;
    std::string  label;
};

auto expectEqualInt(std::int64_t actual, std::int64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "Map enum primitive self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectAll(const std::vector<EnumCase>& tests) -> bool
{
    bool ok = true;
    for (const auto& test : tests)
    {
        ok = expectEqualInt(test.actual, test.expected, test.label) && ok;
    }
    return ok;
}

auto testChatMessageAreas() -> bool
{
    return expectAll({
        { static_cast<std::int64_t>(ChatMessageArea::System), 0, "ChatMessageArea::System" },
        { static_cast<std::int64_t>(ChatMessageArea::Say), 1, "ChatMessageArea::Say" },
        { static_cast<std::int64_t>(ChatMessageArea::Shout), 2, "ChatMessageArea::Shout" },
        { static_cast<std::int64_t>(ChatMessageArea::Party), 3, "ChatMessageArea::Party" },
        { static_cast<std::int64_t>(ChatMessageArea::Yell), 4, "ChatMessageArea::Yell" },
        { static_cast<std::int64_t>(ChatMessageArea::Unity), 5, "ChatMessageArea::Unity" },
        { static_cast<std::int64_t>(ChatMessageArea::Assist), 6, "ChatMessageArea::Assist" },
    });
}

auto testEnvironmentEnums() -> bool
{
    return expectAll({
        { static_cast<std::int64_t>(MusicSlot::ZoneDay), 0, "MusicSlot::ZoneDay" },
        { static_cast<std::int64_t>(MusicSlot::ZoneNight), 1, "MusicSlot::ZoneNight" },
        { static_cast<std::int64_t>(MusicSlot::CombatSolo), 2, "MusicSlot::CombatSolo" },
        { static_cast<std::int64_t>(MusicSlot::CombatParty), 3, "MusicSlot::CombatParty" },
        { static_cast<std::int64_t>(MusicSlot::Mount), 4, "MusicSlot::Mount" },
        { static_cast<std::int64_t>(MusicSlot::Dead), 5, "MusicSlot::Dead" },
        { static_cast<std::int64_t>(MusicSlot::MogHouse), 6, "MusicSlot::MogHouse" },
        { static_cast<std::int64_t>(MusicSlot::Fishing), 7, "MusicSlot::Fishing" },
        { static_cast<std::int64_t>(TerrainType::Object), 0, "TerrainType::Object" },
        { static_cast<std::int64_t>(TerrainType::Path), 1, "TerrainType::Path" },
        { static_cast<std::int64_t>(TerrainType::Grass), 2, "TerrainType::Grass" },
        { static_cast<std::int64_t>(TerrainType::Sand), 3, "TerrainType::Sand" },
        { static_cast<std::int64_t>(TerrainType::Snow), 4, "TerrainType::Snow" },
        { static_cast<std::int64_t>(TerrainType::Stone), 5, "TerrainType::Stone" },
        { static_cast<std::int64_t>(TerrainType::Metal), 6, "TerrainType::Metal" },
        { static_cast<std::int64_t>(TerrainType::Wood), 7, "TerrainType::Wood" },
        { static_cast<std::int64_t>(TerrainType::ShallowWater), 8, "TerrainType::ShallowWater" },
        { static_cast<std::int64_t>(TerrainType::DeepWater), 9, "TerrainType::DeepWater" },
        { static_cast<std::int64_t>(TerrainType::Unknown), 10, "TerrainType::Unknown" },
        { static_cast<std::int64_t>(TerrainType::None), 0xFF, "TerrainType::None" },
        { static_cast<std::int64_t>(Weather::None), 0, "Weather::None" },
        { static_cast<std::int64_t>(Weather::Sunshine), 1, "Weather::Sunshine" },
        { static_cast<std::int64_t>(Weather::Clouds), 2, "Weather::Clouds" },
        { static_cast<std::int64_t>(Weather::Fog), 3, "Weather::Fog" },
        { static_cast<std::int64_t>(Weather::HotSpell), 4, "Weather::HotSpell" },
        { static_cast<std::int64_t>(Weather::HeatWave), 5, "Weather::HeatWave" },
        { static_cast<std::int64_t>(Weather::Rain), 6, "Weather::Rain" },
        { static_cast<std::int64_t>(Weather::Squall), 7, "Weather::Squall" },
        { static_cast<std::int64_t>(Weather::DustStorm), 8, "Weather::DustStorm" },
        { static_cast<std::int64_t>(Weather::SandStorm), 9, "Weather::SandStorm" },
        { static_cast<std::int64_t>(Weather::Wind), 10, "Weather::Wind" },
        { static_cast<std::int64_t>(Weather::Gales), 11, "Weather::Gales" },
        { static_cast<std::int64_t>(Weather::Snow), 12, "Weather::Snow" },
        { static_cast<std::int64_t>(Weather::Blizzards), 13, "Weather::Blizzards" },
        { static_cast<std::int64_t>(Weather::Thunder), 14, "Weather::Thunder" },
        { static_cast<std::int64_t>(Weather::Thunderstorms), 15, "Weather::Thunderstorms" },
        { static_cast<std::int64_t>(Weather::Auroras), 16, "Weather::Auroras" },
        { static_cast<std::int64_t>(Weather::StellarGlare), 17, "Weather::StellarGlare" },
        { static_cast<std::int64_t>(Weather::Gloom), 18, "Weather::Gloom" },
        { static_cast<std::int64_t>(Weather::Darkness), 19, "Weather::Darkness" },
    });
}

auto testItemEnums() -> bool
{
    return expectAll({
        { static_cast<std::int64_t>(ItemState::Free), 0, "ItemState::Free" },
        { static_cast<std::int64_t>(ItemState::Equipped), 1, "ItemState::Equipped" },
        { static_cast<std::int64_t>(ItemState::Bazaar), 2, "ItemState::Bazaar" },
        { static_cast<std::int64_t>(ItemState::PlacedFurniture), 3, "ItemState::PlacedFurniture" },
        { static_cast<std::int64_t>(ItemState::InTransaction), 4, "ItemState::InTransaction" },
        { static_cast<std::int64_t>(ItemType::General), 1, "ItemType::General" },
        { static_cast<std::int64_t>(ItemType::Linkshell), 2, "ItemType::Linkshell" },
        { static_cast<std::int64_t>(ItemType::Furnishing), 3, "ItemType::Furnishing" },
        { static_cast<std::int64_t>(ItemType::Puppet), 4, "ItemType::Puppet" },
        { static_cast<std::int64_t>(ItemType::Usable), 5, "ItemType::Usable" },
        { static_cast<std::int64_t>(ItemType::Equipment), 6, "ItemType::Equipment" },
        { static_cast<std::int64_t>(ItemType::Weapon), 7, "ItemType::Weapon" },
        { static_cast<std::int64_t>(ItemType::Currency), 8, "ItemType::Currency" },
        { static_cast<std::int64_t>(ItemType::FlowerPot), 9, "ItemType::FlowerPot" },
        { static_cast<std::int64_t>(ItemLockFlg::Normal), 0x00, "ItemLockFlg::Normal" },
        { static_cast<std::int64_t>(ItemLockFlg::NoDrop), 0x05, "ItemLockFlg::NoDrop" },
        { static_cast<std::int64_t>(ItemLockFlg::NoSelect), 0x0F, "ItemLockFlg::NoSelect" },
        { static_cast<std::int64_t>(ItemLockFlg::Linkshell), 0x13, "ItemLockFlg::Linkshell" },
        { static_cast<std::int64_t>(ItemLockFlg::Unknown0), 0x19, "ItemLockFlg::Unknown0" },
        { static_cast<std::int64_t>(ItemLockFlg::Mannequin), 0x1B, "ItemLockFlg::Mannequin" },
        { static_cast<std::int64_t>(FurnishingPlacement::Floor), 0, "FurnishingPlacement::Floor" },
        { static_cast<std::int64_t>(FurnishingPlacement::Surface), 1, "FurnishingPlacement::Surface" },
        { static_cast<std::int64_t>(FurnishingPlacement::Wall), 2, "FurnishingPlacement::Wall" },
        { static_cast<std::int64_t>(FurnishingPlacement::OnTable), 3, "FurnishingPlacement::OnTable" },
    });
}

auto testItemFlags() -> bool
{
    const auto flags = ItemFlag::CanUse | ItemFlag::CanEquip | ItemFlag::Rare;
    return expectAll({
        { static_cast<std::int64_t>(ItemFlag::None), 0x00000000, "ItemFlag::None" },
        { static_cast<std::int64_t>(ItemFlag::AugSendable), 0x00000001, "ItemFlag::AugSendable" },
        { static_cast<std::int64_t>(ItemFlag::GMOnly), 0x00000002, "ItemFlag::GMOnly" },
        { static_cast<std::int64_t>(ItemFlag::MysteryBox), 0x00000004, "ItemFlag::MysteryBox" },
        { static_cast<std::int64_t>(ItemFlag::MogGarden), 0x00000008, "ItemFlag::MogGarden" },
        { static_cast<std::int64_t>(ItemFlag::CanSendAccount), 0x00000010, "ItemFlag::CanSendAccount" },
        { static_cast<std::int64_t>(ItemFlag::Inscribable), 0x00000020, "ItemFlag::Inscribable" },
        { static_cast<std::int64_t>(ItemFlag::NoAuction), 0x00000040, "ItemFlag::NoAuction" },
        { static_cast<std::int64_t>(ItemFlag::Scroll), 0x00000080, "ItemFlag::Scroll" },
        { static_cast<std::int64_t>(ItemFlag::Linkshell), 0x00000100, "ItemFlag::Linkshell" },
        { static_cast<std::int64_t>(ItemFlag::CanUse), 0x00000200, "ItemFlag::CanUse" },
        { static_cast<std::int64_t>(ItemFlag::CanTradeNPC), 0x00000400, "ItemFlag::CanTradeNPC" },
        { static_cast<std::int64_t>(ItemFlag::CanEquip), 0x00000800, "ItemFlag::CanEquip" },
        { static_cast<std::int64_t>(ItemFlag::NoSale), 0x00001000, "ItemFlag::NoSale" },
        { static_cast<std::int64_t>(ItemFlag::NoDelivery), 0x00002000, "ItemFlag::NoDelivery" },
        { static_cast<std::int64_t>(ItemFlag::Exclusive), 0x00004000, "ItemFlag::Exclusive" },
        { static_cast<std::int64_t>(ItemFlag::Rare), 0x00008000, "ItemFlag::Rare" },
        { static_cast<std::int64_t>(ItemFlag::NoRecycle), 0x00010000, "ItemFlag::NoRecycle" },
        { static_cast<std::int64_t>(ItemFlag::NoRareCheck), 0x00020000, "ItemFlag::NoRareCheck" },
        { static_cast<std::int64_t>(flags), 0x00008A00, "ItemFlag combined" },
    });
}

auto testRecastAndSynthesisEnums() -> bool
{
    return expectAll({
        { static_cast<std::int64_t>(Recast::Special), 0, "Recast::Special" },
        { static_cast<std::int64_t>(Recast::Sic), 102, "Recast::Sic" },
        { static_cast<std::int64_t>(Recast::BloodPactRage), 173, "Recast::BloodPactRage" },
        { static_cast<std::int64_t>(Recast::BloodPactWard), 174, "Recast::BloodPactWard" },
        { static_cast<std::int64_t>(Recast::RandomDeal), 196, "Recast::RandomDeal" },
        { static_cast<std::int64_t>(Recast::Strategems), 231, "Recast::Strategems" },
        { static_cast<std::int64_t>(Recast::Special2), 254, "Recast::Special2" },
        { static_cast<std::int64_t>(Recast::Mount), 256, "Recast::Mount" },
        { static_cast<std::int64_t>(LootRecastID::Seal), 1, "LootRecastID::Seal" },
        { static_cast<std::int64_t>(LootRecastID::Geode), 2, "LootRecastID::Geode" },
        { static_cast<std::int64_t>(SynthesisEffect::None), 0x0000, "SynthesisEffect::None" },
        { static_cast<std::int64_t>(SynthesisEffect::Water), 0x0010, "SynthesisEffect::Water" },
        { static_cast<std::int64_t>(SynthesisEffect::Wind), 0x0011, "SynthesisEffect::Wind" },
        { static_cast<std::int64_t>(SynthesisEffect::Fire), 0x0012, "SynthesisEffect::Fire" },
        { static_cast<std::int64_t>(SynthesisEffect::Earth), 0x0013, "SynthesisEffect::Earth" },
        { static_cast<std::int64_t>(SynthesisEffect::Lightning), 0x0014, "SynthesisEffect::Lightning" },
        { static_cast<std::int64_t>(SynthesisEffect::Ice), 0x0015, "SynthesisEffect::Ice" },
        { static_cast<std::int64_t>(SynthesisEffect::Light), 0x0016, "SynthesisEffect::Light" },
        { static_cast<std::int64_t>(SynthesisEffect::Dark), 0x0017, "SynthesisEffect::Dark" },
        { static_cast<std::int64_t>(SynthesisResult::Success), 0x00, "SynthesisResult::Success" },
        { static_cast<std::int64_t>(SynthesisResult::Failed), 0x01, "SynthesisResult::Failed" },
        { static_cast<std::int64_t>(SynthesisResult::Interrupted), 0x02, "SynthesisResult::Interrupted" },
        { static_cast<std::int64_t>(SynthesisResult::CancelBadRecipe), 0x03, "SynthesisResult::CancelBadRecipe" },
        { static_cast<std::int64_t>(SynthesisResult::Cancel), 0x04, "SynthesisResult::Cancel" },
        { static_cast<std::int64_t>(SynthesisResult::CancelSkillTooLow), 0x06, "SynthesisResult::CancelSkillTooLow" },
        { static_cast<std::int64_t>(SynthesisResult::CancelRareItem), 0x07, "SynthesisResult::CancelRareItem" },
        { static_cast<std::int64_t>(SynthesisResult::SuccessDesynth), 0x0C, "SynthesisResult::SuccessDesynth" },
        { static_cast<std::int64_t>(SynthesisResult::MustWaitLonger), 0x0D, "SynthesisResult::MustWaitLonger" },
        { static_cast<std::int64_t>(SynthesisResult::InterruptedCritical), 0x0E, "SynthesisResult::InterruptedCritical" },
    });
}

auto testAlterEgoAndAutomatonEnums() -> bool
{
    return expectAll({
        { static_cast<std::int64_t>(AlterEgoCategory::HP), 8, "AlterEgoCategory::HP" },
        { static_cast<std::int64_t>(AlterEgoCategory::MP), 9, "AlterEgoCategory::MP" },
        { static_cast<std::int64_t>(AlterEgoCategory::STR), 10, "AlterEgoCategory::STR" },
        { static_cast<std::int64_t>(AlterEgoCategory::DEX), 11, "AlterEgoCategory::DEX" },
        { static_cast<std::int64_t>(AlterEgoCategory::VIT), 12, "AlterEgoCategory::VIT" },
        { static_cast<std::int64_t>(AlterEgoCategory::AGI), 13, "AlterEgoCategory::AGI" },
        { static_cast<std::int64_t>(AlterEgoCategory::INT), 14, "AlterEgoCategory::INT" },
        { static_cast<std::int64_t>(AlterEgoCategory::MND), 15, "AlterEgoCategory::MND" },
        { static_cast<std::int64_t>(AlterEgoCategory::CHR), 16, "AlterEgoCategory::CHR" },
        { static_cast<std::int64_t>(AlterEgoCategory::CombatSkills), 17, "AlterEgoCategory::CombatSkills" },
        { static_cast<std::int64_t>(AlterEgoCategory::MagicSkills), 18, "AlterEgoCategory::MagicSkills" },
        { static_cast<std::int64_t>(AutomatonFrame::Harlequin), 0x20, "AutomatonFrame::Harlequin" },
        { static_cast<std::int64_t>(AutomatonFrame::Valoredge), 0x21, "AutomatonFrame::Valoredge" },
        { static_cast<std::int64_t>(AutomatonFrame::Sharpshot), 0x22, "AutomatonFrame::Sharpshot" },
        { static_cast<std::int64_t>(AutomatonFrame::Stormwaker), 0x23, "AutomatonFrame::Stormwaker" },
        { static_cast<std::int64_t>(AutomatonHead::Harlequin), 0x01, "AutomatonHead::Harlequin" },
        { static_cast<std::int64_t>(AutomatonHead::Valoredge), 0x02, "AutomatonHead::Valoredge" },
        { static_cast<std::int64_t>(AutomatonHead::Sharpshot), 0x03, "AutomatonHead::Sharpshot" },
        { static_cast<std::int64_t>(AutomatonHead::Stormwaker), 0x04, "AutomatonHead::Stormwaker" },
        { static_cast<std::int64_t>(AutomatonHead::Soulsoother), 0x05, "AutomatonHead::Soulsoother" },
        { static_cast<std::int64_t>(AutomatonHead::Spiritreaver), 0x06, "AutomatonHead::Spiritreaver" },
        { static_cast<std::int64_t>(AutomatonAttachment::AutoRepairKit), 193, "AutomatonAttachment::AutoRepairKit" },
        { static_cast<std::int64_t>(AutomatonAttachment::AutoRepairKitII), 196, "AutomatonAttachment::AutoRepairKitII" },
        { static_cast<std::int64_t>(AutomatonAttachment::OpticFiber), 198, "AutomatonAttachment::OpticFiber" },
        { static_cast<std::int64_t>(AutomatonAttachment::AutoRepairKitIII), 202, "AutomatonAttachment::AutoRepairKitIII" },
        { static_cast<std::int64_t>(AutomatonAttachment::AutoRepairKitIV), 205, "AutomatonAttachment::AutoRepairKitIV" },
        { static_cast<std::int64_t>(AutomatonAttachment::OpticFiberII), 206, "AutomatonAttachment::OpticFiberII" },
        { static_cast<std::int64_t>(AutomatonAttachment::ManaTank), 225, "AutomatonAttachment::ManaTank" },
        { static_cast<std::int64_t>(AutomatonAttachment::ManaTankII), 228, "AutomatonAttachment::ManaTankII" },
        { static_cast<std::int64_t>(AutomatonAttachment::ManaTankIII), 233, "AutomatonAttachment::ManaTankIII" },
        { static_cast<std::int64_t>(AutomatonAttachment::ManaTankIV), 235, "AutomatonAttachment::ManaTankIV" },
    });
}

auto testBlockedStates() -> bool
{
    const auto state = BlockedState::Dead | BlockedState::InEvent | BlockedState::Mounted;
    return expectAll({
        { static_cast<std::int64_t>(BlockedState::Jailed), 0x0001, "BlockedState::Jailed" },
        { static_cast<std::int64_t>(BlockedState::Dead), 0x0002, "BlockedState::Dead" },
        { static_cast<std::int64_t>(BlockedState::Crafting), 0x0004, "BlockedState::Crafting" },
        { static_cast<std::int64_t>(BlockedState::Healing), 0x0008, "BlockedState::Healing" },
        { static_cast<std::int64_t>(BlockedState::Fishing), 0x0010, "BlockedState::Fishing" },
        { static_cast<std::int64_t>(BlockedState::Sitting), 0x0020, "BlockedState::Sitting" },
        { static_cast<std::int64_t>(BlockedState::Mounted), 0x0040, "BlockedState::Mounted" },
        { static_cast<std::int64_t>(BlockedState::Charmed), 0x0080, "BlockedState::Charmed" },
        { static_cast<std::int64_t>(BlockedState::PreventAction), 0x0100, "BlockedState::PreventAction" },
        { static_cast<std::int64_t>(BlockedState::InEvent), 0x0200, "BlockedState::InEvent" },
        { static_cast<std::int64_t>(BlockedState::Engaged), 0x0400, "BlockedState::Engaged" },
        { static_cast<std::int64_t>(BlockedState::AbnormalStatus), 0x0800, "BlockedState::AbnormalStatus" },
        { static_cast<std::int64_t>(BlockedState::Monstrosity), 0x1000, "BlockedState::Monstrosity" },
        { static_cast<std::int64_t>(state), 0x0242, "BlockedState combined" },
    });
}

auto testMissionAndQuestEnums() -> bool
{
    return expectAll({
        { static_cast<std::int64_t>(MissionLog::Sandoria), 0, "MissionLog::Sandoria" },
        { static_cast<std::int64_t>(MissionLog::Bastok), 1, "MissionLog::Bastok" },
        { static_cast<std::int64_t>(MissionLog::Windurst), 2, "MissionLog::Windurst" },
        { static_cast<std::int64_t>(MissionLog::Zilart), 3, "MissionLog::Zilart" },
        { static_cast<std::int64_t>(MissionLog::ToAU), 4, "MissionLog::ToAU" },
        { static_cast<std::int64_t>(MissionLog::WoTG), 5, "MissionLog::WoTG" },
        { static_cast<std::int64_t>(MissionLog::CoP), 6, "MissionLog::CoP" },
        { static_cast<std::int64_t>(MissionLog::Assault), 7, "MissionLog::Assault" },
        { static_cast<std::int64_t>(MissionLog::Campaign), 8, "MissionLog::Campaign" },
        { static_cast<std::int64_t>(MissionLog::ACP), 9, "MissionLog::ACP" },
        { static_cast<std::int64_t>(MissionLog::AMK), 10, "MissionLog::AMK" },
        { static_cast<std::int64_t>(MissionLog::ASA), 11, "MissionLog::ASA" },
        { static_cast<std::int64_t>(MissionLog::SoA), 12, "MissionLog::SoA" },
        { static_cast<std::int64_t>(MissionLog::RoV), 13, "MissionLog::RoV" },
        { static_cast<std::int64_t>(MissionComplete::Campaign1), 0x0030, "MissionComplete::Campaign1" },
        { static_cast<std::int64_t>(MissionComplete::Campaign2), 0x0038, "MissionComplete::Campaign2" },
        { static_cast<std::int64_t>(MissionComplete::Nations), 0x00D0, "MissionComplete::Nations" },
        { static_cast<std::int64_t>(MissionComplete::ToAU_WoTG), 0x00D8, "MissionComplete::ToAU_WoTG" },
        { static_cast<std::int64_t>(QuestLog::Sandoria), 0, "QuestLog::Sandoria" },
        { static_cast<std::int64_t>(QuestLog::Bastok), 1, "QuestLog::Bastok" },
        { static_cast<std::int64_t>(QuestLog::Windurst), 2, "QuestLog::Windurst" },
        { static_cast<std::int64_t>(QuestLog::Jeuno), 3, "QuestLog::Jeuno" },
        { static_cast<std::int64_t>(QuestLog::OtherAreas), 4, "QuestLog::OtherAreas" },
        { static_cast<std::int64_t>(QuestLog::Outlands), 5, "QuestLog::Outlands" },
        { static_cast<std::int64_t>(QuestLog::AhtUrghan), 6, "QuestLog::AhtUrghan" },
        { static_cast<std::int64_t>(QuestLog::CrystalWar), 7, "QuestLog::CrystalWar" },
        { static_cast<std::int64_t>(QuestLog::Abyssea), 8, "QuestLog::Abyssea" },
        { static_cast<std::int64_t>(QuestLog::Adoulin), 9, "QuestLog::Adoulin" },
        { static_cast<std::int64_t>(QuestLog::Coalition), 10, "QuestLog::Coalition" },
        { static_cast<std::int64_t>(QuestComplete::Sandoria), 0x0090, "QuestComplete::Sandoria" },
        { static_cast<std::int64_t>(QuestComplete::Bastok), 0x0098, "QuestComplete::Bastok" },
        { static_cast<std::int64_t>(QuestComplete::Windurst), 0x00A0, "QuestComplete::Windurst" },
        { static_cast<std::int64_t>(QuestComplete::Jeuno), 0x00A8, "QuestComplete::Jeuno" },
        { static_cast<std::int64_t>(QuestComplete::OtherAreas), 0x00B0, "QuestComplete::OtherAreas" },
        { static_cast<std::int64_t>(QuestComplete::Outlands), 0x00B8, "QuestComplete::Outlands" },
        { static_cast<std::int64_t>(QuestComplete::AhtUrghan), 0x00C0, "QuestComplete::AhtUrghan" },
        { static_cast<std::int64_t>(QuestComplete::CrystalWar), 0x00C8, "QuestComplete::CrystalWar" },
        { static_cast<std::int64_t>(QuestComplete::Abyssea), 0x00E8, "QuestComplete::Abyssea" },
        { static_cast<std::int64_t>(QuestComplete::Adoulin), 0x00F8, "QuestComplete::Adoulin" },
        { static_cast<std::int64_t>(QuestComplete::Coalition), 0x0108, "QuestComplete::Coalition" },
        { static_cast<std::int64_t>(QuestOffer::Sandoria), 0x0050, "QuestOffer::Sandoria" },
        { static_cast<std::int64_t>(QuestOffer::Bastok), 0x0058, "QuestOffer::Bastok" },
        { static_cast<std::int64_t>(QuestOffer::Windurst), 0x0060, "QuestOffer::Windurst" },
        { static_cast<std::int64_t>(QuestOffer::Jeuno), 0x0068, "QuestOffer::Jeuno" },
        { static_cast<std::int64_t>(QuestOffer::OtherAreas), 0x0070, "QuestOffer::OtherAreas" },
        { static_cast<std::int64_t>(QuestOffer::Outlands), 0x0078, "QuestOffer::Outlands" },
        { static_cast<std::int64_t>(QuestOffer::AhtUrghan), 0x0080, "QuestOffer::AhtUrghan" },
        { static_cast<std::int64_t>(QuestOffer::CrystalWar), 0x0088, "QuestOffer::CrystalWar" },
        { static_cast<std::int64_t>(QuestOffer::Abyssea), 0x00E0, "QuestOffer::Abyssea" },
        { static_cast<std::int64_t>(QuestOffer::Adoulin), 0x00F0, "QuestOffer::Adoulin" },
        { static_cast<std::int64_t>(QuestOffer::Coalition), 0x0100, "QuestOffer::Coalition" },
    });
}

} // namespace

auto runMapEnumPrimitiveSelfTests() -> bool
{
    return testChatMessageAreas() &&
           testEnvironmentEnums() &&
           testItemEnums() &&
           testItemFlags() &&
           testRecastAndSynthesisEnums() &&
           testAlterEgoAndAutomatonEnums() &&
           testBlockedStates() &&
           testMissionAndQuestEnums();
}
