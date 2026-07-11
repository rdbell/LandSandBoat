#include "test_login_auth_version_error_1308.h"

#include "login/auth_password.h"

#include <array>
#include <cstdint>
#include <iostream>
#include <limits>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "login auth version error 1308 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runLoginAuthVersionError1308SelfTests() -> bool
{
    const std::array<uint8_t, 3> old{ 1, 0, 0 };
    const bool canonical = expect(
        loginHelpers::FormatUnsupportedXiloaderVersionError(old) ==
            "Your xiloader is too old.\nPlease update to version '2.1.x'.\nYour client reported '1.0.0'.",
        "canonical unsupported text");

    const std::array<uint8_t, 3> patchIgnored{ SupportedXiloaderVersion[0], SupportedXiloaderVersion[1], 99 };
    // Formatter is independent of whether the version would pass validation;
    // it always embeds supported major.minor and the three reported components.
    const bool patchRendered = expect(
        loginHelpers::FormatUnsupportedXiloaderVersionError(patchIgnored) ==
            "Your xiloader is too old.\nPlease update to version '2.1.x'.\nYour client reported '2.1.99'.",
        "client patch component rendered");

    const std::array<uint8_t, 3> maxVals{
        std::numeric_limits<uint8_t>::max(),
        std::numeric_limits<uint8_t>::max(),
        std::numeric_limits<uint8_t>::max(),
    };
    const bool maxText = expect(
        loginHelpers::FormatUnsupportedXiloaderVersionError(maxVals) ==
            "Your xiloader is too old.\nPlease update to version '2.1.x'.\nYour client reported '255.255.255'.",
        "unsigned byte boundaries");

    return canonical && patchRendered && maxText;
}
