#include "test_map_char_var_update_1256.h"

#include "map/char_var_update.h"

#include <iostream>
#include <limits>
#include <string>

namespace
{

struct FakeCharacter
{
    uint32 id{};
};

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "map CharVarUpdate 1256 self-test failed: " << label << '\n';
    }
    return condition;
}

auto testMissingCharacterDoesNothing() -> bool
{
    int    lookupCalls{};
    int    updateCalls{};
    uint32 lookedUpId{1};

    mapipc::HandleCharVarUpdate(
        ipc::CharVarUpdate{ .charId = 0, .value = -1, .expiry = 2, .varName = "missing" },
        [&](const uint32 charId) -> FakeCharacter*
        {
            ++lookupCalls;
            lookedUpId = charId;
            return nullptr;
        },
        [&](FakeCharacter*, const std::string&, const int32, const uint32)
        {
            ++updateCalls;
        });

    return expect(lookupCalls == 1 && lookedUpId == 0, "missing zero character ID looked up once") &&
           expect(updateCalls == 0, "missing character cache not updated");
}

auto testFoundCharacterReceivesExactPayload() -> bool
{
    FakeCharacter character{};
    int           lookupCalls{};
    int           updateCalls{};
    FakeCharacter* updatedCharacter{};
    std::string   updatedName{ "sentinel" };
    int32         updatedValue{};
    uint32        updatedExpiry{};
    const ipc::CharVarUpdate message{
        .charId  = 0,
        .value   = std::numeric_limits<int32>::min(),
        .expiry  = std::numeric_limits<uint32>::max(),
        .varName = "",
    };

    mapipc::HandleCharVarUpdate(
        message,
        [&](const uint32 charId) -> FakeCharacter*
        {
            ++lookupCalls;
            return charId == 0 ? &character : nullptr;
        },
        [&](FakeCharacter* found, const std::string& varName, const int32 value, const uint32 expiry)
        {
            ++updateCalls;
            updatedCharacter = found;
            updatedName      = varName;
            updatedValue     = value;
            updatedExpiry    = expiry;
        });

    return expect(lookupCalls == 1 && updateCalls == 1 && updatedCharacter == &character,
                  "found zero character updated exactly once") &&
           expect(updatedName.empty() && updatedValue == message.value && updatedExpiry == message.expiry,
                  "empty name and extreme payload applied unchanged");
}

auto testZeroValueAndExpiryOverwriteCache() -> bool
{
    FakeCharacter            character{};
    int                      updateCalls{};
    std::string              updatedName{};
    int32                    updatedValue{1};
    uint32                   updatedExpiry{1};
    const ipc::CharVarUpdate message{ .charId = 9, .value = 0, .expiry = 0, .varName = "zero" };

    mapipc::HandleCharVarUpdate(
        message,
        [&](const uint32 charId) -> FakeCharacter*
        {
            return charId == 9 ? &character : nullptr;
        },
        [&](FakeCharacter*, const std::string& varName, const int32 value, const uint32 expiry)
        {
            ++updateCalls;
            updatedName   = varName;
            updatedValue  = value;
            updatedExpiry = expiry;
        });

    return expect(updateCalls == 1 && updatedName == "zero" && updatedValue == 0 && updatedExpiry == 0,
                  "zero value and expiry overwrite cache without deletion");
}

} // namespace

auto runMapCharVarUpdate1256SelfTests() -> bool
{
    return testMissingCharacterDoesNothing() && testFoundCharacterReceivesExactPayload() &&
           testZeroValueAndExpiryOverwriteCache();
}
