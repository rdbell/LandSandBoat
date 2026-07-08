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

#include "test_event_info.h"

#include "event_info.h"

#include <iostream>

namespace
{

auto expectEqual(auto actual, auto expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "event info self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectBool(bool actual, bool expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "event info self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto testEventPrepReset() -> bool
{
    EventPrep prep;
    prep.targetEntity = reinterpret_cast<CBaseEntity*>(0x1234);
    prep.scriptFile   = "scripts/zones/Test/DefaultActions.lua";

    prep.reset();

    bool ok = true;
    ok      = expectBool(prep.targetEntity == nullptr, true, "prep target reset") && ok;
    ok      = expectBool(prep.scriptFile.empty(), true, "prep script reset") && ok;
    return ok;
}

auto testEventInfoDefaults() -> bool
{
    EventInfo info;

    bool ok = true;
    ok      = expectEqual(info.eventId, static_cast<int32>(-1), "default event id") && ok;
    ok      = expectEqual(info.option, static_cast<int32>(0), "default option") && ok;
    ok      = expectBool(info.params.empty(), true, "default params") && ok;
    ok      = expectBool(info.strings.empty(), true, "default strings") && ok;
    ok      = expectEqual(info.textTable, static_cast<int16>(-1), "default text table") && ok;
    ok      = expectEqual(info.type, NORMAL, "default type") && ok;
    ok      = expectBool(info.cutsceneOptions.empty(), true, "default cutscene options") && ok;
    ok      = expectEqual(info.interruptText, static_cast<uint16>(0), "default interrupt text") && ok;
    ok      = expectEqual(info.eventFlags, static_cast<uint32>(0), "default event flags") && ok;
    ok      = expectBool(info.canSkip, false, "default can skip") && ok;
    return ok;
}

auto testEventInfoCutsceneOptionLookup() -> bool
{
    EventInfo info;
    info.cutsceneOptions = { 0, 5, 5, -2 };

    bool ok = true;
    ok      = expectBool(info.hasCutsceneOption(0), true, "has option zero") && ok;
    ok      = expectBool(info.hasCutsceneOption(5), true, "has duplicated option") && ok;
    ok      = expectBool(info.hasCutsceneOption(-2), true, "has negative option") && ok;
    ok      = expectBool(info.hasCutsceneOption(7), false, "missing option") && ok;
    return ok;
}

auto testEventInfoReset() -> bool
{
    EventInfo info;
    info.targetEntity = reinterpret_cast<CBaseEntity*>(0x1234);
    info.scriptFile   = "scripts/zones/Test/DefaultActions.lua";
    info.eventId      = 77;
    info.option       = 9;
    info.params[1]    = 100;
    info.params[3]    = 300;
    info.strings[2]   = "hello";
    info.textTable    = 45;
    info.type         = OPTIONAL_CUTSCENE;
    info.cutsceneOptions.push_back(12);
    info.interruptText = 1234;
    info.eventFlags    = 0xDEADBEEF;
    info.canSkip       = true;

    info.reset();

    bool ok = true;
    ok      = expectBool(info.targetEntity == nullptr, true, "reset target") && ok;
    ok      = expectBool(info.scriptFile.empty(), true, "reset script") && ok;
    ok      = expectEqual(info.eventId, static_cast<int32>(-1), "reset event id") && ok;
    ok      = expectEqual(info.option, static_cast<int32>(0), "reset option") && ok;
    ok      = expectBool(info.params.empty(), true, "reset params") && ok;
    ok      = expectBool(info.strings.empty(), true, "reset strings") && ok;
    ok      = expectEqual(info.textTable, static_cast<int16>(-1), "reset text table") && ok;
    ok      = expectEqual(info.type, NORMAL, "reset type") && ok;
    ok      = expectBool(info.cutsceneOptions.empty(), true, "reset cutscene options") && ok;
    ok      = expectEqual(info.interruptText, static_cast<uint16>(1234), "reset preserves interrupt text") && ok;
    ok      = expectEqual(info.eventFlags, static_cast<uint32>(0), "reset event flags") && ok;
    ok      = expectBool(info.canSkip, false, "reset can skip") && ok;
    return ok;
}

} // namespace

auto runEventInfoSelfTests() -> bool
{
    bool ok = true;
    ok      = testEventPrepReset() && ok;
    ok      = testEventInfoDefaults() && ok;
    ok      = testEventInfoCutsceneOptionLookup() && ok;
    ok      = testEventInfoReset() && ok;
    return ok;
}
