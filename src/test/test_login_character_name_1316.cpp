#include "test_login_character_name_1316.h"

#include "login/character_name.h"
#include "omega_self_test_registry.h"

#include <cstring>
#include <iostream>
#include <string>
#include <vector>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "login character name 1316 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runLoginCharacterName1316SelfTests() -> bool
{
    using gate = loginHelpers::character_creation_gate;
    bool ok    = true;

    ok = expect(loginHelpers::CharacterNameMinLength == 3 && loginHelpers::CharacterNameMaxLength == 15,
                "length bounds") &&
         ok;
    ok = expect(loginHelpers::CharacterNameFieldBytes == 15, "field bytes") && ok;

    ok = expect(loginHelpers::ClassifyCharacterCreationGate(0, true) == gate::ALLOW, "creation allowed") && ok;
    ok = expect(loginHelpers::ClassifyCharacterCreationGate(1, true) == gate::DENIED, "maint denies") && ok;
    ok = expect(loginHelpers::ClassifyCharacterCreationGate(0, false) == gate::DENIED, "disabled denies") && ok;
    ok = expect(loginHelpers::ClassifyCharacterCreationGate(2, false) == gate::DENIED, "both deny") && ok;

    ok = expect(loginHelpers::IsAlphabeticCharacterName("Alice"), "alpha ok") && ok;
    ok = expect(loginHelpers::IsAlphabeticCharacterName("Bob"), "alpha short ok") && ok;
    ok = expect(!loginHelpers::IsAlphabeticCharacterName("Al1ce"), "digit rejected") && ok;
    ok = expect(!loginHelpers::IsAlphabeticCharacterName("Al ce"), "space rejected") && ok;
    ok = expect(loginHelpers::IsAlphabeticCharacterName(""), "empty alpha vacuously") && ok;

    ok = expect(loginHelpers::IsValidCharacterNameLength("Abc"), "len 3") && ok;
    ok = expect(loginHelpers::IsValidCharacterNameLength("Abcdefghijklmno"), "len 15") && ok;
    ok = expect(!loginHelpers::IsValidCharacterNameLength("Ab"), "len 2") && ok;
    ok = expect(!loginHelpers::IsValidCharacterNameLength("Abcdefghijklmnop"), "len 16") && ok;

    // Local validate overwrite: invalid chars then invalid length → length wins.
    const auto shortBad = loginHelpers::ValidateCharacterNameLocal("A1");
    ok = expect(shortBad.has_value() && *shortBad == loginHelpers::CharacterNameInvalidLengthReason,
                "length overwrites alpha") &&
         ok;

    const auto alphaOnly = loginHelpers::ValidateCharacterNameLocal("Al1ce");
    ok = expect(alphaOnly.has_value() && *alphaOnly == loginHelpers::CharacterNameInvalidCharactersReason,
                "alpha failure alone") &&
         ok;

    const auto good = loginHelpers::ValidateCharacterNameLocal("Alice");
    ok = expect(!good.has_value(), "valid name") && ok;

    ok = expect(loginHelpers::FormatBannedWordMatchReason("FOO") == "Name matched with bad words list <FOO>.",
                "banned format") &&
         ok;

    const auto banned = loginHelpers::FindBannedWordMatch("EVILNAME", { "GOOD", "EVIL", "NAME" });
    // last match wins: NAME is last substring hit among those that match...
    // EVIL matches first, NAME matches second (both substrings of EVILNAME) → NAME wins.
    ok = expect(banned.has_value() && *banned == "Name matched with bad words list <NAME>.",
                "last banned match wins") &&
         ok;

    const auto noBan = loginHelpers::FindBannedWordMatch("CLEAN", { "FOO", "BAR" });
    ok = expect(!noBan.has_value(), "no banned match") && ok;

    char field[16] = {};
    std::memcpy(field, "Tester", 6);
    ok = expect(loginHelpers::ExtractCharacterNameField(field) == "Tester", "extract field") && ok;

    // VIEW 0x22 applies database and banned-word outcomes in order, allowing
    // later outcomes to overwrite local and earlier query reasons.
    ok = expect(loginHelpers::ResolveCharacterNameInvalidReason(
                    loginHelpers::CharacterNameInvalidLengthReason,
                    true,
                    false,
                    false,
                    true,
                    false,
                    std::nullopt) == loginHelpers::CharacterNameInvalidLengthReason,
                "local reason retained") &&
         ok;
    ok = expect(loginHelpers::ResolveCharacterNameInvalidReason(
                    std::nullopt,
                    false,
                    false,
                    false,
                    true,
                    false,
                    std::nullopt) == loginHelpers::CharacterNameEntityQueryFailedReason,
                "entity query failure") &&
         ok;
    ok = expect(loginHelpers::ResolveCharacterNameInvalidReason(
                    loginHelpers::CharacterNameInvalidCharactersReason,
                    true,
                    true,
                    true,
                    false,
                    false,
                    std::nullopt) == loginHelpers::CharacterNameEntityQueryFailedNoPeriodReason,
                "mob query failure overwrites entity result") &&
         ok;
    ok = expect(loginHelpers::ResolveCharacterNameInvalidReason(
                    std::nullopt,
                    true,
                    false,
                    true,
                    true,
                    true,
                    std::nullopt) == loginHelpers::CharacterNameAlreadyInUseReason,
                "mob name taken") &&
         ok;
    ok = expect(loginHelpers::ResolveCharacterNameInvalidReason(
                    loginHelpers::CharacterNameInvalidCharactersReason,
                    false,
                    false,
                    true,
                    false,
                    false,
                    std::string("Name matched with bad words list <BAD>.") ) ==
                    "Name matched with bad words list <BAD>.",
                "banned reason wins") &&
         ok;

    return ok;
}

OMEGA_REGISTER_SELF_TEST("login-character-name-1316", runLoginCharacterName1316SelfTests);
