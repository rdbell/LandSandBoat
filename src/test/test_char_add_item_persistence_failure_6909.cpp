#include "test_char_add_item_persistence_failure_6909.h"
#include "map/char_add_item_persistence_failure.h"
#include <iostream>
auto runCharAddItemPersistenceFailure6909SelfTests() -> bool { const bool ok = additempersistencefailurehelpers::BuildPlan(false) == additempersistencefailurehelpers::Plan{ true, true, 0xFF } && additempersistencefailurehelpers::BuildPlan(true) == additempersistencefailurehelpers::Plan{}; if (!ok) std::cerr << "add item persistence failure 6909 self-test failed\n"; return ok; }
