#include "test_char_trust_roster_1442.h"

#include "map/char_trust_roster_capacity.h"

#include <iostream>
#include <vector>

namespace
{
struct TestTrust
{
    int  id{};
    bool spawned{};
};
} // namespace

auto runCharTrustRoster1442SelfTests() -> bool
{
    TestTrust first{ 1, true };
    TestTrust second{ 2, true };
    TestTrust sleeping{ 3, false };
    TestTrust outsider{ 4, true };

    std::vector<TestTrust*> trusts{ &first, &second, &sleeping };
    std::vector<int>        calls{};
    chartrustrosterhelpers::Remove(
        trusts,
        &second,
        [&](TestTrust* trust)
        {
            calls.push_back(10 + trust->id);
            return trust->spawned;
        },
        [&](TestTrust* trust) { calls.push_back(20 + trust->id); },
        [&]() { calls.push_back(trusts.size() == 2 ? 30 : 31); });

    bool ok = trusts == std::vector<TestTrust*>{ &first, &sleeping } &&
              calls == std::vector<int>{ 12, 22, 30 };

    calls.clear();
    chartrustrosterhelpers::Remove(
        trusts,
        &sleeping,
        [&](TestTrust* trust)
        {
            calls.push_back(40 + trust->id);
            return trust->spawned;
        },
        [&](TestTrust* trust) { calls.push_back(50 + trust->id); },
        [&]() { calls.push_back(60); });
    ok = trusts == std::vector<TestTrust*>{ &first, &sleeping } && calls == std::vector<int>{ 43 } && ok;

    calls.clear();
    chartrustrosterhelpers::Remove(
        trusts,
        &outsider,
        [](TestTrust* trust) { return trust->spawned; },
        [&](TestTrust* trust) { calls.push_back(70 + trust->id); },
        [&]() { calls.push_back(trusts.size() == 2 ? 80 : 81); });
    ok = trusts == std::vector<TestTrust*>{ &first, &sleeping } && calls == std::vector<int>{ 80 } && ok;

    trusts = { &first, &second, &first };
    calls.clear();
    chartrustrosterhelpers::Remove(
        trusts,
        &first,
        [](TestTrust* trust) { return trust->spawned; },
        [&](TestTrust* trust) { calls.push_back(90 + trust->id); },
        [&]() { calls.push_back(100 + static_cast<int>(trusts.size())); });
    ok = trusts == std::vector<TestTrust*>{ &second, &first } && calls == std::vector<int>{ 91, 102 } && ok;

    calls.clear();
    chartrustrosterhelpers::Clear(
        trusts,
        [&](TestTrust* trust) { calls.push_back(110 + trust->id); },
        [&]() { calls.push_back(trusts.empty() ? 120 : 121); });
    ok = trusts.empty() && calls == std::vector<int>{ 112, 111, 120 } && ok;

    calls.clear();
    chartrustrosterhelpers::Clear(
        trusts,
        [&](TestTrust* trust) { calls.push_back(130 + trust->id); },
        [&]() { calls.push_back(trusts.empty() ? 140 : 141); });
    ok = trusts.empty() && calls == std::vector<int>{ 140 } && ok;

    if (!ok)
    {
        std::cerr << "char trust roster 1442 self-test failed\n";
    }
    return ok;
}
