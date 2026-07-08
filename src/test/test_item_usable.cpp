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

#include "test_item_usable.h"

#include "common/earth_time.h"
#include "common/timer.h"
#include "map/items/item_usable.h"

#include <chrono>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

namespace
{

using namespace std::chrono_literals;

auto expectBool(bool actual, bool expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "item usable self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectUInt(std::uint64_t actual, std::uint64_t expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "item usable self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectDurationSeconds(timer::duration actual, std::int64_t expected, const char* label) -> bool
{
    return expectUInt(timer::count_seconds(actual), expected, label);
}

auto expectTimePoint(timer::time_point actual, timer::time_point expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "item usable self-test failed: " << label << " time points differ\n";
        return false;
    }
    return true;
}

auto exdataU32(const CItemUsable& item, const std::size_t offset) -> std::uint32_t
{
    std::uint32_t value = 0;
    std::memcpy(&value, item.m_extra + offset, sizeof(value));
    return value;
}

auto testConstructorDefaults() -> bool
{
    CItemUsable item(0x1234);

    bool ok = true;
    ok      = expectUInt(item.getID(), 0x1234, "id") && ok;
    ok      = expectBool(item.isType(ITEM_USABLE), true, "usable type flag") && ok;
    ok      = expectBool(item.isType(ITEM_GENERAL), false, "general type flag") && ok;
    ok      = expectUInt(item.getStackSize(), 0, "default stack size") && ok;
    ok      = expectUInt(item.getQuantity(), 0, "default quantity") && ok;
    ok      = expectUInt(item.getBasePrice(), 0, "default base price") && ok;
    ok      = expectDurationSeconds(item.getUseDelay(), 0, "default use delay") && ok;
    ok      = expectDurationSeconds(item.getReuseDelay(), 0, "default reuse delay") && ok;
    ok      = expectDurationSeconds(item.getAnimationTime(), 0, "default animation time") && ok;
    ok      = expectDurationSeconds(item.getActivationTime(), 0, "default activation time") && ok;
    ok      = expectUInt(item.getMaxCharges(), 0, "default max charges") && ok;
    ok      = expectUInt(item.getCurrentCharges(), 0, "default current charges") && ok;
    ok      = expectUInt(static_cast<uint16>(item.getAnimationID()), 0, "default animation") && ok;
    ok      = expectUInt(item.getValidTarget(), 0, "default valid target") && ok;
    ok      = expectUInt(item.getAoE(), 0, "default aoe") && ok;
    ok      = expectTimePoint(item.getLastUseTime(), timer::time_point{}, "default last use time") && ok;
    ok      = expectTimePoint(item.getNextUseTime(), timer::time_point{}, "default next use time") && ok;

    for (auto idx = 0U; idx < CItem::extra_size; ++idx)
    {
        ok = expectUInt(item.m_extra[idx], 0, "default exdata byte") && ok;
    }
    return ok;
}

auto testUsableFields() -> bool
{
    CItemUsable item(0x2000);

    bool ok = true;
    item.setUseDelay(7s);
    item.setReuseDelay(30s);
    item.setMaxCharges(3);
    item.setCurrentCharges(2);
    ok = expectUInt(item.getCurrentCharges(), 2, "current charges below cap") && ok;
    item.setCurrentCharges(9);
    ok = expectUInt(item.getCurrentCharges(), 3, "current charges cap") && ok;
    item.setMaxCharges(1);
    ok = expectUInt(item.getCurrentCharges(), 3, "current charges after lowering max") && ok;

    item.setAnimationID(static_cast<uint16>(ActionAnimation::Teleport));
    item.setAnimationTime(4s);
    item.setActivationTime(5s);
    item.setValidTarget(0x1234);
    item.setAoE(0x4567);

    ok = expectDurationSeconds(item.getUseDelay(), 7, "use delay") && ok;
    ok = expectDurationSeconds(item.getReuseDelay(), 30, "reuse delay") && ok;
    ok = expectUInt(item.getMaxCharges(), 1, "max charges") && ok;
    ok = expectUInt(static_cast<uint16>(item.getAnimationID()), static_cast<uint16>(ActionAnimation::Teleport), "animation") && ok;
    ok = expectDurationSeconds(item.getAnimationTime(), 4, "animation time") && ok;
    ok = expectDurationSeconds(item.getActivationTime(), 5, "activation time") && ok;
    ok = expectUInt(item.getValidTarget(), 0x1234, "valid target") && ok;
    ok = expectUInt(item.getAoE(), 0x4567, "aoe") && ok;
    ok = expectUInt(item.m_extra[1], 3, "raw remaining charges") && ok;
    return ok;
}

auto testUseTimes() -> bool
{
    CItemUsable item(0x2001);

    bool       ok       = true;
    const auto utc      = earth_time::vanadiel_epoch + 12345s + 500ms;
    const auto lastUse  = timer::from_utc(utc);
    const auto expected = earth_time::vanadiel_timestamp(timer::to_utc(lastUse));

    item.setReuseDelay(15s);
    item.setLastUseTime(lastUse);
    ok = expectTimePoint(item.getLastUseTime(), lastUse, "last use time") && ok;
    ok = expectDurationSeconds(item.getNextUseTime() - item.getLastUseTime(), 15, "next use delta") && ok;
    ok = expectUInt(exdataU32(item, 4), expected, "time value 1 exdata") && ok;

    const auto now = timer::now();
    item.setAssignTime(now - 2s);
    item.setUseDelay(12s);
    item.setLastUseTime(now - 3s);
    item.setReuseDelay(30s);
    const auto reuse = timer::count_seconds(item.getReuseTime());
    if (reuse < 20 || reuse > 27)
    {
        std::cerr << "item usable self-test failed: reuse time got " << reuse << " expected 20..27\n";
        ok = false;
    }

    item.setAssignTime(now - 60s);
    item.setUseDelay(1s);
    item.setLastUseTime(now - 60s);
    item.setReuseDelay(1s);
    ok = expectDurationSeconds(item.getReuseTime(), 0, "expired reuse time") && ok;
    return ok;
}

auto testQuantityClampUsesCurrentStackSize() -> bool
{
    CItemUsable item(0x2002);

    bool ok = true;
    item.setQuantity(1);
    ok = expectUInt(item.getQuantity(), 0, "quantity before stack size") && ok;

    item.setStackSize(12);
    item.setQuantity(12);
    ok = expectUInt(item.getQuantity(), 12, "quantity at stack boundary") && ok;

    item.setQuantity(13);
    ok = expectUInt(item.getQuantity(), 12, "quantity clamped to stack") && ok;

    item.setStackSize(1);
    ok = expectUInt(item.getQuantity(), 12, "quantity after shrinking stack size") && ok;
    return ok;
}

auto testCopyConstructorCopiesFields() -> bool
{
    const auto utc     = earth_time::vanadiel_epoch + 12345s + 500ms;
    const auto lastUse = timer::from_utc(utc);
    const auto assign  = timer::now() + 100s;

    CItemUsable original(0x2222);
    original.setStackSize(99);
    original.setQuantity(42);
    original.setBasePrice(777);
    original.setUseDelay(7s);
    original.setMaxCharges(5);
    original.setCurrentCharges(4);
    original.setAnimationID(static_cast<uint16>(ActionAnimation::Teleport));
    original.setAnimationTime(4s);
    original.setActivationTime(5s);
    original.setValidTarget(0x1234);
    original.setReuseDelay(30s);
    original.setAssignTime(assign);
    original.setLastUseTime(lastUse);
    original.setAoE(0x4567);

    CItemUsable copy(original);
    original.setQuantity(1);
    original.setBasePrice(1);
    original.setStackSize(1);
    original.setUseDelay(1s);
    original.setMaxCharges(1);
    original.setCurrentCharges(1);
    original.setAnimationID(1);
    original.setAnimationTime(1s);
    original.setActivationTime(1s);
    original.setValidTarget(1);
    original.setReuseDelay(1s);
    original.setAssignTime(timer::time_point{});
    original.setLastUseTime(timer::time_point{});
    original.setAoE(1);

    bool ok = true;
    ok      = expectUInt(copy.getID(), 0x2222, "copy id") && ok;
    ok      = expectBool(copy.isType(ITEM_USABLE), true, "copy usable type flag") && ok;
    ok      = expectUInt(copy.getQuantity(), 42, "copy quantity") && ok;
    ok      = expectUInt(copy.getBasePrice(), 777, "copy base price") && ok;
    ok      = expectUInt(copy.getStackSize(), 99, "copy stack size") && ok;
    ok      = expectDurationSeconds(copy.getUseDelay(), 7, "copy use delay") && ok;
    ok      = expectUInt(copy.getMaxCharges(), 5, "copy max charges") && ok;
    ok      = expectUInt(copy.getCurrentCharges(), 4, "copy current charges") && ok;
    ok      = expectUInt(static_cast<uint16>(copy.getAnimationID()), static_cast<uint16>(ActionAnimation::Teleport), "copy animation") && ok;
    ok      = expectDurationSeconds(copy.getAnimationTime(), 4, "copy animation time") && ok;
    ok      = expectDurationSeconds(copy.getActivationTime(), 5, "copy activation time") && ok;
    ok      = expectUInt(copy.getValidTarget(), 0x1234, "copy valid target") && ok;
    ok      = expectDurationSeconds(copy.getReuseDelay(), 30, "copy reuse delay") && ok;
    ok      = expectTimePoint(copy.getLastUseTime(), lastUse, "copy last use time") && ok;
    ok      = expectDurationSeconds(copy.getNextUseTime() - copy.getLastUseTime(), 30, "copy next use delta") && ok;
    ok      = expectUInt(copy.getAoE(), 0x4567, "copy aoe") && ok;
    ok      = expectUInt(exdataU32(copy, 4), earth_time::vanadiel_timestamp(timer::to_utc(lastUse)), "copy time value 1") && ok;
    return ok;
}

} // namespace

auto runItemUsableSelfTests() -> bool
{
    bool ok = true;
    ok      = testConstructorDefaults() && ok;
    ok      = testUsableFields() && ok;
    ok      = testUseTimes() && ok;
    ok      = testQuantityClampUsesCurrentStackSize() && ok;
    ok      = testCopyConstructorCopiesFields() && ok;
    return ok;
}
