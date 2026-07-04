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

#include "test_arguments_parser.h"

#include "common/application.h"
#include "common/arguments.h"

#include <iostream>
#include <string>
#include <vector>

namespace
{

auto makeArgv(std::vector<std::string>& values) -> std::vector<char*>
{
    std::vector<char*> argv;
    argv.reserve(values.size());
    for (auto& value : values)
    {
        argv.push_back(value.data());
    }

    return argv;
}

template <typename T, typename U>
auto expectEqual(const T& actual, const U& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "arguments parser self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

auto expectTrue(const bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "arguments parser self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

auto expectFalse(const bool actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "arguments parser self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

auto testCommonAndSpecializedArguments() -> bool
{
    bool ok = true;

    auto config = ApplicationConfig{
        .serverName = "map",
        .arguments  = {
            ArgumentDefinition{
                .name        = "--ip",
                .description = "Specify the IP address to bind to",
            },
            ArgumentDefinition{
                .name        = "--port",
                .description = "Specify the port to bind to",
            },
            ArgumentDefinition{
                .name        = "--lazy",
                .description = "Load zones on demand",
                .type        = ArgumentType::Flag,
            },
            ArgumentDefinition{
                .name        = "--tag",
                .description = "Only run tests with these #tags",
                .type        = ArgumentType::Multiple,
            },
        },
    };

    std::vector<std::string> values{
        "xi_map",
        "--log", "log/custom.log",
        "--append_date",
        "--ci",
        "--ip", "127.0.0.1",
        "--port", "54230",
        "--lazy",
        "--tag", "combat",
        "--tag", "nightly",
    };
    auto argv = makeArgv(values);

    Arguments parsed(config, static_cast<int>(argv.size()), argv.data());

    const auto log = parsed.present("--log");
    ok             = expectTrue(log.has_value(), "log present") && ok;
    ok             = expectEqual(log.value_or(""), std::string("log/custom.log"), "log value") && ok;

    const auto ip = parsed.present("--ip");
    ok            = expectTrue(ip.has_value(), "ip present") && ok;
    ok            = expectEqual(ip.value_or(""), std::string("127.0.0.1"), "ip value") && ok;

    ok = expectEqual(parsed.get<std::string>("--port"), std::string("54230"), "port value") && ok;
    ok = expectTrue(parsed.get<bool>("--append-date"), "append-date alias") && ok;
    ok = expectTrue(parsed.get<bool>("--append_date"), "append_date alias") && ok;
    ok = expectTrue(parsed.get<bool>("--ci"), "ci flag") && ok;
    ok = expectTrue(parsed.get<bool>("--lazy"), "lazy flag") && ok;

    const auto tags = parsed.get<std::vector<std::string>>("--tag");
    ok              = expectEqual(tags, std::vector<std::string>{ "combat", "nightly" }, "multiple tags") && ok;

    return ok;
}

auto testMissingLookups() -> bool
{
    bool ok = true;

    auto config = ApplicationConfig{
        .serverName = "test",
        .arguments  = {
            ArgumentDefinition{
                .name        = "--filter",
                .description = "Only run tests matching the regex pattern",
                .type        = ArgumentType::Multiple,
            },
            ArgumentDefinition{
                .name        = "--verbose",
                .description = "Verbose output of errors",
                .type        = ArgumentType::Flag,
            },
        },
    };

    std::vector<std::string> values{ "xi_test" };
    auto                     argv = makeArgv(values);
    Arguments                parsed(config, static_cast<int>(argv.size()), argv.data());

    ok = expectFalse(parsed.present("--log").has_value(), "missing log present") && ok;
    ok = expectEqual(parsed.get<std::string>("--log"), std::string(""), "missing log get") && ok;
    ok = expectFalse(parsed.get<bool>("--verbose"), "missing verbose flag") && ok;
    ok = expectFalse(parsed.get<bool>("--ci"), "missing ci flag") && ok;
    ok = expectTrue(parsed.get<std::vector<std::string>>("--filter").empty(), "missing multiple empty") && ok;
    ok = expectEqual(parsed.get<std::string>("--unknown"), std::string(""), "unknown get") && ok;

    return ok;
}

} // namespace

auto runArgumentsParserSelfTests() -> bool
{
    bool ok = true;

    ok = testCommonAndSpecializedArguments() && ok;
    ok = testMissingLookups() && ok;

    return ok;
}
