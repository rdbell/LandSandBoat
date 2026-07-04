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

#include "test_c2s_packet_validation_scalars.h"

#include <cstdint>
#include <iostream>
#include <set>
#include <string>

#include "map/packets/c2s/validation.h"

namespace
{

enum class TestMode : std::uint8_t
{
    One = 1,
    Two = 2,
};

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s packet validation scalar self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectFalse(bool actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "c2s packet validation scalar self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectString(const std::string& actual, const std::string& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s packet validation scalar self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

auto expectInvalidError(const PacketValidationResult& result, const std::string& expected, const std::string& label) -> bool
{
    bool ok = true;
    ok      = expectFalse(result.valid(), label + " validity") && ok;
    ok      = expectString(result.errorString(), expected, label + " error string") && ok;
    return ok;
}

auto testResultAggregation() -> bool
{
    auto result = PacketValidationResult();

    bool ok = true;
    ok      = expectTrue(result.valid(), "default result is valid") && ok;
    ok      = expectString(result.errorString(), "", "default error string") && ok;

    result.addError("first").addError("second");

    ok = expectFalse(result.valid(), "result invalid after addError") && ok;
    ok = expectString(result.errorString(), "first, second", "aggregated error string") && ok;
    return ok;
}

auto testScalarPasses() -> bool
{
    auto allowedModes = std::set<TestMode>{ TestMode::One, TestMode::Two };
    auto customCalled = false;
    auto validator    = PacketValidator(nullptr);

    validator.mustEqual(std::uint8_t{ 7 }, 7, "byte mismatch")
        .mustNotEqual(TestMode::One, TestMode::Two, "mode unexpectedly equal")
        .range("Count", std::uint16_t{ 8 }, 1, 16)
        .multipleOf("Param1", std::uint16_t{ 32 }, 16)
        .oneOf("Mode", TestMode::Two, allowedModes)
        .custom(
            [&](PacketValidator& pv)
            {
                customCalled = true;
                pv.mustEqual(true, true, "custom bool failed");
            });

    auto result = static_cast<PacketValidationResult>(validator);

    bool ok = true;
    ok      = expectTrue(customCalled, "custom called for valid chain") && ok;
    ok      = expectTrue(result.valid(), "scalar pass result valid") && ok;
    ok      = expectString(result.errorString(), "", "scalar pass error string") && ok;
    return ok;
}

auto testStaticCastStyleConversions() -> bool
{
    auto value     = 7;
    auto validator = PacketValidator(nullptr);

    validator.mustEqual(std::uint8_t{ 255 }, -1, "signed to unsigned wrap failed")
        .mustEqual(true, std::int8_t{ -2 }, "numeric to bool cast failed")
        .mustEqual(std::uint8_t{ 1 }, true, "bool to numeric cast failed")
        .mustEqual(static_cast<int*>(nullptr), nullptr, "nil pointer cast failed")
        .mustNotEqual(&value, nullptr, "non-nil pointer matched nil")
        .range("Mode", TestMode::Two, 1, 3)
        .range("Wrapped", std::uint8_t{ 255 }, -2, -1)
        .multipleOf("Step", std::uint8_t{ 32 }, std::int16_t{ 16 });

    auto result = static_cast<PacketValidationResult>(validator);

    bool ok = true;
    ok      = expectTrue(result.valid(), "static-cast style conversions valid") && ok;
    ok      = expectString(result.errorString(), "", "static-cast style conversion error string") && ok;
    return ok;
}

auto testScalarFailures() -> bool
{
    bool ok = true;

    {
        auto validator = PacketValidator(nullptr);
        validator.mustEqual(1, 2, "left mismatch");
        ok = expectInvalidError(static_cast<PacketValidationResult>(validator), "left mismatch", "mustEqual") && ok;
    }

    {
        auto validator = PacketValidator(nullptr);
        validator.mustNotEqual(1, 1, "left matched right");
        ok = expectInvalidError(static_cast<PacketValidationResult>(validator), "left matched right", "mustNotEqual") && ok;
    }

    {
        auto validator = PacketValidator(nullptr);
        validator.range("Count", 17, 1, 16);
        ok = expectInvalidError(static_cast<PacketValidationResult>(validator), "Count out of range: 17 not in [1, 16]", "range") && ok;
    }

    {
        auto validator = PacketValidator(nullptr);
        validator.multipleOf("Param1", 18, 16);
        ok = expectInvalidError(static_cast<PacketValidationResult>(validator), "Param1 is not a multiple of 16.", "multipleOf") && ok;
    }

    {
        auto validator = PacketValidator(nullptr);
        validator.oneOf("Mode", static_cast<TestMode>(4), std::set<TestMode>{ TestMode::One, TestMode::Two });
        ok = expectInvalidError(static_cast<PacketValidationResult>(validator), "Mode value 4 is not allowed.", "oneOf set") && ok;
    }

    {
        auto validator = PacketValidator(nullptr);
        validator.custom(
            [](PacketValidator& pv)
            {
                pv.mustEqual(1, 2, "custom failed");
            });
        ok = expectInvalidError(static_cast<PacketValidationResult>(validator), "custom failed", "custom") && ok;
    }

    return ok;
}

auto testShortCircuitOrder() -> bool
{
    auto customCalled = false;
    auto validator    = PacketValidator(nullptr);

    validator.mustEqual(1, 2, "first")
        .mustEqual(2, 3, "second")
        .custom(
            [&](PacketValidator& pv)
            {
                customCalled = true;
                pv.mustEqual(3, 4, "custom");
            });

    bool ok = true;
    ok      = expectInvalidError(static_cast<PacketValidationResult>(validator), "first", "short-circuit first error") && ok;
    ok      = expectFalse(customCalled, "custom skipped after prior error") && ok;
    return ok;
}

auto testCustomShortCircuitOrder() -> bool
{
    auto customCalled = false;
    auto validator    = PacketValidator(nullptr);

    validator.custom(
        [&](PacketValidator& pv)
        {
            customCalled = true;
            pv.mustEqual(1, 2, "custom first")
                .range("Value", 99, 0, 1);
        })
        .mustEqual(3, 4, "after custom");

    bool ok = true;
    ok      = expectTrue(customCalled, "custom called before error") && ok;
    ok      = expectInvalidError(static_cast<PacketValidationResult>(validator), "custom first", "custom short-circuit first error") && ok;
    return ok;
}

} // namespace

auto runC2SPacketValidationScalarSelfTests() -> bool
{
    return testResultAggregation() &&
           testScalarPasses() &&
           testStaticCastStyleConversions() &&
           testScalarFailures() &&
           testShortCircuitOrder() &&
           testCustomShortCircuitOrder();
}
