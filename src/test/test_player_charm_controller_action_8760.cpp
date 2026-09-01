#include "test_player_charm_controller_action_8760.h"

#include "map/ai/controllers/player_charm_controller_action_capacity.h"
#include "omega_self_test_registry.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "player charm controller action 8760 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runPlayerCharmControllerAction8760SelfTests() -> bool
{
    using playercharmcontrolleraction::Action;
    using playercharmcontrolleraction::CanDispatch;

    return expect(!CanDispatch(Action::Cast), "Cast rejects") &&
           expect(!CanDispatch(Action::ChangeTarget), "ChangeTarget rejects") &&
           expect(!CanDispatch(Action::WeaponSkill), "WeaponSkill rejects") &&
           expect(!CanDispatch(Action::Ability), "Ability rejects") &&
           expect(!CanDispatch(Action::RangedAttack), "RangedAttack rejects");
}

OMEGA_REGISTER_SELF_TEST("player-charm-controller-action-8760", runPlayerCharmControllerAction8760SelfTests);
