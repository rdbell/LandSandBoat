#pragma once

namespace mobcontrollerroamhomegate
{
inline auto ShouldPathHome(bool dontRoamHome, bool farFromHome, bool canRoamHome) -> bool
{
    return !dontRoamHome && farFromHome && canRoamHome;
}
} // namespace mobcontrollerroamhomegate
