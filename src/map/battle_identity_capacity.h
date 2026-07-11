#pragma once

#include "common/cbasetypes.h"

namespace battleidentityhelpers
{

class State
{
public:
    void SetBattleID(const uint16 battleID) { battleID_ = battleID; }
    auto GetBattleID() const -> uint16 { return battleID_; }

    void SetBattleTargetID(const uint16 battleTargetID) { battleTargetID_ = battleTargetID; }
    auto GetBattleTargetID() const -> uint16 { return battleTargetID_; }

private:
    uint16 battleID_       = 0;
    uint16 battleTargetID_ = 0;
};

} // namespace battleidentityhelpers
