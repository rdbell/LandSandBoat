#include "test_char_add_item_currency_plan_6906.h"
#include "map/char_add_item_currency_plan.h"
#include <iostream>
auto runCharAddItemCurrencyPlan6906SelfTests() -> bool { const bool ok = additemcurrencyhelpers::BuildPlan(true) == additemcurrencyhelpers::Plan{ true, 0, 0 } && additemcurrencyhelpers::BuildPlan(false) == additemcurrencyhelpers::Plan{}; if (!ok) std::cerr << "add item currency plan 6906 self-test failed\n"; return ok; }
