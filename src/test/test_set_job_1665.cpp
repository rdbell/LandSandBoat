#include "test_set_job_1665.h"

#include "map/set_job_capacity.h"

#include <iostream>

namespace
{
using namespace setjobhelpers;

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "set job 1665 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runSetJob1665SelfTests() -> bool
{
    bool ok = true;

    // --- pin ---
    ok = expect(MaxJobType == 24, "MaxJobType pin") && ok;

    // --- CanSetMainJob boundaries: 0, 1, 23, 24, 255 ---
    ok = expect(!CanSetMainJob(0), "main 0 JOB_NON invalid") && ok;
    ok = expect(CanSetMainJob(1), "main 1 WAR valid") && ok;
    ok = expect(CanSetMainJob(2), "main 2 MNK valid") && ok;
    ok = expect(CanSetMainJob(12), "main 12 mid valid") && ok;
    ok = expect(CanSetMainJob(23), "main 23 last valid") && ok;
    ok = expect(!CanSetMainJob(24), "main 24 MAX_JOBTYPE invalid") && ok;
    ok = expect(!CanSetMainJob(25), "main 25 invalid") && ok;
    ok = expect(!CanSetMainJob(99), "main 99 invalid") && ok;
    ok = expect(!CanSetMainJob(255), "main 255 invalid") && ok;

    // --- CanSetSubJob boundaries: 0 allowed, 1, 23, 24, 255 ---
    ok = expect(CanSetSubJob(0), "sub 0 JOB_NON clear SJ valid") && ok;
    ok = expect(CanSetSubJob(1), "sub 1 WAR valid") && ok;
    ok = expect(CanSetSubJob(2), "sub 2 MNK valid") && ok;
    ok = expect(CanSetSubJob(12), "sub 12 mid valid") && ok;
    ok = expect(CanSetSubJob(23), "sub 23 last valid") && ok;
    ok = expect(!CanSetSubJob(24), "sub 24 MAX_JOBTYPE invalid") && ok;
    ok = expect(!CanSetSubJob(25), "sub 25 invalid") && ok;
    ok = expect(!CanSetSubJob(99), "sub 99 invalid") && ok;
    ok = expect(!CanSetSubJob(255), "sub 255 invalid") && ok;

    // --- ResolveMainJob ---
    {
        auto [acc0, val0] = ResolveMainJob(0);
        ok = expect(!acc0 && val0 == 0, "resolve main 0 reject") && ok;
        auto [acc1, val1] = ResolveMainJob(1);
        ok = expect(acc1 && val1 == 1, "resolve main 1 accept") && ok;
        auto [acc23, val23] = ResolveMainJob(23);
        ok = expect(acc23 && val23 == 23, "resolve main 23 accept") && ok;
        auto [acc24, val24] = ResolveMainJob(24);
        ok = expect(!acc24 && val24 == 24, "resolve main 24 reject") && ok;
        auto [acc255, val255] = ResolveMainJob(255);
        ok = expect(!acc255 && val255 == 255, "resolve main 255 reject") && ok;
    }

    // --- ResolveSubJob ---
    {
        auto [acc0, val0] = ResolveSubJob(0);
        ok = expect(acc0 && val0 == 0, "resolve sub 0 accept clear SJ") && ok;
        auto [acc1, val1] = ResolveSubJob(1);
        ok = expect(acc1 && val1 == 1, "resolve sub 1 accept") && ok;
        auto [acc23, val23] = ResolveSubJob(23);
        ok = expect(acc23 && val23 == 23, "resolve sub 23 accept") && ok;
        auto [acc24, val24] = ResolveSubJob(24);
        ok = expect(!acc24 && val24 == 24, "resolve sub 24 reject") && ok;
        auto [acc255, val255] = ResolveSubJob(255);
        ok = expect(!acc255 && val255 == 255, "resolve sub 255 reject") && ok;
    }

    // --- main vs sub asymmetry at JOB_NON ---
    ok = expect(!CanSetMainJob(0) && CanSetSubJob(0), "JOB_NON main reject sub accept") && ok;
    ok = expect(CanSetMainJob(1) == CanSetSubJob(1), "job 1 both accept") && ok;
    ok = expect(CanSetMainJob(23) == CanSetSubJob(23), "job 23 both accept") && ok;
    ok = expect(!CanSetMainJob(24) && !CanSetSubJob(24), "job 24 both reject") && ok;
    ok = expect(!CanSetMainJob(255) && !CanSetSubJob(255), "job 255 both reject") && ok;

    return ok;
}
