#include "test_hit_interrupt_1406.h"

#include "map/hit_interrupt_capacity.h"

#include <iostream>

namespace
{
struct Attacker
{
};

struct State
{
    void TryInterrupt(Attacker* attacker)
    {
        ++calls;
        received = attacker;
    }

    int       calls    = 0;
    Attacker* received = nullptr;
};

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "hit interrupt 1406 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runHitInterrupt1406SelfTests() -> bool
{
    Attacker attacker;
    State    state;

    hitinterrupthelpers::Route(&state, &attacker);
    bool ok = expect(state.calls == 1, "present state called once");
    ok      = expect(state.received == &attacker, "attacker preserved") && ok;

    hitinterrupthelpers::Route(static_cast<State*>(nullptr), &attacker);
    ok = expect(state.calls == 1, "missing state is a no-op") && ok;
    return ok;
}
