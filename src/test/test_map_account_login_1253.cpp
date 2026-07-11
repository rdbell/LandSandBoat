#include "test_map_account_login_1253.h"

#include "map/account_login.h"

#include "common/blowfish.h"

#include <iostream>

namespace
{

struct FakeSession
{
    bool       forceLinkDead{};
    blowfish_t blowfish{};
    blowfish_t prev_blowfish{};
};

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "map AccountLogin 1253 self-test failed: " << label << '\n';
    }
    return condition;
}

auto testMissingSessionDoesNothing() -> bool
{
    int lookupCalls = 0;
    int wordCalls   = 0;
    int hashCalls   = 0;
    uint32 lookedUpId{};

    mapipc::HandleAccountLogin(
        ipc::AccountLogin{ .accountId = 77 },
        [&](const uint32 accountId) -> FakeSession*
        {
            ++lookupCalls;
            lookedUpId = accountId;
            return nullptr;
        },
        [&]
        {
            ++wordCalls;
            return uint32{ 1 };
        },
        [&]
        {
            ++hashCalls;
            return uint16{ 1 };
        });

    return expect(lookupCalls == 1 && lookedUpId == 77, "missing lookup once by account ID") &&
           expect(wordCalls == 0, "missing session word RNG unused") &&
           expect(hashCalls == 0, "missing session hash RNG unused");
}

auto testExactInvalidationOrder() -> bool
{
    FakeSession session{};
    session.blowfish.status      = BLOWFISH_ACCEPTED;
    session.prev_blowfish.status = BLOWFISH_PENDING_ZONE;

    uint32 wordCalls = 0;
    uint16 hashCalls = 0;
    bool   flagAlwaysSet = true;

    mapipc::HandleAccountLogin(
        ipc::AccountLogin{ .accountId = 88 },
        [&](const uint32 accountId)
        {
            return accountId == 88 ? &session : nullptr;
        },
        [&]
        {
            flagAlwaysSet = flagAlwaysSet && session.forceLinkDead;
            return ++wordCalls;
        },
        [&]
        {
            flagAlwaysSet = flagAlwaysSet && session.forceLinkDead;
            return static_cast<uint16>(254 + hashCalls++);
        });

    bool ok = true;
    ok = expect(session.forceLinkDead, "force-link-dead set") && ok;
    ok = expect(flagAlwaysSet, "flag set before every RNG call") && ok;
    ok = expect(wordCalls == 2094, "exact word RNG count") && ok;
    ok = expect(hashCalls == 32, "exact hash RNG count") && ok;
    ok = expect(session.blowfish.key[0] == 1 && session.blowfish.key[4] == 5, "current key order") && ok;
    ok = expect(session.prev_blowfish.key[0] == 6 && session.prev_blowfish.key[4] == 10, "previous key order") && ok;
    ok = expect(session.blowfish.P[0] == 11 && session.blowfish.P[17] == 28, "current P order") && ok;
    ok = expect(session.prev_blowfish.P[0] == 29 && session.prev_blowfish.P[17] == 46, "previous P order") && ok;
    for (uint16 index = 0; index < 16; ++index)
    {
        ok = expect(session.blowfish.hash[index] == (254 + index) % 255, "current hash modulo/order") && ok;
        ok = expect(session.prev_blowfish.hash[index] == (270 + index) % 255, "previous hash modulo/order") && ok;
    }
    for (uint32 box = 0; box < 4; ++box)
    {
        const auto currentFirst  = 47 + box * 512;
        const auto previousFirst = currentFirst + 256;
        ok = expect(session.blowfish.S[box][0] == currentFirst && session.blowfish.S[box][255] == currentFirst + 255,
                    "current S-box order") &&
             ok;
        ok = expect(session.prev_blowfish.S[box][0] == previousFirst && session.prev_blowfish.S[box][255] == previousFirst + 255,
                    "previous S-box order") &&
             ok;
    }
    ok = expect(session.blowfish.status == BLOWFISH_ACCEPTED, "current status preserved") && ok;
    ok = expect(session.prev_blowfish.status == BLOWFISH_PENDING_ZONE, "previous status preserved") && ok;
    return ok;
}

} // namespace

auto runMapAccountLogin1253SelfTests() -> bool
{
    return testMissingSessionDoesNothing() && testExactInvalidationOrder();
}
