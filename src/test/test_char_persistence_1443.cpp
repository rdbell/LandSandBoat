#include "test_char_persistence_1443.h"

#include "map/char_persistence_capacity.h"

#include <chrono>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

auto runCharPersistence1443SelfTests() -> bool
{
    constexpr uint8 equipFlag    = 0x01;
    constexpr uint8 positionFlag = 0x02;
    constexpr uint8 effectsFlag  = 0x04;

    uint8 pending = 0;
    charpersistencehelpers::Request(pending, equipFlag);
    charpersistencehelpers::Request(pending, effectsFlag);
    charpersistencehelpers::Request(pending, positionFlag);
    bool ok = pending == (equipFlag | positionFlag | effectsFlag);

    std::unordered_set<std::string> changes{ "QuestFlag" };
    std::vector<std::string>        calls{};
    const auto flush = [&]()
    {
        return charpersistencehelpers::Flush(
            changes,
            pending,
            equipFlag,
            positionFlag,
            effectsFlag,
            [&](const std::string& name) { calls.push_back("var:" + name); },
            [&]() { calls.push_back("equip"); },
            [&]() { calls.push_back("look"); },
            [&]() { calls.push_back("position"); },
            [&]() { calls.push_back("effects:true"); });
    };

    ok = flush() && pending == 0 && changes.empty() &&
         calls == std::vector<std::string>{ "var:QuestFlag", "equip", "look", "position", "effects:true" } && ok;

    changes.insert("OnlyVar");
    calls.clear();
    ok = flush() && pending == 0 && changes.empty() &&
         calls == std::vector<std::string>{ "var:OnlyVar" } && ok;

    pending = 0x80;
    calls.clear();
    ok = flush() && pending == 0 && calls.empty() && ok;
    ok = !flush() && calls.empty() && ok;

    using namespace std::chrono_literals;
    auto next = std::chrono::seconds{ 20 };
    unsigned flushCalls = 0;
    ok = !charpersistencehelpers::FlushAt(
             19s,
             next,
             120s,
             [&]()
             {
                 ++flushCalls;
                 return true;
             }) &&
         next == 20s && flushCalls == 0 && ok;

    ok = !charpersistencehelpers::FlushAt(
             20s,
             next,
             120s,
             [&]()
             {
                 ++flushCalls;
                 return false;
             }) &&
         next == 20s && flushCalls == 1 && ok;

    ok = charpersistencehelpers::FlushAt(
             20s,
             next,
             120s,
             [&]()
             {
                 ++flushCalls;
                 return true;
             }) &&
         next == 140s && flushCalls == 2 && ok;

    if (!ok)
    {
        std::cerr << "char persistence 1443 self-test failed\n";
    }
    return ok;
}
