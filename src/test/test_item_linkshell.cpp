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

#include "test_item_linkshell.h"

#include "map/items/item_linkshell.h"

#include <array>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

namespace
{

auto expectBool(bool actual, bool expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "item linkshell self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectString(const std::string& actual, const std::string& expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "item linkshell self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectUInt(std::uint64_t actual, std::uint64_t expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "item linkshell self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(const uint8* actual, const std::array<uint8, N>& expected, const char* label) -> bool
{
    if (std::memcmp(actual, expected.data(), expected.size()) == 0)
    {
        return true;
    }

    std::cerr << "item linkshell self-test failed: " << label << " got";
    for (std::size_t idx = 0; idx < expected.size(); ++idx)
    {
        std::cerr << ' ' << static_cast<unsigned>(actual[idx]);
    }
    std::cerr << " expected";
    for (const auto value : expected)
    {
        std::cerr << ' ' << static_cast<unsigned>(value);
    }
    std::cerr << '\n';
    return false;
}

auto testConstructorDefaults() -> bool
{
    CItemLinkshell item(0x1234);
    const auto     color = item.GetLSColor();

    bool ok = true;
    ok      = expectUInt(item.getID(), 0x1234, "id") && ok;
    ok      = expectBool(item.isType(ITEM_LINKSHELL), true, "linkshell type flag") && ok;
    ok      = expectBool(item.isType(ITEM_GENERAL), false, "general type flag") && ok;
    ok      = expectUInt(item.getStackSize(), 0, "default stack size") && ok;
    ok      = expectUInt(item.getQuantity(), 0, "default quantity") && ok;
    ok      = expectUInt(item.getBasePrice(), 0, "default base price") && ok;
    ok      = expectUInt(item.GetLSID(), 0, "default linkshell id") && ok;
    ok      = expectUInt(item.GetLSType(), LSTYPE_NEW_LINKSHELL, "default linkshell type") && ok;
    ok      = expectUInt(item.GetLSRawColor(), 0, "default raw color") && ok;
    ok      = expectUInt(color.R, 0, "default color r") && ok;
    ok      = expectUInt(color.G, 0, "default color g") && ok;
    ok      = expectUInt(color.B, 0, "default color b") && ok;
    ok      = expectUInt(color.A, 0, "default color a") && ok;
    ok      = expectString(item.getSignature(), "", "default signature") && ok;

    for (auto idx = 0U; idx < CItem::extra_size; ++idx)
    {
        ok = expectUInt(item.m_extra[idx], 0, "default exdata byte") && ok;
    }
    return ok;
}

auto testLinkshellExdataFields() -> bool
{
    CItemLinkshell item(0x2000);

    bool ok = true;
    item.SetLSID(0x11223344);
    item.SetLSType(LSTYPE_PEARLSACK);
    item.SetLSColor(0xF321);

    const auto color = item.GetLSColor();
    ok               = expectUInt(item.GetLSID(), 0x11223344, "linkshell id") && ok;
    ok               = expectUInt(item.GetLSType(), LSTYPE_PEARLSACK, "linkshell type") && ok;
    ok               = expectUInt(item.GetLSRawColor(), 0xF321, "raw color") && ok;
    ok               = expectUInt(color.R, 1, "color r") && ok;
    ok               = expectUInt(color.G, 2, "color g") && ok;
    ok               = expectUInt(color.B, 3, "color b") && ok;
    ok               = expectUInt(color.A, 15, "color a") && ok;
    ok               = expectBytes(item.m_extra, std::array<uint8, 9>{ 0x44, 0x33, 0x22, 0x11, 0x00, 0x00, 0x21, 0xF3, 0x02 }, "raw exdata prefix") && ok;
    return ok;
}

auto testSignature() -> bool
{
    CItemLinkshell item(0x2001);

    bool ok = true;
    item.setSignature("OmegaXI2026");
    ok = expectString(item.getSignature(), "OmegaXI2026", "signature") && ok;
    ok = expectBytes(item.m_extra + 9, std::array<uint8, 15>{ 0xA4, 0xD1, 0x47, 0x07, 0x28, 0xF7, 0xD7, 0x7E, 0xFF }, "encoded mixed signature") && ok;

    item.setSignature("AB_cd!");
    ok = expectString(item.getSignature(), "A", "invalid signature truncates") && ok;
    ok = expectBytes(item.m_extra + 9, std::array<uint8, 15>{ 0x6D, 0xC0, 0x03, 0x10, 0x0F }, "encoded invalid signature") && ok;

    item.setSignature("");
    ok = expectString(item.getSignature(), "", "empty signature") && ok;
    ok = expectBytes(item.m_extra + 9, std::array<uint8, 15>{ 0xFC }, "encoded empty signature") && ok;
    return ok;
}

auto testQuantityClampUsesCurrentStackSize() -> bool
{
    CItemLinkshell item(0x2002);

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
    CItemLinkshell original(0x2222);
    original.setStackSize(99);
    original.setQuantity(42);
    original.setBasePrice(777);
    original.SetLSID(0x11223344);
    original.m_extra[4] = 0xCD;
    original.m_extra[5] = 0xAB;
    original.SetLSColor(0xF321);
    original.SetLSType(LSTYPE_LINKPEARL);
    original.setSignature("OmegaXI2026");

    CItemLinkshell copy(original);
    original.setQuantity(1);
    original.setBasePrice(1);
    original.setStackSize(1);
    original.SetLSID(1);
    original.m_extra[4] = 0;
    original.m_extra[5] = 0;
    original.SetLSColor(1);
    original.SetLSType(LSTYPE_BROKEN);
    original.setSignature("Changed");

    bool ok = true;
    ok      = expectUInt(copy.getID(), 0x2222, "copy id") && ok;
    ok      = expectBool(copy.isType(ITEM_LINKSHELL), true, "copy linkshell type flag") && ok;
    ok      = expectUInt(copy.getQuantity(), 42, "copy quantity") && ok;
    ok      = expectUInt(copy.getBasePrice(), 777, "copy base price") && ok;
    ok      = expectUInt(copy.getStackSize(), 99, "copy stack size") && ok;
    ok      = expectUInt(copy.GetLSID(), 0x11223344, "copy linkshell id") && ok;
    ok      = expectUInt(copy.m_extra[4], 0xCD, "copy group key low byte") && ok;
    ok      = expectUInt(copy.m_extra[5], 0xAB, "copy group key high byte") && ok;
    ok      = expectUInt(copy.GetLSRawColor(), 0xF321, "copy raw color") && ok;
    ok      = expectUInt(copy.GetLSType(), LSTYPE_LINKPEARL, "copy linkshell type") && ok;
    ok      = expectString(copy.getSignature(), "OmegaXI2026", "copy signature") && ok;
    return ok;
}

} // namespace

auto runItemLinkshellSelfTests() -> bool
{
    bool ok = true;
    ok      = testConstructorDefaults() && ok;
    ok      = testLinkshellExdataFields() && ok;
    ok      = testSignature() && ok;
    ok      = testQuantityClampUsesCurrentStackSize() && ok;
    ok      = testCopyConstructorCopiesFields() && ok;
    return ok;
}
