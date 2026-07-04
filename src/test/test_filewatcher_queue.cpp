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

#include "test_filewatcher_queue.h"

#include "common/filewatcher.h"

#include <filesystem>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

namespace
{

template <typename T>
auto expectEqual(const T& actual, const T& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "Filewatcher queue self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

auto expectEntry(const std::pair<std::filesystem::path, Filewatcher::Action>& actual, const std::filesystem::path& path, Filewatcher::Action action, const std::string& label) -> bool
{
    bool ok = true;
    ok      = expectEqual(actual.first.generic_string(), path.generic_string(), label + " path") && ok;
    ok      = expectEqual(actual.second, action, label + " action") && ok;
    return ok;
}

auto testActionFilteringDedupingAndDrain() -> bool
{
    Filewatcher watcher({});

    watcher.handleFileAction(0, "scripts/", "b.lua", efsw::Actions::Modified, "");
    watcher.handleFileAction(0, "scripts/", "a.txt", efsw::Actions::Add, "");
    watcher.handleFileAction(0, "scripts/", "a.lua", efsw::Actions::Add, "");
    watcher.handleFileAction(0, "scripts/", "a.lua", efsw::Actions::Add, "");
    watcher.handleFileAction(0, "scripts/", "a.lua", efsw::Actions::Delete, "");
    watcher.handleFileAction(0, "scripts/", "ignored.lua", static_cast<efsw::Action>(999), "");
    watcher.handleFileAction(0, "", ".lua", efsw::Actions::Add, "");
    watcher.handleFileAction(0, "scripts/", ".lua", efsw::Actions::Add, "");
    watcher.handleFileAction(0, "scripts/", ".profile.lua", efsw::Actions::Modified, "");
    watcher.handleFileAction(0, "scripts/", "trailing.lua/", efsw::Actions::Add, "");
    watcher.handleFileAction(0, "modules/", "moved.lua", efsw::Actions::Moved, "");

    bool ok = true;

    const auto results = watcher.popChangedLuaFilesList();
    ok                 = expectEqual(results.size(), static_cast<std::size_t>(5), "result count") && ok;
    if (results.size() == 5)
    {
        ok = expectEntry(results[0], "modules/moved.lua", Filewatcher::Action::Moved, "modules moved") && ok;
        ok = expectEntry(results[1], "scripts/.profile.lua", Filewatcher::Action::Modified, "hidden profile") && ok;
        ok = expectEntry(results[2], "scripts/a.lua", Filewatcher::Action::Add, "scripts add") && ok;
        ok = expectEntry(results[3], "scripts/a.lua", Filewatcher::Action::Delete, "scripts delete") && ok;
        ok = expectEntry(results[4], "scripts/b.lua", Filewatcher::Action::Modified, "scripts modified") && ok;
    }

    ok = expectEqual(watcher.popChangedLuaFilesList().empty(), true, "drained queue") && ok;

    return ok;
}

auto testRelativePath() -> bool
{
    Filewatcher watcher({});

    watcher.handleFileAction(0, "/", "tmp/absolute.lua", efsw::Actions::Add, "");
    watcher.handleFileAction(0, "", "/var/tmp/rooted.lua", efsw::Actions::Modified, "");

    bool ok = true;

    const auto results = watcher.popChangedLuaFilesList();
    ok                 = expectEqual(results.size(), static_cast<std::size_t>(2), "relative result count") && ok;
    if (results.size() == 2)
    {
        ok = expectEntry(results[0], "tmp/absolute.lua", Filewatcher::Action::Add, "absolute relative") && ok;
        ok = expectEntry(results[1], "var/tmp/rooted.lua", Filewatcher::Action::Modified, "rooted relative") && ok;
    }

    return ok;
}

} // namespace

auto runFilewatcherQueueSelfTests() -> bool
{
    return testActionFilteringDedupingAndDrain() && testRelativePath();
}
