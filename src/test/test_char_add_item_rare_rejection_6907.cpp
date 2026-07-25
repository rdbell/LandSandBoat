#include "test_char_add_item_rare_rejection_6907.h"
#include "map/char_add_item_rare_rejection.h"
#include <iostream>
auto runCharAddItemRareRejection6907SelfTests() -> bool { const bool ok = additemrarerejectionhelpers::BuildPlan(true, true, false) == additemrarerejectionhelpers::Plan{ true, true, 0xFF } && additemrarerejectionhelpers::BuildPlan(true, true, true) == additemrarerejectionhelpers::Plan{ true, false, 0xFF } && additemrarerejectionhelpers::BuildPlan(true, false, false) == additemrarerejectionhelpers::Plan{}; if (!ok) std::cerr << "add item rare rejection 6907 self-test failed\n"; return ok; }
