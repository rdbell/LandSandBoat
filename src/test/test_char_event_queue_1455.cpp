#include "test_char_event_queue_1455.h"

#include "map/char_event_queue_capacity.h"

#include <iostream>
#include <vector>

namespace
{
struct TestEvent
{
    int id;
};
} // namespace

auto runCharEventQueue1455SelfTests() -> bool
{
    std::vector<int> calls{};
    chareventqueuehelpers::EndCurrent(
        [&]() { calls.push_back(1); },
        [&]() { calls.push_back(2); },
        [&]() { calls.push_back(3); },
        [&]() { calls.push_back(4); },
        [&]() { calls.push_back(5); },
        [&]() { calls.push_back(6); });
    bool ok = calls == std::vector<int>{ 1, 2, 3, 4, 5, 6 };

    TestEvent              first{ 10 };
    TestEvent              duplicate{ 10 };
    TestEvent              next{ 20 };
    std::vector<TestEvent*> queue{ &first };

    calls.clear();
    auto queued = chareventqueuehelpers::QueueEvent(
        queue,
        &duplicate,
        [](const TestEvent* event) { return event->id; },
        [&]() { calls.push_back(7); },
        [&](TestEvent*) { calls.push_back(8); },
        [&]() { calls.push_back(9); });
    ok = !queued && queue == std::vector<TestEvent*>{ &first } &&
         calls == std::vector<int>{ 7 } && ok;

    calls.clear();
    queued = chareventqueuehelpers::QueueEvent(
        queue,
        &next,
        [](const TestEvent* event) { return event->id; },
        [&]() { calls.push_back(10); },
        [&](TestEvent* event)
        {
            calls.push_back(event == &next ? 11 : 110);
            queue.push_back(event);
        },
        [&]() { calls.push_back(queue.size() == 2 ? 12 : 120); });
    ok = queued && queue == std::vector<TestEvent*>{ &first, &next } &&
         calls == std::vector<int>{ 11, 12 } && ok;

    if (!ok)
    {
        std::cerr << "char event queue 1455 self-test failed\n";
    }
    return ok;
}
