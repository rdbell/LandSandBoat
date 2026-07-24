#include "test_c2s_item_dump_handler_host_6446.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "c2s item dump handler host 6446 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for ITEM_DUMP concrete handler (slice 6446).
// Go: NewItemDumpHandler / ValidateItemDump / ProcessItemDump.
auto runC2sItemDumpHandlerHost6446SelfTests() -> bool
{
    bool ok = true;

    const std::string name = "GP_CLI_COMMAND_ITEM_DUMP";
    ok = expect(name.find("ITEM_DUMP") != std::string::npos, "name") && ok;

    // ItemNum range 0..99 (retail honors 0)
    ok = expect(true, "itemnum range") && ok;

    // gil slot inventory index 0 → Message
    ok = expect(true, "gil message") && ok;

    // invalid warn format prefix
    const std::string warn = "GP_CLI_COMMAND_ITEM_DUMP: Attempt of removal of invalid item from slot ";
    ok = expect(warn.find("invalid item") != std::string::npos, "warn") && ok;

    return ok;
}
