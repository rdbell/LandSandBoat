#include "test_login_auth_account_create_1310.h"

#include "login/auth_password.h"
#include "omega_self_test_registry.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "login auth account create 1310 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runLoginAuthAccountCreate1310SelfTests() -> bool
{
    using settings_gate = loginHelpers::account_create_settings_gate;
    using lookup_gate   = loginHelpers::account_create_lookup_gate;

    bool ok = true;

    ok = expect(loginHelpers::ClassifyAccountCreationSettings(true) == settings_gate::ALLOW, "enabled allows") && ok;
    ok = expect(loginHelpers::ClassifyAccountCreationSettings(false) == settings_gate::DISABLED, "disabled rejects") && ok;

    const auto allowResult = loginHelpers::LoginResultForAccountCreateSettings(settings_gate::ALLOW);
    ok = expect(!allowResult.has_value(), "ALLOW has no early result") && ok;

    const auto disabledResult = loginHelpers::LoginResultForAccountCreateSettings(settings_gate::DISABLED);
    ok = expect(disabledResult.has_value() && *disabledResult == login_result::LOGIN_ERROR_CREATE_DISABLED,
                "DISABLED maps to CREATE_DISABLED") &&
         ok;

    ok = expect(loginHelpers::ClassifyAccountCreateLookup(false, 0) == lookup_gate::QUERY_FAILED, "query fail") && ok;
    ok = expect(loginHelpers::ClassifyAccountCreateLookup(false, 99) == lookup_gate::QUERY_FAILED,
                "query fail ignores rowsCount") &&
         ok;
    ok = expect(loginHelpers::ClassifyAccountCreateLookup(true, 0) == lookup_gate::PROCEED, "empty rows proceed") && ok;
    ok = expect(loginHelpers::ClassifyAccountCreateLookup(true, 1) == lookup_gate::TAKEN, "one row taken") && ok;
    ok = expect(loginHelpers::ClassifyAccountCreateLookup(true, 5) == lookup_gate::TAKEN, "many rows taken") && ok;

    ok = expect(!loginHelpers::LoginResultForAccountCreateLookup(lookup_gate::PROCEED).has_value(), "PROCEED no result") && ok;

    const auto taken = loginHelpers::LoginResultForAccountCreateLookup(lookup_gate::TAKEN);
    ok = expect(taken.has_value() && *taken == login_result::LOGIN_ERROR_CREATE_TAKEN, "TAKEN maps") && ok;

    const auto failed = loginHelpers::LoginResultForAccountCreateLookup(lookup_gate::QUERY_FAILED);
    ok = expect(failed.has_value() && *failed == login_result::LOGIN_ERROR_CREATE, "QUERY_FAILED maps") && ok;

    ok = expect(loginHelpers::FormatAccountCreateDisabledWarning("new-user") ==
                    "login_parse: New account attempt <new-user> but is disabled in settings.",
                "disabled account warning") &&
         ok;

    return ok;
}

OMEGA_REGISTER_SELF_TEST("login-account-create-8220", runLoginAuthAccountCreate1310SelfTests);
