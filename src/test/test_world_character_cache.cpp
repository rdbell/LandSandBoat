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

#include "test_world_character_cache.h"

#include "world/character_cache.h"

#include <iostream>
#include <string>

namespace
{

template <typename T, typename U>
auto expectEqual(const T& actual, const U& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "world character cache self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

auto expectCachedEndpoint(CharacterCache& cache, const uint32 charId, const IPP& expected, const std::string& label) -> bool
{
    const auto maybeIPP = cache.getCharacterIPP(charId);
    if (!maybeIPP)
    {
        std::cerr << "world character cache self-test failed: " << label << " missing endpoint\n";
        return false;
    }

    return expectEqual(maybeIPP->getRawIPP(), expected.getRawIPP(), label);
}

auto testCharacterCacheLookupLifecycle() -> bool
{
    bool ok = true;

    CharacterCache cache;
    ok = expectEqual(cache.getCharacterIPP(1001).has_value(), false, "empty cache miss") && ok;

    const IPP firstEndpoint(str2ip("127.0.0.1"), 54230);
    const IPP secondEndpoint(str2ip("10.0.0.5"), 54000);

    cache.updateCharacter(1001, firstEndpoint);
    ok = expectCachedEndpoint(cache, 1001, firstEndpoint, "initial endpoint") && ok;

    cache.updateCharacter(1001, secondEndpoint);
    ok = expectCachedEndpoint(cache, 1001, secondEndpoint, "overwritten endpoint") && ok;

    cache.updateCharacter(1002, firstEndpoint);
    cache.removeCharacter(1001);
    cache.removeCharacter(1001);

    ok = expectEqual(cache.getCharacterIPP(1001).has_value(), false, "removed char miss") && ok;
    ok = expectCachedEndpoint(cache, 1002, firstEndpoint, "second char retained") && ok;

    return ok;
}

} // namespace

auto runWorldCharacterCacheSelfTests() -> bool
{
    bool ok = true;

    ok = testCharacterCacheLookupLifecycle() && ok;

    return ok;
}
