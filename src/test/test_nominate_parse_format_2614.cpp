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

#include "test_nominate_parse_format_2614.h"

#include <iostream>
#include <string>
#include <tuple>
#include <vector>

#include "map/nominate_manager.h"

namespace
{

auto expect(const bool condition, const std::string& label) -> bool
{
    if (!condition)
    {
        std::cerr << "nominate parse/format self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runNominateParseFormat2614SelfTests() -> bool
{
    bool ok = true;

    auto [question, options] = ParseNominateInput("\"Where to eat?\" \"Sushi Bar\" \"\" ramen");
    ok                       = expect(question == "Where to eat?", "quoted question") && ok;
    ok                       = expect(options == std::vector<std::string>{ "Sushi Bar", "", "ramen" }, "quoted options") && ok;

    std::tie(question, options) = ParseNominateInput("Q\to1\to2\to3\to4\to5\to6\to7\to8\to9");
    ok                          = expect(question == "Q", "tab-separated question") && ok;
    ok                          = expect(options == std::vector<std::string>{ "o1", "o2", "o3", "o4", "o5", "o6", "o7", "o8" }, "eight-option limit") && ok;

    std::tie(question, options) = ParseNominateInput("Hello \"world end");
    ok                          = expect(question == "Hello" && options == std::vector<std::string>{ "world end" }, "unclosed quote") && ok;

    NominateProposal proposal{
        .question = "Pick",
        .options  = { "A", "B" },
    };
    proposal.voteTbl[1] = 3;
    proposal.voteTbl[2] = 7;
    ok                  = expect(FormatNominateBody(proposal, false) == "[Pick]\n1:A\n2:B", "body without tallies") && ok;
    ok                  = expect(FormatNominateBody(proposal, true) == "[Pick]\n1[3]:A\n2[7]:B", "body with tallies") && ok;

    return ok;
}
