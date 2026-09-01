#include "test_scheduler_spawn_8721.h"

#include "common/scheduler.h"
#include "omega_self_test_registry.h"

#include <atomic>
#include <iostream>
#include <thread>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "scheduler spawn 8721 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runSchedulerSpawn8721SelfTests() -> bool
{
    Scheduler scheduler(1);
    std::atomic<bool> mainStarted{ false };
    auto mainTask = scheduler.spawnOnMainThread([&]()
                                                {
                                                    mainStarted.store(true);
                                                    return 17;
                                                });

    bool ok = true;
    ok      = expect(!mainStarted.load(), "main spawn remains lazy") && ok;
    const auto mainResult = scheduler.blockOnMainThread(std::move(mainTask));
    ok                    = expect(mainStarted.load(), "main spawn executes") && ok;
    ok                    = expect(mainResult == 17, "main spawn returns result") && ok;

    std::atomic<bool> workerStarted{ false };
    std::atomic<bool> workerUsedDifferentThread{ false };
    const auto caller = std::this_thread::get_id();
    auto workerTask   = scheduler.spawnOnWorkerThread([&]()
                                                      {
                                                          workerStarted.store(true);
                                                          workerUsedDifferentThread.store(std::this_thread::get_id() != caller);
                                                          return 23;
                                                      });
    ok                = expect(!workerStarted.load(), "worker spawn remains lazy") && ok;
    const auto workerResult = scheduler.blockOnMainThread(std::move(workerTask));
    ok                     = expect(workerStarted.load(), "worker spawn executes") && ok;
    ok                     = expect(workerUsedDifferentThread.load(), "worker spawn uses worker executor") && ok;
    ok                     = expect(workerResult == 23, "worker spawn returns result") && ok;
    return ok;
}

OMEGA_REGISTER_SELF_TEST("scheduler-spawn-8721", runSchedulerSpawn8721SelfTests);
