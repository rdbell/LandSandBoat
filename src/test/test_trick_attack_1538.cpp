#include "test_trick_attack_1538.h"

#include "map/trick_attack_capacity.h"

#include <iostream>

namespace
{
using trickattackhelpers::AreInLineFromDiff;
using trickattackhelpers::CompareTrickAttackSortKey;
using trickattackhelpers::ShouldEvaluateTrickAttack;
using trickattackhelpers::TrickAttackCandidateDistanceOK;
using trickattackhelpers::TrickAttackCandidateSkip;
using trickattackhelpers::WorldAngleMaxDeviance;
using trickattackhelpers::WorldAngleMinDistance;

auto Check() -> bool
{
    if (!AreInLineFromDiff(0) || !AreInLineFromDiff(8) || AreInLineFromDiff(9))
    {
        return false;
    }
    if (!AreInLineFromDiff(-8) || AreInLineFromDiff(-9))
    {
        return false;
    }
    if (!ShouldEvaluateTrickAttack(true) || ShouldEvaluateTrickAttack(false))
    {
        return false;
    }
    if (!TrickAttackCandidateDistanceOK(1.0f, 2.0f) || TrickAttackCandidateDistanceOK(0.4f, 2.0f) ||
        TrickAttackCandidateDistanceOK(2.0f, 2.0f))
    {
        return false;
    }
    if (!TrickAttackCandidateSkip(true, false) || !TrickAttackCandidateSkip(false, true) || TrickAttackCandidateSkip(false, false))
    {
        return false;
    }
    if (CompareTrickAttackSortKey(1.0f, 2.0f, 9, 1) != -1 || CompareTrickAttackSortKey(2.0f, 1.0f, 1, 9) != 1)
    {
        return false;
    }
    if (CompareTrickAttackSortKey(1.0f, 1.0f, 1, 2) != -1 || CompareTrickAttackSortKey(1.0f, 1.0f, 2, 2) != 0)
    {
        return false;
    }
    if (WorldAngleMinDistance != 0.5f || WorldAngleMaxDeviance != 8)
    {
        return false;
    }
    return true;
}
} // namespace

auto runTrickAttack1538SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "trick_attack_1538 self-tests failed\n";
        return false;
    }
    return true;
}
