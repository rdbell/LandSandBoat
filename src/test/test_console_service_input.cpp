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

#include "test_console_service_input.h"

#include <iostream>
#include <string>

#ifndef _WIN32
#include <unistd.h>

bool stdinHasData();
bool getLine(std::string& line);

namespace
{

class StdinPipe
{
public:
    explicit StdinPipe(const std::string& input)
    {
        originalStdin_ = dup(STDIN_FILENO);
        if (originalStdin_ == -1 || pipe(pipe_) == -1)
        {
            return;
        }

        const auto written = write(pipe_[1], input.data(), input.size());
        close(pipe_[1]);
        pipe_[1] = -1;
        if (written != static_cast<ssize_t>(input.size()) || dup2(pipe_[0], STDIN_FILENO) == -1)
        {
            return;
        }

        std::cin.clear();
        valid_ = true;
    }

    ~StdinPipe()
    {
        if (originalStdin_ != -1)
        {
            dup2(originalStdin_, STDIN_FILENO);
            close(originalStdin_);
        }
        if (pipe_[0] != -1)
        {
            close(pipe_[0]);
        }
        if (pipe_[1] != -1)
        {
            close(pipe_[1]);
        }
        std::cin.clear();
    }

    auto valid() const -> bool
    {
        return valid_;
    }

private:
    int  originalStdin_ = -1;
    int  pipe_[2]       = { -1, -1 };
    bool valid_         = false;
};

auto expectLine(const std::string& expected, const std::string& label) -> bool
{
    std::string line;
    if (!stdinHasData() || !getLine(line) || line != expected)
    {
        std::cerr << "console service input self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

} // namespace
#endif

auto runConsoleServiceInputSelfTests() -> bool
{
#ifdef _WIN32
    // Windows getLine reads one console key at a time and requires an attached
    // console, which the test process intentionally does not provide.
    return true;
#else
    StdinPipe input("first command\nsecond command\npartial command");
    if (!input.valid())
    {
        std::cerr << "console service input self-test failed: pipe setup\n";
        return false;
    }

    bool ok = true;
    ok      = expectLine("first command", "first complete line") && ok;
    ok      = expectLine("second command", "second complete line") && ok;
    ok      = expectLine("partial command", "EOF-terminated line") && ok;
    return ok;
#endif
}
