#pragma once

namespace automatoncontrollermasterloss
{
inline auto ShouldDie(bool hasMaster, bool masterDead, bool automatonAlive) -> bool
{
    return (!hasMaster || masterDead) && automatonAlive;
}
} // namespace automatoncontrollermasterloss
