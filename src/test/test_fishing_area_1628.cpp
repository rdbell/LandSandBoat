#include "test_fishing_area_1628.h"

#include "map/fishing_area_capacity.h"

#include <iostream>

namespace
{
using namespace fishingareahelpers;

auto Check() -> bool
{
    // Segment mid
    {
        const AreaVector p{ 0, 0, 0 };
        const AreaVector r{ 10, 0, 0 };
        const AreaVector q{ 5, 0, 0 };
        if (!OnSegment(p, q, r))
        {
            return false;
        }
    }
    // Orientation colinear
    if (Orientation(AreaVector{ 0, 0, 0 }, AreaVector{ 1, 0, 0 }, AreaVector{ 2, 0, 0 }) != 0)
    {
        return false;
    }
    // Cylinder center inside
    {
        const AreaVector c{ 0, 0, 0 };
        const AreaVector p{ 0, 0, 0 };
        if (!IsInsideCylinder(c, p, 5, 4))
        {
            return false;
        }
        const AreaVector out{ 10, 0, 0 };
        if (IsInsideCylinder(c, out, 5, 4))
        {
            return false;
        }
    }
    // Poly: unit square around origin
    {
        const AreaVector poly[4] = {
            { 0, 0, 0 },
            { 10, 0, 0 },
            { 10, 0, 10 },
            { 0, 0, 10 },
        };
        const AreaVector inside{ 5, 0, 5 };
        const AreaVector outside{ 20, 0, 5 };
        if (!IsInsidePoly(poly, 4, inside, 0, 10) || IsInsidePoly(poly, 4, outside, 0, 10))
        {
            return false;
        }
    }

    if (!RejectFishingAreaInMogHouse(true) || RejectFishingAreaInMogHouse(false))
    {
        return false;
    }
    if (!AreaTypeMatches(AreaTypeWholeZone, false, false))
    {
        return false;
    }
    if (!AreaTypeMatches(AreaTypeRadial, true, false) || AreaTypeMatches(AreaTypeRadial, false, false))
    {
        return false;
    }
    if (!AreaTypeMatches(AreaTypePoly, false, true) || AreaTypeMatches(AreaTypePoly, false, false))
    {
        return false;
    }
    return true;
}
} // namespace

auto runFishingArea1628SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "fishing_area_1628 self-tests failed\n";
        return false;
    }
    return true;
}
