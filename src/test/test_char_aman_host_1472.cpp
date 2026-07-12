#include "test_char_aman_host_1472.h"

#include "map/char_aman_host_capacity.h"

#include <iostream>
#include <optional>

auto runCharAMANHost1472SelfTests() -> bool
{
    std::optional<int> container{};
    int                factoryCalls = 0;
    auto& first = charamanhosthelpers::Get(container, [&]()
    {
        ++factoryCalls;
        return 17;
    });
    first = 23;
    auto& second = charamanhosthelpers::Get(container, [&]()
    {
        ++factoryCalls;
        return 99;
    });

    const bool ok = factoryCalls == 1 && &first == &second && second == 23;
    if (!ok)
    {
        std::cerr << "char AMAN host 1472 self-test failed\n";
    }
    return ok;
}
