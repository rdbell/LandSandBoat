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

#include "test_status_effect_model.h"

#include "map/entities/battle_entity.h"
#include "map/status_effect.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <limits>
#include <type_traits>
#include <utility>

using namespace std::chrono_literals;

namespace
{

static_assert(std::is_same_v<std::underlying_type_t<xi::StatusEffect>, std::uint16_t>);
static_assert(std::is_same_v<std::underlying_type_t<xi::StatusEffectFlag>, std::uint32_t>);
static_assert(std::is_same_v<std::underlying_type_t<EffectSourceType>, std::uint8_t>);
static_assert(std::is_same_v<decltype(std::declval<const CStatusEffect&>().GetStatusID()), xi::StatusEffect>);
static_assert(std::is_same_v<decltype(std::declval<const CStatusEffect&>().GetSubID()), std::uint32_t>);
static_assert(std::is_same_v<decltype(std::declval<const CStatusEffect&>().GetSourceType()), std::uint16_t>);
static_assert(std::is_same_v<decltype(std::declval<const CStatusEffect&>().GetSourceTypeParam()), std::uint32_t>);
static_assert(std::is_same_v<decltype(std::declval<const CStatusEffect&>().GetOriginID()), std::uint32_t>);
static_assert(std::is_same_v<decltype(std::declval<const CStatusEffect&>().GetIcon()), std::uint16_t>);
static_assert(std::is_same_v<decltype(std::declval<const CStatusEffect&>().GetPower()), std::uint16_t>);
static_assert(std::is_same_v<decltype(std::declval<const CStatusEffect&>().GetSubPower()), std::uint16_t>);
static_assert(std::is_same_v<decltype(std::declval<const CStatusEffect&>().GetSubIcon()), std::uint16_t>);
static_assert(std::is_same_v<decltype(std::declval<const CStatusEffect&>().GetTier()), std::uint16_t>);
static_assert(std::is_same_v<decltype(std::declval<const CStatusEffect&>().GetEffectType()), std::uint16_t>);
static_assert(std::is_same_v<decltype(std::declval<const CStatusEffect&>().GetEffectSlot()), std::uint8_t>);
static_assert(std::is_same_v<decltype(std::declval<const CStatusEffect&>().GetElapsedTickCount()), int>);
static_assert(sizeof(int) == 4);

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "status-effect model self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

template <typename Actual, typename Expected>
auto expectEqual(const Actual& actual, const Expected& expected, const char* label) -> bool
{
    return expect(actual == expected, label);
}

auto testGoldenValuesAndDefaults() -> bool
{
    bool ok = true;
    ok      = expectEqual(static_cast<std::uint16_t>(xi::StatusEffect::Poison), std::uint16_t{ 3 }, "StatusEffect::Poison") && ok;
    ok      = expectEqual(static_cast<std::uint16_t>(xi::StatusEffect::None), std::uint16_t{ 255 }, "StatusEffect::None") && ok;
    ok      = expectEqual(static_cast<std::uint32_t>(xi::StatusEffectFlag::Dispelable), std::uint32_t{ 0x00000001 }, "Dispelable flag") && ok;
    ok      = expectEqual(static_cast<std::uint32_t>(xi::StatusEffectFlag::Erasable), std::uint32_t{ 0x00000002 }, "Erasable flag") && ok;
    ok      = expectEqual(static_cast<std::uint32_t>(xi::StatusEffectFlag::Attack), std::uint32_t{ 0x00000004 }, "Attack flag") && ok;
    ok      = expectEqual(static_cast<std::uint32_t>(xi::StatusEffectFlag::NoCancel), std::uint32_t{ 0x00800000 }, "NoCancel flag") && ok;
    ok      = expectEqual(static_cast<std::uint32_t>(xi::StatusEffectFlag::AlwaysExpiring), std::uint32_t{ 0x20000000 }, "AlwaysExpiring flag") && ok;
    ok      = expectEqual(static_cast<std::uint32_t>(xi::StatusEffectFlag::OnAttack), std::uint32_t{ 0x40000000 }, "OnAttack flag") && ok;

    ok = expectEqual(static_cast<std::uint8_t>(SOURCE_NONE), std::uint8_t{ 0 }, "SOURCE_NONE") && ok;
    ok = expectEqual(static_cast<std::uint8_t>(SOURCE_EQUIPPED_ITEM), std::uint8_t{ 1 }, "SOURCE_EQUIPPED_ITEM") && ok;
    ok = expectEqual(static_cast<std::uint8_t>(SOURCE_TEMPORARY_ITEM), std::uint8_t{ 2 }, "SOURCE_TEMPORARY_ITEM") && ok;
    ok = expectEqual(static_cast<std::uint8_t>(SOURCE_MOB), std::uint8_t{ 3 }, "SOURCE_MOB") && ok;
    ok = expectEqual(static_cast<std::uint8_t>(SOURCE_FOOD), std::uint8_t{ 4 }, "SOURCE_FOOD") && ok;
    ok = expectEqual(static_cast<std::uint8_t>(SOURCE_CORSAIR_ROLL), std::uint8_t{ 5 }, "SOURCE_CORSAIR_ROLL") && ok;

    CStatusEffect effect(xi::StatusEffect::Poison, 1, 2, 3s, 4s);
    ok = expectEqual(effect.GetSubID(), std::uint32_t{ 0 }, "default sub ID") && ok;
    ok = expectEqual(effect.GetSubPower(), std::uint16_t{ 0 }, "default sub power") && ok;
    ok = expectEqual(effect.GetSubIcon(), std::uint16_t{ 0 }, "default sub icon") && ok;
    ok = expectEqual(effect.GetTier(), std::uint16_t{ 0 }, "default tier") && ok;
    ok = expectEqual(effect.GetEffectFlags(), xi::StatusEffectFlag::None, "default flags") && ok;
    ok = expectEqual(effect.GetSourceType(), std::uint16_t{ SOURCE_NONE }, "default source type") && ok;
    ok = expectEqual(effect.GetSourceTypeParam(), std::uint32_t{ 0 }, "default source parameter") && ok;
    ok = expectEqual(effect.GetOriginID(), std::uint32_t{ 0 }, "default origin") && ok;
    ok = expectEqual(effect.GetEffectType(), std::uint16_t{ 0 }, "default effect type") && ok;
    ok = expectEqual(effect.GetEffectSlot(), std::uint8_t{ 0 }, "default slot") && ok;
    ok = expectEqual(effect.GetElapsedTickCount(), 0, "default tick count") && ok;
    ok = expectEqual(effect.GetStartTime(), timer::time_point{}, "default start time") && ok;
    ok = expect(effect.GetOwner() == nullptr, "default owner") && ok;
    ok = expect(effect.GetName().empty(), "default name") && ok;
    ok = expect(!effect.isDeleted(), "default deleted state") && ok;
    ok = expect(effect.modList().empty(), "default modifier list") && ok;
    return ok;
}

auto testConstructorAndScalarMutators() -> bool
{
    CStatusEffect effect(
        xi::StatusEffect::Poison,
        0x1234,
        0x2345,
        4250ms,
        90500ms,
        0x3456789A,
        0x4567,
        0x5678,
        0x6789,
        xi::StatusEffectFlag::Dispelable | xi::StatusEffectFlag::NoCancel,
        0x789A,
        0x89ABCDEF,
        0x9ABCDEF0,
        0xAB);

    bool ok = true;
    ok      = expectEqual(effect.GetStatusID(), xi::StatusEffect::Poison, "constructor status ID") && ok;
    ok      = expectEqual(effect.GetIcon(), std::uint16_t{ 0x1234 }, "constructor icon") && ok;
    ok      = expectEqual(effect.GetPower(), std::uint16_t{ 0x2345 }, "constructor power") && ok;
    ok      = expectEqual(effect.GetTickTime(), timer::duration{ 4250ms }, "constructor tick") && ok;
    ok      = expectEqual(effect.GetDuration(), timer::duration{ 90500ms }, "constructor duration") && ok;
    ok      = expectEqual(effect.GetSubID(), std::uint32_t{ 0x3456789A }, "constructor sub ID") && ok;
    ok      = expectEqual(effect.GetSubPower(), std::uint16_t{ 0x4567 }, "constructor sub power") && ok;
    ok      = expectEqual(effect.GetSubIcon(), std::uint16_t{ 0x5678 }, "constructor sub icon") && ok;
    ok      = expectEqual(effect.GetTier(), std::uint16_t{ 0x6789 }, "constructor tier") && ok;
    ok      = expectEqual(effect.GetEffectFlags(), xi::StatusEffectFlag::Dispelable | xi::StatusEffectFlag::NoCancel, "constructor flags") && ok;
    ok      = expectEqual(effect.GetSourceType(), std::uint16_t{ 0x789A }, "constructor source type") && ok;
    ok      = expectEqual(effect.GetSourceTypeParam(), std::uint32_t{ 0x89ABCDEF }, "constructor source parameter") && ok;
    ok      = expectEqual(effect.GetOriginID(), std::uint32_t{ 0x9ABCDEF0 }, "constructor origin") && ok;
    ok      = expectEqual(effect.GetEffectSlot(), std::uint8_t{ 0xAB }, "constructor slot") && ok;

    effect.SetEffectType(std::numeric_limits<std::uint16_t>::max());
    effect.SetEffectSlot(std::numeric_limits<std::uint8_t>::max());
    effect.SetSource(std::numeric_limits<std::uint16_t>::max(), std::numeric_limits<std::uint32_t>::max());
    effect.SetOriginID(0xFEDCBA98);
    effect.SetPower(0xAAAA);
    effect.SetSubPower(0xBBBB);
    effect.SetTier(0xCCCC);
    effect.SetDuration(-1500ms);
    effect.SetTickTime(1500ms);

    ok = expectEqual(effect.GetEffectType(), std::numeric_limits<std::uint16_t>::max(), "mutated effect type") && ok;
    ok = expectEqual(effect.GetEffectSlot(), std::numeric_limits<std::uint8_t>::max(), "mutated effect slot") && ok;
    ok = expectEqual(effect.GetSourceType(), std::numeric_limits<std::uint16_t>::max(), "mutated source type") && ok;
    ok = expectEqual(effect.GetSourceTypeParam(), std::numeric_limits<std::uint32_t>::max(), "mutated source parameter") && ok;
    ok = expectEqual(effect.GetOriginID(), std::uint32_t{ 0xFEDCBA98 }, "mutated origin") && ok;
    ok = expectEqual(effect.GetPower(), std::uint16_t{ 0xAAAA }, "mutated power") && ok;
    ok = expectEqual(effect.GetSubPower(), std::uint16_t{ 0xBBBB }, "mutated sub power") && ok;
    ok = expectEqual(effect.GetTier(), std::uint16_t{ 0xCCCC }, "mutated tier") && ok;
    ok = expectEqual(effect.GetDuration(), timer::duration{ -1500ms }, "mutated duration") && ok;
    ok = expectEqual(effect.GetTickTime(), timer::duration{ 1500ms }, "mutated tick") && ok;
    return ok;
}

auto testFlags() -> bool
{
    CStatusEffect effect(xi::StatusEffect::Poison, 1, 2, 3s, 4s, 0, 0, 0, 0, xi::StatusEffectFlag::Dispelable);

    bool ok = true;
    ok      = expect(effect.HasEffectFlag(xi::StatusEffectFlag::Dispelable | xi::StatusEffectFlag::Erasable), "composite flag matches any bit") && ok;
    ok      = expect(!effect.HasEffectFlag(xi::StatusEffectFlag::None), "None flag never matches") && ok;

    effect.AddEffectFlag(xi::StatusEffectFlag::Erasable | xi::StatusEffectFlag::Attack);
    ok = expectEqual(effect.GetEffectFlags(), xi::StatusEffectFlag::Dispelable | xi::StatusEffectFlag::Erasable | xi::StatusEffectFlag::Attack, "flags after add") && ok;

    effect.DelEffectFlag(xi::StatusEffectFlag::Dispelable | xi::StatusEffectFlag::Attack);
    ok = expectEqual(effect.GetEffectFlags(), xi::StatusEffectFlag::Erasable, "flags after delete") && ok;

    effect.SetEffectFlags(xi::StatusEffectFlag::NoCancel | xi::StatusEffectFlag::OnAttack);
    ok = expectEqual(effect.GetEffectFlags(), xi::StatusEffectFlag::NoCancel | xi::StatusEffectFlag::OnAttack, "flags after replace") && ok;
    return ok;
}

auto testTickStartNameAndDeletedState() -> bool
{
    CStatusEffect effect(xi::StatusEffect::Poison, 1, 2, 3s, 4s);
    effect.IncrementElapsedTickCount();
    effect.IncrementElapsedTickCount();

    bool ok = true;
    ok      = expectEqual(effect.GetElapsedTickCount(), 2, "incremented tick count") && ok;

    const auto start = timer::time_point{ 123456789ns };
    effect.SetStartTime(start);
    ok = expectEqual(effect.GetStartTime(), start, "set start time") && ok;
    ok = expectEqual(effect.GetElapsedTickCount(), 0, "set start resets tick count") && ok;

    effect.IncrementElapsedTickCount();
    ok = expectEqual(effect.GetElapsedTickCount(), 1, "post-reset tick count") && ok;

    effect.SetEffectName("poison");
    ok = expectEqual(effect.GetName(), std::string{ "poison" }, "effect name") && ok;
    effect.markDeleted();
    ok = expect(effect.isDeleted(), "marked deleted") && ok;
    return ok;
}

auto testOwnerGatedIcons() -> bool
{
    CStatusEffect effect(xi::StatusEffect::Poison, 10, 2, 3s, 4s, 0, 0, 20);
    effect.SetIcon(11);
    effect.SetSubIcon(21);

    bool ok = true;
    ok      = expectEqual(effect.GetIcon(), std::uint16_t{ 10 }, "nil owner refuses icon") && ok;
    ok      = expectEqual(effect.GetSubIcon(), std::uint16_t{ 20 }, "nil owner refuses sub icon") && ok;

    CBattleEntity owner;
    effect.SetOwner(&owner);
    ok = expect(effect.GetOwner() == &owner, "attached owner") && ok;

    // A default battle entity is TYPE_NONE, so UpdateStatusIcons returns before
    // player-only work while still safely exercising the production call path.
    effect.SetIcon(11);
    effect.SetSubIcon(21);
    ok = expectEqual(effect.GetIcon(), std::uint16_t{ 11 }, "owned icon mutation") && ok;
    ok = expectEqual(effect.GetSubIcon(), std::uint16_t{ 21 }, "owned sub icon mutation") && ok;

    effect.SetOwner(nullptr);
    effect.SetIcon(12);
    ok = expectEqual(effect.GetIcon(), std::uint16_t{ 11 }, "detached owner refuses icon") && ok;
    return ok;
}

auto testModifierAccumulationAndOwnerDeltas() -> bool
{
    CStatusEffect effect(xi::StatusEffect::Poison, 1, 2, 3s, 4s);
    effect.addMod(Mod::FASTCAST, 10);
    effect.addMod(Mod::FASTCAST, -3);
    effect.setMod(Mod::STORETP, -8);

    bool ok = true;
    ok      = expectEqual(effect.modList().size(), std::size_t{ 2 }, "initial unique modifier count") && ok;
    ok      = expectEqual(effect.modList()[0].getModID(), Mod::FASTCAST, "first modifier ID") && ok;
    ok      = expectEqual(effect.modList()[0].getModAmount(), std::int16_t{ 7 }, "duplicate add accumulation") && ok;
    ok      = expectEqual(effect.modList()[1].getModID(), Mod::STORETP, "second modifier ID") && ok;
    ok      = expectEqual(effect.modList()[1].getModAmount(), std::int16_t{ -8 }, "negative modifier amount") && ok;

    CBattleEntity owner;
    effect.SetOwner(&owner);
    ok = expectEqual(owner.getMod(Mod::FASTCAST), std::int16_t{ 0 }, "owner attachment does not apply existing add mod") && ok;
    ok = expectEqual(owner.getMod(Mod::STORETP), std::int16_t{ 0 }, "owner attachment does not apply existing set mod") && ok;

    effect.addMod(Mod::FASTCAST, -12); // Stored value: -5; owner delta: -12.
    effect.setMod(Mod::FASTCAST, 4);   // Existing value delta: +9.
    effect.setMod(Mod::STORETP, -8);   // Existing equal value has zero delta.
    effect.setMod(Mod::HP, std::numeric_limits<std::int16_t>::min());
    effect.addMod(Mod::HP, std::numeric_limits<std::int16_t>::max());

    ok = expectEqual(owner.getMod(Mod::FASTCAST), std::int16_t{ -3 }, "owner add/set accumulated delta") && ok;
    ok = expectEqual(owner.getMod(Mod::STORETP), std::int16_t{ 0 }, "owner equal set zero delta") && ok;
    ok = expectEqual(owner.getMod(Mod::HP), std::int16_t{ -1 }, "owner negative int16 deltas") && ok;
    ok = expectEqual(effect.modList().size(), std::size_t{ 3 }, "final unique modifier count") && ok;
    ok = expectEqual(effect.modList()[0].getModAmount(), std::int16_t{ 4 }, "set replaces accumulated modifier") && ok;
    ok = expectEqual(effect.modList()[1].getModAmount(), std::int16_t{ -8 }, "equal set retains modifier") && ok;
    ok = expectEqual(effect.modList()[2].getModAmount(), std::int16_t{ -1 }, "duplicate negative modifier result") && ok;

    // LSB explicitly exposes a mutable vector reference. Go intentionally
    // returns a defensive value snapshot instead.
    effect.modList()[0].setModAmount(1234);
    ok = expectEqual(effect.modList()[0].getModAmount(), std::int16_t{ 1234 }, "mutable modifier-list reference") && ok;
    return ok;
}

} // namespace

auto runStatusEffectModelSelfTests() -> bool
{
    bool ok = true;
    ok      = testGoldenValuesAndDefaults() && ok;
    ok      = testConstructorAndScalarMutators() && ok;
    ok      = testFlags() && ok;
    ok      = testTickStartNameAndDeletedState() && ok;
    ok      = testOwnerGatedIcons() && ok;
    ok      = testModifierAccumulationAndOwnerDeltas() && ok;
    return ok;
}
