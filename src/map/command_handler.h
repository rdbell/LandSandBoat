/*
===========================================================================

  Copyright (c) 2010-2015 Darkstar Dev Teams

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

#pragma once

#include <common/cbasetypes.h>
#include <common/logging.h>
#include <common/scheduler.h>

#include <list>
#include <string>
#include <variant>
#include <vector>

namespace commandhandler::detail
{

using CommandArg = std::variant<bool, int, double, std::string>;

struct ParsedCommand
{
    std::string             name;
    std::vector<CommandArg> args;
    bool                    valid{};
};

auto ParseCommandLine(const std::string& commandline, const std::string& parameters) -> ParsedCommand;

} // namespace commandhandler::detail

//
// Forward declarations
//

class CCharEntity;

namespace sol
{

class state;

}

enum class CommandResult : uint8
{
    Success,
    Failure,
};

class CCommandHandler
{
public:
    static auto call(Scheduler& scheduler, sol::state& lua, CCharEntity* PChar, const std::string& commandline) -> CommandResult;
};
