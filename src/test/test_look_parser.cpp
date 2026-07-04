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

#include "test_look_parser.h"

#include "common/utils.h"

#include <array>
#include <iostream>
#include <string>
#include <vector>

namespace
{

auto makeLook(
    uint16 size,
    uint8 face,
    uint8 race,
    uint16 head,
    uint16 body,
    uint16 hands,
    uint16 legs,
    uint16 feet,
    uint16 main,
    uint16 sub,
    uint16 ranged) -> look_t
{
    look_t look{};
    look.size   = size;
    look.face   = face;
    look.race   = race;
    look.head   = head;
    look.body   = body;
    look.hands  = hands;
    look.legs   = legs;
    look.feet   = feet;
    look.main   = main;
    look.sub    = sub;
    look.ranged = ranged;
    return look;
}

auto expectLook(const look_t& actual, const look_t& expected, const std::string& label) -> bool
{
    if (actual.size != expected.size ||
        actual.face != expected.face ||
        actual.race != expected.race ||
        actual.modelid != expected.modelid ||
        actual.head != expected.head ||
        actual.body != expected.body ||
        actual.hands != expected.hands ||
        actual.legs != expected.legs ||
        actual.feet != expected.feet ||
        actual.main != expected.main ||
        actual.sub != expected.sub ||
        actual.ranged != expected.ranged)
    {
        std::cerr << "look parser self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

} // namespace

auto runLookParserSelfTests() -> bool
{
    bool ok = true;

    const auto zeroLook = look_t{};

    {
        uint16 words[10] = {
            0x0001,
            0x0302,
            0x0234,
            0x0345,
            0x0456,
            0x0567,
            0x0678,
            0x0789,
            0x089A,
            0x09AB,
        };
        const auto expected = makeLook(0x0001, 0x02, 0x03, 0x0234, 0x0345, 0x0456, 0x0567, 0x0678, 0x0789, 0x089A, 0x09AB);
        ok                  = expectLook(look_t(words), expected, "look_t word constructor") && ok;
    }

    {
        const auto input    = std::string("010002033412452356346745785689679A78AB89");
        const auto expected = makeLook(0x0001, 0x02, 0x03, 0x0234, 0x0345, 0x0456, 0x0567, 0x0678, 0x0789, 0x089A, 0x09AB);
        ok                  = expectLook(stringToLook(input), expected, "stringToLook normal") && ok;
        ok                  = expectLook(stringToLook("0x" + input), expected, "stringToLook lowercase prefix") && ok;
    }

    ok = expectLook(stringToLook("0x0000340000000000000000000000000000000000"), makeLook(0, 0x34, 0, 0, 0, 0, 0, 0, 0, 0, 0), "stringToLook npc sql") && ok;

    const auto invalidInputs = std::vector<std::string>{
        "",
        "0000",
        "0x0000",
        "0X010002033412452356346745785689679A78AB89",
        "0x010002033412452356346745785689679A78AB8",
        "010002033412452356346745785689679A78AB8900",
    };
    for (const auto& input : invalidInputs)
    {
        ok = expectLook(stringToLook(input), zeroLook, "stringToLook rejects non-full string") && ok;
    }

    {
        const auto expected = makeLook(0x0100, 0x00, 0x01, 0x0200, 0x1200, 0, 0, 0, 0, 0, 0);
        ok                  = expectLook(stringToLook("0001zzzz12zzzzzz000000000000000000000000"), expected, "stringToLook from_chars quirks") && ok;
    }

    return ok;
}
