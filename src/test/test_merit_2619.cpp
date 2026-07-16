#include "test_merit_2619.h"
#include "map/merit.h"
#include "map/merit_index.h"
#include <iostream>
auto runMerit2619SelfTests() -> bool
{
    bool ok = true;
    auto expect = [&](bool value, const char* label) { if (!value) std::cerr << "merit 2619 failed: " << label << '\n'; ok = value && ok; };
    expect(GetMeritCategory(MERIT_MAX_HP) == 0 && GetMeritID(MERIT_MAX_HP) == 0, "hp index");
    expect(GetMeritCategory(MERIT_MAX_MERIT) == 0 && GetMeritID(MERIT_MAX_MERIT) == 2, "max merit index");
    expect(GetMeritCategory(MERIT_STR) == 1 && GetMeritID(MERIT_STR) == 0, "attribute index");
    expect(GetMeritCategory(MERIT_HANDBELL) == 3 && GetMeritID(MERIT_HANDBELL) == 13, "magic tail index");
    return ok;
}
