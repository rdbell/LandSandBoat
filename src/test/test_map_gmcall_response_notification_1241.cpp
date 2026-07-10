#include "test_map_gmcall_response_notification_1241.h"

#include "map/gmcall_response_notification.h"

#include <iostream>
#include <string>

namespace
{

struct FakeCharacter
{
    uint32 id;
};

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "map GM-call response notification 1241 self-test failed: " << label << '\n';
    }
    return condition;
}

auto testLocalCharacterIsRefreshed() -> bool
{
    FakeCharacter  character{ .id = 77 };
    uint32         lookedUpId         = 0;
    int            lookupCount        = 0;
    int            refreshCount       = 0;
    FakeCharacter* refreshedCharacter = nullptr;

    mapipc::HandleGMCallResponse(
        ipc::GMCallResponse{
            .callId  = 123,
            .charId  = character.id,
            .message = "notification body is not authoritative",
        },
        [&](const uint32 charId)
        {
            ++lookupCount;
            lookedUpId = charId;
            return &character;
        },
        [&](FakeCharacter* target)
        {
            ++refreshCount;
            refreshedCharacter = target;
        });

    return expect(lookupCount == 1 && lookedUpId == character.id, "lookup by character ID") &&
           expect(refreshCount == 1 && refreshedCharacter == &character, "refresh local character once");
}

auto testMissingCharacterIsIgnored() -> bool
{
    uint32 lookedUpId   = 0;
    int    refreshCount = 0;

    mapipc::HandleGMCallResponse(
        ipc::GMCallResponse{
            .callId  = 456,
            .charId  = 88,
            .message = "queued for a different map process",
        },
        [&](const uint32 charId) -> FakeCharacter*
        {
            lookedUpId = charId;
            return nullptr;
        },
        [&](FakeCharacter*)
        {
            ++refreshCount;
        });

    return expect(lookedUpId == 88, "missing lookup character ID") &&
           expect(refreshCount == 0, "missing character is not refreshed");
}

auto testZeroCharacterIdIsStillLookedUp() -> bool
{
    FakeCharacter  character{};
    int            lookupCount        = 0;
    int            refreshCount       = 0;
    FakeCharacter* refreshedCharacter = nullptr;

    mapipc::HandleGMCallResponse(
        ipc::GMCallResponse{
            .callId  = 5,
            .charId  = 0,
            .message = "zero is still routed",
        },
        [&](const uint32 charId)
        {
            ++lookupCount;
            if (charId != 0)
            {
                return static_cast<FakeCharacter*>(nullptr);
            }
            return &character;
        },
        [&](FakeCharacter* target)
        {
            ++refreshCount;
            refreshedCharacter = target;
        });

    return expect(lookupCount == 1, "zero-ID lookup once") &&
           expect(refreshCount == 1 && refreshedCharacter == &character, "zero-ID refresh once");
}

} // namespace

auto runMapGMCallResponseNotification1241SelfTests() -> bool
{
    bool ok = true;
    ok      = testLocalCharacterIsRefreshed() && ok;
    ok      = testMissingCharacterIsIgnored() && ok;
    ok      = testZeroCharacterIdIsStillLookedUp() && ok;
    return ok;
}
