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

#include "test_item_flowerpot.h"

#include "map/items/item_flowerpot.h"

#include <cstdint>
#include <cstring>
#include <iostream>

namespace
{

auto expectBool(bool actual, bool expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "item flowerpot self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectUInt(std::uint64_t actual, std::uint64_t expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "item flowerpot self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto exdataU32(const CItemFlowerpot& item, const std::size_t offset) -> std::uint32_t
{
    std::uint32_t value = 0;
    std::memcpy(&value, item.m_extra + offset, sizeof(value));
    return value;
}

auto testConstructorDefaults() -> bool
{
    CItemFlowerpot item(0x1234);
    const auto     size = item.size();

    bool ok = true;
    ok      = expectUInt(item.getID(), 0x1234, "id") && ok;
    ok      = expectBool(item.isType(ITEM_FURNISHING), true, "furnishing type flag") && ok;
    ok      = expectBool(item.isType(ITEM_FLOWERPOT), true, "flowerpot type flag") && ok;
    ok      = expectBool(item.isGardeningPot(), true, "gardening pot") && ok;
    ok      = expectUInt(item.getQuantity(), 0, "default quantity") && ok;
    ok      = expectUInt(item.getStorage(), 0, "default storage") && ok;
    ok      = expectUInt(size.first, 1, "default size x") && ok;
    ok      = expectUInt(size.second, 1, "default size y") && ok;
    ok      = expectBool(item.isPlanted(), false, "default planted") && ok;
    ok      = expectBool(item.isTree(), false, "default tree") && ok;
    ok      = expectBool(item.isDried(), false, "default dried") && ok;
    ok      = expectBool(item.canGrow(), false, "default can grow") && ok;
    ok      = expectUInt(item.getPlant(), FLOWERPOT_PLANT_NONE, "default plant") && ok;
    ok      = expectUInt(item.getStage(), FLOWERPOT_STAGE_EMPTY, "default stage") && ok;
    ok      = expectUInt(item.getExtraCrystalFeed(), FLOWERPOT_ELEMENT_NONE, "default first crystal") && ok;
    ok      = expectUInt(item.getCommonCrystalFeed(), FLOWERPOT_ELEMENT_NONE, "default second crystal") && ok;
    ok      = expectUInt(item.getPlantTimestamp(), 0, "default plant timestamp") && ok;
    ok      = expectUInt(item.getStageTimestamp(), 0, "default stage timestamp") && ok;
    ok      = expectBool(item.wasExamined(), false, "default examined") && ok;
    ok      = expectUInt(item.getStrength(), 0, "default strength") && ok;

    for (auto idx = 0U; idx < CItem::extra_size; ++idx)
    {
        ok = expectUInt(item.m_extra[idx], 0, "default exdata byte") && ok;
    }
    return ok;
}

auto testGrowthFields() -> bool
{
    CItemFlowerpot item(0x2000);

    bool ok = true;
    item.setPlant(FLOWERPOT_PLANT_TREE_CUTTINGS);
    item.setStage(FLOWERPOT_STAGE_FIRST_SPROUTS);
    item.setDried(false);
    ok = expectBool(item.isPlanted(), true, "planted after stage") && ok;
    ok = expectBool(item.isTree(), true, "tree cutting is tree") && ok;
    ok = expectBool(item.canGrow(), true, "can grow while sprouting") && ok;

    item.setDried(true);
    ok = expectBool(item.isDried(), true, "dried") && ok;
    ok = expectBool(item.canGrow(), false, "dried cannot grow") && ok;

    item.setDried(false);
    item.setPlant(FLOWERPOT_PLANT_HERB_SEEDS);
    ok = expectBool(item.isTree(), false, "herb seed is not tree") && ok;
    item.setStage(FLOWERPOT_STAGE_MATURE_PLANT);
    ok = expectBool(item.canGrow(), false, "mature cannot grow") && ok;
    return ok;
}

auto testMappings() -> bool
{
    bool ok = true;
    ok      = expectUInt(CItemFlowerpot::getSeedID(FLOWERPOT_PLANT_NONE), 0, "none seed id") && ok;
    ok      = expectUInt(CItemFlowerpot::getSeedID(FLOWERPOT_PLANT_HERB_SEEDS), 572, "herb seed id") && ok;
    ok      = expectUInt(CItemFlowerpot::getSeedID(FLOWERPOT_PLANT_VEGETABLE_SEEDS), 573, "vegetable seed id") && ok;
    ok      = expectUInt(CItemFlowerpot::getSeedID(FLOWERPOT_PLANT_FRUIT_SEEDS), 574, "fruit seed id") && ok;
    ok      = expectUInt(CItemFlowerpot::getSeedID(FLOWERPOT_PLANT_GRAIN_SEEDS), 575, "grain seed id") && ok;
    ok      = expectUInt(CItemFlowerpot::getSeedID(FLOWERPOT_PLANT_CACTUS_STEMS), 1236, "cactus seed id") && ok;
    ok      = expectUInt(CItemFlowerpot::getSeedID(FLOWERPOT_PLANT_TREE_CUTTINGS), 1237, "cutting seed id") && ok;
    ok      = expectUInt(CItemFlowerpot::getSeedID(FLOWERPOT_PLANT_TREE_SAPLINGS), 1238, "sapling seed id") && ok;
    ok      = expectUInt(CItemFlowerpot::getSeedID(FLOWERPOT_PLANT_WILDGRASS_SEEDS), 2235, "wildgrass seed id") && ok;

    ok = expectUInt(CItemFlowerpot::getPlantFromSeed(572), FLOWERPOT_PLANT_HERB_SEEDS, "herb plant from seed") && ok;
    ok = expectUInt(CItemFlowerpot::getPlantFromSeed(575), FLOWERPOT_PLANT_GRAIN_SEEDS, "grain plant from seed") && ok;
    ok = expectUInt(CItemFlowerpot::getPlantFromSeed(573), FLOWERPOT_PLANT_VEGETABLE_SEEDS, "vegetable plant from seed") && ok;
    ok = expectUInt(CItemFlowerpot::getPlantFromSeed(574), FLOWERPOT_PLANT_FRUIT_SEEDS, "fruit plant from seed") && ok;
    ok = expectUInt(CItemFlowerpot::getPlantFromSeed(1236), FLOWERPOT_PLANT_CACTUS_STEMS, "cactus plant from seed") && ok;
    ok = expectUInt(CItemFlowerpot::getPlantFromSeed(1237), FLOWERPOT_PLANT_TREE_CUTTINGS, "cutting plant from seed") && ok;
    ok = expectUInt(CItemFlowerpot::getPlantFromSeed(1238), FLOWERPOT_PLANT_TREE_SAPLINGS, "sapling plant from seed") && ok;
    ok = expectUInt(CItemFlowerpot::getPlantFromSeed(2235), FLOWERPOT_PLANT_WILDGRASS_SEEDS, "wildgrass plant from seed") && ok;
    ok = expectUInt(CItemFlowerpot::getPlantFromSeed(9999), FLOWERPOT_PLANT_NONE, "unknown plant from seed") && ok;

    ok = expectUInt(CItemFlowerpot::getItemFromElement(FLOWERPOT_ELEMENT_FIRE), 4096, "fire crystal item") && ok;
    ok = expectUInt(CItemFlowerpot::getItemFromElement(FLOWERPOT_ELEMENT_DARK), 4103, "dark crystal item") && ok;
    ok = expectUInt(CItemFlowerpot::getElementFromItem(4096), FLOWERPOT_ELEMENT_FIRE, "fire element from item") && ok;
    ok = expectUInt(CItemFlowerpot::getElementFromItem(4103), FLOWERPOT_ELEMENT_DARK, "dark element from item") && ok;
    return ok;
}

auto testExdataFieldsAndCleanPot() -> bool
{
    CItemFlowerpot item(0x2001);

    bool ok = true;
    item.setInstalled(true);
    item.setDried(true);
    item.setPlant(FLOWERPOT_PLANT_VEGETABLE_SEEDS);
    item.setStage(FLOWERPOT_STAGE_SECOND_SPROUTS_CRYSTAL);
    item.setFirstCrystalFeed(FLOWERPOT_ELEMENT_FIRE);
    item.setSecondCrystalFeed(FLOWERPOT_ELEMENT_WATER);
    item.setPlantTimestamp(0x11223344);
    item.setStageTimestamp(0x55667788);
    item.markExamined();
    item.setStrength(77);
    item.m_extra[6]  = 0x21;
    item.m_extra[7]  = 0x22;
    item.m_extra[8]  = 0x23;
    item.m_extra[9]  = 0x24;
    item.m_extra[20] = 0xA1;
    item.m_extra[21] = 0xA2;
    item.m_extra[22] = 0xA3;
    item.m_extra[23] = 0xA4;

    ok = expectUInt(item.m_extra[0], FLOWERPOT_STAGE_SECOND_SPROUTS_CRYSTAL, "raw stage") && ok;
    ok = expectUInt(item.m_extra[1], 0xC0, "raw installed dried bits") && ok;
    ok = expectUInt(item.m_extra[2], FLOWERPOT_ELEMENT_FIRE, "raw first crystal") && ok;
    ok = expectUInt(item.m_extra[3], FLOWERPOT_ELEMENT_WATER, "raw second crystal") && ok;
    ok = expectUInt(item.m_extra[4], FLOWERPOT_PLANT_VEGETABLE_SEEDS, "raw plant") && ok;
    ok = expectUInt(item.m_extra[5], 0x9B, "raw examined strength") && ok;
    ok = expectUInt(exdataU32(item, 12), 0x11223344, "raw plant timestamp") && ok;
    ok = expectUInt(exdataU32(item, 16), 0x55667788, "raw stage timestamp") && ok;
    ok = expectBool(item.wasExamined(), true, "examined") && ok;
    ok = expectUInt(item.getStrength(), 77, "strength") && ok;

    item.cleanPot();
    ok = expectBool(item.isDried(), false, "clean dried") && ok;
    ok = expectUInt(item.getPlant(), FLOWERPOT_PLANT_NONE, "clean plant") && ok;
    ok = expectUInt(item.getStage(), FLOWERPOT_STAGE_EMPTY, "clean stage") && ok;
    ok = expectUInt(item.getExtraCrystalFeed(), FLOWERPOT_ELEMENT_NONE, "clean first crystal") && ok;
    ok = expectUInt(item.getCommonCrystalFeed(), FLOWERPOT_ELEMENT_NONE, "clean second crystal") && ok;
    ok = expectUInt(item.getPlantTimestamp(), 0, "clean plant timestamp") && ok;
    ok = expectUInt(item.getStageTimestamp(), 0, "clean stage timestamp") && ok;
    ok = expectBool(item.isInstalled(), true, "clean keeps installed") && ok;
    ok = expectBool(item.wasExamined(), true, "clean keeps examined") && ok;
    ok = expectUInt(item.getStrength(), 77, "clean keeps strength") && ok;
    ok = expectUInt(item.m_extra[6], 0x21, "clean keeps raw x") && ok;
    ok = expectUInt(item.m_extra[7], 0x22, "clean keeps raw z") && ok;
    ok = expectUInt(item.m_extra[8], 0x23, "clean keeps raw y") && ok;
    ok = expectUInt(item.m_extra[9], 0x24, "clean keeps raw rotation") && ok;
    ok = expectUInt(item.m_extra[20], 0xA1, "clean keeps unknown tail 0") && ok;
    ok = expectUInt(item.m_extra[21], 0xA2, "clean keeps unknown tail 1") && ok;
    ok = expectUInt(item.m_extra[22], 0xA3, "clean keeps unknown tail 2") && ok;
    ok = expectUInt(item.m_extra[23], 0xA4, "clean keeps unknown tail 3") && ok;

    item.clearExamined();
    ok = expectBool(item.wasExamined(), false, "examined cleared") && ok;
    ok = expectUInt(item.getStrength(), 77, "clear examined keeps strength") && ok;
    return ok;
}

auto testCopyConstructorCopiesFields() -> bool
{
    CItemFlowerpot original(0x2222);
    original.setStorage(80);
    original.setSize(3, 4);
    original.setInstalled(true);
    original.setDried(true);
    original.setPlant(FLOWERPOT_PLANT_CACTUS_STEMS);
    original.setStage(FLOWERPOT_STAGE_FIRST_SPROUTS);
    original.setFirstCrystalFeed(FLOWERPOT_ELEMENT_LIGHT);
    original.setSecondCrystalFeed(FLOWERPOT_ELEMENT_DARK);
    original.setPlantTimestamp(0x11223344);
    original.setStageTimestamp(0x55667788);
    original.markExamined();
    original.setStrength(55);

    CItemFlowerpot copy(original);
    original.cleanPot();
    original.setStorage(1);
    original.setSize(1, 1);
    original.clearExamined();
    original.setStrength(1);

    const auto size = copy.size();
    bool       ok   = true;
    ok             = expectUInt(copy.getID(), 0x2222, "copy id") && ok;
    ok             = expectBool(copy.isType(ITEM_FURNISHING), true, "copy furnishing type flag") && ok;
    ok             = expectBool(copy.isType(ITEM_FLOWERPOT), true, "copy flowerpot type flag") && ok;
    ok             = expectUInt(copy.getStorage(), 80, "copy storage") && ok;
    ok             = expectUInt(size.first, 3, "copy size x") && ok;
    ok             = expectUInt(size.second, 4, "copy size y") && ok;
    ok             = expectBool(copy.isInstalled(), true, "copy installed") && ok;
    ok             = expectBool(copy.isDried(), true, "copy dried") && ok;
    ok             = expectUInt(copy.getPlant(), FLOWERPOT_PLANT_CACTUS_STEMS, "copy plant") && ok;
    ok             = expectUInt(copy.getStage(), FLOWERPOT_STAGE_FIRST_SPROUTS, "copy stage") && ok;
    ok             = expectUInt(copy.getExtraCrystalFeed(), FLOWERPOT_ELEMENT_LIGHT, "copy first crystal") && ok;
    ok             = expectUInt(copy.getCommonCrystalFeed(), FLOWERPOT_ELEMENT_DARK, "copy second crystal") && ok;
    ok             = expectUInt(copy.getPlantTimestamp(), 0x11223344, "copy plant timestamp") && ok;
    ok             = expectUInt(copy.getStageTimestamp(), 0x55667788, "copy stage timestamp") && ok;
    ok             = expectBool(copy.wasExamined(), true, "copy examined") && ok;
    ok             = expectUInt(copy.getStrength(), 55, "copy strength") && ok;
    return ok;
}

} // namespace

auto runItemFlowerpotSelfTests() -> bool
{
    bool ok = true;
    ok      = testConstructorDefaults() && ok;
    ok      = testGrowthFields() && ok;
    ok      = testMappings() && ok;
    ok      = testExdataFieldsAndCleanPot() && ok;
    ok      = testCopyConstructorCopiesFields() && ok;
    return ok;
}
