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

#include "test_version_info.h"

#include "common/version.h"

#include <iostream>
#include <string>

namespace
{

auto expectTrue(const bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "version info self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

template <typename T, typename U>
auto expectEqual(const T& actual, const U& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "version info self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

auto safeString(const char* value) -> std::string
{
    if (value == nullptr)
    {
        return {};
    }

    return value;
}

} // namespace

auto runVersionInfoSelfTests() -> bool
{
    bool ok = true;

    const auto* sha     = version::GetGitSha();
    const auto* branch  = version::GetGitBranch();
    const auto* date    = version::GetGitDate();
    const auto* subject = version::GetGitCommitSubject();
    const auto* display = version::GetVersionString();

    ok = expectTrue(sha != nullptr, "sha non-null") && ok;
    ok = expectTrue(branch != nullptr, "branch non-null") && ok;
    ok = expectTrue(date != nullptr, "date non-null") && ok;
    ok = expectTrue(subject != nullptr, "subject non-null") && ok;
    ok = expectTrue(display != nullptr, "version non-null") && ok;

    const auto branchString  = safeString(branch);
    const auto shaString     = safeString(sha);
    const auto versionString = safeString(display);

    ok = expectEqual(versionString, branchString + " (" + shaString + ")", "version string") && ok;

    return ok;
}
