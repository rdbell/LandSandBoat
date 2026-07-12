#include "test_char_highest_job_1460.h"

#include "map/char_highest_job_capacity.h"

#include <array>
#include <cstdint>
#include <iostream>

auto runCharHighestJob1460SelfTests() -> bool
{
    std::array<std::uint8_t, 24> levels{};
    bool ok = charhighestjobhelpers::HighestLevel(levels) == 0;

    levels[0]  = 99;
    levels[1]  = 50;
    levels[23] = 98;
    ok = charhighestjobhelpers::HighestLevel(levels) == 99 && ok;

    levels[0]  = 1;
    levels[23] = 100;
    ok = charhighestjobhelpers::HighestLevel(levels) == 100 && ok;

    if (!ok)
    {
        std::cerr << "char highest job 1460 self-test failed\n";
    }
    return ok;
}
