/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses/

===========================================================================
*/

#include "test_scheduler_combinators.h"

#include "common/scheduler.h"

#include <asio/multiple_exceptions.hpp>

#include <atomic>
#include <chrono>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <variant>

namespace
{

using namespace std::chrono_literals;

static_assert(!std::is_copy_constructible_v<Task<int>>);
static_assert(!std::is_copy_assignable_v<Task<int>>);
static_assert(std::is_move_constructible_v<Task<int>>);

auto expectTrue(const bool value, const std::string& label) -> bool
{
    if (!value)
    {
        std::cerr << "scheduler combinator self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectInt(const int actual, const int expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "scheduler combinator self-test failed: " << label
                  << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

void updateMaximum(std::atomic<int>& maximum, const int current)
{
    auto old = maximum.load();
    while (current > old && !maximum.compare_exchange_weak(old, current))
    {
    }
}

auto firstExceptionMessage(const std::exception_ptr& exception) -> std::string
{
    try
    {
        std::rethrow_exception(exception);
    }
    catch (const asio::multiple_exceptions& multiple)
    {
        return firstExceptionMessage(multiple.first_exception());
    }
    catch (const std::exception& error)
    {
        return error.what();
    }
    catch (...)
    {
        return "non-standard exception";
    }
}

auto firstExceptionIsCancellation(const std::exception_ptr& exception) -> bool
{
    try
    {
        std::rethrow_exception(exception);
    }
    catch (const asio::multiple_exceptions& multiple)
    {
        return firstExceptionIsCancellation(multiple.first_exception());
    }
    catch (const asio::system_error& error)
    {
        return error.code() == asio::error::operation_aborted;
    }
    catch (...)
    {
        return false;
    }
}

auto delayedValue(const int value, const std::chrono::steady_clock::duration delay) -> Task<int>
{
    co_await Scheduler::yieldFor(delay);
    co_return value;
}

auto immediateValue(const int value) -> Task<int>
{
    co_return value;
}

auto overlappingValue(const int value, const std::chrono::steady_clock::duration delay, std::atomic<int>& active, std::atomic<int>& maximum) -> Task<int>
{
    const auto current = active.fetch_add(1) + 1;
    updateMaximum(maximum, current);
    co_await Scheduler::yieldFor(delay);
    active.fetch_sub(1);
    co_return value;
}

auto delayedFailure(std::string message, const std::chrono::steady_clock::duration delay) -> Task<int>
{
    co_await Scheduler::yieldFor(delay);
    throw std::runtime_error(std::move(message));
}

auto cancellationProbe(std::atomic<bool>& started, std::atomic<bool>& canceled) -> Task<int>
{
    started.store(true);
    try
    {
        co_await Scheduler::yieldFor(1s);
    }
    catch (const asio::system_error& error)
    {
        if (error.code() == asio::error::operation_aborted)
        {
            canceled.store(true);
        }
        throw;
    }
    co_return 0;
}

auto failureAfterStart(std::atomic<bool>& otherStarted, std::string message) -> Task<int>
{
    while (!otherStarted.load())
    {
        co_await Scheduler::yield();
    }
    throw std::runtime_error(std::move(message));
}

auto valueAfterStart(const int value, std::atomic<bool>& otherStarted) -> Task<int>
{
    while (!otherStarted.load())
    {
        co_await Scheduler::yield();
    }
    co_return value;
}

auto groupMember(const bool shouldThrow, std::atomic<bool>& building, std::atomic<bool>& startedDuringBuild, std::atomic<int>& active, std::atomic<int>& maximum, std::atomic<int>& completed) -> Task<void>
{
    if (building.load())
    {
        startedDuringBuild.store(true);
    }
    const auto current = active.fetch_add(1) + 1;
    updateMaximum(maximum, current);
    co_await Scheduler::yieldFor(3ms);
    active.fetch_sub(1);
    completed.fetch_add(1);
    if (shouldThrow)
    {
        throw std::runtime_error("ignored task-group failure");
    }
}

auto testAllOrderingConcurrencyAndFailure() -> bool
{
    Scheduler        scheduler(1);
    std::atomic<int> active{ 0 };
    std::atomic<int> maximum{ 0 };
    const auto       result = scheduler.blockOnMainThread(
        All(overlappingValue(10, 8ms, active, maximum), overlappingValue(20, 1ms, active, maximum)));

    bool ok = true;
    ok      = expectInt(std::get<0>(result), 10, "All first input result") && ok;
    ok      = expectInt(std::get<1>(result), 20, "All second input result") && ok;
    ok      = expectInt(maximum.load(), 2, "All concurrent task count") && ok;

    std::atomic<bool> siblingStarted{ false };
    std::atomic<bool> siblingCanceled{ false };
    try
    {
        static_cast<void>(scheduler.blockOnMainThread(
            All(failureAfterStart(siblingStarted, "all failure"), cancellationProbe(siblingStarted, siblingCanceled))));
        ok = expectTrue(false, "All failure propagates") && ok;
    }
    catch (...)
    {
        ok = expectTrue(firstExceptionMessage(std::current_exception()) == "all failure",
                        "All first-input exception selection") &&
             ok;
    }
    ok = expectTrue(siblingCanceled.load(), "All drains canceled sibling") && ok;

    std::atomic<bool> firstStarted{ false };
    std::atomic<bool> firstCanceled{ false };
    try
    {
        static_cast<void>(scheduler.blockOnMainThread(
            All(cancellationProbe(firstStarted, firstCanceled), failureAfterStart(firstStarted, "later failure"))));
        ok = expectTrue(false, "All multiple failures propagate") && ok;
    }
    catch (...)
    {
        ok = expectTrue(firstExceptionIsCancellation(std::current_exception()),
                        "All multiple failures prefer lower input cancellation") &&
             ok;
    }
    ok = expectTrue(firstCanceled.load(), "All lower input cancellation observed") && ok;
    return ok;
}

auto testOneSuccessCancellationAndFailures() -> bool
{
    Scheduler scheduler(1);
    bool      ok = true;

    std::atomic<bool> nestedStarted1{ false };
    std::atomic<bool> nestedStarted2{ false };
    std::atomic<bool> nestedStarted3{ false };
    std::atomic<bool> nestedCanceled1{ false };
    std::atomic<bool> nestedCanceled2{ false };
    std::atomic<bool> nestedCanceled3{ false };
    auto              immediateWinner = scheduler.blockOnMainThread(
        One(immediateValue(77),
            cancellationProbe(nestedStarted1, nestedCanceled1),
            cancellationProbe(nestedStarted2, nestedCanceled2),
            cancellationProbe(nestedStarted3, nestedCanceled3)));
    ok = expectInt(static_cast<int>(immediateWinner.index()), 0, "One immediate winner input index") && ok;
    ok = expectInt(std::get<0>(immediateWinner), 77, "One immediate winner value") && ok;
    ok = expectTrue(nestedStarted1.load() && nestedStarted2.load() && nestedStarted3.load(),
                    "One immediate winner starts all nested losers") &&
         ok;
    ok = expectTrue(nestedCanceled1.load() && nestedCanceled2.load() && nestedCanceled3.load(),
                    "One immediate winner drains all nested losers") &&
         ok;

    auto afterFailure = scheduler.blockOnMainThread(One(delayedFailure("ignored failure", 1ms), delayedValue(9, 4ms)));
    ok                = expectInt(static_cast<int>(afterFailure.index()), 1, "One ignores failure index") && ok;
    ok                = expectInt(std::get<1>(afterFailure), 9, "One later success value") && ok;

    std::atomic<bool> loserStarted{ false };
    std::atomic<bool> loserCanceled{ false };
    auto              winner = scheduler.blockOnMainThread(
        One(cancellationProbe(loserStarted, loserCanceled), valueAfterStart(42, loserStarted)));
    ok = expectInt(static_cast<int>(winner.index()), 1, "One winner input index") && ok;
    ok = expectInt(std::get<1>(winner), 42, "One winner value") && ok;
    ok = expectTrue(loserCanceled.load(), "One drains canceled loser") && ok;

    try
    {
        static_cast<void>(scheduler.blockOnMainThread(
            One(delayedFailure("slow failure", 8ms), delayedFailure("fast failure", 1ms))));
        ok = expectTrue(false, "One all failures propagate") && ok;
    }
    catch (...)
    {
        ok = expectTrue(firstExceptionMessage(std::current_exception()) == "fast failure",
                        "One all failures use completion order") &&
             ok;
    }

    // One is a right fold: the inner pair does not complete until both of its
    // failures settle, so the outer failure wins despite not being the first
    // leaf failure.
    try
    {
        static_cast<void>(scheduler.blockOnMainThread(
            One(delayedFailure("outer middle failure", 5ms),
                delayedFailure("inner fast failure", 1ms),
                delayedFailure("inner slow failure", 10ms))));
        ok = expectTrue(false, "One right-fold failures propagate") && ok;
    }
    catch (...)
    {
        ok = expectTrue(firstExceptionMessage(std::current_exception()) == "outer middle failure",
                        "One right-fold all-failure selection") &&
             ok;
    }
    return ok;
}

auto testTaskGroupDeferralConcurrencyAndExceptions() -> bool
{
    Scheduler         scheduler(1);
    std::atomic<bool> building{ true };
    std::atomic<bool> startedDuringBuild{ false };
    std::atomic<int>  active{ 0 };
    std::atomic<int>  maximum{ 0 };
    std::atomic<int>  completed{ 0 };

    scheduler.blockOnMainThread(Scheduler::TaskGroup(
        2,
        [&](auto add)
        {
            add(groupMember(false, building, startedDuringBuild, active, maximum, completed));
            add(groupMember(true, building, startedDuringBuild, active, maximum, completed));
            building.store(false);
        }));

    bool ok = true;
    ok      = expectTrue(!startedDuringBuild.load(), "TaskGroup defers start until build returns") && ok;
    ok      = expectInt(maximum.load(), 2, "TaskGroup concurrent task count") && ok;
    ok      = expectInt(completed.load(), 2, "TaskGroup waits for throwing task") && ok;

    scheduler.blockOnMainThread(Scheduler::TaskGroup(0, [](auto)
                                                     {
                                                     }));
    ok = expectTrue(true, "empty TaskGroup completes") && ok;
    return ok;
}

auto testWithTimeoutCompletionCancellationAndFailures() -> bool
{
    Scheduler scheduler(1);
    bool      ok = true;

    const auto completed = scheduler.blockOnMainThread(Scheduler::withTimeout(delayedValue(7, 1ms), 30ms));
    ok                   = expectTrue(completed.has_value(), "withTimeout completed value present") && ok;
    ok                   = expectInt(completed.value_or(0), 7, "withTimeout completed value") && ok;

    std::atomic<bool> taskStarted{ false };
    std::atomic<bool> taskCanceled{ false };
    const auto        timedOut = scheduler.blockOnMainThread(
        Scheduler::withTimeout(cancellationProbe(taskStarted, taskCanceled), 1ms));
    ok = expectTrue(!timedOut.has_value(), "withTimeout timer wins") && ok;
    ok = expectTrue(taskStarted.load(), "withTimeout task starts") && ok;
    ok = expectTrue(taskCanceled.load(), "withTimeout drains canceled task") && ok;

    const auto before = std::chrono::steady_clock::now();
    const auto failed = scheduler.blockOnMainThread(
        Scheduler::withTimeout(delayedFailure("suppressed timeout failure", 1ms), 8ms));
    const auto elapsed = std::chrono::steady_clock::now() - before;
    ok                 = expectTrue(!failed.has_value(), "withTimeout suppresses failed task") && ok;
    ok                 = expectTrue(elapsed >= 4ms, "withTimeout failed task waits for timer success") && ok;

    std::atomic<bool> negativeStarted{ false };
    std::atomic<bool> negativeCanceled{ false };
    const auto        negative = scheduler.blockOnMainThread(
        Scheduler::withTimeout(cancellationProbe(negativeStarted, negativeCanceled), -1ms));
    ok = expectTrue(!negative.has_value(), "withTimeout negative duration expires") && ok;
    ok = expectTrue(negativeCanceled.load(), "withTimeout negative duration drains task") && ok;
    return ok;
}

} // namespace

auto runSchedulerCombinatorSelfTests() -> bool
{
    try
    {
        bool ok = true;
        ok      = testAllOrderingConcurrencyAndFailure() && ok;
        ok      = testOneSuccessCancellationAndFailures() && ok;
        ok      = testTaskGroupDeferralConcurrencyAndExceptions() && ok;
        ok      = testWithTimeoutCompletionCancellationAndFailures() && ok;
        return ok;
    }
    catch (const std::exception& error)
    {
        std::cerr << "scheduler combinator self-test failed with unexpected exception: " << error.what() << '\n';
        return false;
    }
    catch (...)
    {
        std::cerr << "scheduler combinator self-test failed with unexpected non-standard exception\n";
        return false;
    }
}
