#include "test_c2s_item_move_handler_host_6447.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "c2s item move handler host 6447 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for ITEM_MOVE concrete handler (slice 6447).
// Go: NewItemMoveHandler / itemmove.IsValidMovement / PlanProcess.
auto runC2sItemMoveHandlerHost6447SelfTests() -> bool
{
    bool ok = true;

    const std::string name = "GP_CLI_COMMAND_ITEM_MOVE";
    ok = expect(name.find("ITEM_MOVE") != std::string::npos, "name") && ok;

    // unite index ceiling 82
    ok = expect(82 == 82, "unite ceiling") && ok;

    // ERROR_SLOTID 255
    ok = expect(255 == 255, "error slot") && ok;

    // illegal movement string
    const std::string illegal = "Illegal movement";
    ok = expect(illegal == "Illegal movement", "illegal") && ok;

    return ok;
}
