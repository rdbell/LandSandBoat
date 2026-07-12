#include "test_sub_job_stats_1597.h"

#include "map/sub_job_stats_capacity.h"

#include <iostream>

namespace
{
using namespace subjobstatshelpers;

struct Case
{
    std::uint8_t  rank;
    std::uint16_t level;
    std::uint16_t stat;
    std::uint16_t want;
};

// Goldens match internal/mobutils TestGetSubJobStats.
constexpr Case kCases[] = {
    // default → half
    { 0, 1, 20, 10 },
    { 0, 50, 40, 20 },
    { 8, 30, 15, 7 },
    // A
    { 1, 1, 5, 2 },
    { 1, 25, 17, 3 },
    { 1, 30, 19, 4 },
    { 1, 35, 22, 7 },
    { 1, 40, 24, 9 },
    { 1, 45, 27, 10 },
    { 1, 50, 29, 10 },
    // B
    { 2, 1, 4, 2 },
    { 2, 30, 17, 5 },
    { 2, 35, 19, 6 },
    { 2, 40, 21, 8 },
    { 2, 45, 23, 9 },
    { 2, 50, 26, 12 },
    // C
    { 3, 1, 4, 2 },
    { 3, 30, 15, 3 },
    { 3, 35, 17, 5 },
    { 3, 40, 19, 5 },
    { 3, 45, 21, 8 },
    { 3, 50, 23, 12 },
    // D
    { 4, 1, 3, 1 },
    { 4, 30, 13, 2 },
    { 4, 35, 14, 4 },
    { 4, 40, 16, 5 },
    { 4, 45, 18, 7 },
    { 4, 50, 20, 8 },
    // E
    { 5, 1, 3, 1 },
    { 5, 30, 11, 2 },
    { 5, 35, 13, 3 },
    { 5, 40, 14, 5 },
    { 5, 45, 16, 6 },
    { 5, 50, 17, 6 },
    // F
    { 6, 1, 2, 1 },
    { 6, 30, 9, 1 },
    { 6, 35, 10, 3 },
    { 6, 40, 11, 4 },
    { 6, 45, 13, 5 },
    { 6, 50, 14, 6 },
    // G
    { 7, 1, 2, 1 },
    { 7, 30, 7, 1 },
    { 7, 35, 8, 2 },
    { 7, 40, 9, 4 },
    { 7, 45, 10, 4 },
    { 7, 50, 11, 5 },
};

auto Check() -> bool
{
    for (const auto& tc : kCases)
    {
        if (GetSubJobStats(tc.rank, tc.level, tc.stat) != tc.want)
        {
            std::cerr << "GetSubJobStats(" << static_cast<int>(tc.rank) << ", " << tc.level << ", " << tc.stat
                      << ") = " << GetSubJobStats(tc.rank, tc.level, tc.stat) << " want " << tc.want << "\n";
            return false;
        }
    }
    return true;
}
} // namespace

auto runSubJobStats1597SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "sub_job_stats_1597 self-tests failed\n";
        return false;
    }
    return true;
}
