#include "test_tp_return_1562.h"

#include "map/tp_return_capacity.h"

#include <iostream>

namespace
{
using tpreturnhelpers::CalculateTPReturn;
using tpreturnhelpers::IsCharmedPCPet;
using tpreturnhelpers::UsePCOrPetTPFormula;

auto Check() -> bool
{
    // Formula selection
    if (!UsePCOrPetTPFormula(false, false) || !UsePCOrPetTPFormula(false, true))
    {
        return false;
    }
    if (UsePCOrPetTPFormula(true, false) || !UsePCOrPetTPFormula(true, true))
    {
        return false;
    }
    if (!IsCharmedPCPet(true, true, true) || IsCharmedPCPet(true, true, false) ||
        IsCharmedPCPet(true, false, true) || IsCharmedPCPet(false, true, true))
    {
        return false;
    }

    // PC band pins (parity with Go tp_return_test)
    if (CalculateTPReturn(true, 0) != 29 || CalculateTPReturn(true, 96) != 46 ||
        CalculateTPReturn(true, 180) != 61 || CalculateTPReturn(true, 540) != 149 ||
        CalculateTPReturn(true, 720) != 161 || CalculateTPReturn(true, 900) != 173 ||
        CalculateTPReturn(true, 1080) != 187)
    {
        return false;
    }

    // Mob band pins (LSB discontinuity: 530→155, 531→~145)
    if (CalculateTPReturn(false, 0) != 35 || CalculateTPReturn(false, 180) != 50 ||
        CalculateTPReturn(false, 450) != 115 || CalculateTPReturn(false, 480) != 130 ||
        CalculateTPReturn(false, 530) != 155)
    {
        return false;
    }

    // Boundary: just above band edges
    if (CalculateTPReturn(true, 181) != 61 || CalculateTPReturn(true, 541) != 149 ||
        CalculateTPReturn(false, 181) != 50 || CalculateTPReturn(false, 531) != 145)
    {
        return false;
    }

    return true;
}
} // namespace

auto runTPReturn1562SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "tp_return_1562 self-tests failed\n";
        return false;
    }
    return true;
}
