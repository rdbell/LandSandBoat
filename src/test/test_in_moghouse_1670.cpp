#include "test_in_moghouse_1670.h"

#include "map/in_moghouse_capacity.h"

#include <iostream>

namespace
{
using namespace inmoghousehelpers;

auto Check() -> bool
{
    if (InMogHouse(false, 1) || InMogHouse(false, 0))
    {
        return false;
    }
    if (InMogHouse(true, 0))
    {
        return false;
    }
    if (!InMogHouse(true, 1) || !InMogHouse(true, 99))
    {
        return false;
    }
    return true;
}
} // namespace

auto runInMogHouse1670SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "in_moghouse_1670 self-tests failed\n";
        return false;
    }
    return true;
}
