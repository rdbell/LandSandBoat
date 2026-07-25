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

#include "test_file_utils.h"

#include "common/utils.h"

#include <chrono>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <unistd.h>
#include <vector>

namespace
{

template <typename T, typename U>
auto expectEqual(const T& actual, const U& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "file utils self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

auto expectPathSequence(const std::vector<std::filesystem::path>& actual, const std::vector<std::filesystem::path>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "file utils self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

auto pathSetToVector(const std::set<std::filesystem::path>& paths) -> std::vector<std::filesystem::path>
{
    return { paths.begin(), paths.end() };
}

auto testOpenFile(const std::filesystem::path& root) -> bool
{
    bool ok = true;

    const auto path = root / "sample.txt";
    auto       file = utils::openFile(path.string(), "wb");
    ok             = expectEqual(static_cast<bool>(file), true, "openFile creates writable file") && ok;
    if (file)
    {
        ok = expectEqual(std::fputs("OmegaXI", file.get()) >= 0, true, "openFile write") && ok;
        std::fflush(file.get());
        std::fclose(file.release());
    }

    std::ifstream in(path);
    std::string   text;
    in >> text;
    ok = expectEqual(text, std::string("OmegaXI"), "openFile wrote expected contents") && ok;

    auto missing = utils::openFile((root / "missing.txt").string(), "rb");
    ok           = expectEqual(static_cast<bool>(missing), false, "openFile missing read returns null") && ok;

    return ok;
}

auto testSortedDirectoryIterator(const std::filesystem::path& root) -> bool
{
    bool ok = true;

    const auto dir = root / "sorted";
    std::filesystem::create_directories(dir / "gamma");
    std::ofstream(dir / "zeta.txt").put('z');
    std::ofstream(dir / "alpha.txt").put('a');
    std::ofstream(dir / "gamma" / "beta.txt").put('b');
    std::filesystem::create_directories(dir / "a");
    std::ofstream(dir / "a" / "b.txt").put('b');
    std::ofstream(dir / "a-1.txt").put('a');

    const auto actualDirect = sorted_directory_iterator<std::filesystem::directory_iterator>(dir.string());
    const std::set<std::filesystem::path> expectedDirect{
        dir / "a",
        dir / "a-1.txt",
        dir / "alpha.txt",
        dir / "gamma",
        dir / "zeta.txt",
    };
    ok = expectEqual(actualDirect, expectedDirect, "sorted_directory_iterator returns lexical path set") && ok;

    const auto actualRecursive = sorted_directory_iterator<std::filesystem::recursive_directory_iterator>(dir.string());
    const std::set<std::filesystem::path> expectedRecursive{
        dir / "a",
        dir / "a" / "b.txt",
        dir / "a-1.txt",
        dir / "alpha.txt",
        dir / "gamma",
        dir / "gamma" / "beta.txt",
        dir / "zeta.txt",
    };
    ok = expectEqual(actualRecursive, expectedRecursive, "sorted_directory_iterator supports recursive iterator") && ok;

    const auto originalPath = std::filesystem::current_path();
    std::filesystem::current_path(root);
    std::filesystem::create_directories("./relative/nested");
    std::filesystem::create_directories("./relative/a");
    std::ofstream("./relative/a/b.txt").put('b');
    std::ofstream("./relative/a-1.txt").put('a');
    std::ofstream("./relative/a.txt").put('a');
    std::ofstream("./relative/nested/b.txt").put('b');

    const auto actualRelativeDirect = sorted_directory_iterator<std::filesystem::directory_iterator>("./relative");
    const std::set<std::filesystem::path> expectedRelativeDirect{
        "./relative/a",
        "./relative/a-1.txt",
        "./relative/a.txt",
        "./relative/nested",
    };
    ok = expectEqual(actualRelativeDirect, expectedRelativeDirect, "sorted_directory_iterator preserves relative path spelling") && ok;
    ok = expectPathSequence(
             pathSetToVector(actualRelativeDirect),
             {
                 "./relative/a",
                 "./relative/a-1.txt",
                 "./relative/a.txt",
                 "./relative/nested",
             },
             "sorted_directory_iterator uses filesystem path ordering") &&
         ok;

    const auto actualRelativeRecursive = sorted_directory_iterator<std::filesystem::recursive_directory_iterator>("./relative");
    const std::set<std::filesystem::path> expectedRelativeRecursive{
        "./relative/a",
        "./relative/a/b.txt",
        "./relative/a-1.txt",
        "./relative/a.txt",
        "./relative/nested",
        "./relative/nested/b.txt",
    };
    ok = expectEqual(actualRelativeRecursive, expectedRelativeRecursive, "recursive sorted_directory_iterator preserves relative path spelling") && ok;
    ok = expectPathSequence(
             pathSetToVector(actualRelativeRecursive),
             {
                 "./relative/a",
                 "./relative/a/b.txt",
                 "./relative/a-1.txt",
                 "./relative/a.txt",
                 "./relative/nested",
                 "./relative/nested/b.txt",
             },
             "recursive sorted_directory_iterator uses filesystem path ordering") &&
         ok;

    std::filesystem::current_path(originalPath);
    return ok;
}

} // namespace

auto runFileUtilsSelfTests() -> bool
{
    const auto suffix = std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
    const auto root   = std::filesystem::temp_directory_path() / ("lsb_file_utils_" + std::to_string(::getpid()) + "_" + suffix);
    std::filesystem::create_directories(root);

    bool ok = true;
    ok      = testOpenFile(root) && ok;
    ok      = testSortedDirectoryIterator(root) && ok;

    std::filesystem::remove_all(root);
    return ok;
}
