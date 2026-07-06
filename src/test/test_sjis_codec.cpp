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

#include "test_sjis_codec.h"

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "common/sjis.h"

namespace
{

struct ConvertCase
{
    std::string              input;
    std::vector<std::uint8_t> expected;
    std::string              label;
};

auto expectBytes(const std::string& actual, const std::vector<std::uint8_t>& expected, const std::string& label) -> bool
{
    if (actual.size() != expected.size())
    {
        std::cerr << "SJIS codec self-test failed: " << label << " size got " << actual.size() << " expected " << expected.size() << '\n';
        return false;
    }

    for (std::size_t i = 0; i < expected.size(); ++i)
    {
        const auto got = static_cast<std::uint8_t>(actual[i]);
        if (got != expected[i])
        {
            std::cerr << "SJIS codec self-test failed: " << label << " byte " << i << " got " << static_cast<std::uint32_t>(got)
                      << " expected " << static_cast<std::uint32_t>(expected[i]) << '\n';
            return false;
        }
    }

    return true;
}

auto testUTF8ToShiftJIS() -> bool
{
    const std::vector<ConvertCase> cases{
        { "OmegaXI", { 0x4F, 0x6D, 0x65, 0x67, 0x61, 0x58, 0x49 }, "ASCII" },
        { std::string("A\0B", 3), { 0x41, 0x00, 0x42 }, "embedded NUL" },
        { "\xE3\x81\x82\xE6\xBC\xA2", { 0x82, 0xA0, 0x8A, 0xBF }, "hiragana and kanji" },
        { "\xEF\xBD\xB6", { 0xB6 }, "halfwidth kana" },
        { "\xE2\x91\xA0\xE9\xAB\x99", { 0x87, 0x40, 0xEE, 0xE0 }, "CP932 extensions" },
        { "\xE2\x80\x95\xE3\x80\x9C\xEF\xBD\x9E", { 0x81, 0x5C, 0x81, 0x60, 0x81, 0x60 }, "wave punctuation" },
        { "\xEE\x80\x80\xEE\x9D\x97", { 0xF0, 0x40, 0xF9, 0xFC }, "CP932 private use range" },
        { "\xEF\xA3\xB0\xEF\xA3\xB1\xEF\xA3\xB2\xEF\xA3\xB3", { 0xA0, 0xFD, 0xFE, 0xFF }, "CP932 private use single-byte tail" },
        { "\xF0\x9F\x98\x80\xC2\xA5", { 0x3F, 0x3F }, "unsupported replacement" },
        { "\xC2\x80", { 0x80 }, "C1 control" },
    };

    bool ok = true;
    for (const auto& testCase : cases)
    {
        ok = expectBytes(encoding::utf8ToShiftJis(testCase.input), testCase.expected, testCase.label) && ok;
    }
    return ok;
}

} // namespace

auto runSJISCodecSelfTests() -> bool
{
    return testUTF8ToShiftJIS();
}
