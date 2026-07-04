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

#include "test_string_utils.h"

#include "common/utils.h"

#include <iostream>
#include <string>
#include <vector>

namespace
{

template <typename T, typename U>
auto expectEqual(const T& actual, const U& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "string utils self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

auto expectSplit(const std::vector<std::string>& actual, const std::vector<std::string>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "string utils self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

} // namespace

auto runStringUtilsSelfTests() -> bool
{
    bool ok = true;

    ok = expectEqual(escape("plain"), std::string("plain"), "escape plain") && ok;
    ok = expectEqual(escape("can\\do"), std::string("can\\\\do"), "escape backslash") && ok;
    ok = expectEqual(escape("O'Reilly"), std::string("O\\'Reilly"), "escape quote") && ok;
    ok = expectEqual(escape("\\'both'\\"), std::string("\\\\\\'both\\'\\\\"), "escape both") && ok;

    ok = expectSplit(split("alpha beta gamma"), { "alpha", "beta", "gamma" }, "split default space") && ok;
    ok = expectSplit(split("a,b,c", ","), { "a", "b", "c" }, "split comma") && ok;
    ok = expectSplit(split("a,,b,", ","), { "a", "", "b", "" }, "split consecutive") && ok;
    ok = expectSplit(split("abc", "|"), { "abc" }, "split missing") && ok;
    ok = expectSplit(split("a--b--", "--"), { "a", "b", "" }, "split multi char") && ok;

    std::string asciiCaseInput;
    asciiCaseInput.push_back('A');
    asciiCaseInput.push_back('b');
    asciiCaseInput.push_back('C');
    asciiCaseInput.push_back('1');
    asciiCaseInput.push_back('_');
    asciiCaseInput.push_back(static_cast<char>(0xC0));
    asciiCaseInput.push_back('z');

    std::string lowerExpected;
    lowerExpected.push_back('a');
    lowerExpected.push_back('b');
    lowerExpected.push_back('c');
    lowerExpected.push_back('1');
    lowerExpected.push_back('_');
    lowerExpected.push_back(static_cast<char>(0xC0));
    lowerExpected.push_back('z');

    std::string upperExpected;
    upperExpected.push_back('A');
    upperExpected.push_back('B');
    upperExpected.push_back('C');
    upperExpected.push_back('1');
    upperExpected.push_back('_');
    upperExpected.push_back(static_cast<char>(0xC0));
    upperExpected.push_back('Z');

    ok = expectEqual(to_lower(asciiCaseInput), lowerExpected, "to_lower ascii") && ok;
    ok = expectEqual(to_upper(asciiCaseInput), upperExpected, "to_upper ascii") && ok;

    ok = expectEqual(trim(" \tOmega\t "), std::string("Omega"), "trim default") && ok;
    ok = expectEqual(trim("\nOmega\n"), std::string("\nOmega\n"), "trim default keeps newline") && ok;
    ok = expectEqual(trim("..Omega..", "."), std::string("Omega"), "trim custom") && ok;
    ok = expectEqual(trim("\t \t"), std::string(""), "trim all whitespace") && ok;
    ok = expectEqual(trim("xyxy", "xy"), std::string(""), "trim custom all") && ok;

    std::string highByteTrimInput = std::string({ static_cast<char>(0x81), 'x', static_cast<char>(0x81) });
    ok = expectEqual(trim(highByteTrimInput, std::string({ static_cast<char>(0x80) })), highByteTrimInput, "trim high byte non match") && ok;
    ok = expectEqual(trim(highByteTrimInput, std::string({ static_cast<char>(0x81) })), std::string("x"), "trim high byte match") && ok;

    std::string rtrimMixed = "Omega \t\n\r";
    rtrim(rtrimMixed);
    ok = expectEqual(rtrimMixed, std::string("Omega"), "rtrim mixed whitespace") && ok;

    std::string rtrimMiddle = "Omega\nXi";
    rtrim(rtrimMiddle);
    ok = expectEqual(rtrimMiddle, std::string("Omega\nXi"), "rtrim middle newline") && ok;

    std::string rtrimAll = "   ";
    rtrim(rtrimAll);
    ok = expectEqual(rtrimAll, std::string(""), "rtrim all") && ok;

    std::string rtrimNul;
    rtrimNul.push_back('O');
    rtrimNul.push_back('\0');
    rtrim(rtrimNul);
    ok = expectEqual(rtrimNul, std::string({ 'O', '\0' }), "rtrim nul") && ok;

    ok = expectEqual(matches("Omega2026", R"(Omega\d+)"), true, "matches full") && ok;
    ok = expectEqual(matches("xOmega2026", R"(Omega\d+)"), false, "matches requires beginning") && ok;
    ok = expectEqual(matches("Omega2026x", R"(Omega\d+)"), false, "matches requires end") && ok;
    ok = expectEqual(matches("", R"(.*)"), true, "matches empty") && ok;
    ok = expectEqual(matches("ab", R"(a|ab)"), true, "matches full alternative") && ok;

    std::string utf8EAcute;
    utf8EAcute.push_back(static_cast<char>(0xC3));
    utf8EAcute.push_back(static_cast<char>(0xA9));
    ok = expectEqual(matches(utf8EAcute, "."), false, "matches byte dot") && ok;
    ok = expectEqual(matches(utf8EAcute, utf8EAcute), true, "matches literal high bytes") && ok;

    ok = expectEqual(starts_with("OmegaXI", "Omega"), true, "starts_with prefix") && ok;
    ok = expectEqual(starts_with("OmegaXI", ""), true, "starts_with empty") && ok;
    ok = expectEqual(starts_with("OmegaXI", "XI"), false, "starts_with mismatch") && ok;

    ok = expectEqual(replace("abc123def456", R"(\d+)", "#"), std::string("abc#def#"), "replace digits") && ok;
    ok = expectEqual(replace("item=42", R"(([a-z]+)=(\d+))", "$1:$2"), std::string("item:42"), "replace captures") && ok;
    ok = expectEqual(replace("abc123def", R"(\d+)", "[$&]"), std::string("abc[123]def"), "replace whole match") && ok;
    ok = expectEqual(replace("item=42", R"(([a-z]+)=(\d+))", "$1x:$2"), std::string("itemx:42"), "replace adjacent literal") && ok;
    ok = expectEqual(replace("abc123def", R"(\d+)", "[$0]"), std::string("abc[123]def"), "replace zero capture") && ok;
    ok = expectEqual(replace("item=42", R"(([a-z]+)=(\d+))", "$01:$02"), std::string("item:42"), "replace leading zero captures") && ok;
    ok = expectEqual(replace("item=42", R"(([a-z]+)=(\d+))", "$10:$2"), std::string(":42"), "replace missing two digit capture") && ok;
    ok = expectEqual(replace(utf8EAcute, ".", "x"), std::string("xx"), "replace byte dot") && ok;
    ok = expectEqual(replace(utf8EAcute, utf8EAcute, "x"), std::string("x"), "replace literal high bytes") && ok;
    ok = expectEqual(replace("abc", "[", "x"), std::string(""), "replace invalid") && ok;

    ok = expectEqual(utils::isPrintableASCII(' ', utils::ASCIIMode::IncludeSpace), true, "ascii space include") && ok;
    ok = expectEqual(utils::isPrintableASCII(' ', utils::ASCIIMode::ExcludeSpace), false, "ascii space exclude") && ok;
    ok = expectEqual(utils::isPrintableASCII('!', utils::ASCIIMode::ExcludeSpace), true, "ascii bang exclude") && ok;
    ok = expectEqual(utils::isPrintableASCII(0x7E, utils::ASCIIMode::IncludeSpace), true, "ascii tilde") && ok;
    ok = expectEqual(utils::isPrintableASCII(0x7F, utils::ASCIIMode::IncludeSpace), false, "ascii delete") && ok;
    ok = expectEqual(utils::isPrintableASCII(0x1F, utils::ASCIIMode::IncludeSpace), false, "ascii control") && ok;

    ok = expectEqual(utils::isStringPrintable("Omega XI", utils::ASCIIMode::IncludeSpace), true, "string printable include") && ok;
    ok = expectEqual(utils::isStringPrintable("Omega XI", utils::ASCIIMode::ExcludeSpace), false, "string printable exclude") && ok;
    ok = expectEqual(utils::isStringPrintable("", utils::ASCIIMode::ExcludeSpace), true, "string printable empty") && ok;
    ok = expectEqual(utils::isStringPrintable(std::string({ 'O', 0x7F }), utils::ASCIIMode::IncludeSpace), false, "string printable delete") && ok;

    std::string toASCIIInput;
    toASCIIInput.push_back('O');
    toASCIIInput.push_back('\0');
    toASCIIInput.push_back('m');
    toASCIIInput.push_back(0x7F);
    toASCIIInput.push_back('e');
    toASCIIInput.push_back(static_cast<char>(0x80));
    ok = expectEqual(utils::toASCII(toASCIIInput, '?'), std::string("O?m?e?"), "toASCII replacement") && ok;

    return ok;
}
