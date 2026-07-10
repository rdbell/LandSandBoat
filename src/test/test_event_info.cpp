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

auto testEventInfoCopyLifecycle() -> bool
{
    EventInfo original;
    original.targetEntity          = reinterpret_cast<CBaseEntity*>(0x1234);
    original.scriptFile            = "scripts/zones/Test/DefaultActions.lua";
    original.eventId               = 77;
    original.option                = -9;
    original.params[0]             = 0;
    original.params[255]           = UINT32_MAX;
    original.strings[0]            = "";
    original.strings[255]          = "original";
    original.textTable             = 45;
    original.type                  = OPTIONAL_CUTSCENE;
    original.cutsceneOptions       = { -1, 0, 5 };
    original.interruptText         = 1234;
    original.eventFlags            = UINT32_MAX;
    original.canSkip               = true;

    EventInfo copy = original;
    copy.params[255]          = 7;
    copy.params[1]            = 100;
    copy.strings[255]         = "copy";
    copy.strings[1]           = "added";
    copy.cutsceneOptions[0]   = 99;
    copy.cutsceneOptions.push_back(12);

    bool ok = true;
    ok      = expectBool(copy.targetEntity == original.targetEntity, true, "copy target identity") && ok;
    ok      = expectBool(copy.scriptFile == original.scriptFile, true, "copy script file") && ok;
    ok      = expectEqual(copy.eventId, static_cast<int32>(77), "copy event id") && ok;
    ok      = expectEqual(copy.option, static_cast<int32>(-9), "copy option") && ok;
    ok      = expectEqual(copy.textTable, static_cast<int16>(45), "copy text table") && ok;
    ok      = expectEqual(copy.type, OPTIONAL_CUTSCENE, "copy type") && ok;
    ok      = expectEqual(copy.interruptText, static_cast<uint16>(1234), "copy interrupt text") && ok;
    ok      = expectEqual(copy.eventFlags, UINT32_MAX, "copy event flags") && ok;
    ok      = expectBool(copy.canSkip, true, "copy can skip") && ok;
    ok      = expectBool(copy.hasCutsceneOption(99), true, "copy option lookup after mutation") && ok;
    ok      = expectBool(copy.hasCutsceneOption(-1), false, "copy replaced original option") && ok;
    ok      = expectEqual(original.params.at(255), UINT32_MAX, "copy owns params values") && ok;
    ok      = expectBool(original.params.contains(1), false, "copy owns params insertions") && ok;
    ok      = expectBool(original.strings.at(255) == "original", true, "copy owns string values") && ok;
    ok      = expectBool(original.strings.contains(1), false, "copy owns string insertions") && ok;
    ok      = expectEqual(original.cutsceneOptions.size(), static_cast<std::size_t>(3), "copy owns option length") && ok;
    ok      = expectEqual(original.cutsceneOptions[0], static_cast<int32>(-1), "copy owns option values") && ok;

    copy.reset();
    ok = expectEqual(copy.interruptText, static_cast<uint16>(1234), "copy reset preserves interrupt text") && ok;
    ok = expectEqual(original.eventId, static_cast<int32>(77), "copy reset preserves original event id") && ok;
    ok = expectEqual(original.params.size(), static_cast<std::size_t>(2), "copy reset preserves original params") && ok;
    ok = expectEqual(original.strings.size(), static_cast<std::size_t>(2), "copy reset preserves original strings") && ok;
    ok = expectEqual(original.cutsceneOptions.size(), static_cast<std::size_t>(3), "copy reset preserves original options") && ok;
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
    ok      = testEventInfoCopyLifecycle() && ok;
    return ok;
}
