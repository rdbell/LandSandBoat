#include "test_aman_2624.h"
#include <iostream>
#include "map/aman.h"
auto runAMAN2624SelfTests() -> bool
{
    const bool ok = AMANMentorRankFromEvaluations(0) == 1 && AMANMentorRankFromEvaluations(49) == 1 && AMANMentorRankFromEvaluations(50) == 2 && AMANMentorRankFromEvaluations(99) == 2 && AMANMentorRankFromEvaluations(100) == 3 && AMANMentorRankFromEvaluations(1000) == 3;
    if (!ok) std::cerr << "AMAN self-test failed\n";
    return ok;
}
