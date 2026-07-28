#include "test_monstrosity_death_menu_7483.h"

#include "map/monstrosity.h"

#include <iostream>

auto runMonstrosityDeathMenu7483SelfTests() -> bool
{
    using monstrosity::PlanDeathMenu;

    const auto absent = PlanDeathMenu(false, GP_CLI_COMMAND_ACTION_HOMEPOINTMENU::MonstrosityRetry);
    if (absent.restoreHpMp || absent.retryAtOrigin)
    {
        std::cerr << "monstrosity death menu: absent data failed\n";
        return false;
    }

    const auto cancel = PlanDeathMenu(true, GP_CLI_COMMAND_ACTION_HOMEPOINTMENU::MonstrosityCancel);
    if (!cancel.restoreHpMp || !cancel.clearAnimation || !cancel.markHpUpdate || !cancel.returnToEntrance || cancel.retryAtOrigin)
    {
        std::cerr << "monstrosity death menu: cancel failed\n";
        return false;
    }

    const auto retry = PlanDeathMenu(true, GP_CLI_COMMAND_ACTION_HOMEPOINTMENU::MonstrosityRetry);
    if (!retry.restoreHpMp || !retry.retryAtOrigin || !retry.clearDeathTime || !retry.setDisappear || !retry.clearPacketList || !retry.restartCurrentZone || retry.returnToEntrance)
    {
        std::cerr << "monstrosity death menu: retry failed\n";
        return false;
    }

    const auto accept = PlanDeathMenu(true, GP_CLI_COMMAND_ACTION_HOMEPOINTMENU::Accept);
    if (!accept.restoreHpMp || !accept.clearAnimation || !accept.markHpUpdate || accept.returnToEntrance || accept.retryAtOrigin)
    {
        std::cerr << "monstrosity death menu: other action failed\n";
        return false;
    }

    return true;
}
