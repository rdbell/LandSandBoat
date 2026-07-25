#pragma once

namespace equiparmormainattacktimerhelpers
{
struct Facts
{
    bool isEngaged{};
    bool currentStateIsAttack{};
};

struct Plan
{
    bool resetAttackTimer{};
};

constexpr Plan PlanFor(Facts facts)
{
    return { .resetAttackTimer = facts.isEngaged && facts.currentStateIsAttack };
}
} // namespace equiparmormainattacktimerhelpers
