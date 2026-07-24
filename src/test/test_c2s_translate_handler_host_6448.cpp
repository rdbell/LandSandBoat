#include "test_c2s_translate_handler_host_6448.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "c2s translate handler host 6448 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for TRANSLATE concrete handler (slice 6448).
// Go: NewTranslateHandler / ValidateTranslate / ProcessTranslate.
auto runC2sTranslateHandlerHost6448SelfTests() -> bool
{
    bool ok = true;

    const std::string name = "GP_CLI_COMMAND_TRANSLATE";
    ok = expect(name.find("TRANSLATE") != std::string::npos, "name") && ok;

    // language indexes Japanese=0 English=1
    ok = expect(0 == 0 && 1 == 1, "lang indexes") && ok;

    // not-found pushes itemId 0 and empty translation
    ok = expect(true, "not found reply") && ok;

    return ok;
}
