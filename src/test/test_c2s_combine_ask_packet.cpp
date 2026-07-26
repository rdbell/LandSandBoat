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

#include "test_c2s_combine_ask_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <set>
#include <string>
#include <string_view>

#include "common/cbasetypes.h"
#include "map/items.h"
#include "map/packets/c2s/0x096_combine_ask.h"

namespace
{

using CombineAskPacketBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_COMBINE_ASK)>;

const std::set<ITEMID> kValidCrystals = {
    FIRE_CRYSTAL,
    ICE_CRYSTAL,
    WIND_CRYSTAL,
    EARTH_CRYSTAL,
    LIGHTNING_CRYSTAL,
    WATER_CRYSTAL,
    LIGHT_CRYSTAL,
    DARK_CRYSTAL,
    DARK_CLUSTER,
    INFERNO_CRYSTAL,
    GLACIER_CRYSTAL,
    CYCLONE_CRYSTAL,
    TERRA_CRYSTAL,
    PLASMA_CRYSTAL,
    TORRENT_CRYSTAL,
    AURORA_CRYSTAL,
    TWILIGHT_CRYSTAL,
    PYRE_CRYSTAL,
    FROST_CRYSTAL,
    VORTEX_CRYSTAL,
    GEO_CRYSTAL,
    BOLT_CRYSTAL,
    FLUID_CRYSTAL,
    GLIMMER_CRYSTAL,
    SHADOW_CRYSTAL,
};

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s COMBINE_ASK packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s COMBINE_ASK packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectFalse(bool actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "c2s COMBINE_ASK packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s COMBINE_ASK packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(const std::array<std::uint8_t, N>& actual, const std::array<std::uint8_t, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s COMBINE_ASK packet self-test failed: " << label << " got";
        for (const auto value : actual)
        {
            std::cerr << ' ' << static_cast<unsigned>(value);
        }
        std::cerr << " expected";
        for (const auto value : expected)
        {
            std::cerr << ' ' << static_cast<unsigned>(value);
        }
        std::cerr << '\n';
        return false;
    }
    return true;
}

auto expectValid(const PacketValidationResult& result, const std::string& label) -> bool
{
    bool ok = true;
    ok      = expectTrue(result.valid(), label + " valid") && ok;
    ok      = expectEqualString(result.errorString(), "", label + " error string") && ok;
    return ok;
}

auto expectInvalidError(const PacketValidationResult& result, const std::string& expected, const std::string& label) -> bool
{
    bool ok = true;
    ok      = expectFalse(result.valid(), label + " valid") && ok;
    ok      = expectEqualString(result.errorString(), expected, label + " error string") && ok;
    return ok;
}

auto encodedPacketBytes(const GP_CLI_COMMAND_COMBINE_ASK& packet) -> CombineAskPacketBytes
{
    auto bytes = CombineAskPacketBytes{};
    std::memcpy(bytes.data(), &packet, bytes.size());
    return bytes;
}

auto makeCombineAskPacket() -> GP_CLI_COMMAND_COMBINE_ASK
{
    auto packet         = GP_CLI_COMMAND_COMBINE_ASK{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_COMBINE_ASK);
    packet.header.size = sizeof(GP_CLI_COMMAND_COMBINE_ASK) / 4U;
    packet.header.sync = 0xBEEF;
    packet.HashNo      = 0x11;
    packet.padding00   = 0x22;
    packet.Crystal     = FIRE_CRYSTAL;
    packet.CrystalIdx  = 0x33;
    packet.Items       = 8;
    packet.ItemNo[0]   = 0x0102;
    packet.ItemNo[1]   = 0x0304;
    packet.ItemNo[2]   = 0x0506;
    packet.ItemNo[3]   = 0x0708;
    packet.ItemNo[4]   = 0x090A;
    packet.ItemNo[5]   = 0x0B0C;
    packet.ItemNo[6]   = 0x0D0E;
    packet.ItemNo[7]   = 0x0F10;
    packet.TableNo[0]  = 0x21;
    packet.TableNo[1]  = 0x22;
    packet.TableNo[2]  = 0x23;
    packet.TableNo[3]  = 0x24;
    packet.TableNo[4]  = 0x25;
    packet.TableNo[5]  = 0x26;
    packet.TableNo[6]  = 0x27;
    packet.TableNo[7]  = 0x28;
    packet.padding01   = 0x4455;
    return packet;
}

auto validateCombineAskPure(ITEMID crystal, std::uint8_t items) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.oneOf("Crystal", crystal, kValidCrystals)
        .range("Items", items, 1, 8);
    return validator;
}

auto testCombineAskLayoutMetadataAndPayload() -> bool
{
    const auto packet = makeCombineAskPacket();
    bool       ok     = true;

    ok = expectEqualString(GP_CLI_COMMAND_COMBINE_ASK::name, "GP_CLI_COMMAND_COMBINE_ASK", "COMBINE_ASK name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_COMBINE_ASK::packetId), 0x096, "COMBINE_ASK packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_COMBINE_ASK), 36, "COMBINE_ASK sizeof") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_COMBINE_ASK, HashNo), 4, "COMBINE_ASK HashNo offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_COMBINE_ASK, padding00), 5, "COMBINE_ASK padding00 offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_COMBINE_ASK, Crystal), 6, "COMBINE_ASK Crystal offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_COMBINE_ASK, CrystalIdx), 8, "COMBINE_ASK CrystalIdx offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_COMBINE_ASK, Items), 9, "COMBINE_ASK Items offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_COMBINE_ASK, ItemNo), 10, "COMBINE_ASK ItemNo offset") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_COMBINE_ASK::ItemNo), 16, "COMBINE_ASK ItemNo bytes") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_COMBINE_ASK, TableNo), 26, "COMBINE_ASK TableNo offset") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_COMBINE_ASK::TableNo), 8, "COMBINE_ASK TableNo bytes") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_COMBINE_ASK, padding01), 34, "COMBINE_ASK padding01 offset") && ok;
    ok = expectBytes(encodedPacketBytes(packet),
                     CombineAskPacketBytes{
                         0x96, 0x12, 0xEF, 0xBE,
                         0x11, 0x22, 0x00, 0x10,
                         0x33, 0x08,
                         0x02, 0x01, 0x04, 0x03,
                         0x06, 0x05, 0x08, 0x07,
                         0x0A, 0x09, 0x0C, 0x0B,
                         0x0E, 0x0D, 0x10, 0x0F,
                         0x21, 0x22, 0x23, 0x24,
                         0x25, 0x26, 0x27, 0x28,
                         0x55, 0x44 },
                     "COMBINE_ASK encoded packet") &&
         ok;
    ok = expectEqualInt(packet.HashNo, 0x11, "COMBINE_ASK HashNo") && ok;
    ok = expectEqualInt(packet.padding00, 0x22, "COMBINE_ASK padding00") && ok;
    ok = expectEqualInt(packet.Crystal, FIRE_CRYSTAL, "COMBINE_ASK Crystal") && ok;
    ok = expectEqualInt(packet.CrystalIdx, 0x33, "COMBINE_ASK CrystalIdx") && ok;
    ok = expectEqualInt(packet.Items, 8, "COMBINE_ASK Items") && ok;
    ok = expectEqualInt(packet.ItemNo[0], 0x0102, "COMBINE_ASK ItemNo 0") && ok;
    ok = expectEqualInt(packet.ItemNo[7], 0x0F10, "COMBINE_ASK ItemNo 7") && ok;
    ok = expectEqualInt(packet.TableNo[0], 0x21, "COMBINE_ASK TableNo 0") && ok;
    ok = expectEqualInt(packet.TableNo[7], 0x28, "COMBINE_ASK TableNo 7") && ok;
    ok = expectEqualInt(packet.padding01, 0x4455, "COMBINE_ASK padding01") && ok;
    return ok;
}

auto testCombineAskCrystalConstants() -> bool
{
    bool ok = true;
    ok      = expectEqualInt(FIRE_CRYSTAL, 4096, "FIRE_CRYSTAL") && ok;
    ok      = expectEqualInt(ICE_CRYSTAL, 4097, "ICE_CRYSTAL") && ok;
    ok      = expectEqualInt(WIND_CRYSTAL, 4098, "WIND_CRYSTAL") && ok;
    ok      = expectEqualInt(EARTH_CRYSTAL, 4099, "EARTH_CRYSTAL") && ok;
    ok      = expectEqualInt(LIGHTNING_CRYSTAL, 4100, "LIGHTNING_CRYSTAL") && ok;
    ok      = expectEqualInt(WATER_CRYSTAL, 4101, "WATER_CRYSTAL") && ok;
    ok      = expectEqualInt(LIGHT_CRYSTAL, 4102, "LIGHT_CRYSTAL") && ok;
    ok      = expectEqualInt(DARK_CRYSTAL, 4103, "DARK_CRYSTAL") && ok;
    ok      = expectEqualInt(DARK_CLUSTER, 4111, "DARK_CLUSTER") && ok;
    ok      = expectEqualInt(INFERNO_CRYSTAL, 4238, "INFERNO_CRYSTAL") && ok;
    ok      = expectEqualInt(GLACIER_CRYSTAL, 4239, "GLACIER_CRYSTAL") && ok;
    ok      = expectEqualInt(CYCLONE_CRYSTAL, 4240, "CYCLONE_CRYSTAL") && ok;
    ok      = expectEqualInt(TERRA_CRYSTAL, 4241, "TERRA_CRYSTAL") && ok;
    ok      = expectEqualInt(PLASMA_CRYSTAL, 4242, "PLASMA_CRYSTAL") && ok;
    ok      = expectEqualInt(TORRENT_CRYSTAL, 4243, "TORRENT_CRYSTAL") && ok;
    ok      = expectEqualInt(AURORA_CRYSTAL, 4244, "AURORA_CRYSTAL") && ok;
    ok      = expectEqualInt(TWILIGHT_CRYSTAL, 4245, "TWILIGHT_CRYSTAL") && ok;
    ok      = expectEqualInt(PYRE_CRYSTAL, 6506, "PYRE_CRYSTAL") && ok;
    ok      = expectEqualInt(FROST_CRYSTAL, 6507, "FROST_CRYSTAL") && ok;
    ok      = expectEqualInt(VORTEX_CRYSTAL, 6508, "VORTEX_CRYSTAL") && ok;
    ok      = expectEqualInt(GEO_CRYSTAL, 6509, "GEO_CRYSTAL") && ok;
    ok      = expectEqualInt(BOLT_CRYSTAL, 6510, "BOLT_CRYSTAL") && ok;
    ok      = expectEqualInt(FLUID_CRYSTAL, 6511, "FLUID_CRYSTAL") && ok;
    ok      = expectEqualInt(GLIMMER_CRYSTAL, 6512, "GLIMMER_CRYSTAL") && ok;
    ok      = expectEqualInt(SHADOW_CRYSTAL, 6513, "SHADOW_CRYSTAL") && ok;
    return ok;
}

auto testCombineAskPureValidationFacts() -> bool
{
    bool ok = true;
    ok      = expectValid(validateCombineAskPure(FIRE_CRYSTAL, 1), "COMBINE_ASK fire crystal validation") && ok;
    ok      = expectValid(validateCombineAskPure(DARK_CLUSTER, 8), "COMBINE_ASK dark cluster validation") && ok;
    ok      = expectValid(validateCombineAskPure(TWILIGHT_CRYSTAL, 1), "COMBINE_ASK twilight crystal validation") && ok;
    ok      = expectValid(validateCombineAskPure(SHADOW_CRYSTAL, 1), "COMBINE_ASK shadow crystal validation") && ok;
    ok      = expectInvalidError(validateCombineAskPure(static_cast<ITEMID>(4095), 1), "Crystal value 4095 is not allowed.", "COMBINE_ASK invalid crystal validation") && ok;
    ok      = expectInvalidError(validateCombineAskPure(FIRE_CRYSTAL, 0), "Items out of range: 0 not in [1, 8]", "COMBINE_ASK zero items validation") && ok;
    ok      = expectInvalidError(validateCombineAskPure(FIRE_CRYSTAL, 9), "Items out of range: 9 not in [1, 8]", "COMBINE_ASK high items validation") && ok;
    ok      = expectInvalidError(validateCombineAskPure(static_cast<ITEMID>(4095), 0), "Crystal value 4095 is not allowed.", "COMBINE_ASK validation order") && ok;
    return ok;
}

auto testCombineAskIngredientPlan() -> bool
{
    using namespace combineaskhelpers;

    constexpr auto available = [](const uint16 itemID, const uint8 inventorySlot, const uint32 quantity) {
        return IngredientFact{
            .requestedID     = itemID,
            .inventorySlot   = inventorySlot,
            .present         = true,
            .inventoryItemID = itemID,
            .quantity        = quantity,
        };
    };

    const auto accepted = BuildIngredientPlan(std::array<IngredientFact, 8>{
                                                  available(100, 4, 1),
                                                  available(200, 9, 2),
                                                  available(300, 8, 1),
                                              },
                                              3);
    const auto unavailable = BuildIngredientPlan(std::array<IngredientFact, 8>{
                                                     available(100, 4, 1),
                                                     IngredientFact{ .requestedID = 200, .inventorySlot = 5 },
                                                     IngredientFact{ .requestedID = 300, .inventorySlot = 6, .present = true, .inventoryItemID = 301, .quantity = 1 },
                                                     IngredientFact{ .requestedID = 400, .inventorySlot = 7, .present = true, .inventoryItemID = 400, .quantity = 1, .busy = true },
                                                     IngredientFact{ .requestedID = 500, .inventorySlot = 8, .present = true, .inventoryItemID = 500, .quantity = 1, .locked = true },
                                                     available(100, 4, 1),
                                                 },
                                                 6);
    const auto bounded = BuildIngredientPlan(std::array<IngredientFact, 8>{ available(100, 4, 1), available(200, 5, 1) }, 1);

    bool ok = true;
    ok      = expectTrue(accepted.accepted[0] && accepted.accepted[1] && accepted.accepted[2], "COMBINE_ASK accepted ingredient order") && ok;
    ok      = expectTrue(unavailable.accepted[0], "COMBINE_ASK keeps available ingredient") && ok;
    ok      = expectFalse(unavailable.accepted[1], "COMBINE_ASK skips missing ingredient") && ok;
    ok      = expectFalse(unavailable.accepted[2], "COMBINE_ASK skips mismatched ingredient") && ok;
    ok      = expectFalse(unavailable.accepted[3], "COMBINE_ASK skips busy ingredient") && ok;
    ok      = expectFalse(unavailable.accepted[4], "COMBINE_ASK skips locked ingredient") && ok;
    ok      = expectFalse(unavailable.accepted[5], "COMBINE_ASK skips duplicate slot beyond quantity") && ok;
    ok      = expectTrue(bounded.accepted[0] && !bounded.accepted[1], "COMBINE_ASK respects ingredient count") && ok;
    return ok;
}

auto testCombineAskSynthCooldown() -> bool
{
    using namespace std::chrono_literals;
    using combineaskhelpers::shouldWaitForSynth;

    const auto last = timer::time_point{};
    bool ok         = true;
    ok              = expectTrue(shouldWaitForSynth(last, last + 14s + 999ms), "COMBINE_ASK synth cooldown before boundary") && ok;
    ok              = expectFalse(shouldWaitForSynth(last, last + 15s), "COMBINE_ASK synth cooldown exact boundary") && ok;
    ok              = expectFalse(shouldWaitForSynth(last, last + 16s), "COMBINE_ASK synth cooldown after boundary") && ok;
    return ok;
}

auto testCombineAskMatchedTradePending() -> bool
{
    using combineaskhelpers::shouldClearMatchedTradePending;

    bool ok = true;
    ok      = expectTrue(shouldClearMatchedTradePending(true, 42, 42), "COMBINE_ASK matched pending trade") && ok;
    ok      = expectFalse(shouldClearMatchedTradePending(false, 42, 42), "COMBINE_ASK missing pending-trade target") && ok;
    ok      = expectFalse(shouldClearMatchedTradePending(true, 42, 43), "COMBINE_ASK mismatched pending-trade ID") && ok;
    return ok;
}

auto testCombineAskPeerTradeCancel() -> bool
{
    using combineaskhelpers::shouldCancelPeerTrade;

    return expectTrue(shouldCancelPeerTrade(true), "COMBINE_ASK cancels a resolved peer trade") &&
           expectFalse(shouldCancelPeerTrade(false), "COMBINE_ASK does not cancel a missing peer trade");
}

auto testCombineAskCrystalAvailability() -> bool
{
    using combineaskhelpers::ClassifyCrystal;
    using combineaskhelpers::CrystalAvailability;
    using combineaskhelpers::CrystalFact;

    constexpr auto requested = static_cast<uint16>(FIRE_CRYSTAL);
    bool ok = true;
    ok      = expectTrue(ClassifyCrystal(requested, {}) == CrystalAvailability::Invalid, "COMBINE_ASK missing crystal") && ok;
    ok      = expectTrue(ClassifyCrystal(requested, CrystalFact{ .present = true, .inventoryItemID = static_cast<uint16>(EARTH_CRYSTAL), .quantity = 1 }) == CrystalAvailability::Invalid, "COMBINE_ASK mismatched crystal") && ok;
    ok      = expectTrue(ClassifyCrystal(requested, CrystalFact{ .present = true, .inventoryItemID = requested }) == CrystalAvailability::Invalid, "COMBINE_ASK depleted crystal") && ok;
    ok      = expectTrue(ClassifyCrystal(requested, CrystalFact{ .present = true, .inventoryItemID = requested, .quantity = 1, .busy = true }) == CrystalAvailability::Busy, "COMBINE_ASK busy crystal") && ok;
    ok      = expectTrue(ClassifyCrystal(requested, CrystalFact{ .present = true, .inventoryItemID = requested, .quantity = 1, .locked = true }) == CrystalAvailability::Busy, "COMBINE_ASK locked crystal") && ok;
    ok      = expectTrue(ClassifyCrystal(requested, CrystalFact{ .present = true, .inventoryItemID = requested, .quantity = 1 }) == CrystalAvailability::Usable, "COMBINE_ASK usable crystal") && ok;
    return ok;
}

} // namespace

auto runC2SCombineAskPacketSelfTests() -> bool
{
    return testCombineAskLayoutMetadataAndPayload() &&
           testCombineAskCrystalConstants() &&
           testCombineAskPureValidationFacts() &&
           testCombineAskIngredientPlan() &&
           testCombineAskSynthCooldown() &&
           testCombineAskMatchedTradePending() &&
           testCombineAskPeerTradeCancel() &&
           testCombineAskCrystalAvailability();
}
