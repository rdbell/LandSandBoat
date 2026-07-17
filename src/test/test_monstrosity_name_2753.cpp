#include "test_monstrosity_name_2753.h"

#include "monstrosity_name.h"

#include <iostream>

auto runMonstrosityName2753SelfTests() -> bool
{
    if (monstrosity::PackName(0x010C, 0x12, 0x34) != 0x3412810C ||
        monstrosity::PackName(0x0001, 0x00, 0x00) != 0x00008001 ||
        monstrosity::PackName(0xFFFF, 0xFF, 0xFF) != 0xFFFFFFFF)
    {
        std::cerr << "monstrosity name self-test failed\n";
        return false;
    }
    return true;
}
