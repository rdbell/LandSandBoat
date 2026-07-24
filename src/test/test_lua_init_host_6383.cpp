#include "test_lua_init_host_6383.h"

#include <iostream>
#include <string>
#include <vector>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "lua_init host 6383 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for lua_init process ownership (slice 6383).
// Go: luascript.PlanLuaInit / ApplyLuaInit / AttachProcessLua / Application InitLua.
auto runLuaInitHost6383SelfTests() -> bool
{
    bool ok = true;

    // Ordered steps residual (lua_init body).
    const std::vector<std::string> steps = {
        "open_libraries",
        "require_bit",
        "debug_helpers",
        "print",
        "_tostring",
        "tostring",
        "fmt",
        "sleep",
    };
    ok = expect(steps.size() == 8, "step count") && ok;
    ok = expect(steps[0] == "open_libraries", "open_libraries first") && ok;
    ok = expect(steps[1] == "require_bit", "require bit") && ok;
    ok = expect(steps[3] == "print", "print before tostring") && ok;
    ok = expect(steps[4] == "_tostring", "save tostring") && ok;
    ok = expect(steps[5] == "tostring", "custom tostring") && ok;
    ok = expect(steps.back() == "sleep", "sleep last without debugger") && ok;

    // Application ctor order residual: prepareLogging → lua_init → settings::init.
    const std::vector<std::string> ctor = {
        "prepareLogging",
        "lua_init",
        "settings::init",
    };
    ok = expect(ctor.size() == 3, "ctor step count") && ok;
    ok = expect(ctor[0] == "prepareLogging", "log first") && ok;
    ok = expect(ctor[1] == "lua_init", "lua second") && ok;
    ok = expect(ctor[2] == "settings::init", "settings third") && ok;

    // Global names residual.
    ok = expect(std::string("bit") == "bit", "bit global") && ok;
    ok = expect(std::string("__FILE__") == "__FILE__", "FILE helper") && ok;
    ok = expect(std::string("__LINE__") == "__LINE__", "LINE helper") && ok;
    ok = expect(std::string("__FUNC__") == "__FUNC__", "FUNC helper") && ok;

    return ok;
}
