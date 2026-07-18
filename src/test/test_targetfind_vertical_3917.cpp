#include "test_targetfind_vertical_3917.h"
#include "map/ai/helpers/targetfind_vertical_capacity.h"
#include <iostream>
namespace { auto expect(bool v, const char* l) -> bool { if (!v) std::cerr << "targetfind vertical 3917 failed: " << l << '\n'; return v; } }
auto runTargetfindVertical3917SelfTests() -> bool
{
    using targetfindverticalhelpers::ExceedsVerticalRange;
    bool ok = true;
    ok = expect(!ExceedsVerticalRange(7.99f, false, false), "standard below cap") && ok;
    ok = expect(ExceedsVerticalRange(8.0f, false, false), "standard inclusive cap") && ok;
    ok = expect(ExceedsVerticalRange(8.0f, true, false), "non-mob self centered standard cap") && ok;
    ok = expect(!ExceedsVerticalRange(8.49f, true, true), "mob self centered below raised cap") && ok;
    ok = expect(ExceedsVerticalRange(8.5f, true, true), "mob self centered inclusive cap") && ok;
    return ok;
}
