#pragma once

namespace petcontrollermasterloss
{
inline auto ShouldDie(bool hasMaster, bool masterDead, bool petAlive, bool isMob) -> bool
{
    return (!hasMaster || masterDead) && petAlive && !isMob;
}
} // namespace petcontrollermasterloss
