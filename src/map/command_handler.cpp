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

#include "command_handler.h"
#include "command_handler_capacity.h"

#include "common/database.h"
#include "common/utils.h"

#include "autotranslate.h"

#include "entities/char_entity.h"

#include "lua/lua_base_entity.h"
#include "lua/luautils.h"

#include <algorithm>
#include <charconv>
#include <iostream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>

auto commandhandler::detail::ParseCommandLine(const std::string& commandline, const std::string& parameters) -> ParsedCommand
{
    constexpr auto trimLeft = [](std::string_view& sv)
    {
        sv.remove_prefix(std::min(sv.find_first_not_of(" \t"), sv.size()));
    };
    constexpr auto popToken = [](std::string_view& sv) -> std::string_view
    {
        const auto end   = sv.find_first_of(" \t");
        const auto token = sv.substr(0, end);
        sv.remove_prefix(end != std::string_view::npos ? end + 1 : sv.size());
        return token;
    };
    auto view = std::string_view(commandline);
    trimLeft(view);
    // Pure empty-commandline gate after trimLeft (slice 2836).
    if (commandhandlerhelpers::ShouldRejectEmptyCommandLine(view.empty()))
        return {};
    auto out = ParsedCommand{ .name = std::string(popToken(view)), .valid = true };
    out.args.reserve(parameters.size());
    for (const auto paramType : parameters)
    {
        if (paramType == 'b')
        {
            out.args.emplace_back(commandline);
            continue;
        }
        trimLeft(view);
        if (view.empty())
            break;
        if (paramType == 's')
        {
            if (parameters.size() == 1)
            {
                out.args.emplace_back(std::string(view));
                break;
            }
            out.args.emplace_back(std::string(popToken(view)));
        }
        else if (paramType == 'i')
        {
            auto       val   = 0;
            const auto token = popToken(view);
            std::from_chars(token.data(), token.data() + token.size(), val);
            out.args.emplace_back(val);
        }
        else if (paramType == 'd')
        {
            auto       val   = 0.0;
            const auto token = popToken(view);
            std::from_chars(token.data(), token.data() + token.size(), val);
            out.args.emplace_back(val);
        }
        else
        {
            ShowError("cmdhandler::call: (%s) undefined type for param: symbol: %c", out.name.c_str(), paramType);
        }
    }
    return out;
}

auto CCommandHandler::call(Scheduler& scheduler, sol::state& lua, CCharEntity* const PChar, const std::string& commandline) -> CommandResult
{
    TracyZoneScoped;

    // Pure null-char gate dual-wire (slice 2982; residual 2792).
    if (commandhandlerhelpers::ShouldRejectNullChar(PChar == nullptr))
    {
        ShowError("cmdhandler::call: nullptr character attempted to use command");
        return CommandResult::Failure;
    }

    const auto parsedName = commandhandler::detail::ParseCommandLine(commandline, "");
    // Pure empty-name gate (slice 2792).
    if (commandhandlerhelpers::ShouldRejectEmptyCommandName(parsedName.valid))
    {
        ShowError("cmdhandler::call: function name was empty");
        return CommandResult::Failure;
    }

    const auto& cmdName = parsedName.name;

    TracyZoneString(PChar->name);
    TracyZoneString(commandline);

    // Lua/host table lookup remains host-side.
    const auto maybeCommand = lua["xi"]["commands"][cmdName].get<sol::optional<sol::table>>();
    if (!maybeCommand)
    {
        ShowError("cmdhandler::call: Function does not exist (%s)", cmdName.c_str());
        return CommandResult::Failure;
    }
    const auto& commandTable = *maybeCommand;

    const auto maybeCmdProp = commandTable.get<sol::optional<sol::table>>("cmdprops");
    if (!maybeCmdProp)
    {
        ShowError("cmdhandler::call: (%s): Undefined 'cmdprops' table", cmdName.c_str());
        return CommandResult::Failure;
    }
    const auto& cmdprops = *maybeCmdProp;

    const auto maybePerm   = cmdprops.get<sol::optional<int8>>("permission");
    const auto maybeParams = cmdprops.get<sol::optional<std::string>>("parameters");
    if (!maybePerm || !maybeParams)
    {
        ShowError("cmdhandler::call: (%s): Invalid or missing permission/parameters in cmdprops", cmdName.c_str());
        return CommandResult::Failure;
    }

    const auto& permission = *maybePerm;
    const auto& parameters = *maybeParams;

    // Pure permission + audit plan after cmdprops load (slice 2792).
    // permission is int8 from Lua; m_GMlevel / AUDIT_GM_CMD are uint8.
    const auto auditLevel = settings::get<uint8>("map.AUDIT_GM_CMD");
    const auto postProps  = commandhandlerhelpers::PlanCommandCallPostProps(PChar->m_GMlevel, permission, auditLevel);
    if (postProps.rejectPermission)
    {
        ShowWarning("cmdhandler::call: Character %s attempting to use higher permission command %s", PChar->name.c_str(), cmdName.c_str());
        return CommandResult::Failure;
    }

    if (postProps.scheduleAudit)
    {
        scheduler.postToWorkerThread(
            [name = PChar->name, cmd = cmdName, cmdlinestr = autotranslate::replaceBytes(commandline)]() mutable
            {
                const auto query = "INSERT into audit_gm (date_time, gm_name, command, full_string) VALUES(CURRENT_TIMESTAMP(3), ?, ?, ?)";
                if (!db::preparedStmt(query, db::escapeString(name), db::escapeString(cmd), db::escapeString(cmdlinestr)))
                {
                    ShowError("cmdhandler::call: Failed to log GM command.");
                }
            });
    }

    const auto maybeOnTrigger = commandTable.get<sol::optional<sol::function>>("onTrigger");
    if (!maybeOnTrigger)
    {
        ShowError("cmdhandler::call: (%s) missing onTrigger function", cmdName.c_str());
        return CommandResult::Failure;
    }
    const auto& onTrigger = *maybeOnTrigger;

    auto args = commandhandler::detail::ParseCommandLine(commandline, parameters).args;

    const auto result = onTrigger(PChar, sol::as_args(args));
    if (!result.valid())
    {
        const sol::error err = result;
        ShowError("cmdhandler::call: (%s) error: %s", cmdName.c_str(), err.what());
        return CommandResult::Failure;
    }

    return CommandResult::Success;
}
