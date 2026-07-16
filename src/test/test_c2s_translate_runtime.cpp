#include "test_c2s_translate_runtime.h"
#include "map/packets/c2s/0x02b_translate.h"
#include <iostream>

auto runC2STranslateRuntimeSelfTests() -> bool
{
    const auto found   = translatehelpers::BuildResponsePlan(true, 123, "potion");
    const auto missing = translatehelpers::BuildResponsePlan(false, 123, "ignored");
    const auto ok      = found.itemID == 123 && found.translation == "potion" && missing.itemID == 0 && missing.translation.empty();
    if (!ok)
        std::cerr << "c2s TRANSLATE runtime self-test failed\n";
    return ok;
}
