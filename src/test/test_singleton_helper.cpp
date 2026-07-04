/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses/

===========================================================================
*/

#include "test_singleton_helper.h"

#include "common/singleton.h"

#include <iostream>
#include <string>

namespace
{

class LocalSingleton : public Singleton<LocalSingleton>
{
public:
    int value = 0;

    static int constructions;
    static int destructions;

protected:
    LocalSingleton()
    : value(++constructions)
    {
    }

public:
    ~LocalSingleton()
    {
        ++destructions;
    }
};

int LocalSingleton::constructions = 0;
int LocalSingleton::destructions  = 0;

template <typename T, typename U>
auto expectEqual(const T actual, const U expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "singleton helper self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

auto expectTrue(const bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "singleton helper self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

auto testSingletonLifecycle() -> bool
{
    bool ok = true;

    LocalSingleton::delInstance();
    LocalSingleton::constructions = 0;
    LocalSingleton::destructions  = 0;

    auto* first = LocalSingleton::getInstance();
    ok          = expectTrue(first != nullptr, "first non-null") && ok;
    ok          = expectEqual(LocalSingleton::constructions, 1, "first construction count") && ok;
    ok          = expectEqual(first->value, 1, "first value") && ok;

    first->value = 42;
    auto* second = LocalSingleton::getInstance();
    ok           = expectTrue(second == first, "same pointer before delete") && ok;
    ok           = expectEqual(second->value, 42, "state preserved") && ok;
    ok           = expectEqual(LocalSingleton::constructions, 1, "second construction count") && ok;

    LocalSingleton::delInstance();
    ok = expectEqual(LocalSingleton::destructions, 1, "destruction count") && ok;

    auto* third = LocalSingleton::getInstance();
    ok          = expectTrue(third != nullptr, "third non-null") && ok;
    ok          = expectEqual(LocalSingleton::constructions, 2, "third construction count") && ok;
    ok          = expectEqual(third->value, 2, "third value") && ok;

    LocalSingleton::delInstance();
    ok = expectEqual(LocalSingleton::destructions, 2, "final destruction count") && ok;

    return ok;
}

} // namespace

auto runSingletonHelperSelfTests() -> bool
{
    return testSingletonLifecycle();
}
