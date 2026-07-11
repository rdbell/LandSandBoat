#include "test_map_lua_function_1304.h"

#include "map/lua_function.h"

#include <iostream>
#include <limits>
#include <optional>
#include <string>
#include <vector>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "map lua function 1304 self-test failed: " << label << '\n';
    }
    return condition;
}

auto testFormatError() -> bool
{
    const auto formatted = mapipc::FormatLuaFunctionError("boom", "return xi.test()");
    return expect(formatted == "IPCClient::handleMessage_LuaFunction: error: boom: return xi.test()", "format exact") &&
           expect(mapipc::FormatLuaFunctionError("a%b{c}", "d%e{f}") ==
                      "IPCClient::handleMessage_LuaFunction: error: a%b{c}: d%e{f}",
                  "format treats values as data");
}

auto testSuccessDoesNotLog() -> bool
{
    std::string executed{};
    int         logs{};
    mapipc::HandleLuaFunction(
        ipc::LuaFunction{
            .requesterZoneId = std::numeric_limits<uint16>::max(),
            .executorZoneId  = std::numeric_limits<uint16>::max() - 1,
            .funcString      = "return 1",
        },
        [&](const std::string& funcString) -> std::optional<std::string>
        {
            executed = funcString;
            return std::nullopt;
        },
        [&](const std::string&)
        {
            ++logs;
        });

    return expect(executed == "return 1", "success executes funcString") &&
           expect(logs == 0, "success skips error log");
}

auto testFailureLogsFormattedError() -> bool
{
    std::vector<std::string> logs{};
    std::string              executed{};
    const ipc::LuaFunction   message{
          .requesterZoneId = 7,
          .executorZoneId  = 8,
          .funcString      = "error('x')",
    };

    mapipc::HandleLuaFunction(
        message,
        [&](const std::string& funcString) -> std::optional<std::string>
        {
            executed = funcString;
            return std::string{ "runtime failure" };
        },
        [&](const std::string& line)
        {
            logs.push_back(line);
        });

    return expect(executed == message.funcString, "failure still executes funcString") &&
           expect(logs.size() == 1, "one error log") &&
           expect(logs[0] == mapipc::FormatLuaFunctionError("runtime failure", message.funcString), "error log matches format helper");
}

auto testEmptyFuncStringPreserved() -> bool
{
    std::string executed{ "unset" };
    mapipc::HandleLuaFunction(
        ipc::LuaFunction{},
        [&](const std::string& funcString) -> std::optional<std::string>
        {
            executed = funcString;
            return std::nullopt;
        },
        [](const std::string&)
        {
        });

    return expect(executed.empty(), "empty funcString preserved");
}

} // namespace

auto runMapLuaFunction1304SelfTests() -> bool
{
    return testFormatError() && testSuccessDoesNotLog() && testFailureLogsFormattedError() && testEmptyFuncStringPreserved();
}
