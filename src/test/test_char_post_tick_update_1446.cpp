#include "test_char_post_tick_update_1446.h"

#include "map/char_post_tick_update_capacity.h"

#include <chrono>
#include <iostream>
#include <vector>

auto runCharPostTickUpdate1446SelfTests() -> bool
{
    using namespace std::chrono_literals;

    bool ok = !charposttickupdatehelpers::UpdateDue(0x00, 21000ms, 20000ms) &&
              !charposttickupdatehelpers::UpdateDue(0x01, 20000ms, 20000ms) &&
              charposttickupdatehelpers::UpdateDue(0x01, 21000ms, 20000ms);

    auto             nextUpdate       = 20000ms;
    uint8            updateMask       = 0x85;
    bool             sendServerStatus = true;
    bool             charmed          = false;
    std::vector<int> calls{};
    charposttickupdatehelpers::Apply(
        21000ms,
        nextUpdate,
        updateMask,
        sendServerStatus,
        0x01,
        0x04,
        250ms,
        false,
        [&]()
        {
            calls.push_back(1);
            charmed = true;
            return true;
        },
        [&](uint8 mask)
        {
            calls.push_back(mask == 0x85 ? 2 : 20);
        },
        [&]()
        {
            calls.push_back(3);
            return charmed;
        },
        [&](uint8 mask)
        {
            calls.push_back(mask == 0x85 ? 4 : 40);
        },
        [&]()
        {
            calls.push_back(5);
        },
        [&]()
        {
            calls.push_back(6);
        },
        [&]()
        {
            calls.push_back(updateMask == 0 && !sendServerStatus ? 7 : 70);
        });
    ok = nextUpdate == 21250ms && updateMask == 0 && !sendServerStatus &&
         calls == std::vector<int>{ 1, 2, 3, 4, 5, 6, 7 } && ok;

    nextUpdate       = 20000ms;
    updateMask       = 0x01;
    sendServerStatus = false;
    calls.clear();
    charposttickupdatehelpers::Apply(
        20000ms,
        nextUpdate,
        updateMask,
        sendServerStatus,
        0x01,
        0x04,
        250ms,
        false,
        [&]()
        {
            calls.push_back(10);
            return true;
        },
        [&](uint8)
        {
            calls.push_back(11);
        },
        [&]()
        {
            calls.push_back(12);
            return true;
        },
        [&](uint8)
        {
            calls.push_back(13);
        },
        [&]()
        {
            calls.push_back(14);
        },
        [&]()
        {
            calls.push_back(15);
        },
        [&]()
        {
            calls.push_back(16);
        });
    ok = nextUpdate == 20000ms && updateMask == 0x01 && !sendServerStatus &&
         calls == std::vector<int>{ 16 } && ok;

    nextUpdate       = 20000ms;
    updateMask       = 0x01;
    sendServerStatus = false;
    calls.clear();
    charposttickupdatehelpers::Apply(
        21000ms,
        nextUpdate,
        updateMask,
        sendServerStatus,
        0x01,
        0x04,
        250ms,
        true,
        [&]()
        {
            calls.push_back(20);
            return true;
        },
        [&](uint8)
        {
            calls.push_back(21);
        },
        [&]()
        {
            calls.push_back(22);
            return false;
        },
        [&](uint8)
        {
            calls.push_back(23);
        },
        [&]()
        {
            calls.push_back(24);
        },
        [&]()
        {
            calls.push_back(25);
        },
        [&]()
        {
            calls.push_back(26);
        });
    ok = nextUpdate == 21250ms && updateMask == 0 && !sendServerStatus &&
         calls == std::vector<int>{ 20, 22, 26 } && ok;

    nextUpdate       = 20000ms;
    updateMask       = 0x01;
    sendServerStatus = true;
    calls.clear();
    charposttickupdatehelpers::Apply(
        21000ms,
        nextUpdate,
        updateMask,
        sendServerStatus,
        0x01,
        0x04,
        250ms,
        false,
        []()
        {
            return false;
        },
        [&](uint8)
        {
            calls.push_back(30);
        },
        []()
        {
            return false;
        },
        [&](uint8)
        {
            calls.push_back(31);
        },
        [&]()
        {
            calls.push_back(32);
        },
        [&]()
        {
            calls.push_back(33);
        },
        [&]()
        {
            calls.push_back(34);
        });
    ok = calls == std::vector<int>{ 33, 34 } && ok;

    nextUpdate       = 20000ms;
    updateMask       = 0;
    sendServerStatus = true;
    calls.clear();
    charposttickupdatehelpers::Apply(
        21000ms,
        nextUpdate,
        updateMask,
        sendServerStatus,
        0x01,
        0x04,
        250ms,
        false,
        [&]()
        {
            calls.push_back(40);
            return true;
        },
        [&](uint8)
        {
            calls.push_back(41);
        },
        [&]()
        {
            calls.push_back(42);
            return true;
        },
        [&](uint8)
        {
            calls.push_back(43);
        },
        [&]()
        {
            calls.push_back(44);
        },
        [&]()
        {
            calls.push_back(45);
        },
        [&]()
        {
            calls.push_back(46);
        });
    ok = nextUpdate == 20000ms && updateMask == 0 && sendServerStatus &&
         calls == std::vector<int>{ 46 } && ok;

    if (!ok)
    {
        std::cerr << "char post-tick update 1446 self-test failed\n";
    }
    return ok;
}
