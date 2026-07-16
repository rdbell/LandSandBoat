#include "test_c2s_itemsearch_runtime.h"

#include <iostream>

#include "map/packets/c2s/0x02c_itemsearch.h"

auto runC2SItemSearchRuntimeSelfTests() -> bool
{
    const auto found   = itemsearchhelpers::BuildResponsePlan(true, 123, "potion");
    const auto missing = itemsearchhelpers::BuildResponsePlan(false, 123, "potion");
    const auto ok      = found.itemID == 123 && found.inputName == "potion" && missing.itemID == 0 && missing.inputName == "potion";
    if (!ok)
    {
        std::cerr << "c2s ITEMSEARCH runtime self-test failed\n";
    }
    return ok;
}
