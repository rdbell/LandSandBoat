#include "test_login_auth_account_id_1309.h"

#include "login/auth_password.h"

#include <cstdint>
#include <iostream>
#include <limits>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "login auth account id 1309 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runLoginAuthAccountID1309SelfTests() -> bool
{
    bool ok = true;

    ok = expect(loginHelpers::MinAccountID == 1000, "MinAccountID is 1000") && ok;

    // Empty table: COALESCE(MAX(id),0) = 0 → candidate 1 → floor 1000.
    ok = expect(loginHelpers::NextAccountID(0) == 1000, "empty table floors to 1000") && ok;

    // Just below floor: max 998 → 999 → floor 1000; max 999 → 1000.
    ok = expect(loginHelpers::NextAccountID(998) == 1000, "998 floors to 1000") && ok;
    ok = expect(loginHelpers::NextAccountID(999) == 1000, "999 yields 1000") && ok;

    // At and above floor: no further adjustment.
    ok = expect(loginHelpers::NextAccountID(1000) == 1001, "1000 yields 1001") && ok;
    ok = expect(loginHelpers::NextAccountID(5000) == 5001, "mid-range increments") && ok;

    // uint32 wrap parity with LSB unchecked +1.
    // max + 1 wraps to 0, then floors to 1000.
    ok = expect(loginHelpers::NextAccountID(std::numeric_limits<uint32>::max()) == 1000,
                "max uint32 wraps then floors to 1000") &&
         ok;
    // max-1 + 1 = max (>= 1000), unchanged.
    ok = expect(loginHelpers::NextAccountID(std::numeric_limits<uint32>::max() - 1) == std::numeric_limits<uint32>::max(),
                "max-1 yields max") &&
         ok;

    return ok;
}
