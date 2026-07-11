#include "test_daken_sange_ammo_1401.h"

#include "map/daken_sange_ammo_capacity.h"

#include <iostream>
#include <vector>

namespace
{
auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "Daken Sange ammo 1401 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runDakenSangeAmmo1401SelfTests() -> bool
{
    using namespace dakensangeammohelpers;
    bool ok = true;

    auto run = [](uint8 type, bool container, bool sange, bool character, bool shuriken, std::vector<int>& calls) {
        return ShouldRemoveShuriken(
            type, container,
            [&]() { calls.push_back(1); return sange; },
            [&]() { calls.push_back(2); return character; },
            [&]() { calls.push_back(3); return shuriken; });
    };

    std::vector<int> calls;
    ok = expect(!run(0, true, true, true, true, calls) && calls.empty(), "non-Daken lazy") && ok;
    calls.clear();
    ok = expect(!run(AttackTypeDaken, false, true, true, true, calls) && calls.empty(), "no container") && ok;
    calls.clear();
    ok = expect(!run(AttackTypeDaken, true, false, true, true, calls) && calls == std::vector<int>{ 1 }, "no Sange") && ok;
    calls.clear();
    ok = expect(!run(AttackTypeDaken, true, true, false, true, calls) && calls == std::vector<int>{ 1, 2 }, "non-character") && ok;
    calls.clear();
    ok = expect(!run(AttackTypeDaken, true, true, true, false, calls) && calls == std::vector<int>{ 1, 2, 3 }, "non-shuriken ammo") && ok;
    calls.clear();
    ok = expect(run(AttackTypeDaken, true, true, true, true, calls) && calls == std::vector<int>{ 1, 2, 3 }, "remove one") && ok;
    ok = expect(AttackTypeDaken == 9, "Daken pin") && ok;
    return ok;
}
