#include "test_char_add_item_request_plan_6905.h"
#include "map/char_add_item_request_plan.h"
#include <iostream>
auto runCharAddItemRequestPlan6905SelfTests() -> bool { using additemrequesthelpers::Decision; const bool ok = additemrequesthelpers::BuildPlan(0, 1, true) == Decision::Reject && additemrequesthelpers::BuildPlan(1, 0, true) == Decision::Reject && additemrequesthelpers::BuildPlan(1, 1, false) == Decision::MissingItem && additemrequesthelpers::BuildPlan(1, 1, true) == Decision::SetQuantityAndDelegate; if (!ok) std::cerr << "add item request plan 6905 self-test failed\n"; return ok; }
