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

#include "test_map_fourcc_exdata_enums.h"

#include <cstdint>
#include <iostream>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include <magic_enum/magic_enum.hpp>

#include "map/enums/exdata.h"
#include "map/enums/four_cc.h"

static_assert(std::is_same_v<std::underlying_type_t<FourCC>, std::uint32_t>);
static_assert(std::is_same_v<std::underlying_type_t<Exdata::AugmentKindFlags>, std::uint8_t>);
static_assert(std::is_same_v<std::underlying_type_t<Exdata::AugmentSubKindFlags>, std::uint8_t>);
static_assert(std::is_same_v<std::underlying_type_t<Exdata::Type>, std::uint8_t>);
static_assert(magic_enum::customize::enum_range<Exdata::AugmentKindFlags>::is_flags);
static_assert(magic_enum::customize::enum_range<Exdata::AugmentSubKindFlags>::is_flags);

namespace
{

struct IntCase
{
    std::uint64_t actual;
    std::uint64_t expected;
    std::string   label;
};

struct FourCCCase
{
    FourCC      actual;
    std::uint32_t expected;
    std::string tag;
    std::string label;
};

template <typename T>
auto enumValue(T value) -> std::uint64_t
{
    return static_cast<std::uint64_t>(value);
}

auto fourCCTag(FourCC value) -> std::string
{
    const auto raw = static_cast<std::uint32_t>(value);
    char       tag[5]{
        static_cast<char>(raw & 0xFFU),
        static_cast<char>((raw >> 8U) & 0xFFU),
        static_cast<char>((raw >> 16U) & 0xFFU),
        static_cast<char>((raw >> 24U) & 0xFFU),
        '\0',
    };
    return tag;
}

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "map fourcc/exdata enum self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "map fourcc/exdata enum self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectAll(const std::vector<IntCase>& tests) -> bool
{
    bool ok = true;
    for (const auto& test : tests)
    {
        ok = expectEqualInt(test.actual, test.expected, test.label) && ok;
    }
    return ok;
}

auto testFourCCValuesAndTags() -> bool
{
    const auto tests = std::vector<FourCCCase>{
        { FourCC::BasicAttack, 0x306B7461, "atk0", "FourCC::BasicAttack" },
        { FourCC::SkillUse, 0x65746163, "cate", "FourCC::SkillUse" },
        { FourCC::SkillInterrupt, 0x65747073, "spte", "FourCC::SkillInterrupt" },
        { FourCC::ItemUse, 0x74696163, "cait", "FourCC::ItemUse" },
        { FourCC::ItemInterrupt, 0x74697073, "spit", "FourCC::ItemInterrupt" },
        { FourCC::RangedStart, 0x676C6163, "calg", "FourCC::RangedStart" },
        { FourCC::RangedInterrupt, 0x676C7073, "splg", "FourCC::RangedInterrupt" },
        { FourCC::RangedFinish, 0x676C6873, "shlg", "FourCC::RangedFinish" },
        { FourCC::WhiteMagicCast, 0x68776163, "cawh", "FourCC::WhiteMagicCast" },
        { FourCC::BlackMagicCast, 0x6B626163, "cabk", "FourCC::BlackMagicCast" },
        { FourCC::BlueMagicCast, 0x6C626163, "cabl", "FourCC::BlueMagicCast" },
        { FourCC::SongMagicCast, 0x6F736163, "caso", "FourCC::SongMagicCast" },
        { FourCC::NinjutsuMagicCast, 0x6A6E6163, "canj", "FourCC::NinjutsuMagicCast" },
        { FourCC::SummonMagicCast, 0x6D736163, "casm", "FourCC::SummonMagicCast" },
        { FourCC::GeomancyMagicCast, 0x65676163, "cage", "FourCC::GeomancyMagicCast" },
        { FourCC::TrustMagicCast, 0x61666163, "cafa", "FourCC::TrustMagicCast" },
        { FourCC::WhiteMagicInterrupt, 0x68777073, "spwh", "FourCC::WhiteMagicInterrupt" },
        { FourCC::BlackMagicInterrupt, 0x6B627073, "spbk", "FourCC::BlackMagicInterrupt" },
        { FourCC::BlueMagicInterrupt, 0x6C627073, "spbl", "FourCC::BlueMagicInterrupt" },
        { FourCC::SongMagicInterrupt, 0x6F737073, "spso", "FourCC::SongMagicInterrupt" },
        { FourCC::NinjutsuMagicInterrupt, 0x6A6E7073, "spnj", "FourCC::NinjutsuMagicInterrupt" },
        { FourCC::SummonMagicInterrupt, 0x6D737073, "spsm", "FourCC::SummonMagicInterrupt" },
        { FourCC::GeomancyMagicInterrupt, 0x65677073, "spge", "FourCC::GeomancyMagicInterrupt" },
        { FourCC::TrustMagicInterrupt, 0x61667073, "spfa", "FourCC::TrustMagicInterrupt" },
        { FourCC::FadeOut, 0x7573656B, "kesu", "FourCC::FadeOut" },
        { FourCC::Sweating, 0x6C746968, "hitl", "FourCC::Sweating" },
    };

    bool ok = expectEqualInt(tests.size(), 26, "FourCC catalog count");
    for (const auto& test : tests)
    {
        ok = expectEqualInt(enumValue(test.actual), test.expected, test.label) && ok;
        ok = expectEqualString(fourCCTag(test.actual), test.tag, test.label + " tag") && ok;
    }
    return ok;
}

auto testExdataFlagValuesAndOperators() -> bool
{
    const auto kindTests = std::vector<IntCase>{
        { enumValue(Exdata::AugmentKindFlags::HasAugments), 0x02, "AugmentKindFlags::HasAugments" },
        { enumValue(Exdata::AugmentKindFlags::Bundled), 0x03, "AugmentKindFlags::Bundled" },
    };
    const auto subKindTests = std::vector<IntCase>{
        { enumValue(Exdata::AugmentSubKindFlags::Standard), 0x03, "AugmentSubKindFlags::Standard" },
        { enumValue(Exdata::AugmentSubKindFlags::Escutcheon), 0x08, "AugmentSubKindFlags::Escutcheon" },
        { enumValue(Exdata::AugmentSubKindFlags::Serialized), 0x10, "AugmentSubKindFlags::Serialized" },
        { enumValue(Exdata::AugmentSubKindFlags::Mezzotint), 0x20, "AugmentSubKindFlags::Mezzotint" },
        { enumValue(Exdata::AugmentSubKindFlags::Trial), 0x40, "AugmentSubKindFlags::Trial" },
        { enumValue(Exdata::AugmentSubKindFlags::Evolith), 0x80, "AugmentSubKindFlags::Evolith" },
    };

    bool ok = expectAll(kindTests) && expectAll(subKindTests);

    const auto bundled = Exdata::AugmentKindFlags::HasAugments | static_cast<Exdata::AugmentKindFlags>(0x01);
    ok                 = expectEqualInt(enumValue(bundled), enumValue(Exdata::AugmentKindFlags::Bundled), "AugmentKindFlags bundled bits") && ok;

    const auto standard = static_cast<Exdata::AugmentSubKindFlags>(0x01) | static_cast<Exdata::AugmentSubKindFlags>(0x02);
    ok                  = expectEqualInt(enumValue(standard), enumValue(Exdata::AugmentSubKindFlags::Standard), "AugmentSubKindFlags standard bits") && ok;

    const auto subKind = Exdata::AugmentSubKindFlags::Standard |
                         Exdata::AugmentSubKindFlags::Escutcheon |
                         Exdata::AugmentSubKindFlags::Serialized;
    ok = expectEqualInt(enumValue(subKind), 0x1B, "AugmentSubKindFlags combined bits") && ok;

    return ok;
}

auto testExdataTypes() -> bool
{
    return expectAll({
        { enumValue(Exdata::Type::None), 0, "Exdata::Type::None" },
        { enumValue(Exdata::Type::Augment), 1, "Exdata::Type::Augment" },
        { enumValue(Exdata::Type::Usable), 2, "Exdata::Type::Usable" },
        { enumValue(Exdata::Type::Mannequin), 3, "Exdata::Type::Mannequin" },
        { enumValue(Exdata::Type::Furniture), 4, "Exdata::Type::Furniture" },
        { enumValue(Exdata::Type::FlowerPot), 5, "Exdata::Type::FlowerPot" },
        { enumValue(Exdata::Type::Linkshell), 6, "Exdata::Type::Linkshell" },
        { enumValue(Exdata::Type::Fish), 7, "Exdata::Type::Fish" },
        { enumValue(Exdata::Type::BettingSlip), 8, "Exdata::Type::BettingSlip" },
        { enumValue(Exdata::Type::SoulPlate), 9, "Exdata::Type::SoulPlate" },
        { enumValue(Exdata::Type::SoulReflector), 10, "Exdata::Type::SoulReflector" },
        { enumValue(Exdata::Type::AssaultLog), 11, "Exdata::Type::AssaultLog" },
        { enumValue(Exdata::Type::LotteryTicket), 12, "Exdata::Type::LotteryTicket" },
        { enumValue(Exdata::Type::Tabula), 13, "Exdata::Type::Tabula" },
        { enumValue(Exdata::Type::Evolith), 14, "Exdata::Type::Evolith" },
        { enumValue(Exdata::Type::CraftingSet), 15, "Exdata::Type::CraftingSet" },
        { enumValue(Exdata::Type::BrennerBook), 16, "Exdata::Type::BrennerBook" },
        { enumValue(Exdata::Type::GlowingLamp), 17, "Exdata::Type::GlowingLamp" },
        { enumValue(Exdata::Type::LegionPass), 18, "Exdata::Type::LegionPass" },
        { enumValue(Exdata::Type::Serialized), 19, "Exdata::Type::Serialized" },
        { enumValue(Exdata::Type::PerpetualHourglass), 20, "Exdata::Type::PerpetualHourglass" },
        { enumValue(Exdata::Type::ChocoboEgg), 21, "Exdata::Type::ChocoboEgg" },
        { enumValue(Exdata::Type::ChocoboCard), 22, "Exdata::Type::ChocoboCard" },
        { enumValue(Exdata::Type::Escutcheon), 23, "Exdata::Type::Escutcheon" },
        { enumValue(Exdata::Type::RaceCertificate), 24, "Exdata::Type::RaceCertificate" },
        { enumValue(Exdata::Type::MeebleGrimoire), 25, "Exdata::Type::MeebleGrimoire" },
        { enumValue(Exdata::Type::HoneymoonTicket), 26, "Exdata::Type::HoneymoonTicket" },
        { enumValue(Exdata::Type::WeaponUnlock), 27, "Exdata::Type::WeaponUnlock" },
    });
}

} // namespace

auto runMapFourCCExdataEnumSelfTests() -> bool
{
    return testFourCCValuesAndTags() && testExdataFlagValuesAndOperators() && testExdataTypes();
}
