#include "test_char_add_item_insert_failure_6908.h"
#include "map/char_add_item_insert_failure.h"
#include <iostream>
auto runCharAddItemInsertFailure6908SelfTests() -> bool { const bool ok = additeminsertfailurehelpers::BuildPlan(0xFF) == additeminsertfailurehelpers::Plan{ true, true, 0xFF } && additeminsertfailurehelpers::BuildPlan(3) == additeminsertfailurehelpers::Plan{}; if (!ok) std::cerr << "add item insert failure 6908 self-test failed\n"; return ok; }
