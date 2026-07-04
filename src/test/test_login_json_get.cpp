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

#include "test_login_json_get.h"

#include "login/login_helpers.h"

#include <array>
#include <iostream>
#include <string>

namespace
{

template <typename T>
auto expectValue(const Maybe<T>& actual, const T& expected, const std::string& label) -> bool
{
    if (!actual || *actual != expected)
    {
        std::cerr << "login jsonGet self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

template <typename T>
auto expectMissing(const Maybe<T>& actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "login jsonGet self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

auto makeDocument() -> json
{
    return json::parse(R"JSON(
{
  "command": 5,
  "negative": -1,
  "ratio": 1.5,
  "integer": 1,
  "username": "Omega",
  "enabled": true,
  "bool_as_number": 1,
  "string_number": "1",
  "version": [1, 2, 3],
  "signed_numbers": [-1, 2, 3],
  "float_numbers": [1.75, -2.25, 3],
  "wrong_size": [1, 2],
  "mixed_type": [1, true, 3],
  "unsigned_rejects_float": [1, 2.5, 3],
  "not_array": 1
}
)JSON");
}

auto testScalarJSONGet(const json& document) -> bool
{
    bool ok = true;

    ok = expectValue(loginHelpers::jsonGet<std::string>(document, "username"), std::string("Omega"), "string") && ok;
    ok = expectValue(loginHelpers::jsonGet<bool>(document, "enabled"), true, "bool") && ok;
    ok = expectValue(loginHelpers::jsonGet<uint8>(document, "command"), static_cast<uint8>(5), "uint8") && ok;
    ok = expectValue(loginHelpers::jsonGet<int8>(document, "command"), static_cast<int8>(5), "signed scalar accepts unsigned number") && ok;
    ok = expectValue(loginHelpers::jsonGet<float>(document, "ratio"), 1.5F, "float") && ok;

    ok = expectMissing(loginHelpers::jsonGet<std::string>(document, "command"), "string rejects number") && ok;
    ok = expectMissing(loginHelpers::jsonGet<bool>(document, "bool_as_number"), "bool rejects number") && ok;
    ok = expectMissing(loginHelpers::jsonGet<double>(document, "integer"), "float rejects integer") && ok;
    ok = expectMissing(loginHelpers::jsonGet<int8>(document, "negative"), "signed scalar rejects signed number") && ok;
    ok = expectMissing(loginHelpers::jsonGet<uint8>(document, "negative"), "unsigned rejects signed number") && ok;
    ok = expectMissing(loginHelpers::jsonGet<uint8>(document, "missing"), "missing key") && ok;
    ok = expectMissing(loginHelpers::jsonGet<uint8>(document, "string_number"), "number rejects string") && ok;

    return ok;
}

auto testArrayJSONGet(const json& document) -> bool
{
    bool ok = true;

    const auto version       = std::array<uint8, 3>{ 1, 2, 3 };
    const auto signedVersion = std::array<int8, 3>{ 1, 2, 3 };
    const auto signedNumbers = std::array<int8, 3>{ -1, 2, 3 };
    const auto floatNumbers  = std::array<int8, 3>{ 1, -2, 3 };

    ok = expectValue(loginHelpers::jsonGet<uint8, 3>(document, "version"), version, "uint8 array") && ok;
    ok = expectValue(loginHelpers::jsonGet<int8, 3>(document, "version"), signedVersion, "signed array accepts unsigned") && ok;
    ok = expectValue(loginHelpers::jsonGet<int8, 3>(document, "signed_numbers"), signedNumbers, "signed array accepts signed") && ok;
    ok = expectValue(loginHelpers::jsonGet<int8, 3>(document, "float_numbers"), floatNumbers, "signed array accepts floats") && ok;

    ok = expectMissing(loginHelpers::jsonGet<uint8, 3>(document, "signed_numbers"), "unsigned array rejects signed") && ok;
    ok = expectMissing(loginHelpers::jsonGet<uint8, 3>(document, "unsigned_rejects_float"), "unsigned array rejects float") && ok;
    ok = expectMissing(loginHelpers::jsonGet<uint8, 3>(document, "wrong_size"), "array wrong size") && ok;
    ok = expectMissing(loginHelpers::jsonGet<uint8, 3>(document, "mixed_type"), "array mixed type") && ok;
    ok = expectMissing(loginHelpers::jsonGet<uint8, 3>(document, "not_array"), "not array") && ok;
    ok = expectMissing(loginHelpers::jsonGet<uint8, 3>(document, "missing"), "missing array") && ok;

    return ok;
}

} // namespace

auto runLoginJSONGetSelfTests() -> bool
{
    const auto document = makeDocument();

    bool ok = true;
    ok = testScalarJSONGet(document) && ok;
    ok = testArrayJSONGet(document) && ok;

    return ok;
}
