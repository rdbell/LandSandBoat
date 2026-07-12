#include "test_char_moghancement_furniture_1462.h"

#include "map/char_moghancement_furniture_capacity.h"

#include <iostream>
#include <vector>

auto runCharMoghancementFurniture1462SelfTests() -> bool
{
    using charmoghancementfurniturehelpers::Furniture;
    using charmoghancementfurniturehelpers::Select;

    bool ok = Select(std::vector<Furniture>{}) == 0;
    ok = Select(std::vector<Furniture>{
             { false, false, 1, 100, 1, 101 },
             { true, true, 2, 100, 1, 102 },
         }) == 0 && ok;

    ok = Select(std::vector<Furniture>{
             { true, false, 1, 10, 5, 201 },
             { true, false, 1, 20, 9, 202 },
             { true, false, 2, 25, 1, 203 },
         }) == 202 && ok;

    ok = Select(std::vector<Furniture>{
             { true, false, 1, 10, 1, 301 },
             { true, false, 2, 10, 1, 302 },
         }) == 0 && ok;

    ok = Select(std::vector<Furniture>{
             { true, false, 1, 10, 1, 401 },
             { true, false, 2, 10, 1, 402 },
             { true, false, 3, 20, 1, 403 },
         }) == 403 && ok;

    ok = Select(std::vector<Furniture>{
             { true, false, 4, 30, 8, 501 },
             { true, false, 4, 30, 3, 502 },
             { true, false, 4, 30, 3, 503 },
         }) == 502 && ok;

    if (!ok)
    {
        std::cerr << "char moghancement furniture 1462 self-test failed\n";
    }
    return ok;
}
