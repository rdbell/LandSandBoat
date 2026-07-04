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

#include "test_database_binding.h"

#include "common/database/binding.h"
#include "common/database/blob.h"

#include <array>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace
{

enum class TestEnum : uint16
{
    Value = 16,
};

template <typename T, typename U>
auto expectEqual(const T& actual, const U& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "Database binding self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

template <typename T, typename U>
auto expectVariant(const db::BoundValue& value, const U& expected, const std::string& label) -> bool
{
    if (!std::holds_alternative<T>(value))
    {
        std::cerr << "Database binding self-test failed: " << label << " type" << '\n';
        return false;
    }

    return expectEqual(std::get<T>(value), expected, label);
}

auto testScalarLowering() -> bool
{
    int         plainInt = -5;
    auto        text     = std::string("hello");
    const char* literal  = "literal";
    char        mutableText[] = "mutable";
    auto        blobBytes     = std::array<uint8, 11>{ '\0', '\'', '"', '\b', '\n', '\r', '\t', '\x1A', '\\', '%', 'A' };
    const auto  wideSize      = static_cast<size_t>(std::numeric_limits<uint32>::max()) + 7U;

    auto params = db::detail::lowerBoundValues(
        plainInt,
        static_cast<int8>(-8),
        static_cast<uint8>(8),
        static_cast<int16>(-16),
        TestEnum::Value,
        static_cast<int32>(-32),
        static_cast<uint32>(32),
        wideSize,
        true,
        1.25F,
        2.5,
        text,
        literal,
        mutableText,
        blobBytes);

    bool ok = true;
    ok      = expectEqual(params.size(), static_cast<std::size_t>(15), "parameter count") && ok;

    ok = expectVariant<int32>(params[0], static_cast<int32>(-5), "plain int lowers as int32") && ok;
    ok = expectVariant<int8>(params[1], static_cast<int8>(-8), "int8") && ok;
    ok = expectVariant<uint8>(params[2], static_cast<uint8>(8), "uint8") && ok;
    ok = expectVariant<int16>(params[3], static_cast<int16>(-16), "int16") && ok;
    ok = expectVariant<uint16>(params[4], static_cast<uint16>(16), "enum lowers as uint16") && ok;
    ok = expectVariant<int32>(params[5], static_cast<int32>(-32), "int32") && ok;
    ok = expectVariant<uint32>(params[6], static_cast<uint32>(32), "uint32") && ok;
    ok = expectVariant<uint32>(params[7], static_cast<uint32>(6), "size_t lowers as uint32") && ok;
    ok = expectVariant<bool>(params[8], true, "bool") && ok;
    ok = expectVariant<float>(params[9], 1.25F, "float") && ok;
    ok = expectVariant<double>(params[10], 2.5, "double") && ok;
    ok = expectVariant<std::string>(params[11], std::string("hello"), "std::string") && ok;
    ok = expectVariant<std::string>(params[12], std::string("literal"), "const char pointer") && ok;
    ok = expectVariant<std::string>(params[13], std::string("mutable"), "char pointer") && ok;

    if (!std::holds_alternative<std::shared_ptr<db::BlobWrapper>>(params[14]))
    {
        std::cerr << "Database binding self-test failed: blob type" << '\n';
        ok = false;
    }
    else
    {
        const auto blob = std::get<std::shared_ptr<db::BlobWrapper>>(params[14]);
        ok = expectEqual(blob->size, static_cast<std::size_t>(11), "blob size") && ok;
        ok = expectEqual(blob->toString(), std::string(R"(\0\'\"\b\n\r\t\Z\\%%A)"), "blob escaped string") && ok;
    }

    blobBytes[0] = 'x';
    if (std::holds_alternative<std::shared_ptr<db::BlobWrapper>>(params[14]))
    {
        const auto blob = std::get<std::shared_ptr<db::BlobWrapper>>(params[14]);
        ok = expectEqual(blob->toString(), std::string(R"(\0\'\"\b\n\r\t\Z\\%%A)"), "blob owns copied bytes") && ok;
    }

    return ok;
}

} // namespace

auto runDatabaseBindingSelfTests() -> bool
{
    return testScalarLowering();
}
